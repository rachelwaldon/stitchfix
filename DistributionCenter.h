#ifndef DISTRIBUTIONCENTER_H
#define DISTRIBUTIONCENTER_H

#include <sst/core/component.h>
#include <sst/core/link.h>
#include <sst/core/interfaces/stringEvent.h>
#include "Order.h"
#include "Vector.h"

#include <queue>
#include <vector>
#include <memory>
#include <cstddef>

namespace SST::StitchFix {
        
class DistributionCenter final : public SST::Component {
public:
    SST_ELI_REGISTER_COMPONENT(
        DistributionCenter,
        "StitchFix",
        "DistributionCenter",
        SST_ELI_ELEMENT_VERSION(1,0,0),
        "Distribution Center receives and fulfills new orders and sends out to customers",
        COMPONENT_CATEGORY_UNCATEGORIZED
    );
	    
    SST_ELI_DOCUMENT_PARAMS(
        { "num_blazers", "number of blazers in inventory", nullptr },
        { "num_pants", "number of pants in inventory", nullptr },
        { "num_dresses", "number of dresses in inventory", nullptr }
    );

    SST_ELI_DOCUMENT_PORTS(
        { "orders_port_in", "port on which orders are received", {"StitichFix.Order"} },
        { "orders_port_out", "port to send orders to transport", {"StitchFix.Vector"} },
        { "self_port", "port for scheduling events for the Distribution Center", {"sst.Interfaces.StringEvent"} },
        { "inventory_port_in", "port to receive inventory from the Warehouse", {"StitchFix.DataEvent"} }
    );

    DistributionCenter(SST::ComponentId_t id, SST::Params& params);
    ~DistributionCenter() {}

    void setup() override;
    void printInventory() const;
    void printQueue(std::queue<Order*> q) const;
    void receiveOrder(SST::Event*);
    void getInventory(SST::Event*);
    void fillOrder(SST::Event*);

private:
    int numBlazers, numPants, numDresses;
    std::queue<Order*> q;

    SST::Link* ordersPortOut {};
    SST::Link* selfPort {};
    std::unique_ptr<SST::Output> out {};
    Vector* ordersPtr {};
    std::vector<Order> orders;
};
    
}

#endif
