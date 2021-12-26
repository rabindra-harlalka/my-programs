using System;
using NUnit.Framework;

namespace MongoDBQueryCache.Tests
{
    [TestFixture()]
    public class MongoDBQueryRewriterTests
    {
        private readonly string query1 = @"[
  {
    ""$match"": {
      ""$or"": [
        { ""name"": { ""$regex"": ""Beach"" } },
        { ""property_type"": ""House"" }
      ]
    }
  },
  {
    ""$project"": {
      ""_id"": 0,
      ""name"": 1,
      ""property_type"":  2
    }
  }
]";

        private readonly string query2 = @"[
  {
    ""$match"": {
      ""$and"": [
        { ""name"": { ""$regex"": ""Beach"" } },
        { ""property_type"": ""House"" }
      ]
    }
  },
  {
    ""$project"": {
      ""_id"": 0,
      ""name"": 1
    }
  }
]";

        private readonly string query3 = @"[
    {
        ""$match"": {
            ""name"": { ""$regex"": ""Beach"" },
            ""property_type"": ""House""
        }
    }
]";

        private readonly string query4 = @"[{
    ""$match"": {
      ""$and"": [
        {
          ""$or"": [
            { ""bedrooms"": 3 },
            { ""beds"": 5 },
            { ""guests_included"": 6 }
          ] 
        },
        {
          ""$and"": [
            { ""name"": { ""$regex"": ""Beach"" } },
            { ""property_type"": ""House"" }
          ]
        }
      ]
    }
  },
  {
    ""$project"": {
      ""_id"": 0,
      ""name"": 1,
      ""property_type"": 2,
      ""beds"": 3,
      ""bedrooms"": 4,
      ""guests_included"": 5 
    }
  }
]";

        [Test()]
        public void QueryCoversTest()
        {
            var rewriter = new MongoDBQueryParser();
            var query1Parsed = rewriter.ParseQuery(query1);
            var query2Parsed = rewriter.ParseQuery(query2);
            var query3Parsed = rewriter.ParseQuery(query3);
            Assert.That(query1Parsed.Covers(query2Parsed));
            Assert.That(!query2Parsed.Covers(query1Parsed));
            Assert.That(query2Parsed.Covers(query3Parsed));
            Assert.That(!query3Parsed.Covers(query2Parsed));
        }

        [Test()]
        public void ParseQueryTest()
        {
            void PrintQuery(Query query)
            {
                Console.WriteLine("query tree");
                Console.WriteLine(query);

                foreach (var binaryExpression in BinaryExpression.BuildFromTree(query.ExpressionTree.Root))
                {
                    Console.WriteLine(binaryExpression);
                }
                Console.WriteLine(new string('-', 40));
            }

            var rewriter = new MongoDBQueryParser();

            var query = rewriter.ParseQuery(query1);
            PrintQuery(query);
            query = rewriter.ParseQuery(query2);
            PrintQuery(query);
            query = rewriter.ParseQuery(query3);
            PrintQuery(query);
            query = rewriter.ParseQuery(query4);
            PrintQuery(query);
            //            rewriter.AnalyzeQuery(@"[
            //    {
            //        ""$match"": [
            //            { ""name"": { ""$regex"": ""Beach"" } },
            //            { ""property_type"": ""House"" }
            //        ]
            //    }
            //]");
        }
    }
}