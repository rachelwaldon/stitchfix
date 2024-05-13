#include "sst_config.h" 
#include "Warehouse.h"
#include "Constants.h"
#include "DataEvent.h"

#include <string>
#include <iostream>

using SST::Interfaces::StringEvent;
using std::to_string;
using std::string;
using namespace SST::StitchFix::Constants;

namespace SST::StitchFix {

Warehouse::Warehouse(ComponentId_t id, Params& params) :
    Component{ id },
    portCount{ params.find<uint64_t>("num_ports") },
    numBlazers{ params.find<int>("num_blazers") },
    numPants{ params.find<int>("num_pants") },
    numDresses{ params.find<int>("num_dresses") }
{
    for (uint64_t i=0; i != portCount; i++)
    {
        string portName = string("inventory_port_out.p") + to_string(i);
        if (!isPortConnected(portName)) throw("Missing required port" + portName);
        links[portName] = configureLink(portName);
    }

    selfPort = configureSelfLink("self_port", new Event::Handler<Warehouse>(this, &Warehouse::sendInventory));
    registerTimeBase("1s");
}

void Warehouse::setup()
{
    selfPort->send(new StringEvent(""));
}

void Warehouse::sendInventory(SST::Event* event)
{
    if (event)
    {
        for ( auto& [linkName, port] : links )
        {
            auto* inv = new DataEvent<int>(20,20,20);
            port->send(inv);
        }
        selfPort->send(SIX_HOURS,new StringEvent(""));
    }
    delete event;
}

}