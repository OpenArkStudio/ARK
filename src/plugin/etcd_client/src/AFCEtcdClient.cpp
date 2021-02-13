#include "json/json.hpp"
#include "base/AFLogger.hpp"
#include "base/AFStringUtils.hpp"
#include "etcd_client/include/AFCEtcdClient.hpp"

namespace ark {

#define ETCD_API_V3_ERROR_HTTP_CODE_AUTH 401
#define ETCD_API_V3_ERROR_HTTP_INVALID_PARAM 400
#define ETCD_API_V3_ERROR_HTTP_PRECONDITION 412
// @see https://godoc.org/google.golang.org/grpc/codes
#define ETCD_API_V3_ERROR_GRPC_CODE_UNAUTHENTICATED 16

#define ETCD_API_V3_MEMBER_LIST "/v3/cluster/member/list"
#define ETCD_API_V3_AUTH_AUTHENTICATE "/v3/auth/authenticate"
#define ETCD_API_V3_AUTH_USER_GET "/v3/auth/user/get"

#define ETCD_API_V3_KV_GET "/v3/kv/range"
#define ETCD_API_V3_KV_SET "/v3/kv/put"
#define ETCD_API_V3_KV_DELETE "/v3/kv/deleterange"

#define ETCD_API_V3_WATCH "/v3/watch"

#define ETCD_API_V3_LEASE_REVOKE "/v3/kv/lease/revoke"

static const std::string kLeaseGrantAPI = "/v3/lease/grant";
static const std::string kLeaseKeepaliveAPI = "/v3/lease/keepalive";

AFCEtcdClient::AFCEtcdClient(
    AFIHttpClientModule* httpClient, std::vector<std::string> endpoints, int connectTimeout, int ioTimeout)
    : httpClient_(httpClient)
    , connectTimeout_(connectTimeout)
    , ioTimeout_(ioTimeout)
{
    std::vector<std::string> hostSvc;
    for (const auto& ep : endpoints)
    {
        AFStringUtils::split(hostSvc, ep, ':');
        if (hostSvc.size() == 2)
        {
            endpoints_.emplace_back(std::make_pair(std::move(hostSvc[0]), std::move(hostSvc[1])));
        }
        else
        {
            ARK_LOG_WARN("Invalid endpoint: {}", ep);
        }
        hostSvc.clear();
    }

    assert(httpClient_);
    assert(endpoints_.size());

    rand_.SetSeed(static_cast<uint32_t>(time(0)));
    selectedEndpointIndex_ = rand_.Random() % endpoints_.size();
}

void AFCEtcdClient::post(std::function<void(std::pair<bool, std::string>)> cb, const std::string& api,
    std::shared_ptr<std::string> body, size_t epIdx, size_t triedCnt)
{
    const auto& ep = endpoints_[epIdx];
    httpClient_->Post(
        [this, cb = std::move(cb), &api, body, epIdx, triedCnt](std::pair<bool, std::string> rsp) mutable {
            if (rsp.first)
            {
                cb(std::move(rsp));
                selectedEndpointIndex_ = epIdx;
                return;
            }

            if (triedCnt < endpoints_.size())
            {
                if (++epIdx >= endpoints_.size())
                {
                    epIdx %= endpoints_.size();
                }
                post(std::move(cb), api, std::move(body), epIdx, triedCnt + 1);
            }
            else
            {
                cb(std::move(rsp));
            }
        },
        ep.first, ep.second, api, nullptr, body.get(), nullptr, nullptr, ioTimeout_, connectTimeout_);
}

void AFCEtcdClient::Grant(std::function<void(std::pair<int64_t, std::string>)> cb, int ttl)
{
    post(
        [cb = std::move(cb)](std::pair<bool, std::string> rsp) {
            if (rsp.first)
            {
                std::pair<int64_t, std::string> cbRsp;

                auto j = nlohmann::json::parse(rsp.second);
                if (auto it = j.find("ID"); it != j.end())
                {
                    if (it->is_string())
                    {
                        cbRsp.first = std::stoll(it->get<std::string>());
                    }
                    else
                    {
                        cbRsp.first = -1;
                        cbRsp.second = "Unknown error";
                    }
                }
                else if (it = j.find("error"); it != j.end())
                {
                    cbRsp.first = -1;
                    if (it->is_string())
                    {
                        cbRsp.second = it->get<std::string>();
                    }
                    else
                    {
                        cbRsp.second = "Unknown error";
                    }
                }
                else
                {
                    cbRsp.first = -1;
                    cbRsp.second = "Unknown error";
                }

                cb(cbRsp);
            }
            else
            {
                cb(std::make_pair(static_cast<int64_t>(-1), rsp.second));
            }
        },
        kLeaseGrantAPI, std::make_shared<std::string>(ARK_FORMAT("{}\"TTL\": {}{}", '{', ttl, '}')),
        selectedEndpointIndex_, 1);
}

void AFCEtcdClient::Keepalive(std::function<void(std::pair<bool, std::string>)> cb, int64_t leaseID)
{
    //{"error":"unexpected EOF","message":"unexpected EOF","code":2}
    //{"result":{"header":{"cluster_id":"14841639068965178418","member_id":"10276657743932975437","revision":"371","raft_term":"7"},"ID":"7587849661051446219","TTL":"5000"}}
    // 如果没有TTL，表示lease已经过期
}

/*
<<COMMENT
    https://www.base64encode.org/
foo is 'Zm9v' in Base64
bar is 'YmFy'
COMMENT

    curl -L http://localhost:2379/v3/kv/put \
  -X POST -d '{"key": "Zm9v", "value": "YmFy"}'
# {"header":{"cluster_id":"12585971608760269493","member_id":"13847567121247652255","revision":"2","raft_term":"3"}}

curl -L http://localhost:2379/v3/kv/range \
  -X POST -d '{"key": "Zm9v"}'
# {"header":{"cluster_id":"12585971608760269493","member_id":"13847567121247652255","revision":"2","raft_term":"3"},"kvs":[{"key":"Zm9v","create_revision":"2","mod_revision":"2","version":"1","value":"YmFy"}],"count":"1"}

# get all keys prefixed with "foo"
curl -L http://localhost:2379/v3/kv/range \
  -X POST -d '{"key": "Zm9v", "range_end": "Zm9w"}'
# {"header":{"cluster_id":"12585971608760269493","member_id":"13847567121247652255","revision":"2","raft_term":"3"},"kvs":[{"key":"Zm9v","create_revision":"2","mod_revision":"2","version":"1","value":"YmFy"}],"count":"1"}


curl -N http://localhost:2379/v3/watch \
  -X POST -d '{"create_request": {"key":"Zm9v"} }' &
# {"result":{"header":{"cluster_id":"12585971608760269493","member_id":"13847567121247652255","revision":"1","raft_term":"2"},"created":true}}

curl -L http://localhost:2379/v3/kv/put \
  -X POST -d '{"key": "Zm9v", "value": "YmFy"}' >/dev/null 2>&1
# {"result":{"header":{"cluster_id":"12585971608760269493","member_id":"13847567121247652255","revision":"2","raft_term":"2"},"events":[{"kv":{"key":"Zm9v","create_revision":"2","mod_revision":"2","version":"1","value":"YmFy"}}]}}

*/

} // namespace ark
