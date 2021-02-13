#pragma once

#include "base/AFPlatform.hpp"
#include "AFSimulateMsgDefine.hpp"
#include "base/AFDefine.hpp"

namespace ark {

using SimulateNetID = int64_t;

using SIMULATE_TRIGGER_CB_FUNCTION =
    std::function<void(const std::string& scene_id, const int32_t index, const simulate::TriggerPair& pair)>;

class AFSimulateScene
{
public:
    AFSimulateScene() {}

    AFSimulateScene(const guid_t& scene_obj_id)
        : scene_obj_id_(scene_obj_id)
    {
    }

    const guid_t& GetSceneObjID()
    {
        return scene_obj_id_;
    }

    bool Exist(const SimulateNetID obj_id)
    {
        auto iter = net_id_actor_list_.find(obj_id);
        return iter != net_id_actor_list_.end();
    }

    void InsertObjActor(const SimulateNetID obj_id, const guid_t& actor_obj_id)
    {
        net_id_actor_list_.insert(std::make_pair(obj_id, actor_obj_id));
        actor_net_id_list_.insert(std::make_pair(actor_obj_id, obj_id));
    }

    const guid_t& FindActor(const SimulateNetID obj_id)
    {
        auto iter = net_id_actor_list_.find(obj_id);
        if (iter == net_id_actor_list_.end())
        {
            return NULL_GUID;
        }

        return iter->second;
    }

    SimulateNetID FindObj(const guid_t& actor_obj_id)
    {
        auto iter = actor_net_id_list_.find(actor_obj_id);
        if (iter == actor_net_id_list_.end())
        {
            return 0;
        }

        return iter->second;
    }

    void RemoveObj(const SimulateNetID obj_id)
    {
        auto iter = net_id_actor_list_.find(obj_id);
        if (iter == net_id_actor_list_.end())
        {
            return;
        }

        auto actor = iter->second;
        net_id_actor_list_.erase(iter);
        actor_net_id_list_.erase(actor);
    }

    void RemoveActor(const guid_t& actor_obj_id)
    {
        auto iter = actor_net_id_list_.find(actor_obj_id);
        if (iter == actor_net_id_list_.end())
        {
            return;
        }

        auto obj_id = iter->second;
        net_id_actor_list_.erase(obj_id);
        actor_net_id_list_.erase(iter);
    }

private:
    using NetActorMap = std::map<SimulateNetID, guid_t>;
    using ActorNetMap = std::map<guid_t, SimulateNetID>;

    guid_t scene_obj_id_{0U};
    NetActorMap net_id_actor_list_;
    ActorNetMap actor_net_id_list_;
};

using IndexSceneMap = std::map<int32_t, AFSimulateScene>;
using SceneIndexSceneMap = std::unordered_map<std::string, IndexSceneMap>;

using SceneIndexPair = std::pair<std::string, int32_t>;
using SceneIndexList = std::map<guid_t, SceneIndexPair>;

} // namespace ark
