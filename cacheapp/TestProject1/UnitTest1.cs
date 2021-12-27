using System;
using System.Linq;
using MongoDBQueryCache;
using NUnit.Framework;

namespace TestProject1
{
    public class Tests
    {
        [Test]
        public void Test3()
        {
            var queryCache = new QueryCache(CachingMongoDbProxy.OpenDatabase(
                @"C:\Work\git\GitHub\my-programs\cacheapp\ConsoleApp1\bin\Debug\net5.0\data\cache_app.db"), 1000);
            var queries = queryCache.Load().ToArray();
            foreach (var item in queries)
            {
                Console.WriteLine($"item id = {item.Id}, timestamp = {item.Timestamp}");
            }
        }

        [Test]
        public void Test1()
        {
            var liteDatabase = CachingMongoDbProxy.OpenDatabase(@"data/test_cache.db");
            var cache = new QueryResultCache(liteDatabase, 100);
            cache.Store(@"{""name"": ""long beach house""}", 1, out _, out _);
            cache.Store(@"{""name"": ""beach face resort""}", 1, out _, out _);
            foreach (var item in cache.Load())
            {
                Console.WriteLine(item);
            }

            cache.Clear();
            foreach (var item in cache.Load())
            {
                Console.WriteLine(item);
            }
            CachingMongoDbProxy.CloseDatabase(liteDatabase);
        }

        [Test]
        public void Test2()
        {
            var liteDatabase = CachingMongoDbProxy.OpenDatabase(@"data/test_cache.db");
            var cache = new QueryResultCache(liteDatabase, 100);
            cache.Store(@"{""name"": ""long beach house""}", 1, out _, out _);
            cache.Store(@"{""name"": ""beach face resort""}", 1, out _, out _);
            //Assert.IsTrue(cache.CheckHitOrMiss(@"{""name"": ""long beach house""}"));
            //Assert.IsTrue(cache.CheckHitOrMiss(@"{""name"": ""beach face resort""}"));
            //Assert.IsFalse(cache.CheckHitOrMiss(@"{""name"": ""long beach hous""}"));
            CachingMongoDbProxy.CloseDatabase(liteDatabase);
        }
    }
}