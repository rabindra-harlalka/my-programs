using System.Collections.Generic;

namespace MongoDBQueryCache
{
    public class Node
    {
        private readonly List<Node> _children = new();

        public int Id { get; }
        public string Value { get; }
        public Node Parent { get; }
        public Node[] Children => _children.ToArray();

        public Node(int id, string value, Node parent)
        {
            Id = id;
            Value = value;
            Parent = parent;
        }

        public Node AddChild(int id, string value)
        {
            var node = new Node(id, value, this);
            _children.Add(node);
            return node;
        }

        public override string ToString()
        {
            return $"[{Id}:{Value}]";
        }
    }
}