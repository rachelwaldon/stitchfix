#include "sst_config.h"
#include "Repeater.h"
#include "Order.h"

namespace SST::StitchFix {

Repeater::Repeater(ComponentId_t id, Params& params) : 
    Component{ id },
    latency{ params.find<int>("component_latency") }
{ 
    registerTimeBase("1s");
    inPort = configureLink("port_in", new Event::Handler<Repeater>(this, &Repeater::handleEvent));
    outPort = configureLink("port_out");
}

void Repeater::handleEvent(SST::Event* ev)
{
    if (ev) outPort->send(latency,ev);
}

}