//
// Desktop WebSocket backend — wraps Boost::Beast/Asio.
//
// Runs its own io_context on a dedicated thread (Beast has no built-in
// background thread) and enqueues events for poll() to deliver, matching the
// Emscripten backend's threading model.
//
#ifndef __EMSCRIPTEN__

#include "lights/core/net/web_socket.h"

#include <boost/asio/executor_work_guard.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/post.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

#include <chrono>
#include <deque>
#include <mutex>
#include <optional>
#include <thread>
#include <utility>

namespace OZZ::net {

    namespace {
        namespace beast     = boost::beast;
        namespace asio      = boost::asio;
        namespace websocket = beast::websocket;
        using tcp           = asio::ip::tcp;

        struct ParsedUrl {
            std::string host;
            std::string port;
            std::string target;
        };

        // Handles "ws://host[:port][/path]". No TLS support (matches the previous USE_TLS OFF setup).
        ParsedUrl ParseUrl(const std::string& url) {
            std::string rest = url;
            if (const auto schemeEnd = rest.find("://"); schemeEnd != std::string::npos) {
                rest = rest.substr(schemeEnd + 3);
            }

            const auto pathStart = rest.find('/');
            const std::string authority = pathStart == std::string::npos ? rest : rest.substr(0, pathStart);

            ParsedUrl result;
            result.target = pathStart == std::string::npos ? "/" : rest.substr(pathStart);

            if (const auto portStart = authority.find(':'); portStart == std::string::npos) {
                result.host = authority;
                result.port = "80";
            } else {
                result.host = authority.substr(0, portStart);
                result.port = authority.substr(portStart + 1);
            }
            return result;
        }

        class BoostWebSocket final : public WebSocket {
        public:
            ~BoostWebSocket() override { stop(); }

            void setUrl(const std::string& u) override { url = u; }

            void setOnMessageCallback(OnMessageCallback callback) override {
                userCallback = std::move(callback);
            }

            void start() override {
                const ParsedUrl parsed = ParseUrl(url);
                host   = parsed.host;
                target = parsed.target;

                workGuard.emplace(ioc.get_executor());
                ioThread = std::thread([this] { ioc.run(); });

                resolver.async_resolve(host, parsed.port, [this](const beast::error_code ec, const tcp::resolver::results_type results) {
                    onResolve(ec, results);
                });
            }

            void stop() override {
                if (!ioThread.joinable()) return;

                asio::post(ioc, [this] { requestClose(); });

                workGuard.reset();
                ioThread.join();
                ioc.restart();
            }

            void sendBinary(const std::string& data) override { send(data, true); }
            void sendText(const std::string& data) override { send(data, false); }

            void poll() override {
                std::deque<WebSocketMessage> drained;
                {
                    std::lock_guard<std::mutex> lock(queueMutex);
                    drained.swap(queue);
                }
                if (!userCallback) return;
                for (const auto& msg : drained) {
                    userCallback(msg);
                }
            }

        private:
            void enqueue(WebSocketMessage msg) {
                std::lock_guard<std::mutex> lock(queueMutex);
                queue.push_back(std::move(msg));
            }

            void enqueueError(const std::string& reason) {
                WebSocketMessage msg;
                msg.type             = WebSocketMessageType::Error;
                msg.errorInfo.reason = reason;
                enqueue(std::move(msg));
            }

            void onResolve(beast::error_code ec, const tcp::resolver::results_type& results) {
                if (ec) {
                    enqueueError("resolve failed: " + ec.message());
                    return;
                }
                beast::get_lowest_layer(ws).expires_after(std::chrono::seconds(30));
                beast::get_lowest_layer(ws).async_connect(results, [this](const beast::error_code ec, const tcp::resolver::results_type::endpoint_type&) {
                    onConnect(ec);
                });
            }

            void onConnect(beast::error_code ec) {
                if (ec) {
                    enqueueError("connect failed: " + ec.message());
                    return;
                }
                beast::get_lowest_layer(ws).expires_never();
                ws.set_option(websocket::stream_base::timeout::suggested(beast::role_type::client));
                ws.async_handshake(host, target, [this](const beast::error_code ec) { onHandshake(ec); });
            }

            void onHandshake(beast::error_code ec) {
                if (ec) {
                    enqueueError("handshake failed: " + ec.message());
                    return;
                }
                WebSocketMessage msg;
                msg.type         = WebSocketMessageType::Open;
                msg.openInfo.uri = url;
                enqueue(std::move(msg));
                doRead();
            }

            void doRead() {
                ws.async_read(buffer, [this](const beast::error_code ec, const std::size_t bytesTransferred) {
                    onRead(ec, bytesTransferred);
                });
            }

            void onRead(beast::error_code ec, std::size_t) {
                if (ec == websocket::error::closed) {
                    WebSocketMessage msg;
                    msg.type             = WebSocketMessageType::Close;
                    msg.closeInfo.remote = true;
                    enqueue(std::move(msg));
                    return;
                }
                if (ec) {
                    enqueueError("read failed: " + ec.message());
                    return;
                }

                WebSocketMessage msg;
                msg.type   = WebSocketMessageType::Message;
                msg.binary = ws.got_binary();
                msg.data.assign(static_cast<const char*>(buffer.data().data()), buffer.size());
                enqueue(std::move(msg));

                buffer.consume(buffer.size());
                doRead();
            }

            void send(const std::string& data, bool binary) {
                asio::post(ioc, [this, data, binary] { enqueueAndMaybeWrite(data, binary); });
            }

            void enqueueAndMaybeWrite(std::string payload, bool binary) {
                outbox.emplace_back(std::move(payload), binary);
                if (writing) {
                    // Write already in flight (Beast allows only one); onWrite() will pick this up.
                    return;
                }

                writing = true;
                ws.binary(outbox.front().second);
                ws.async_write(asio::buffer(outbox.front().first), [this](const beast::error_code ec, const std::size_t bytesTransferred) {
                    onWrite(ec, bytesTransferred);
                });
            }

            void onWrite(beast::error_code ec, std::size_t) {
                if (ec) {
                    enqueueError("write failed: " + ec.message());
                    return;
                }

                outbox.pop_front();
                if (!outbox.empty()) {
                    ws.binary(outbox.front().second);
                    ws.async_write(asio::buffer(outbox.front().first), [this](const beast::error_code ec, const std::size_t bytesTransferred) {
                        onWrite(ec, bytesTransferred);
                    });
                    return;
                }
                writing = false;
            }

            // Aborts whatever stage of connection we're at, so the io thread can drain and stop().
            void requestClose() {
                resolver.cancel();
                if (ws.is_open()) {
                    ws.async_close(websocket::close_code::normal, [](beast::error_code) {});
                } else {
                    beast::error_code ec;
                    beast::get_lowest_layer(ws).socket().close(ec);
                }
            }

            asio::io_context ioc;
            std::optional<asio::executor_work_guard<asio::io_context::executor_type>> workGuard;
            std::thread ioThread;
            tcp::resolver resolver{ioc};
            websocket::stream<beast::tcp_stream> ws{ioc};
            beast::flat_buffer buffer;

            std::string url;
            std::string host;
            std::string target;

            OnMessageCallback userCallback;
            std::mutex queueMutex;
            std::deque<WebSocketMessage> queue;

            // Only ever touched on ioc's thread -- see class comment.
            std::deque<std::pair<std::string, bool>> outbox; // (payload, binary)
            bool writing{false};
        };
    } // namespace

    std::unique_ptr<WebSocket> CreateWebSocket() {
        return std::make_unique<BoostWebSocket>();
    }

} // namespace OZZ::net

#endif // !__EMSCRIPTEN__
