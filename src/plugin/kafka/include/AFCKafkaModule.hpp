#pragma once

#include "base/AFPluginManager.hpp"
#include "base/container/AFArrayMap.hpp"
#include "kafka/include/AFKafka.hpp"
#include "kafka/interface/AFIKafkaModule.hpp"

namespace ark {

class AFCKafkaModule
    : public AFIKafkaModule
    , public RdKafka::DeliveryReportCb
    , public RdKafka::EventCb
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    bool Init() override;
    bool Update() override;

    bool InitProducer(std::string const& brokers, std::string const& topic) override;
    int Produce(std::string const& topic, std::string const& payload, void* user_data) override;

protected:
    void dr_cb(RdKafka::Message& message) override;
    void event_cb(RdKafka::Event& event) override;

private:
    AFArrayMap<std::string, AFKafkaProducer> producers_;
};

} // namespace ark