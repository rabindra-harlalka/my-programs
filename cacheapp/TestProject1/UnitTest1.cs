using System;
using MongoDBQueryCache;
using NUnit.Framework;

namespace TestProject1
{
    public class Tests
    {
        [Test]
        public void Test1()
        {
            var liteDatabase = CachingMongoDbProxy.OpenDatabase(@"data/test_cache.db");
            var cache = new QueryResultCache(liteDatabase);
            cache.Store(@"{""name"": ""long beach house""}", 1);
            cache.Store(@"{""name"": ""beach face resort""}", 1);
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
            var cache = new QueryResultCache(liteDatabase);
            cache.Store(@"{""name"": ""long beach house""}", 1);
            cache.Store(@"{""name"": ""beach face resort""}", 1);
            //Assert.IsTrue(cache.CheckHitOrMiss(@"{""name"": ""long beach house""}"));
            //Assert.IsTrue(cache.CheckHitOrMiss(@"{""name"": ""beach face resort""}"));
            //Assert.IsFalse(cache.CheckHitOrMiss(@"{""name"": ""long beach hous""}"));
            CachingMongoDbProxy.CloseDatabase(liteDatabase);
        }
    }
}