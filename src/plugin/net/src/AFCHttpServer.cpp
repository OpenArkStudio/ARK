#include "net/include/AFCHttpServer.hpp"

namespace ark {

#ifndef ASIO_STANDALONE

AFCHttpServer::AFCHttpServer()
{
    server_ = std::make_shared<zephyr::http_server>();
    //server_->
}

AFCHttpServer::~AFCHttpServer()
{
    // TODO
}

#endif //ASIO_STANDALONE

} // namespace ark