using LiteDB;

namespace MongoDBQueryCache
{
    public class QueryResultCache : LocalPersistentCache<QueryResultCacheItem>
    {
        public QueryResultCache(ILiteDatabase liteDatabase) : base("results", liteDatabase)
        {
        }

        public void Store(string result, int queryId) => Store(new QueryResultCacheItem(NextId, queryId, result));
    }
}
