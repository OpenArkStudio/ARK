#pragma once

#include "ark/etcd_client/etcd_client.hpp"
#include "ark/net/http_client_module.hpp"
#include "ark/random/random_generator.hpp"

namespace ark {

/**
 * AFCEtcdClient is NOT thread-safe, please create an AFCEtcdClient object per thread, or use a lock mechanism.
 */
class etcd_client_impl final : public etcd_client
{
public:
    /**
     * Constructor
     * @param httpClient
     * @param endpoints { "172.20.3.74:2379", "172.20.3.75:2379" } for example
     * @param connectTimeout in seconds
     * @param ioTimeout in seconds
     */
    etcd_client_impl(
        http_client_module* httpClient, std::vector<std::string> endpoints, int connectTimeout, int ioTimeout);

    void grant(std::function<void(std::pair<int64_t, std::string>)> cb, int ttl) override;
    void keepalive(std::function<void(std::pair<bool, std::string>)> cb, int64_t leaseID) override;

private:
    void post(std::function<void(std::pair<bool, std::string>)> cb, const std::string& api,
        std::shared_ptr<std::string> body, size_t epIdx, size_t triedCnt);

private:
    http_client_module* const httpClient_;
    int const connectTimeout_;
    int const ioTimeout_;

    random_generator rand_; // TODO might not need this
    size_t selectedEndpointIndex_;
    std::vector<std::pair<std::string, std::string>> endpoints_;
};

} // namespace ark
