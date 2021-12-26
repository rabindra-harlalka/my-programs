using System.Linq;
using LiteDB;

namespace MongoDBQueryCache
{
    public class QueryCache : LocalPersistentCache<QueryCacheItem>
    {
        public QueryCache(ILiteDatabase liteDatabase) : base("queries", liteDatabase)
        {
        }

        public int Store(string query)
        {
            var id = NextId;
            Store(new QueryCacheItem(id, query, NextTimestamp));
            return id;
        }

        public bool CheckHitOrMiss(string query, out QueryCacheItem cachedItem)
        {
            cachedItem = null;

            var cachedQueries = Load().ToList();
            if (!cachedQueries.Any()) return false;
            // the easy case - find if there is an exact match
            cachedItem = cachedQueries.Find(_ => _.QueryJsonString == query);
            if (cachedItem != null) return true;

            // otherwise check if a cached query covers the incoming query
            var queryRewriter = new MongoDBQueryParser();
            var incomingQueryParsed = queryRewriter.ParseQuery(query);
            cachedItem = cachedQueries.Find(q => queryRewriter.ParseQuery(q.QueryJsonString).Covers(incomingQueryParsed));
            return cachedItem != null;
        }
    }
}