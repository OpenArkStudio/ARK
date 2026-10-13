#include "net/include/AFCHttpServer.hpp"

namespace ark {

AFCHttpServer::AFCHttpServer()
{
    server_ = std::make_shared<arknet::http_server>();
}

AFCHttpServer::~AFCHttpServer()
{
    server_->stop();
}

} // namespace ark
