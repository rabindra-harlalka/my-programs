from typing import Callable
from queue import Queue

"""A type of depth-first walk of a tree (parent, left, right)"""
def pre_order_walk(node, result: list, left: Callable, right: Callable, parent: Callable):
    if node is not None:
        result.append(node)
        if left(node) is not None:
            pre_order_walk(left(node), result, left, right, parent)
        if right(node) is not None:
            pre_order_walk(right(node), result, left, right, parent)

"""A type of depth-first walk of a tree (left, parent, right)"""
def in_order_walk(node, result: list, left: Callable, right: Callable, parent: Callable):
    if node is not None:
        if left(node) is not None:
            in_order_walk(left(node), result, left, right, parent)
        result.append(node)
        if right(node) is not None:
            in_order_walk(right(node), result, left, right, parent)

"""A type of depth-first walk of a tree (left, right, parent)"""
def post_order_walk(node, result: list, left: Callable, right: Callable, parent: Callable):
    if node is not None:
        if left(node) is not None:
            post_order_walk(left(node), result, left, right, parent)
        if right(node) is not None:
            post_order_walk(right(node), result, left, right, parent)
        result.append(node)

def add_child(node, child_node):
    if not "left" in node:
        node["left"] = child_node
        child_node["parent"] = node
    elif not "right" in node:
        node["right"] = child_node
        child_node["parent"] = node
    else:
        raise Exception("parent node is full")

def is_full(node) -> bool:
    return "left" in node and "right" in node

def make_node(data):
    node = { "data" : data }
    return node

def make_tree(items: list):
    tree = []
    q = Queue()
    current_parent = q.get(block=False) if q.empty() is False else None
    for item in items:
        print('DEBUG: adding item %s' % item)
        node = make_node(item)
        q.put(node)
        tree.append(node)
        if current_parent is not None:
            if (is_full(current_parent)):
                current_parent = q.get(block=False)
            add_child(current_parent, node)
        else:
            current_parent = q.get(block=False)
    return tree

def print_tree(tree: list):
    for node in tree:
        parent = node["parent"]["data"] if "parent" in node else None
        left = node["left"]["data"] if "left" in node else None
        right = node["right"]["data"] if "right" in node else None
        print("%s <- %s: %s %s" % (parent, node["data"], left, right))

def print_tree_minimal(tree: list):
    for node in tree:
        print("%s" % node["data"], end=' ')
    print()

def main():
    tree = make_tree([25, 23, 22, 21, 12, 20, 17, 15, 16, 10, 9, 19, 18, 14, 7, 4, 13, 11])
    print_tree(tree)

    pre_order_walk_result = []
    pre_order_walk(tree[0], pre_order_walk_result,
        left=lambda node: node["left"] if "left" in node else None,
        right=lambda node: node["right"] if "right" in node else None,
        parent=lambda node: node["parent"] if "parent" in node else None)
    print_tree_minimal(pre_order_walk_result)

    in_order_walk_result = []
    in_order_walk(tree[0], in_order_walk_result,
        left=lambda node: node["left"] if "left" in node else None,
        right=lambda node: node["right"] if "right" in node else None,
        parent=lambda node: node["parent"] if "parent" in node else None)
    print_tree_minimal(in_order_walk_result)

    post_order_walk_result = []
    post_order_walk(tree[0], post_order_walk_result,
        left=lambda node: node["left"] if "left" in node else None,
        right=lambda node: node["right"] if "right" in node else None,
        parent=lambda node: node["parent"] if "parent" in node else None)
    print_tree_minimal(post_order_walk_result)

main()
