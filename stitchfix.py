from ahp_graph.Device import Device, PortInfo
from ahp_graph.DeviceGraph import DeviceGraph
from ahp_graph.SSTGraph import SSTGraph


class Warehouse(Device):
    library = 'StitchFix.Warehouse'
    portinfo = PortInfo()

    def __init__(self, name, model_params):
        super().__init__(name)
        self.attr.update(model_params)
        Warehouse.portinfo.add('inventory_port_out', 'DataEvent', model_params['num_ports'], True, '.p#')



class Transport(Device):
    """The Transport class defines factory methods to make objects involved in transporting orders."""
    library = 'StitchFix.Repeater'
    id = 0

    portinfo = PortInfo()
    portinfo.add('port_in', 'Vector')
    portinfo.add('port_out', 'Vector')

    def __init__(self, name, component_latency, link_latency):
        Transport.id += 1
        super().__init__(f'{name}_{Transport.id}')
        self.attr['component_latency'] = component_latency*3600
        self._link_latency = f'{link_latency*3600}s'
        
    @property
    def link_latency(self):
        return self._link_latency

    @classmethod
    def truck(cls):
        return cls('Truck', 0, 8)   # <------- latencies are in hours. Converted to seconds for SST

    @classmethod
    def airport(cls):
        return cls('Airport', 5, 0)

    @classmethod
    def loading(cls):
        return cls('CargoLoading', 1, 0)

    @classmethod
    def airplane(cls):
        return cls('Airplane', 0, 4)



class Transportation(Device):
    """The Transportation class is an assembly for abstracting a transport chain into one Device node."""

    portinfo = PortInfo()
    portinfo.add('port_in', 'Vector')
    portinfo.add('port_out', 'Vector')

    def __init__(self, name, transports):
        super().__init__(name)
        self._transports = transports

    def expand(self, graph):
        for i,transport in enumerate(self._transports[:-1]):
            graph.link(transport.port_out, self._transports[i+1].port_in, transport.link_latency)

        graph.link(self.port_in, self._transports[0].port_in)
        graph.link(self.port_out, self._transports[-1].port_out)
        return graph



class Customers(Device):
    library = 'StitchFix.Customers'

    portinfo = PortInfo()
    portinfo.add('orders_port_in', 'Vector')
    portinfo.add('orders_port_out', 'Order')
    portinfo.add('customers_port_right', 'Vector')
    portinfo.add('customers_port_left', 'Vector')

    def __init__(self, name, orders_interval=1):
        super().__init__(name)
        self.attr['orders_interval'] = orders_interval



class DistributionCenter(Device):
    library = 'StitchFix.DistributionCenter'

    portinfo = PortInfo()
    portinfo.add('orders_port_out', 'Vector')
    portinfo.add('orders_port_in', 'Order')
    portinfo.add('inventory_port_in', 'DataEvent')

    def __init__(self, name, model_params):
        super().__init__(name)
        self.attr.update(model_params)



class Region(Device):

    portinfo = PortInfo()
    portinfo.add('port_in', 'DataEvent')
    portinfo.add('customers_port_right', 'Vector')
    portinfo.add('customers_port_left', 'Vector')

    def __init__(self, name, num_transports):
        super().__init__(name)
        self._num_transports = num_transports

    def expand(self, graph):
        dist_center_params = {
            'num_blazers': 10,
            'num_pants': 10,
            'num_dresses': 10
        }
        dist_center = DistributionCenter('DistributionCenter', dist_center_params)
        customers = Customers('Customers')

        transports = ()
        for i in range(self._num_transports // 5):
            transports += ( Transport.truck(), Transport.airport(), Transport.loading(), Transport.airplane(), Transport.truck() )
        transport_chain = Transportation('Transportation', transports)

        graph.link(self.port_in, dist_center.inventory_port_in)
        graph.link(self.customers_port_right, customers.customers_port_right)
        graph.link(self.customers_port_left, customers.customers_port_left)
        graph.link(customers.orders_port_out, dist_center.orders_port_in, '1s')
        graph.link(dist_center.orders_port_out, transport_chain.port_in)
        graph.link(transport_chain.port_out, customers.orders_port_in)



def build_supply_chain(num_regions, num_transports):
    graph = DeviceGraph()

    warehouse_params = {
        'num_ports': num_regions,
        'num_blazers': 200,
        'num_pants': 200,
        'num_dresses': 200
    }
    warehouse = Warehouse('Warehouse', warehouse_params)

    regions = [Region(f'region_{i}', num_transports) for i in range(num_regions)]

    for i in range(num_regions):
        graph.link(warehouse.inventory_port_out(i), regions[i].port_in)
        graph.link(regions[i-1].customers_port_right, regions[i].customers_port_left)

    return graph


if __name__ == '__main__':
    import argparse
    try:
        import sst
        SST = True
    except ImportError:
        SST = False

    parser = argparse.ArgumentParser(description='StitchFix')
    parser.add_argument('--regions', type=int, default=5)
    parser.add_argument('--transports', type=int, default=20)
    args = parser.parse_args()

    graph = build_supply_chain(args.regions,args.transports)
    sst_graph = SSTGraph(graph)


    if SST:
        sst_graph.build()

    #else:
    #    graph.verify_links()
    #    graph.write_dot('assembly_graph', True, True, True)
    #    graph.flatten()
    #    graph.verify_links()
    #    graph.write_dot('flat_graph', True, True, False)
    #    sst_graph.write_json('flat_graph')
