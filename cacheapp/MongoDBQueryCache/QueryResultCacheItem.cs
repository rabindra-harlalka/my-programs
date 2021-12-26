namespace MongoDBQueryCache
{
    public class QueryResultCacheItem : CacheItem
    {
        public int QueryId { get; }
        public string ResultDocument { get; set; }

        public QueryResultCacheItem(int id, int queryId, string resultDocument) : base(id)
        {
            QueryId = queryId;
            ResultDocument = resultDocument;
        }
    }
}