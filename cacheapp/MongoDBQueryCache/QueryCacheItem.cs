namespace MongoDBQueryCache
{
    public class QueryCacheItem : CacheItem
    {
        public string QueryJsonString { get; }

        public QueryCacheItem(int id, string queryJsonString, int timestamp)
            : base(id, timestamp)
        {
            QueryJsonString = queryJsonString;
        }

        public override CacheItem UpdateTimestamp(int timestamp)
        {
            return new QueryCacheItem(Id, QueryJsonString, timestamp);
        }
    }
}