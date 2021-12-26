using System;
using System.Collections.Generic;
using System.IO;
using LiteDB;

namespace MongoDBQueryCache
{
    public class LocalPersistentCache<T> where T : CacheItem
    {
        private readonly ILiteCollection<T> _liteCollection;

        //private MinHeap<T> _lruHeap;

        private int _nextId;
        private int _nextTimestamp;
        public int NextId => ++_nextId;
        public int NextTimestamp => ++_nextTimestamp;

        public LocalPersistentCache(string collectionName, ILiteDatabase liteDatabase)
        {
            _liteCollection = liteDatabase.GetCollection<T>(collectionName);
            if (_liteCollection.Count() > 0) _nextId = _liteCollection.Max(_ => _.Id);
        }

        public void Store(T item)
        {
            _liteCollection.Insert(item);
        }

        public IEnumerable<T> Load()
        {
            return _liteCollection.FindAll();
        }

        public void Clear()
        {
            _liteCollection.DeleteAll();
        }
    }
}