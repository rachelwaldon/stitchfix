#ifndef ORDER_H
#define ORDER_H

#include <sst/core/event.h>
#include <string>

namespace SST::StitchFix {

    namespace ClothingType {
        enum Type { blazer, pants, dress };
    }

    typedef ClothingType::Type Garment;

    class Order final : public SST::Event {
    public:
        Order() : SST::Event() {}
        Order(int _id) : SST::Event(), id{_id} {}
        inline static int order_id = 0;
        int id;
        bool is_fulfilled;
        Garment order;
        bool is_return = false;

        void serialize_order(SST::Core::Serialization::serializer &ser) override {
            Event::serialize_order(ser);
            ser & id;
            ser & is_fulfilled;
            ser & order;
            ser & is_return;
        }

        ImplementSerializable(SST::StitchFix::Order);
    };
}

#endif
