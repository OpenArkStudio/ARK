#pragma once

#include "base/random/AFRandom.hpp"
#include "net/interface/AFIHttpClientModule.hpp"
#include "etcd_client/interface/AFIEtcdClient.hpp"

namespace ark {

/**
 * AFCEtcdClient is NOT thread-safe, please create an AFCEtcdClient object per thread, or use a lock mechanism.
 */
class AFCEtcdClient final : public AFIEtcdClient {
public:
    /**
     * Constructor
     * @param httpClient
     * @param endpoints { "172.20.3.74:2379", "172.20.3.75:2379" } for example
     * @param connectTimeout in seconds
     * @param ioTimeout in seconds
     */
    AFCEtcdClient(AFIHttpClientModule* httpClient, std::vector<std::string> endpoints, int connectTimeout, int ioTimeout);

    void Grant(std::function<void(std::pair<int64_t, std::string>)> cb, int ttl) override;
    void Keepalive(std::function<void(std::pair<bool, std::string>)> cb, int64_t leaseID) override;

private:
    void post(std::function<void(std::pair<bool, std::string>)> cb, const std::string& api, std::shared_ptr<std::string> body, size_t epIdx, size_t triedCnt);

private:
    AFIHttpClientModule* const httpClient_;
    int const connectTimeout_;
    int const ioTimeout_;

    AFRandom rand_; // TODO might not need this
    size_t selectedEndpointIndex_;
    std::vector<std::pair<std::string, std::string>> endpoints_;
};

} // namespace ark
