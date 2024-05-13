#ifndef VECTOR_H
#define VECTOR_H


#include <sst/core/event.h>
#include <sst/core/serialization/serializable.h>
#include <vector>
#include "Order.h"

namespace SST::StitchFix { 

class Vector : public SST::Event {
public:
    Vector() : SST::Event() {}
    Vector(std::vector<Order> _orders) : SST::Event(), orders(_orders) {}
    std::vector<Order> getOrders() { return orders; }

    void serialize_order(SST::Core::Serialization::serializer &ser) override {
        Event::serialize_order(ser);
        ser & orders;
    }

    ImplementSerializable(SST::StitchFix::Vector);
    
private:
    std::vector<Order> orders;
};

}

#endif