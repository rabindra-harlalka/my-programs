namespace MongoDBQueryCache
{
    public class QueryCacheItem : CacheItem
    {
        public string QueryJsonString { get; }
        public string QueryFilename { get; }

        public QueryCacheItem(int id, string queryJsonString, string queryFilename, int timestamp)
            : base(id, timestamp)
        {
            QueryJsonString = queryJsonString;
            QueryFilename = queryFilename;
        }

        public override CacheItem UpdateTimestamp(int timestamp)
        {
            return new QueryCacheItem(Id, QueryJsonString, QueryFilename, timestamp);
        }
    }
}