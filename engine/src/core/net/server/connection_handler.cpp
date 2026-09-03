#ifndef __EMSCRIPTEN__

#include "lights/core/net/server/connection_handler.h"

#include "lights/core/util/assert.h"

#include <spdlog/spdlog.h>

namespace OZZ::net::server {

    ConnectionHandler::ConnectionHandler(tcp::socket&& socket,
                                         uint64_t id,
                                         std::function<void(uint64_t)> onClosedCallback)
        : ws(std::move(socket))
        , id(id)
        , onClosedCallback(std::move(onClosedCallback)) {}

    void ConnectionHandler::AttachDelegate(std::shared_ptr<ConnectionDelegate> newDelegate) {
        if (delegate) {
            delegate->OnDetached(ChangeDelegate);
        }

        delegate = std::move(newDelegate);

        if (delegate) {
            delegate->OnAttached();
        }
    }

    void ConnectionHandler::Run() {
        OZZ_ASSERT(delegate, "ConnectionHandler::Run() requires SetDelegate() to have been called first");

        // dispatch not post: already on this socket's executor when called, so it runs inline.
        asio::dispatch(ws.get_executor(), [self = shared_from_this()] {
            self->ws.set_option(websocket::stream_base::timeout::suggested(beast::role_type::server));
            self->ws.async_accept(beast::bind_front_handler(&ConnectionHandler::onAccept, self));
        });
    }

    void ConnectionHandler::onAccept(beast::error_code ec) {
        if (ec) {
            spdlog::warn("Connection {} handshake failed: {}", id, ec.message());
            doClose("Handshake failed");
            return;
        }
        doRead();
    }

    void ConnectionHandler::doRead() {
        ws.async_read(buffer, beast::bind_front_handler(&ConnectionHandler::onRead, shared_from_this()));
    }

    void ConnectionHandler::onRead(beast::error_code ec, [[maybe_unused]] std::size_t bytesTransferred) {
        if (ec == websocket::error::closed) {
            doClose("Connection closed");
            return;
        }
        if (ec) {
            doClose("Read error: " + ec.message());
            return;
        }

        // Keep the delegate alive for the whole call: OnMessage() may call SetDelegate(),
        // which would otherwise drop the last reference to the object still executing.
        auto currentDelegate = delegate;
        currentDelegate->OnMessage({static_cast<const uint8_t*>(buffer.data().data()), buffer.size()}, ws.got_binary());

        buffer.consume(buffer.size());
        doRead();
    }

    void ConnectionHandler::Send(std::string payload) {
        asio::post(ws.get_executor(), [self = shared_from_this(), payload = std::move(payload)]() mutable {
            self->enqueueAndMaybeWrite(std::move(payload));
        });
    }

    void ConnectionHandler::enqueueAndMaybeWrite(std::string payload) {
        outbox.push_back(std::move(payload));
        if (writing) {
            // Write already in flight (Beast allows only one); onWrite() will pick this up.
            return;
        }

        writing = true;
        ws.binary(true);
        ws.async_write(asio::buffer(outbox.front()),
                       beast::bind_front_handler(&ConnectionHandler::onWrite, shared_from_this()));
    }

    void ConnectionHandler::onWrite(beast::error_code ec, [[maybe_unused]] std::size_t bytesTransferred) {
        if (ec) {
            doClose("Write error: " + ec.message());
            return;
        }

        outbox.pop_front();
        if (!outbox.empty()) {
            ws.async_write(asio::buffer(outbox.front()),
                           beast::bind_front_handler(&ConnectionHandler::onWrite, shared_from_this()));
            return;
        }

        writing = false;
        if (closeRequested) {
            doClose(pendingCloseReason);
        }
    }

    void ConnectionHandler::Kill(const std::string& killReason) {
        asio::post(ws.get_executor(), [self = shared_from_this(), killReason] {
            self->requestClose(killReason);
        });
    }

    void ConnectionHandler::requestClose(const std::string& reason) {
        closeRequested = true;
        pendingCloseReason = reason;
        if (!writing) {
            doClose(reason);
        }
        // else: onWrite() will call doClose() once the outbox has drained.
    }

    void ConnectionHandler::doClose(const std::string& reason) {
        std::call_once(closedOnce, [&] {
            onClosedPromise.set_value(reason);
            if (onClosedCallback) {
                onClosedCallback(id);
            }
            if (delegate) {
                auto currentDelegate = delegate;
                currentDelegate->OnDetached(ConnectionClosed);
            }
        });

        if (ws.is_open()) {
            ws.async_close(websocket::close_code::normal, [self = shared_from_this()](beast::error_code) {
                // Nothing to do -- the socket is torn down once the last shared_ptr to `self` drops.
            });
        }
    }

    std::shared_future<std::string> ConnectionHandler::OnClosed() {
        return onClosedFuture;
    }

} // namespace OZZ::net::server

#endif // !__EMSCRIPTEN__
