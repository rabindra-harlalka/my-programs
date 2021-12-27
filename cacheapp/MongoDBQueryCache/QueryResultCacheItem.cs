namespace MongoDBQueryCache
{
    public class QueryResultCacheItem : CacheItem
    {
        public int QueryId { get; }
        public string ResultDocument { get; set; }

        public QueryResultCacheItem(int id, int queryId, string resultDocument, int timestamp)
            : base(id, timestamp)
        {
            QueryId = queryId;
            ResultDocument = resultDocument;
        }

        public override CacheItem UpdateTimestamp(int timestamp)
        {
            return new QueryResultCacheItem(Id, QueryId, ResultDocument, timestamp);
        }
    }
}