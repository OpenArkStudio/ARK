#include "ark/net/http_server.hpp"

namespace ark {

http_server::http_server() {
    server_ = std::make_shared<arknet::http_server>();
}

http_server::~http_server() {
    server_->stop();
}

} // namespace ark
