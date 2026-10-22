#include <type_traits>

#include <ark/mysql/mysql_module.hpp>
#include <ark/redis/redis_module.hpp>
#include <ark/utility/timer_module.hpp>

static_assert(std::is_abstract_v<ark::mysql_module>);
static_assert(std::is_abstract_v<ark::redis_module>);

int main() {
    return 0;
}
