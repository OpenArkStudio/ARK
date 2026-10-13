#pragma once

#include <memory>

#include <arknet/http/http_server.hpp>

namespace ark {

class AFCHttpServer
{
private:
    std::shared_ptr<arknet::http_server> server_;

public:
    AFCHttpServer();
    ~AFCHttpServer();
};

} // namespace ark
