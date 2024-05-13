#include "sst_config.h" 
#include "DistributionCenter.h"
#include "Constants.h"
#include "DataEvent.h"

#include <string>

using SST::Interfaces::StringEvent;
using std::string;
using std::to_string;
using namespace SST::StitchFix::Constants;


namespace SST::StitchFix {

DistributionCenter::DistributionCenter(ComponentId_t id, Params& params) :
    Component{ id },
    numBlazers{ params.find<int>("num_blazers") },
    numPants{ params.find<int>("num_pants") },
    numDresses{ params.find<int>("num_dresses") },
    out { std::make_unique<SST::Output>("", 1, 0, Output::STDOUT) }
{
    configureLink("orders_port_in", new Event::Handler<DistributionCenter>(this, &DistributionCenter::receiveOrder));
    selfPort = configureSelfLink("self_port", new Event::Handler<DistributionCenter>(this, &DistributionCenter::fillOrder));
    ordersPortOut = configureLink("orders_port_out");
    configureLink("inventory_port_in", new Event::Handler<DistributionCenter>(this, &DistributionCenter::getInventory));

    printInventory();
    registerTimeBase("1s");
}

void DistributionCenter::setup()
{
    selfPort->send(ONE_SECOND,new StringEvent(""));  // let the first order arrive then check the queue
}

void DistributionCenter::receiveOrder(SST::Event* event)
{
    out->output("Adding order to queue\n");
    if (event)
    {
        std::size_t queueSize = q.size();
        Order* order = dynamic_cast<Order*>(event);
        q.push(order);
        if (!queueSize) selfPort->send(new StringEvent(""));
    }
}

void DistributionCenter::getInventory(SST::Event* event)
{
    if (event)
    {
        auto* inv_ptr = dynamic_cast<DataEvent<int>*>(event);
        DataEvent<int>& inv = *inv_ptr;
        numBlazers += inv.data1;
        numPants += inv.data2;
        numDresses += inv.data3;
        // printInventory();
    }
    delete event;
}

// Assume there is some constraint on the Distribution Center --> One order is filled per hour
void DistributionCenter::fillOrder(SST::Event* event)
{
    if (event)
    {
        if (!q.empty()) 
        {
            printQueue(q);
            auto orderPtr = q.front();
            Order& orderRef = *orderPtr;
            q.pop();

            out->output("\nIn %s filling order, order id = %d\n", getName().c_str(), orderRef.id);
            switch(orderRef.order)
            {
                case Garment::blazer:
                    numBlazers--;
                    break;
                case Garment::pants:
                    numPants--;
                    break;
                case Garment::dress:
                    numDresses--;
                    break;
            }

            orders.push_back(*orderPtr);

            if (orders.size() == 20 || q.empty())
            {
                std::cout << "dist center orders full sending to customer through transport\n";
                ordersPtr = new Vector(orders);
                orders.clear();
                ordersPortOut->send(ordersPtr);
            }
        }
        else
        {
            // out->output("%s Queue is empty\n", getName().c_str());
        }
       
        selfPort->send(ONE_HOUR,new StringEvent(""));
        delete event;
    }
}

//copy
void DistributionCenter::printQueue(std::queue<Order*> q) const
{
    out->output("%s Printing queue\n", getName().c_str());
    string orders;
    while (!q.empty())
    {
        orders += "Order# " + to_string(q.front()->id) + " ";
        q.pop();
    }
    out->output((orders + '\n').c_str());
}

void DistributionCenter::printInventory() const 
{
    out->output(R"(%s Current Inventory:
    Blazers = %d
    Pants = %d
    Dresses = %d
)", getName().c_str(), numBlazers, numPants, numDresses);
}

}
