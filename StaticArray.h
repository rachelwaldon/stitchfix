#ifndef STATICARRAY_H
#define STATICARRAY_H

#include <sst/core/event.h>
#include <sst/core/serialization/serializable.h>
#include <array>
#include <cstddef>
#include <iostream>

namespace SST::StitchFix {

template <class T>
class StaticArray final : public SST::Event {
public:

    StaticArray() : SST::Event() {}
    ~StaticArray() {}
    // T& operator[](std::size_t i) { return arr[i]; }
    // const T& operator[](std::size_t i) const { return arr[i]; }
    constexpr std::size_t size() { return N; }
    void clear() { arr.fill(0); }

    void set(std::size_t i, T val) { arr[i] = val; }
    T get(std::size_t i ) { return arr[i]; }


    // void serialize_order(SST::Core::Serialization::serializer &ser) override {
    //     Event::serialize_order(ser);
    //     ser & arr;
    // }

    // ImplementSerializable(StaticArray);
    NotSerializable(StaticArray);

private:
    static constexpr std::size_t N = 20;
    std::array<T,N> arr;
};

}

// namespace SST::Core::Serialization {
//     template <>
//     class serialize<SST::StitchFix::StaticArray> {
//     public:
//         inline void operator()(SST::StitchFix::StaticArray &s, serializer& ser)
//         {
//             s.serialize_order(ser);
//         }
//     };
// }

#endif