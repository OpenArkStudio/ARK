#include "base/AFPlatform.hpp"

namespace ark {

template<typename T>
class AFThreadSafeQueue
{
private:
    std::queue<T> data_queue;
    std::mutex mutex_;

public:
    AFThreadSafeQueue() = default;

    size_t size()
    {
        std::lock_guard guard(mutex_);
        return data_queue.size();
    }

    void push(T new_value)
    {
        std::lock_guard guard(mutex_);
        data_queue.push(new_value);
    }
    bool try_pop(T& value)
    {
        std::lock_guard guard(mutex_);
        if (data_queue.empty())
        {
            return false;
        }

        value = data_queue.front();
        data_queue.pop();
        return true;
    }

    std::queue<T> grab_all()
    {
        std::queue<T> ret;
        {
            std::lock_guard guard(mutex_);
            std::swap(ret, data_queue);
        }
        return ret;
    }

    bool empty() const
    {
        std::lock_guard guard(mutex_);
        return data_queue.empty();
    }
};
} // namespace ark