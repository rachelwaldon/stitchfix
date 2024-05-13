# stitchfix
Simulates the supply chain process for the popular styling service company Stitch Fix.
* Customers order from a list of clothes where a choice is simulated with a random number generator.
* The order is fulfilled through the Warehouse and delivered back to the customer through a transport chain.
* C++ code uses a strategy pattern to distinguish Customers as influencers or followers. Influencers make recommendations to other customers.

### Dependencies
* C++ components are made with the [Structural Simulation Toolkit](https://sst-simulator.org/)
* stitichfix.py uses [ahp_graph](https://github.com/lpsmodsimteam/ahp_graph) to set up the supply chain graph

### Sample run
$ sst --model-options --regions=500 --print-timing-info stitchfix.py
* stitchfix.py is the python script with the ahp_graph code
* regions is the number of regions in the graph
* each region is a device node that expands to 22 SST components.




