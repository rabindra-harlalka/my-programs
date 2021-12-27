using LiteDB;

namespace MongoDBQueryCache
{
    public class QueryResultCache : LocalPersistentCache<QueryResultCacheItem>
    {
        public QueryResultCache(ILiteDatabase liteDatabase, int cacheCapacity)
            : base("results", liteDatabase, cacheCapacity)
        {
        }

        public void Store(string result, int queryId, out bool evicted, out int evictedItemId) =>
            Store(new QueryResultCacheItem(NextId, queryId, result, NextTimestamp), out evicted, out evictedItemId);
    }
}
