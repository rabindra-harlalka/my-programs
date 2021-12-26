namespace MongoDBQueryCache
{
    public class QueryCacheItem : CacheItem
    {
        public string QueryJsonString { get; }
        public int Timestamp { get; }

        public QueryCacheItem(int id, string queryJsonString, int timestamp) : base(id)
        {
            QueryJsonString = queryJsonString;
            Timestamp = timestamp;
        }
    }
}