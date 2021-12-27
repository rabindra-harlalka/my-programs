using System.Collections.Generic;
using System.Linq;
using System.Text;
using MongoDB.Bson;

namespace MongoDBQueryCache
{
    public class Query
    {
        public Tree ExpressionTree { get; }
        public HashSet<string> ProjectedAttributes { get; } = new ();

        public BsonDocument MatchBsonDocument { get; private set; }
        public BsonDocument ProjectBsonDocument { get; private set; }

        public Query(Tree expressionTree) => ExpressionTree = expressionTree;
        public void SetMatchDoc(BsonDocument doc) => MatchBsonDocument = doc;
        public void SetProjectDoc(BsonDocument doc) => ProjectBsonDocument = doc;

        public bool Covers(Query incomingQuery)
        {
            // if this query doesn't have all the attributes projected that incoming query needs, then it's already NO
            if (!incomingQuery.ProjectedAttributes.Intersect(ProjectedAttributes)
                .SequenceEqual(incomingQuery.ProjectedAttributes))
            {
                return false;
            }

            var myExpressions = BinaryExpression.BuildFromTree(ExpressionTree.Root).ToList();
            var incomingExpressions = BinaryExpression.BuildFromTree(incomingQuery.ExpressionTree.Root).ToList();
            return Covers(myExpressions, incomingExpressions);
        }

        private static bool Covers(IReadOnlyCollection<BinaryExpression> myExpressions,
            IReadOnlyCollection<BinaryExpression> incomingExpressions)
        {
            foreach (var incomingExpression in incomingExpressions.Where(_ => _.Type is BinaryExpression.ExpressionType.Simple))
            {
                var foundExpr = myExpressions.FirstOrDefault(_ => _.IsEquivalentTo(incomingExpression));
                if (foundExpr == null) return false;
                // walk up the incomingExpression and foundExpr and see if the equivalency is maintained
                var myParent = foundExpr.Operator.Parent;
                var incomingParent = incomingExpression.Operator.Parent;
                while (myParent != null && incomingParent != null)
                {
                    if (myParent.Value != incomingParent.Value && myParent.Value == "$and") return false;
                    if (myParent.Value != incomingParent.Value && myParent.Children.Length != incomingParent.Children.Length &&
                        (myParent.Value == "$and" || incomingParent.Value == "$and")) return false;

                    myParent = myParent.Parent;
                    incomingParent = incomingParent.Parent;
                }
            }
            return true;
        }

        public override string ToString()
        {
            var stringBuilder = new StringBuilder();
            stringBuilder.AppendLine(ExpressionTree.ToString());
            foreach (var attribute in ProjectedAttributes)
            {
                stringBuilder.AppendLine(attribute);
            }

            return stringBuilder.ToString();
        }
    }
}