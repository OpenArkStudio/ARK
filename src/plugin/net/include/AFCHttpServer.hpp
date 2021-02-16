#pragma once

#include <zephyr/zephyr.hpp>
#include "net/interface/AFINet.hpp"

namespace ark {

#ifndef ASIO_STANDALONE

class AFCHttpServer
{
private:
    std::shared_ptr<zephyr::http_server> server_;

public:
    AFCHttpServer();
    ~AFCHttpServer();
};

#endif //ASIO_STANDALONE

} // namespace ark
