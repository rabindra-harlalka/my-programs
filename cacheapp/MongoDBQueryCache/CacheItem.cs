using System;

namespace MongoDBQueryCache
{
    public abstract class CacheItem : IEquatable<CacheItem>
    {
        public int Id { get; }
        public int Timestamp { get; private set; }

        protected CacheItem(int id, int timestamp)
        {
            Id = id;
            Timestamp = timestamp;
        }

        public abstract CacheItem UpdateTimestamp(int timestamp);

        public bool Equals(CacheItem other)
        {
            if (ReferenceEquals(null, other)) return false;
            if (ReferenceEquals(this, other)) return true;
            return Id == other.Id;
        }

        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            if (obj.GetType() != this.GetType()) return false;
            return Equals((CacheItem) obj);
        }

        public override int GetHashCode()
        {
            return Id;
        }
    }
}