using System.Collections.Generic;

namespace MongoDBQueryCache
{
    public interface IMongoDBProxy
    {
        IAsyncEnumerable<string> ExecQueryAsync(string query, string filename);
        void Close();
    }
}