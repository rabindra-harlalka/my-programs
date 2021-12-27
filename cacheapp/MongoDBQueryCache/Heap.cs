using System;
using System.Collections.Generic;
using System.Text;

namespace MongoDBQueryCache
{
    public class MinHeap<T> : Heap<T>
        where T : IEquatable<T>
    {
        public static bool MinHeapComparer(T parent, T child, IComparer<T> comparer)
        {
            return comparer.Compare(parent, child) <= 0;
        }

        public MinHeap(IComparer<T> comparer) : base((parent, child) => MinHeapComparer(parent, child, comparer))
        {
        }

        public MinHeap(T[] items, IComparer<T> comparer) : base(items, (parent, child) => MinHeapComparer(parent, child, comparer))
        {
        }

        public MinHeap(Func<T, T, bool> heapPropertyComparator) : base(heapPropertyComparator)
        {
        }

        public MinHeap(T[] items, Func<T, T, bool> heapPropertyComparator) : base(items, heapPropertyComparator)
        {
        }
    }

    public abstract class Heap<T>
        where T : IEquatable<T>
    {
        private readonly List<T> _store;

        private readonly Func<T, T, bool> _heapPropertyComparer;

        public int Count => _store.Count;

        public T Root => Count > 0 ? _store[0] : default(T);

        protected Heap(Func<T, T, bool> heapPropertyComparator)
        {
            _store = new List<T>();
            _heapPropertyComparer = heapPropertyComparator;
        }

        protected Heap(T[] items, Func<T, T, bool> heapPropertyComparator)
        {
            _store = new List<T>(items);
            _heapPropertyComparer = heapPropertyComparator;
            BuildHeap();
        }

        private int Parent(int i) => (i - 1) / 2;

        private int Left(int i) => 2 * i + 1;

        private int Right(int i) => 2 * i + 2;

        private void BuildHeap()
        {
            if (Count < 2) return;
            // no of leaf nodes = ceil(n/2); so non-leaf nodes are 0 .. ceil(n/2) - 1.
            // simpler way to look at it  is: parent of last item
            for (var i = Parent(Count - 1); i >= 0; i--)
            {
                Heapify(i);
            }
        }

        private void Heapify(int i)
        {
            // check if left or right child is not satisfying the heap property
            // if yes, exchange that child with the parent (ith) node
            // then heapify at the exchanged child node
            var leftIndex = Left(i);
            var rightIndex = Right(i);

            var selectedIndex = i;
            if (leftIndex < Count && !_heapPropertyComparer(_store[selectedIndex], _store[leftIndex]))
            {
                selectedIndex = leftIndex;
            }
            if (rightIndex < Count && !_heapPropertyComparer(_store[selectedIndex], _store[rightIndex]))
            {
                selectedIndex = rightIndex;
            }
            if (selectedIndex != i)
            {
                Swap(i, selectedIndex);
                Heapify(selectedIndex);
            }
        }

        public T Extract()
        {
            if (Count == 0) throw new Exception("heap underflow");
            var rootItem = Root;
            _store[0] = _store[Count - 1];
            _store.RemoveAt(Count - 1);
            Heapify(0);
            return rootItem;
        }

        public void Insert(T item)
        {
            _store.Add(item);
            HeapifyUp(Count - 1);
        }

        private void Swap(int i, int j)
        {
            (_store[i], _store[j]) = (_store[j], _store[i]);
        }

        public void UpdateNode(T item, T newItem)
        {
            var i = _store.IndexOf(item);
            if (!_heapPropertyComparer(newItem, _store[i])) throw new Exception($"{newItem} must satisfy the heap property");
            _store[i] = newItem;
            HeapifyUp(i);
        }

        public void Clear()
        {
            _store.Clear();
        }

        private void HeapifyUp(int i)
        {
            if (i == 0) return;
            var node = _store[i];
            // traverse up the tree and swap wherever the heap property is not satisfied
            var parentNode = _store[Parent(i)];
            while (i > 0 && !_heapPropertyComparer(parentNode, node))
            {
                Swap(Parent(i), i);
                i = Parent(i);
                parentNode = _store[Parent(i)];
                node = _store[i];
            }
        }

        public override string ToString()
        {
            if (Count == 0) return "(empty)";
            var stringBuilder = new StringBuilder();
            foreach (var item in _store)
            {
                stringBuilder.Append($"{item} -> ");
            }
            stringBuilder.Append("null");

            return stringBuilder.ToString();
        }
    }
}
