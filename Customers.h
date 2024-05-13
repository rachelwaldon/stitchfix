#ifndef CUSTOMERS_H
#define CUSTOMERS_H

#include <sst/core/component.h>
#include <sst/core/link.h>
#include <sst/core/interfaces/stringEvent.h>
#include "Order.h"

#include <memory>
#include <vector>
#include <random>

namespace SST::StitchFix {

class Customers;

class CustomersStrategy {
public:
    virtual ~CustomersStrategy() {}
    virtual void sendOrders(Customers& customers, SST::Event* event ) = 0;
    virtual void receiveOrders(Customers& customers, SST::Event* event) = 0;
};

class InfluencerStrategy : public CustomersStrategy {
public:
    virtual ~InfluencerStrategy() {}
    void sendOrders(Customers& customers, SST::Event* event );
    void receiveOrders(Customers& customers, SST::Event* event);
};

class FollowerStrategy : public CustomersStrategy {
public:
    virtual ~FollowerStrategy() {}
    void sendOrders(Customers& customers, SST::Event* event );
    void receiveOrders(Customers& customers, SST::Event* event);
};
        
class Customers final : public SST::Component {
public:
    SST_ELI_REGISTER_COMPONENT(
        Customers,
        "StitchFix",
        "Customers",
        SST_ELI_ELEMENT_VERSION(1,0,0),
        "Component that generates orders",
        COMPONENT_CATEGORY_UNCATEGORIZED
    );

    SST_ELI_DOCUMENT_PARAMS(
        { "orders_interval", "the time (in hours) between two sets of orders", nullptr }
    );

    SST_ELI_DOCUMENT_PORTS(
        { "orders_port_in", "port to receive orders from Distribution Center", {"StitchFix.Vector"} },
        { "orders_port_out", "port to send orders to the Warehouse", {"StitchFix.Order"} },
        { "self_port", "port for scheduling events for Customers", {"sst.Interfaces.StringEvent"} },
        { "customers_port_right", "port to receive recommendations from neighbor Customers", {"StitchFix.Vector"} },
        { "customers_port_left", "port to send recommendations to neighboring Customers", {"StitchFix.Vector"} }
    );

    Customers(SST::ComponentId_t id, SST::Params& params);
    ~Customers() {}

    void setup() override;
    void printSummary();
    void printOrders(const std::vector<Order> &);
    void receiveOrders(SST::Event *ev);
    void sendOrders(SST::Event*);
    void respondToRecommendation(SST::Event*);
    Order* generateNewOrder();
    inline void exit() 
    { 
        printSummary();
        primaryComponentOKToEndSim(); 
    }

    SST::Link* ordersPortOut {};
    SST::Link* selfPort {};
    SST::Link* customersPortLeft {};
    SST::Link* customersPortRight {};

    int numCycles = 1;
    int sentOrders = 0;
    int receivedOrders = 0;
    int orderInterval = 0;
    bool receivedRec = false;
    std::string customerType {};

    std::unique_ptr<SST::Output> out {};
    std::unique_ptr<CustomersStrategy> strategy {};

    std::random_device rd;
    std::mt19937 mt;
    std::uniform_real_distribution<double> real_dist;
    std::uniform_int_distribution<int> order_type_dist;
    std::uniform_int_distribution<int> num_orders_dist;
};
    
}

#endif
