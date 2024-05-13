#include "sst_config.h"
#include "Customers.h"
#include "Order.h"
#include "Constants.h"
#include "Vector.h"

#include <cstdlib>
#include <string>
#include <iostream>

using SST::Interfaces::StringEvent;
using std::vector;
using std::size_t;
using std::string;
using namespace SST::StitchFix::Constants;

namespace SST::StitchFix {

Customers::Customers(SST::ComponentId_t id, SST::Params& params) :
    Component{ id },
    orderInterval{ params.find<int>("orders_interval")*60*60 },
    out { std::make_unique<SST::Output>("", 1, 0, Output::STDOUT) },
    mt { rd() },
    real_dist {0,1.0},
    order_type_dist {0,2},
    num_orders_dist {1,10}
{
    registerAsPrimaryComponent();
    primaryComponentDoNotEndSim();

    configureLink("orders_port_in", new Event::Handler<Customers>(this, &Customers::receiveOrders));
    ordersPortOut = configureLink("orders_port_out");
    selfPort = configureSelfLink("self_port", new Event::Handler<Customers>(this, &Customers::sendOrders));
    customersPortRight = configureLink("customers_port_right", new Event::Handler<Customers>(this, &Customers::respondToRecommendation));
    customersPortLeft = configureLink("customers_port_left");

    if (real_dist(mt) < 0.5) 
    {
        customerType = "influencer";
        strategy = std::make_unique<InfluencerStrategy>();
    } 
    else
    {
        customerType = "follower";
        strategy = std::make_unique<FollowerStrategy>();
    }
    out->output("%s , %s component", getName().c_str(), customerType.c_str());

    registerTimeBase("1s");
}

void Customers::setup()
{
    if ( customerType == "influencer")
    {
        // schedule the first set of orders to go out
        selfPort->send(orderInterval,new StringEvent("startorders"));
    }
    else  // follower
    {
        // sleep for a long time
        selfPort->send(691200, new StringEvent("wakeup"));
    }
}

Order* Customers::generateNewOrder()
{
    Order* newOrder = new Order(Order::order_id++);

    switch (order_type_dist(mt))
    {
        case 0:
            newOrder->order = Garment::blazer;
            break;
        case 1:
            newOrder->order = Garment::pants;
            break;
        case 2:
            newOrder->order = Garment::dress;
            break;
    }

    std::cout << "New order, id = " << newOrder->id << " , order type is " << ENUM_STRINGS[newOrder->order] << '\n';
    return newOrder;
}

void Customers::printOrders(const std::vector<Order> &orders)
{
    for (auto & element : orders)
    {
        out->output("%s Customer received order with id = %d\n\n", getName().c_str(), element.id);
        receivedOrders++;
    }
}

void Customers::printSummary()
{
    out->output(R"(
        COMPONENT : %s
        CURRENT TIME : %d hrs
        ELAPSED SIM TIME : %s
        SENT ORDERS : %d
        RECEIVED ORDERS : %d
        )", 
        getName().c_str(),
        getCurrentSimTime(TICK_FREQ),
        getElapsedSimTime().toString().c_str(),
        sentOrders,
        receivedOrders
    );
}

void Customers::sendOrders(SST::Event* ev)
{
    if (ev)
    {
        auto msg = dynamic_cast<StringEvent*> (ev);
        if (msg->getString() == "startorders")
        {
            if (strategy) strategy->sendOrders(*this,ev);
        }
        else if (!receivedRec)
        {
            exit();
        }
    }
}

void InfluencerStrategy::sendOrders(Customers& customers, SST::Event* message)
{
    if (customers.numCycles > 0)
    {
        int numOrders = customers.num_orders_dist(customers.mt);
        customers.sentOrders += numOrders;

        for (int i = 0; i < numOrders; i++) customers.ordersPortOut->send(customers.generateNewOrder());
        
        customers.numCycles--;
        customers.selfPort->send(customers.orderInterval,new StringEvent("startorders"));
    }
    delete message;
}

void FollowerStrategy::sendOrders(Customers& customers, SST::Event* _orders)
{
    auto ordersPtr = dynamic_cast<Vector*> (_orders);

    if (ordersPtr)
    {
        std::vector<Order> orders = ordersPtr->getOrders();
        for (const auto &o : orders) 
        {
            Order* newOrder = new Order(Order::order_id++);
            newOrder->order = o.order;
            customers.ordersPortOut->send(newOrder);
        } 
        customers.sentOrders += static_cast<int>(orders.size());
    }
    delete _orders;
}

void Customers::receiveOrders(SST::Event* ev)
{
    if (strategy) strategy->receiveOrders(*this,ev);
}

void InfluencerStrategy::receiveOrders(Customers& customers, SST::Event* ev)
{
    if (ev)
    {
        auto ordersPtr = dynamic_cast<Vector*> (ev);
        std::vector orders = ordersPtr->getOrders();  
        customers.printOrders(orders);
        customers.customersPortLeft->send(ordersPtr);
        if (customers.receivedOrders == customers.sentOrders) customers.exit();
    }
}

void FollowerStrategy::receiveOrders(Customers& customers, SST::Event* ev) 
{
    if (ev)
    {
        customers.receivedRec = true;
        auto ordersPtr = dynamic_cast<Vector*> (ev);
        std::vector orders = ordersPtr->getOrders();
        customers.printOrders(orders);
        if (customers.receivedOrders == customers.sentOrders) customers.exit();
        delete ev;
    }
}

void Customers::respondToRecommendation(SST::Event* orders)
{
    if (strategy) strategy->sendOrders(*this,orders);
}

}
