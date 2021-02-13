#pragma once

#include "AFSimulateDefine.hpp"
#include "msgpack/include/msgpack.hpp"
#include <list>

namespace simulate {

enum class SimulateMsgType : uint16_t
{
    SIMULATE_MSG_UNKNOWN = 0,

    ///req msg starts from 1
    SIMULATE_MSG_REQ_CREATE_SCENE = 1,
    SIMULATE_MSG_REQ_DESTROY_SCENE = 2,
    SIMULATE_MSG_REQ_CREATE_DYNAMIC_BOX = 3,
    SIMULATE_MSG_REQ_CREATE_DYNAMIC_CAPSULE = 4,
    SIMULATE_MSG_REQ_CREATE_DYNAMIC_SPHERE = 5,
    SIMULATE_MSG_REQ_CREATE_STATIC_BOX = 6,
    SIMULATE_MSG_REQ_CREATE_STATIC_CAPSULE = 7,
    SIMULATE_MSG_REQ_CREATE_STATIC_SPHERE = 8,
    SIMULATE_MSG_REQ_UPDATE_ACTOR = 9,
    SIMULATE_MSG_REQ_DESTROY_ACTOR = 10,
    SIMULATE_MSG_REQ_RAY_CAST = 11,

    ///ack msg starts from 1001
    SIMULATE_MSG_ACK_RAY_CAST = 1001,
    SIMULATE_MSG_ACK_TRIGGER = 1002,

    ///reg server
    SIMULATE_REG_SERVER = 65535,
};

class SimulateVec3
{
public:
    SimulateVec3() {}

    SimulateVec3(float position_x, float position_y, float position_z)
        : x(position_x)
        , y(position_y)
        , z(position_z)
    {
    }

    float x{0.0F};
    float y{0.0F};
    float z{0.0F};

public:
    MSGPACK_DEFINE(x, y, z);
};

///create scene
class ReqCreateScene //SIMULATE_MSG_REQ_CREATE_SCENE
{
public:
    std::string scene_id{""};
    int32_t index{0};

public:
    MSGPACK_DEFINE(scene_id, index);
};

///destroy scene
class ReqDestroyScene //SIMULATE_MSG_REQ_DESTROY_SCENE
{
public:
    std::string scene_id{""};
    int32_t index{0};

public:
    MSGPACK_DEFINE(scene_id, index);
};

//create dynamic actor
class ReqCreateDynamicBox //SIMULATE_MSG_REQ_CREATE_DYNAMIC_BOX
{
public:
    std::string scene_id{""};
    int32_t index{0};
    int64_t obj_id{0};
    SimulateVec3 position{SimulateVec3()};
    SimulateVec3 size{SimulateVec3()};

public:
    MSGPACK_DEFINE(scene_id, index, obj_id, position, size);
};

class ReqCreateDynamicCapsule //SIMULATE_MSG_REQ_CREATE_DYNAMIC_CAPSULE
{
public:
    std::string scene_id{""};
    int32_t index{0};
    int64_t obj_id{0};
    SimulateVec3 position{SimulateVec3()};
    float raidus{0.0F};
    float height{0.0F};

public:
    MSGPACK_DEFINE(scene_id, index, obj_id, position, raidus, height);
};

class ReqCreateDynamicSphere //SIMULATE_MSG_REQ_CREATE_DYNAMIC_SPHERE
{
public:
    std::string scene_id{""};
    int32_t index{0};
    int64_t obj_id{0};
    SimulateVec3 position{SimulateVec3()};
    float raidus{0.0F};

public:
    MSGPACK_DEFINE(scene_id, index, obj_id, position, raidus);
};

//create static actor
class ReqCreateStaticBox //SIMULATE_MSG_REQ_CREATE_STATIC_BOX
{
public:
    std::string scene_id{""};
    int32_t index{0};
    int64_t obj_id{0};
    SimulateVec3 position{SimulateVec3()};
    SimulateVec3 size{SimulateVec3()};

public:
    MSGPACK_DEFINE(scene_id, index, obj_id, position, size);
};

class ReqCreateStaticCapsule //SIMULATE_MSG_REQ_CREATE_STATIC_CAPSULE
{
public:
    std::string scene_id{""};
    int32_t index{0};
    int64_t obj_id{0};
    SimulateVec3 position{SimulateVec3()};
    float raidus{0.0F};
    float height{0.0F};

public:
    MSGPACK_DEFINE(scene_id, index, obj_id, position, raidus, height);
};

class ReqCreateStaticSphere //SIMULATE_MSG_REQ_CREATE_STATIC_SPHERE
{
public:
    std::string scene_id{""};
    int32_t index{0};
    int64_t obj_id{0};
    SimulateVec3 position{SimulateVec3()};
    float raidus{0.0F};

public:
    MSGPACK_DEFINE(scene_id, index, obj_id, position, raidus);
};

///update actor position
class ReqUpdateActor //SIMULATE_MSG_REQ_UPDATE_ACTOR
{
public:
    std::string scene_id{""};
    int32_t index{0};
    int64_t obj_id{0};
    SimulateVec3 position{SimulateVec3()};

public:
    MSGPACK_DEFINE(scene_id, index, obj_id, position);
};

///destroy actor
class ReqDestroyActor //SIMULATE_MSG_REQ_DESTROY_ACTOR
{
public:
    std::string scene_id{""};
    int32_t index{0};
    int64_t obj_id{0};

public:
    MSGPACK_DEFINE(scene_id, index, obj_id);
};

///ray cast
class ReqRayCast //SIMULATE_MSG_REQ_RAY_CAST
{
public:
    std::string scene_id{""};
    int32_t index{0};
    SimulateVec3 start{SimulateVec3()};
    SimulateVec3 direction{SimulateVec3()};
    float distance{0.0F};

public:
    MSGPACK_DEFINE(scene_id, index, start, direction, distance);
};

class AckRayCast //SIMULATE_MSG_ACK_RAY_CAST
{
public:
    std::string scene_id{""};
    int32_t index{0};
    bool is_hit{false};
    SimulateVec3 hit_pos{SimulateVec3()};
    int64_t hit_obj_id{0};

public:
    MSGPACK_DEFINE(scene_id, index, is_hit, hit_pos, hit_obj_id);
};

///trigger
class TriggerPair
{
public:
    int64_t trigger_obj_id{0}; //trigger object
    int64_t other_obj_id{0};   //other object

public:
    MSGPACK_DEFINE(trigger_obj_id, other_obj_id);
};

class AckTrigger //SIMULATE_MSG_ACK_TRIGGER
{
public:
    std::string scene_id{""};
    int32_t index{0};
    std::list<TriggerPair> pairs;

public:
    MSGPACK_DEFINE(scene_id, index, pairs);
};

} // namespace simulate
