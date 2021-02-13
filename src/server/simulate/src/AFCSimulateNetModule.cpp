/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2020 ArkNX authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"),
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "simulate/include/AFCSimulateNetModule.hpp"

namespace ark {
//method realization

bool AFCSimulateNetModule::Init()
{
    m_pSimulateModule = FindModule<AFISimulateModule>();
    m_pBusModule = FindModule<AFIBusModule>();
    m_pNetServiceManagerModule = FindModule<AFINetServiceManagerModule>();

    m_pSimulateModule->RegisterTriggerCallBack(this, &AFCSimulateNetModule::TriggerCallBack);

    return true;
}

bool AFCSimulateNetModule::PostInit()
{
    StartServer();

    return true;
}

int AFCSimulateNetModule::StartServer()
{
    auto ret = m_pNetServiceManagerModule->CreateServer();
    //ret.Then([=](const std::pair<bool, std::string>& resp) {
    //    if (!resp.first)
    //    {
    //        ARK_LOG_ERROR(
    //            "Cannot start server net, busid = {}, error = {}", m_pBusModule->GetSelfBusName(), resp.second);
    //        ARK_ASSERT_NO_EFFECT(0);
    //        exit(0);
    //    }

    //    m_pNetServerService = m_pNetServiceManagerModule->GetSelfNetServer();
    //    if (m_pNetServerService == nullptr)
    //    {
    //        ARK_LOG_ERROR("Cannot find server net, busid = {}", m_pBusModule->GetSelfBusName());
    //        exit(0);
    //    }

    //    m_pNetServerService->RegMsgCallback(0, this, &AFCSimulateNetModule::OnRecvMsg);
    //});
    m_pNetServerService = m_pNetServiceManagerModule->GetInterNetServer();
    if (m_pNetServerService == nullptr)
    {
        ARK_LOG_ERROR("Cannot find server net, busid = {}", m_pBusModule->GetSelfBusName());
        exit(0);
    }

    using namespace simulate;
    m_pNetServerService->RegMsgCallback(
        (int)SimulateMsgType::SIMULATE_MSG_REQ_CREATE_SCENE, this, &AFCSimulateNetModule::OnRecvMsg);
    m_pNetServerService->RegMsgCallback(
        (int)SimulateMsgType::SIMULATE_MSG_REQ_DESTROY_SCENE, this, &AFCSimulateNetModule::OnRecvMsg);
    m_pNetServerService->RegMsgCallback(
        (int)SimulateMsgType::SIMULATE_MSG_REQ_CREATE_DYNAMIC_BOX, this, &AFCSimulateNetModule::OnRecvMsg);
    m_pNetServerService->RegMsgCallback(
        (int)SimulateMsgType::SIMULATE_MSG_REQ_CREATE_DYNAMIC_CAPSULE, this, &AFCSimulateNetModule::OnRecvMsg);
    m_pNetServerService->RegMsgCallback(
        (int)SimulateMsgType::SIMULATE_MSG_REQ_CREATE_DYNAMIC_SPHERE, this, &AFCSimulateNetModule::OnRecvMsg);
    m_pNetServerService->RegMsgCallback(
        (int)SimulateMsgType::SIMULATE_MSG_REQ_CREATE_STATIC_BOX, this, &AFCSimulateNetModule::OnRecvMsg);
    m_pNetServerService->RegMsgCallback(
        (int)SimulateMsgType::SIMULATE_MSG_REQ_CREATE_STATIC_CAPSULE, this, &AFCSimulateNetModule::OnRecvMsg);
    m_pNetServerService->RegMsgCallback(
        (int)SimulateMsgType::SIMULATE_MSG_REQ_CREATE_STATIC_SPHERE, this, &AFCSimulateNetModule::OnRecvMsg);
    m_pNetServerService->RegMsgCallback(
        (int)SimulateMsgType::SIMULATE_MSG_REQ_UPDATE_ACTOR, this, &AFCSimulateNetModule::OnRecvMsg);
    m_pNetServerService->RegMsgCallback(
        (int)SimulateMsgType::SIMULATE_MSG_REQ_DESTROY_ACTOR, this, &AFCSimulateNetModule::OnRecvMsg);
    m_pNetServerService->RegMsgCallback(
        (int)SimulateMsgType::SIMULATE_MSG_REQ_RAY_CAST, this, &AFCSimulateNetModule::OnRecvMsg);

    return 0;
}

void AFCSimulateNetModule::OnRecvMsg(const AFNetMsg* msg, conv_id_t session_id)
{
    msgpack::object_handle obj_handle;
    msgpack::unpack(obj_handle, msg->GetMsgData(), msg->GetMsgLength());
    auto unpack_obj = obj_handle.get();
    if (unpack_obj.is_nil())
    {
        ARK_LOG_ERROR("unpack failed, busid = {} msg id = {}", m_pBusModule->GetSelfBusName(), msg->GetMsgId());
        return;
    }

    using namespace simulate;
    bool bResult = false;
    auto msg_id = (SimulateMsgType)msg->GetMsgId();
    switch (msg_id)
    {
    case SimulateMsgType::SIMULATE_MSG_REQ_CREATE_SCENE:
    {
        auto obj = unpack_obj.as<ReqCreateScene>();
        UpdateBusID(obj.scene_id, obj.index, msg->GetSrcBus());

        bResult = m_pSimulateModule->CreateScene(obj.scene_id, obj.index);
    }
    break;
    case SimulateMsgType::SIMULATE_MSG_REQ_DESTROY_SCENE:
    {
        auto obj = unpack_obj.as<ReqDestroyScene>();
        UpdateBusID(obj.scene_id, obj.index, msg->GetSrcBus());

        bResult = m_pSimulateModule->DestroyScene(obj.scene_id, obj.index);
    }
    break;
    case SimulateMsgType::SIMULATE_MSG_REQ_CREATE_DYNAMIC_BOX:
    {
        auto obj = unpack_obj.as<ReqCreateDynamicBox>();
        UpdateBusID(obj.scene_id, obj.index, msg->GetSrcBus());

        auto position = SimulateVec3ToAFVec(obj.position);
        auto size = SimulateVec3ToAFVec(obj.size);
        bResult = m_pSimulateModule->CreateBox(obj.scene_id, obj.index, obj.obj_id, position, size, true);
    }
    break;
    case SimulateMsgType::SIMULATE_MSG_REQ_CREATE_DYNAMIC_CAPSULE:
    {
        auto obj = unpack_obj.as<ReqCreateDynamicCapsule>();
        UpdateBusID(obj.scene_id, obj.index, msg->GetSrcBus());

        auto position = SimulateVec3ToAFVec(obj.position);
        bResult = m_pSimulateModule->CreateCapsule(
            obj.scene_id, obj.index, obj.obj_id, position, obj.raidus, obj.height, true);
    }
    break;
    case SimulateMsgType::SIMULATE_MSG_REQ_CREATE_DYNAMIC_SPHERE:
    {
        auto obj = unpack_obj.as<ReqCreateDynamicSphere>();
        UpdateBusID(obj.scene_id, obj.index, msg->GetSrcBus());

        auto position = SimulateVec3ToAFVec(obj.position);
        bResult = m_pSimulateModule->CreateSphere(obj.scene_id, obj.index, obj.obj_id, position, obj.raidus, true);
    }
    break;
    case SimulateMsgType::SIMULATE_MSG_REQ_CREATE_STATIC_BOX:
    {
        auto obj = unpack_obj.as<ReqCreateStaticBox>();
        UpdateBusID(obj.scene_id, obj.index, msg->GetSrcBus());

        auto position = SimulateVec3ToAFVec(obj.position);
        auto size = SimulateVec3ToAFVec(obj.size);
        bResult = m_pSimulateModule->CreateBox(obj.scene_id, obj.index, obj.obj_id, position, size, false);
    }
    break;
    case SimulateMsgType::SIMULATE_MSG_REQ_CREATE_STATIC_CAPSULE:
    {
        auto obj = unpack_obj.as<ReqCreateStaticCapsule>();
        UpdateBusID(obj.scene_id, obj.index, msg->GetSrcBus());

        auto position = SimulateVec3ToAFVec(obj.position);
        bResult = m_pSimulateModule->CreateCapsule(
            obj.scene_id, obj.index, obj.obj_id, position, obj.raidus, obj.height, false);
    }
    break;
    case SimulateMsgType::SIMULATE_MSG_REQ_CREATE_STATIC_SPHERE:
    {
        auto obj = unpack_obj.as<ReqCreateStaticSphere>();
        UpdateBusID(obj.scene_id, obj.index, msg->GetSrcBus());

        auto position = SimulateVec3ToAFVec(obj.position);
        bResult = m_pSimulateModule->CreateSphere(obj.scene_id, obj.index, obj.obj_id, position, obj.raidus, false);
    }
    break;
    case SimulateMsgType::SIMULATE_MSG_REQ_UPDATE_ACTOR:
    {
        auto obj = unpack_obj.as<ReqUpdateActor>();
        UpdateBusID(obj.scene_id, obj.index, msg->GetSrcBus());

        auto position = SimulateVec3ToAFVec(obj.position);
        bResult = m_pSimulateModule->UpdateActor(obj.scene_id, obj.index, obj.obj_id, position);
    }
    break;
    case SimulateMsgType::SIMULATE_MSG_REQ_DESTROY_ACTOR:
    {
        auto obj = unpack_obj.as<ReqDestroyActor>();
        UpdateBusID(obj.scene_id, obj.index, msg->GetSrcBus());

        bResult = m_pSimulateModule->DestroyActor(obj.scene_id, obj.index, obj.obj_id);
    }
    break;
    case SimulateMsgType::SIMULATE_MSG_REQ_RAY_CAST:
    {
        auto obj = unpack_obj.as<ReqRayCast>();
        UpdateBusID(obj.scene_id, obj.index, msg->GetSrcBus());

        auto start = SimulateVec3ToAFVec(obj.start);
        auto direction = SimulateVec3ToAFVec(obj.direction);

        AckRayCast ackRayCast;
        bResult = m_pSimulateModule->RayCast(obj.scene_id, obj.index, start, direction, obj.distance, ackRayCast);
        if (bResult)
        {
            SendAckRayCast(obj.scene_id, obj.index, ackRayCast);
        }
    }
    break;
    default:
    {
        ARK_LOG_ERROR("unknown msg id = {}", msg->GetMsgId());
    }
    break;
    }

    if (!bResult)
    {
        ARK_LOG_ERROR("deal message failed, busid = {} msg id = {}", m_pBusModule->GetSelfBusName(), msg->GetMsgId());
        return;
    }
}

AFPhysic3D AFCSimulateNetModule::SimulateVec3ToAFVec(const simulate::SimulateVec3& vec3)
{
    return AFPhysic3D(vec3.x, vec3.y, vec3.z);
}

void AFCSimulateNetModule::TriggerCallBack(
    const std::string& scene_id, const int32_t index, const simulate::TriggerPair& pair)
{
    simulate::AckTrigger trigger;
    trigger.scene_id = scene_id;
    trigger.index = index;
    trigger.pairs.push_back(pair);

    SendAckTrigger(scene_id, index, trigger);
}

void AFCSimulateNetModule::SendMsg(const std::string& scene_id, const int32_t index,
    const simulate::SimulateMsgType msg_id, const msgpack::sbuffer& buffer)
{
    int32_t target_bus_id;
    if (!GetBusID(scene_id, index, target_bus_id))
    {
        ARK_LOG_ERROR("get bus id failed scene id = {} index = {} msg id = {}", scene_id, index, (int)msg_id);
        return;
    }

    auto bus_id = m_pBusModule->GetSelfBusID();
    auto pNetService = m_pNetServiceManagerModule->GetInterNetServer();
    if (!pNetService)
    {
        ARK_LOG_ERROR("get net service failed scene id = {} index = {} msg id = {}", scene_id, index, (int)msg_id);
        return;
    }

    auto pNet = pNetService->GetNet();
    if (!pNet)
    {
        ARK_LOG_ERROR("get net failed scene id = {} index = {} msg id = {}", scene_id, index, (int)msg_id);
        return;
    }

    auto session_id = m_pNetServiceManagerModule->GetSessionID(target_bus_id);
    if (session_id == 0)
    {
        ARK_LOG_ERROR("get session id failed scene id = {} index = {} msg id = {}", scene_id, index, (int)msg_id);
        return;
    }

    AFSSMsgHeader head;
    head.SetMessageId((msg_id_t)msg_id);
    head.SetMessageLength(buffer.size());
    head.SetSourceBusId(bus_id);
    head.SetDestBusId(target_bus_id);

    pNet->SendMsg(&head, buffer.data(), session_id);
}

void AFCSimulateNetModule::SendAckRayCast(
    const std::string& scene_id, const int32_t index, const simulate::AckRayCast& ackMsg)
{
    msgpack::sbuffer buffer;
    msgpack::pack(buffer, ackMsg);

    SendMsg(scene_id, index, simulate::SimulateMsgType::SIMULATE_MSG_ACK_RAY_CAST, buffer);
}

void AFCSimulateNetModule::SendAckTrigger(
    const std::string& scene_id, const int32_t index, const simulate::AckTrigger& ackMsg)
{
    msgpack::sbuffer buffer;
    msgpack::pack(buffer, ackMsg);

    SendMsg(scene_id, index, simulate::SimulateMsgType::SIMULATE_MSG_ACK_TRIGGER, buffer);
}

void AFCSimulateNetModule::UpdateBusID(const std::string& scene_id, const int32_t index, const int32_t bus_id)
{
    auto iter = scene_index_bus_list_.find(scene_id);
    if (iter == scene_index_bus_list_.end())
    {
        IndexBusMap index_bus;
        iter = scene_index_bus_list_.insert(std::make_pair(scene_id, index_bus)).first;
        if (iter == scene_index_bus_list_.end())
        {
            return;
        }
    }

    auto& index_bus_map = iter->second;
    auto iter_index = index_bus_map.find(index);
    if (iter_index == index_bus_map.end())
    {
        index_bus_map.insert(std::make_pair(index, bus_id));
    }
    else
    {
        iter_index->second = bus_id;
    }
}

bool AFCSimulateNetModule::GetBusID(const std::string& scene_id, const int32_t index, int32_t& bus_id)
{
    auto iter = scene_index_bus_list_.find(scene_id);
    if (iter == scene_index_bus_list_.end())
    {
        return false;
    }

    auto& index_bus_map = iter->second;
    auto iter_index = index_bus_map.find(index);
    if (iter_index == index_bus_map.end())
    {
        return false;
    }

    bus_id = iter_index->second;
    return true;
}

} // namespace ark
