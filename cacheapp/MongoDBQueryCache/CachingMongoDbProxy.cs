﻿using System;
using MongoDB.Driver;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
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

        public CachingMongoDbProxy(string mongoDbConnectionString, string databaseName, string collectionName)
        {
            var settings = MongoClientSettings.FromConnectionString(mongoDbConnectionString);
            var client = new MongoClient(settings);
            var database = client.GetDatabase(databaseName);

            _liteDb = OpenDatabase(@"data/cache_app.db");
            _queryCache = new QueryCache(_liteDb);
            _queryResultCache = new QueryResultCache(_liteDb);
            _queryCache.Clear();
            _queryResultCache.Clear();
            _mongoCollection = database.GetCollection<BsonDocument>(collectionName);
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

        private static bool ResultSatisfiesCondition(Node subTree, BsonDocument resultDocument)
        {
            Debug.Assert(IsOperatorNode(subTree));

            var op = subTree.Value;
            if (op is "$and" or "$or")
            {
                var hits = subTree.Children.Select(_ => ResultSatisfiesCondition(_, resultDocument));
                return subTree.Value is "$and" ? hits.All(_ => _ is true) : hits.Any(_ => _ is true);
            }

            var value = resultDocument[subTree.Children[0].Value].ToString();
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
            Console.WriteLine("Query cache hit: {0}", hit);
            if (hit)
            {
                // get result from cache
                foreach (var result in _queryResultCache.Load()
                    //.Where(_ => _.QueryId == queryCacheItem.Id)
                    .Where(_ => ResultSatisfiesCondition(queryParsed.ExpressionTree.Root.Children[0], BsonDocument.Parse(_.ResultDocument)))
                    .Select(_ => _.ResultDocument)) // TODO: drop the attributes that the query didn't ask for
                {
                    yield return result;
                }
            }
            else
            {
                var pipelineStages = new List<BsonDocument> { queryParsed.MatchBsonDocument };
                if (queryParsed.ProjectBsonDocument != null) pipelineStages.Add(queryParsed.ProjectBsonDocument);

                var pipelineDefinition = PipelineDefinition<BsonDocument, BsonDocument>.Create(pipelineStages);
                var result = _mongoCollection.Aggregate<BsonDocument>(pipelineDefinition);

                // store query in query cache
                var queryId = _queryCache.Store(query);

                while (await result.MoveNextAsync())
                {
                    foreach (var document in result.Current)
                    {
                        // store result in cache
                        var docJson = document.ToJson(/*new JsonWriterSettings {OutputMode = JsonOutputMode.RelaxedExtendedJson}*/);
                        _queryResultCache.Store(docJson, queryId);
                        yield return docJson;
                    }
                }
            }
        }
    }
}