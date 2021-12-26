using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;

namespace MongoDBQueryCache
{
    public class BinaryExpression
    {
        public enum ExpressionType { Simple, Compound }

        public Node LeftOperand { get; }
        public Node Operator { get; }
        public Node RightOperand { get; }
        public ExpressionType Type { get; }

        public BinaryExpression(Node leftOperand, Node @operator, Node rightOperand)
        {
            LeftOperand = leftOperand;
            Operator = @operator;
            RightOperand = rightOperand;

            Type = Operator.Value is "$and" or "$or" ? ExpressionType.Compound : ExpressionType.Simple;
        }

        public override string ToString()
        {
            return $"{LeftOperand} {Operator} {RightOperand}";
        }

        public static IEnumerable<BinaryExpression> BuildFromTree(Node rootNode)
        {
            Debug.Assert(rootNode.Value == "$match" && rootNode.Children.Length == 1);
            // BFS traversal
            var queue = new Queue<Node>();
            queue.Enqueue(rootNode.Children[0]);
            while (queue.Any())
            {
                var node = queue.Dequeue();
                for (var i = 0; i < node.Children.Length - 1; i++)
                {
                    var leftOperand = node.Children[i];
                    var rightOperand = node.Children[i + 1];
                    var expr = new BinaryExpression(leftOperand, node, rightOperand);
                    if (leftOperand.Children.Any()) queue.Enqueue(leftOperand);
                    if (i == (node.Children.Length - 2))    // prevent double enqueue
                    {
                        if (rightOperand.Children.Any()) queue.Enqueue(rightOperand);
                    }
                    yield return expr;
                }
            }
        }

        public bool IsEquivalentTo(BinaryExpression incomingExpression)
        {
            return LeftOperand.Value == incomingExpression.LeftOperand.Value &&
                   Operator.Value == incomingExpression.Operator.Value &&
                   RightOperand.Value == incomingExpression.RightOperand.Value;
        }
    }
}