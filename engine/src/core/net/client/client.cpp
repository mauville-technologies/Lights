#include "lights/core/net/client/client.h"

#include <utility>

namespace OZZ::net::client {

    class Client::LifecycleDelegate final : public ConnectionDelegate {
    public:
        LifecycleDelegate(Client& owner, std::shared_ptr<ConnectionDelegate> inner)
            : owner(owner)
            , inner(std::move(inner)) {}

        void OnOpen() override {
            owner.lifecycle = ConnectionLifecycle::Connected;
            owner.hasConnectedBefore = true;
            if (inner) inner->OnOpen();
        }

        void OnMessage(std::span<const uint8_t> data, bool binary) override {
            if (inner) inner->OnMessage(data, binary);
        }

        void OnClose(const std::string& reason) override {
            owner.lifecycle = ConnectionLifecycle::Disconnected;
            if (inner) inner->OnClose(reason);
        }

        void OnError(const std::string& reason) override {
            owner.lifecycle = ConnectionLifecycle::Disconnected;
            if (inner) inner->OnError(reason);
        }

    private:
        Client& owner;
        std::shared_ptr<ConnectionDelegate> inner;
    };

    Client::Client(std::string url, ConnectionFactory factory) : url(std::move(url)), factory(std::move(factory)) {}

    void Client::Start() {
        lifecycle = ConnectionLifecycle::Connecting;
        conn.SetDelegate(std::make_shared<LifecycleDelegate>(*this, factory(conn)));
        conn.Connect(url);
    }

    void Client::Stop() {
        conn.Disconnect();
        lifecycle = ConnectionLifecycle::Disconnected;
    }

    bool Client::IsConnected() const {
        return lifecycle == ConnectionLifecycle::Connected;
    }

    void Client::Poll() {
        conn.Poll();
    }

} // namespace OZZ::net::client
