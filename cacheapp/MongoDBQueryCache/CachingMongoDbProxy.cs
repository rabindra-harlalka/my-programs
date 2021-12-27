using System;
using MongoDB.Driver;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Linq.Expressions;
using System.Text.RegularExpressions;
using LiteDB;
using MongoDB.Bson;
using BsonDocument = MongoDB.Bson.BsonDocument;

namespace MongoDBQueryCache
{
    public class CachingMongoDbProxy : IMongoDBProxy
    {
        private readonly IMongoCollection<BsonDocument> _mongoCollection;
        private readonly QueryCache _queryCache;
        private readonly QueryResultCache _queryResultCache;
        private readonly ILiteDatabase _liteDb;

        public CachingMongoDbProxy(string mongoDbConnectionString, string databaseName, string collectionName,
            int queryCacheCapacity, int resultCacheCapacity)
        {
            var settings = MongoClientSettings.FromConnectionString(mongoDbConnectionString);
            var client = new MongoClient(settings);
            var remoteDatabase = client.GetDatabase(databaseName);
            _mongoCollection = remoteDatabase.GetCollection<BsonDocument>(collectionName);

            _liteDb = OpenDatabase(@"data/cache_app.db");
            _queryCache = new QueryCache(_liteDb, queryCacheCapacity);
            _queryResultCache = new QueryResultCache(_liteDb, resultCacheCapacity);
            _queryCache.Clear();
            _queryResultCache.Clear();
        }

        public static ILiteDatabase OpenDatabase(string dbFilePath)
        {
            var directory = Path.GetDirectoryName(dbFilePath);
            if (!Directory.Exists(directory))
            {
                Directory.CreateDirectory(directory!);
            }
            return new LiteDatabase(dbFilePath);
        }

        public static void CloseDatabase(ILiteDatabase liteDatabase) => liteDatabase?.Dispose();

        public void Close() => _liteDb.Dispose();

        private static bool ResultSatisfiesCondition(Node subTree, BsonDocument resultDocument, int resultId)
        {
            Debug.Assert(IsOperatorNode(subTree));

            var op = subTree.Value;
            if (op is "$and" or "$or")
            {
                var hits = subTree.Children.Select(_ => ResultSatisfiesCondition(_, resultDocument, resultId));
                return subTree.Value is "$and" ? hits.All(_ => _ is true) : hits.Any(_ => _ is true);
            }

            var name = subTree.Children[0].Value;
            if (!resultDocument.Contains(name))
            {
                Console.WriteLine($"WARN: result document {resultId} doesn't contain {name}.");
                return false;
            }
            var value =  resultDocument[name].ToString();
            return op switch
            {
                "=" => value == subTree.Children[1].Value,
                "$regex" => Regex.IsMatch(value!, subTree.Children[1].Value),
                _ => throw new InvalidOperationException("Unrecognized operator")
            };
        }

        private static bool IsOperatorNode(Node node)
        {
            return node.Value is "$and" or "$or" or "=" or "$regex";
        }

        public async IAsyncEnumerable<string> ExecQueryAsync(string query)
        {
            var queryParsed = new MongoDBQueryParser().ParseQuery(query);

            // check query hit/miss in cache
            bool hit = _queryCache.CheckHitOrMiss(query, out var queryCacheItem);
            Console.WriteLine("Query cache hit: {0}{1}", hit, hit ? $" query id is {queryCacheItem.Id}" : null);
            if (hit)
            {
                _queryCache.UpdateAccessTime(queryCacheItem.Id);
                // get result from cache
                foreach (var result in _queryResultCache.Load($"$.QueryId = {queryCacheItem.Id}")
                    .Where(_ =>
                        ResultSatisfiesCondition(queryParsed.ExpressionTree.Root.Children[0], BsonDocument.Parse(_.ResultDocument), _.Id)))
                {
                    _queryResultCache.UpdateAccessTime(result.Id);
                    yield return result.ResultDocument;
                }
            }
            else
            {
                var pipelineStages = new List<BsonDocument> { queryParsed.MatchBsonDocument };
                if (queryParsed.ProjectBsonDocument != null) pipelineStages.Add(queryParsed.ProjectBsonDocument);

                var pipelineDefinition = PipelineDefinition<BsonDocument, BsonDocument>.Create(pipelineStages);
                var result = _mongoCollection.Aggregate<BsonDocument>(pipelineDefinition);

                // store query in query cache
                var queryId = _queryCache.Store(query, out var evicted, out var evictedQueryId);
                if (evicted)
                {
                    Console.WriteLine($"Query {evictedQueryId} evicted from cache.");
                    // if query has been evicted, evict the results too
                    var numRemoved = _queryResultCache.Remove($"$.QueryId = {evictedQueryId}");
                    Console.WriteLine($"Evicted all the {numRemoved} results belonging to query {evictedQueryId}.");
                }

                var resultEvictedCount = 0;
                while (await result.MoveNextAsync())
                {
                    foreach (var document in result.Current)
                    {
                        // store result in cache
                        var docJson = document.ToJson(/*new JsonWriterSettings {OutputMode = JsonOutputMode.RelaxedExtendedJson}*/);
                        _queryResultCache.Store(docJson, queryId, out var evicted1, out var evictedResultId);
                        if (evicted1)
                        {
                            //Console.WriteLine($"Result {evictedResultId} evicted from cache.");
                            resultEvictedCount++;
                        }
                        yield return docJson;
                    }
                }
                if (resultEvictedCount > 0) Console.WriteLine($"{resultEvictedCount} results evicted from cache.");
            }
        }
    }
}
