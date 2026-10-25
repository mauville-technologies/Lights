#include "lights/core/net/client/client.h"

#include <utility>

namespace OZZ::net::client {

    Client::Client(std::string url, ConnectionFactory factory) : url(std::move(url)), factory(std::move(factory)) {}

    void Client::Start() {
        conn.SetDelegate(factory(conn));
        conn.Connect(url);
    }

    void Client::Stop() {
        conn.Disconnect();
    }

    void Client::Poll() {
        conn.Poll();
    }

} // namespace OZZ::net::client
