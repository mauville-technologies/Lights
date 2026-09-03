#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

#include <cstdint>
#include <deque>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <string>

#include "connection_delegate.h"

namespace OZZ::net::server {

    namespace beast     = boost::beast;
    namespace asio       = boost::asio;
    namespace websocket = beast::websocket;
    using tcp            = asio::ip::tcp;

    // Pinned to the reactor thread that accepted its socket. Send()/Kill() post onto
    // that executor, so outbox/writing/etc. need no mutex -- touched from one thread only.
    class ConnectionHandler : public std::enable_shared_from_this<ConnectionHandler> {
    public:
        ConnectionHandler(tcp::socket&& socket, uint64_t id, std::function<void(uint64_t)> onClosedCallback);

        uint64_t Id() const { return id; }

        // Installs the message handler. Must be called (by the connection factory) before
        // Run(), or from inside a delegate's own OnMessage() to transition phases.
        void AttachDelegate(std::shared_ptr<ConnectionDelegate> newDelegate);

        // Kicks off the WebSocket accept handshake. Must be called once, after SetDelegate().
        void Run();

        // Thread-safe: queues payload, posts the write onto this connection's own executor.
        void Send(std::string payload);

        // Thread-safe: requests a graceful close, deferred until any queued sends have drained.
        void Kill(const std::string& killReason);

        std::shared_future<std::string> OnClosed();

    private:
        void onAccept(beast::error_code ec);
        void doRead();
        void onRead(beast::error_code ec, std::size_t bytesTransferred);
        void onWrite(beast::error_code ec, std::size_t bytesTransferred);

        void enqueueAndMaybeWrite(std::string payload);
        void requestClose(const std::string& reason);
        void doClose(const std::string& reason);

        websocket::stream<beast::tcp_stream> ws;
        beast::flat_buffer buffer;

        uint64_t id;
        std::function<void(uint64_t)> onClosedCallback;
        std::shared_ptr<ConnectionDelegate> delegate;

        // Only ever touched on ws.get_executor() -- see class comment.
        std::deque<std::string> outbox;
        bool writing{false};
        bool closeRequested{false};
        std::string pendingCloseReason;

        std::once_flag closedOnce;
        std::promise<std::string> onClosedPromise;
        std::shared_future<std::string> onClosedFuture{onClosedPromise.get_future().share()};
    };

} // namespace OZZ::net::server
