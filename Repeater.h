#ifndef REPEATER_H
#define REPEATER_H

#include <sst/core/component.h>
#include <sst/core/link.h>

namespace SST::StitchFix {

class Repeater : public SST::Component {
public:
    SST_ELI_REGISTER_COMPONENT(
        Repeater,
        "StitchFix",
        "Repeater",
        SST_ELI_ELEMENT_VERSION(1,0,0),
        "The Repeater component passes message through it, represents the transportion of orders",
        COMPONENT_CATEGORY_UNCATEGORIZED
    );

    SST_ELI_DOCUMENT_PARAMS(
        { "component_latency", "the processing time for this component (in seconds)", nullptr }
    );

    SST_ELI_DOCUMENT_PORTS(
        { "port_in", "input port for transportation component", {"StitchFix.StaticArray"} },
        { "port_out", "output port for transportation component", {"StitchFix.StaticArray"} }
    );

    Repeater(ComponentId_t id, Params& params);
    ~Repeater() {}

    void handleEvent(SST::Event* ev);

private:
    SST::Link* outPort {};
    SST::Link* inPort {};
    int latency;
};

}

#endif