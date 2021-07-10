class Node:
    def __init__(self, data):
        self.data = data
        self.next = None

    def __repr__(self):
        return str(self.data)


class LinkedList:
    def __init__(self, items = None):
        self.head = self.tail = None
        if (items is not None):
            for item in items:
                self._append_item(item)
    
    def _append_item(self, item):
        new_node = Node(item)
        if self.head is None:
            self.head = new_node
        else:
            self.tail.next = new_node
        self.tail = new_node

    def __repr__(self):
        nodes = []
        cur_node = self.head
        while cur_node is not None:
            nodes.append(cur_node)
            cur_node = cur_node.next
        return str(list(nodes))


def calc_network_size(adj_list, n, u):
    network_size = [-1] # -1 not 0 so that u is not counted as it's own connection/friend
    visited = [False] * (n + 1) # + 1 to make algo work with 1-based indexing
    # visit the nodes reachable from u depth-first
    visit_node(adj_list, visited, u, network_size)
    return network_size[0]

def visit_node(adj_list, visited, x, network_size):
    if visited[x]:
        return None
    network_size[0] += 1
    visited[x] = True
    node = adj_list[x].head
    while node is not None:
        visit_node(adj_list, visited, node.data, network_size)
        node = node.next

def main():
    print("network #1")
    adjacency_list = [
        LinkedList([]), # dummy to make algo work with 1-based indexing
        LinkedList([2, 3, 4]),
        LinkedList([1, 3, 4]),
        LinkedList([1, 2, 5]),
        LinkedList([1, 2]),
        LinkedList([3])
    ]
    for linked_list in adjacency_list:
        print(str(linked_list))

    print(calc_network_size(adjacency_list, 5, 3))
    print(calc_network_size(adjacency_list, 5, 5))

    print("network #2")
    adjacency_list = [
        LinkedList([]), # dummy to make algo work with 1-based indexing
        LinkedList([2, 3, 4]),
        LinkedList([1, 4]),
        LinkedList([1]),
        LinkedList([2, 1]),
        LinkedList([7, 6]),
        LinkedList([5, 7]),
        LinkedList([5, 6]),
        LinkedList([9]),
        LinkedList([8])
    ]

    for i in list(range(1, 10)):
        print(calc_network_size(adjacency_list, 9, i))

main()