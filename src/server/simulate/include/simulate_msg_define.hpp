#pragma once

#include <list>

#include "msgpack/include/msgpack.hpp"
#include "simulate_define.hpp"

namespace simulate {

enum class simulate_msg_type : uint16_t
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

class simulate_vec3
{
public:
    simulate_vec3() {}

    simulate_vec3(float position_x, float position_y, float position_z)
        : x(position_x)
        , y(position_y)
        , z(position_z) {}

    float x{0.0F};
    float y{0.0F};
    float z{0.0F};

public:
    MSGPACK_DEFINE(x, y, z);
};

///create scene
class req_create_scene //SIMULATE_MSG_REQ_CREATE_SCENE
{
public:
    std::string scene_id{""};
    int32_t index{0};

public:
    MSGPACK_DEFINE(scene_id, index);
};

///destroy scene
class req_destroy_scene //SIMULATE_MSG_REQ_DESTROY_SCENE
{
public:
    std::string scene_id{""};
    int32_t index{0};

public:
    MSGPACK_DEFINE(scene_id, index);
};

//create dynamic actor
class req_create_dynamic_box //SIMULATE_MSG_REQ_CREATE_DYNAMIC_BOX
{
public:
    std::string scene_id{""};
    int32_t index{0};
    int64_t obj_id{0};
    simulate_vec3 position{simulate_vec3()};
    simulate_vec3 size{simulate_vec3()};

public:
    MSGPACK_DEFINE(scene_id, index, obj_id, position, size);
};

class req_create_dynamic_capsule //SIMULATE_MSG_REQ_CREATE_DYNAMIC_CAPSULE
{
public:
    std::string scene_id{""};
    int32_t index{0};
    int64_t obj_id{0};
    simulate_vec3 position{simulate_vec3()};
    float raidus{0.0F};
    float height{0.0F};

public:
    MSGPACK_DEFINE(scene_id, index, obj_id, position, raidus, height);
};

class req_create_dynamic_sphere //SIMULATE_MSG_REQ_CREATE_DYNAMIC_SPHERE
{
public:
    std::string scene_id{""};
    int32_t index{0};
    int64_t obj_id{0};
    simulate_vec3 position{simulate_vec3()};
    float raidus{0.0F};

public:
    MSGPACK_DEFINE(scene_id, index, obj_id, position, raidus);
};

//create static actor
class req_create_static_box //SIMULATE_MSG_REQ_CREATE_STATIC_BOX
{
public:
    std::string scene_id{""};
    int32_t index{0};
    int64_t obj_id{0};
    simulate_vec3 position{simulate_vec3()};
    simulate_vec3 size{simulate_vec3()};

public:
    MSGPACK_DEFINE(scene_id, index, obj_id, position, size);
};

class req_create_static_capsule //SIMULATE_MSG_REQ_CREATE_STATIC_CAPSULE
{
public:
    std::string scene_id{""};
    int32_t index{0};
    int64_t obj_id{0};
    simulate_vec3 position{simulate_vec3()};
    float raidus{0.0F};
    float height{0.0F};

public:
    MSGPACK_DEFINE(scene_id, index, obj_id, position, raidus, height);
};

class req_create_static_sphere //SIMULATE_MSG_REQ_CREATE_STATIC_SPHERE
{
public:
    std::string scene_id{""};
    int32_t index{0};
    int64_t obj_id{0};
    simulate_vec3 position{simulate_vec3()};
    float raidus{0.0F};

public:
    MSGPACK_DEFINE(scene_id, index, obj_id, position, raidus);
};

///update actor position
class req_update_actor //SIMULATE_MSG_REQ_UPDATE_ACTOR
{
public:
    std::string scene_id{""};
    int32_t index{0};
    int64_t obj_id{0};
    simulate_vec3 position{simulate_vec3()};

public:
    MSGPACK_DEFINE(scene_id, index, obj_id, position);
};

///destroy actor
class req_destroy_actor //SIMULATE_MSG_REQ_DESTROY_ACTOR
{
public:
    std::string scene_id{""};
    int32_t index{0};
    int64_t obj_id{0};

public:
    MSGPACK_DEFINE(scene_id, index, obj_id);
};

///ray cast
class req_ray_cast //SIMULATE_MSG_REQ_RAY_CAST
{
public:
    std::string scene_id{""};
    int32_t index{0};
    simulate_vec3 start{simulate_vec3()};
    simulate_vec3 direction{simulate_vec3()};
    float distance{0.0F};

public:
    MSGPACK_DEFINE(scene_id, index, start, direction, distance);
};

class ack_ray_cast //SIMULATE_MSG_ACK_RAY_CAST
{
public:
    std::string scene_id{""};
    int32_t index{0};
    bool is_hit{false};
    simulate_vec3 hit_pos{simulate_vec3()};
    int64_t hit_obj_id{0};

public:
    MSGPACK_DEFINE(scene_id, index, is_hit, hit_pos, hit_obj_id);
};

///trigger
class trigger_pair
{
public:
    int64_t trigger_obj_id{0}; //trigger object
    int64_t other_obj_id{0};   //other object

public:
    MSGPACK_DEFINE(trigger_obj_id, other_obj_id);
};

class ack_trigger //SIMULATE_MSG_ACK_TRIGGER
{
public:
    std::string scene_id{""};
    int32_t index{0};
    std::list<trigger_pair> pairs;

public:
    MSGPACK_DEFINE(scene_id, index, pairs);
};

} // namespace simulate
