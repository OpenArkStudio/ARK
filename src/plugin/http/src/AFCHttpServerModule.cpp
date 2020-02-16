#include "http/include/AFCHttpServerModule.hpp"

namespace ark {

bool AFCHttpServerModule::Init()
{
    http_server_ = std::make_shared<OB::Belle::Server>();
    return true;
}

bool AFCHttpServerModule::PreShut()
{
    if (http_server_ != nullptr)
    {
        http_server_->io().stop();
    }
}

bool AFCHttpServerModule::CreateServer(std::string const& address, uint16_t port = 8080, uint8_t thread_num = 1)
{
    http_server_->address(address);
    http_server_->port(port);
    http_server_->threads(thread_num);
    http_server_->websocket(false); // ws is not thread-safe, so disable it.

    // Set default http headers
    OB::Belle::Headers headers;
    headers.set(OB::Belle::Header::server, "ARK");
    http_server_->http_headers(headers);

    // heandle signals
    http_server_->signals(SIGINT, SIGTERM);
    http_server_->on_signals([&](auto ec, auto sig) {
        std::cerr << "\nSignal: " << sig << " recved" << std::endl;
        http_server_->io().stop();
    });
}

bool AFCHttpServerModule::AddRoute(
    std::string const& url, OB::Belle::Method method, OB::Belle::fn_on_http&& http_function)
{
}

bool AFCHttpServerModule::StartWork()
{
    http_server_->listen();
}

} // namespace ark