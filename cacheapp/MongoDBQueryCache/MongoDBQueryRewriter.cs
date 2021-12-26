using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using MongoDB.Bson;
using MongoDB.Bson.Serialization;

namespace MongoDBQueryCache
{
    public class MongoDBQueryParser
    {
        private const string _matchNodeName = "$match";
        private const string _projectNodeName = "$project";
        private const string _andNodeName = "$and";
        private const string _orNodeName = "$or";
        private const string _regexNodeName = "$regex";

        public Query ParseQuery(string query)
        {
            var documents = BsonSerializer.Deserialize<List<BsonDocument>>(query);
            if (documents == null) throw new ArgumentException("Failed to parse query.");

            var matchDoc = documents.Find(_ => _.ElementAt(0).Name == _matchNodeName);
            var projectDoc = documents.Find(_ => _.ElementAt(0).Name == _projectNodeName);

            if (matchDoc == null) throw new ArgumentException("Query doesn't have match filter.");
            var match = matchDoc.ElementAt(0);

            var queryParsed = new Query(new Tree(match.Name));
            queryParsed.SetMatchDoc(matchDoc);
            queryParsed.SetProjectDoc(projectDoc);

            ParseMatchJson(match, queryParsed);

            if (projectDoc == null) return queryParsed;
            var project = projectDoc.ElementAt(0);

            Trace.WriteLine($"Project {project.ToBsonDocument().ElementCount} fields");
            foreach (var attr in project.Value.AsBsonDocument.Elements.Select(element => element.Name))
            {
                queryParsed.ProjectedAttributes.Add(attr);
            }

            return queryParsed;
        }

        private static void ParseMatchJson(BsonElement match, Query queryParsed)
        {
            Trace.WriteLine($"Match has {match.Value.AsBsonDocument.ElementCount} elements.");
            var node = queryParsed.ExpressionTree.Root;

            if (match.Value.AsBsonDocument.ElementCount == 1)
            {
                var matchElement = match.Value.AsBsonDocument.ElementAt(0);
                if (matchElement.Name is _andNodeName or _orNodeName)
                {
                    // compound and/or expression
                    BuildCompoundExpressionTree(matchElement, node, queryParsed.ExpressionTree);
                }
                else
                {
                    // single simple expression
                    BuildSimpleExpressionTree(matchElement, node, queryParsed.ExpressionTree);
                }
            }
            else
            {
                // collection of simple expressions; $and is implicit
                node = node.AddChild(queryParsed.ExpressionTree.NextNodeId, _andNodeName);
                foreach (var matchElement in match.Value.AsBsonDocument.Elements)
                {
                    BuildSimpleExpressionTree(matchElement, node, queryParsed.ExpressionTree);
                }
            }
        }

        private static void BuildSimpleExpressionTree(BsonElement element, Node subTree, Tree tree)
        {
            if (element.Value is BsonString)
            {
                var equalNode = subTree.AddChild(tree.NextNodeId, "=");
                equalNode.AddChild(tree.NextNodeId, element.Name);
                //equalNode.AddChild(tree.NextNodeId, $"'{element.Value}'");
                equalNode.AddChild(tree.NextNodeId, element.Value.ToString());
            }
            else if (element.Value is BsonRegularExpression)
            {
                var regexNode = subTree.AddChild(tree.NextNodeId, _regexNodeName);
                regexNode.AddChild(tree.NextNodeId, element.Name);
                //regexNode.AddChild(tree.NextNodeId, $"'{element.Value.AsBsonRegularExpression.Pattern}'");
                regexNode.AddChild(tree.NextNodeId, element.Value.AsBsonRegularExpression.Pattern);
            }
            else
            {
                var equalNode = subTree.AddChild(tree.NextNodeId, "=");
                equalNode.AddChild(tree.NextNodeId, element.Name);
                equalNode.AddChild(tree.NextNodeId, element.Value.ToString());
            }
        }

        private static void BuildCompoundExpressionTree(BsonElement element, Node subTree, Tree tree)
        {
            Trace.WriteLine($"Type of element is {element.Value.BsonType}");
            var conjunctionDocument = element.Value.AsBsonArray;

            subTree = subTree.AddChild(tree.NextNodeId, element.Name);

            foreach (var expression in conjunctionDocument!.Values.Cast<BsonDocument>())
            {
                var firstElement = expression.ElementAt(0);
                Trace.WriteLine($"expression is: {firstElement.Name}: {firstElement.Value}");
                Trace.WriteLine($"type of first element: {firstElement.Value.BsonType}");
                if (firstElement.Name is _andNodeName or _orNodeName)
                {
                    BuildCompoundExpressionTree(firstElement, subTree, tree);
                }
                else
                {
                    BuildSimpleExpressionTree(firstElement, subTree, tree);
                }
            }
        }
    }
}