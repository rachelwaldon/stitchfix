#ifndef DATAEVENT_H
#define DATAEVENT_H

#include <sst/core/event.h>

namespace SST::StitchFix {

template <typename T>
struct DataEvent : public SST::Event {
    
    explicit DataEvent(T _data1=T(), T _data2=T(), T _data3=T())
        : SST::Event(), data1(_data1), data2(_data2), data3(_data3) {}

    T data1, data2, data3;

    void serialize_order(SST::Core::Serialization::serializer &ser) override 
    {
        Event::serialize_order(ser);
        ser & data1;
        ser & data2;
        ser & data3;
    }

    ImplementSerializable(DataEvent);
};

}

#endif