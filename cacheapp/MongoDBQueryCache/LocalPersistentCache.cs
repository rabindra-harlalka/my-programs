using System;
using System.Collections.Generic;
using System.Linq;
using LiteDB;

namespace MongoDBQueryCache
{
    public class LocalPersistentCache<T> where T : CacheItem
    {
        private readonly ILiteCollection<T> _liteCollection;
        private readonly MinHeap<CacheItem> _lruHeap;
        private readonly int _cacheCapacity;

        private int _nextId;
        private int _nextTimestamp;
        protected int NextId => ++_nextId;
        protected int NextTimestamp => ++_nextTimestamp;

        public LocalPersistentCache(string collectionName, ILiteDatabase liteDatabase, int cacheCapacity)
        {
            _liteCollection = liteDatabase.GetCollection<T>(collectionName);
            if (_liteCollection.Count() > 0)
            {
                _nextId = _liteCollection.Max(_ => _.Id);
                _lruHeap = new MinHeap<CacheItem>(_liteCollection.FindAll().Cast<CacheItem>().ToArray(),
                    (item1, item2) => item1.Id.CompareTo(item2.Id) <= 0);
            }
            else
            {
                _lruHeap = new MinHeap<CacheItem>((item1, item2) => item1.Id.CompareTo(item2.Id) <= 0);
            }

            _cacheCapacity = cacheCapacity;
        }

        public void Store(T item, out bool evicted, out int evictedItemId)
        {
            evicted = false;
            evictedItemId = -1;
            if (_lruHeap.Count == _cacheCapacity)
            {
                var evictedItem = _lruHeap.Extract();
                evicted = _liteCollection.Delete(evictedItem.Id);
                evictedItemId = evictedItem.Id;
            }
            _liteCollection.Insert(item);
            _lruHeap.Insert(item);
        }

        public IEnumerable<T> Load()
        {
            return _liteCollection.FindAll();
        }

        public T Load(int id)
        {
            return _liteCollection.FindById(id);
        }

        public IEnumerable<T> Load(/*Func<T, bool> */string predicate)
        {
            return _liteCollection.Find(predicate);
        }

        public void UpdateAccessTime(int id)
        {
            var item = _liteCollection.FindById(id);
            var newItem = item.UpdateTimestamp(NextTimestamp);
            _lruHeap.UpdateNode(item, newItem);
        }

        public int Remove(string predicate)
        {
            var numRemoved = _liteCollection.DeleteMany(predicate);
            // rebuild the heap
            _lruHeap.Clear();
            foreach (var item in _liteCollection.FindAll())
            {
                _lruHeap.Insert(item);
            }

            return numRemoved;
        }

        public void Clear()
        {
            _liteCollection.DeleteAll();
            _lruHeap.Clear();
            _nextId = 0;
        }
    }
}