#ifndef WAREHOUSE_H
#define WAREHOUSE_H

#include <sst/core/component.h>
#include <sst/core/link.h>
#include <sst/core/interfaces/stringEvent.h>

namespace SST::StitchFix {
        
class Warehouse final : public SST::Component {
public:
    SST_ELI_REGISTER_COMPONENT(
        Warehouse,
        "StitchFix",
        "Warehouse",
        SST_ELI_ELEMENT_VERSION(1,0,0),
        "The Warehouse stores inventory long-term and sends bulk inventory to Distribution Centers",
        COMPONENT_CATEGORY_UNCATEGORIZED
    );
	    
    SST_ELI_DOCUMENT_PARAMS(
        { "num_blazers", "number of blazers in inventory", nullptr },
        { "num_pants", "number of pants in inventory", nullptr },
        { "num_dresses", "number of dresses in inventory", nullptr },
        { "num_ports", "number of ports to be connected to this component", nullptr}
    );

    SST_ELI_DOCUMENT_PORTS(
        { "inventory_port_out.p%d", "port to send inventory to Distribution Centers", {"StitchFix.DataEvent"} },
        { "self_port", "port for scheduling events for the Warehouse itself", {"sst.Interfaces.StringEvent"} }
    );

    Warehouse(SST::ComponentId_t id, SST::Params& params);
    ~Warehouse() {}

    void setup() override;
    void sendInventory(SST::Event*);

private:
    int numBlazers, numPants, numDresses;

    SST::Link* selfPort {};
    std::unordered_map<std::string, SST::Link*> links;
    uint64_t portCount = 0;
};
    
}

#endif