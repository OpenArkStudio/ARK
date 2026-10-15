#pragma once

#include "ark/module.hpp"

namespace ark {

class kafka_module : public module
{
public:
    // single producer
    virtual bool InitProducer(std::string const& brokers, std::string const& topic) = 0;
    virtual int Produce(std::string const& topic, std::string const& payload, void* user_data) = 0;

    // producer pool
    //virtual bool InitProducerPool(size_t pool_size, std::string const& brokers, std::string const& topic) = 0;
};

} // namespace ark
