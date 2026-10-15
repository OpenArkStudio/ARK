#include "ark/container/queue.hpp"

namespace ark::test {

void compile_queue_operations() {
    queue<int> values;
    values.push(1);
    int value = 0;
    values.pop(value);
    values.count();
}

} // namespace ark::test
