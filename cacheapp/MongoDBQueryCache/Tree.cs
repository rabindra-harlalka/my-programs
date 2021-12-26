using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace MongoDBQueryCache
{
    public class Tree
    {
        private int _lastNodeId = 0;

        public Node Root { get; }
        public int NextNodeId => _lastNodeId++;
        
        public Tree(string rootValue)
        {
            Root = new Node(_lastNodeId++, rootValue, null);
        }

        public override string ToString()
        {
            var builder = new StringBuilder();

            // BFS traversal
            var queue = new Queue<Node>();
            queue.Enqueue(Root);
            while (queue.Any())
            {
                var node = queue.Dequeue();
                builder.Append($"{node} -> ");
                foreach (var childNode in node.Children)
                {
                    builder.Append($"{childNode};");
                    if (childNode.Children.Any()) queue.Enqueue(childNode);
                }
                builder.AppendLine();
            }

            return builder.ToString();
        }
    }
}