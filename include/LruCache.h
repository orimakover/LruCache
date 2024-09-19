#ifndef __LRUCACHE_H__
#define __LRUCACHE_H__

#include <list>
#include <unordered_map>
#include <mutex>

namespace lru_cache
{
    template<typename Key, typename Value>
    class LruCache final
    {
    public:
        explicit LruCache(const int capacity);

        [[nodiscard]] Value Get(const Key& key);

        void Set(const Key& key, const Value& value);

        [[nodiscard]] inline size_t GetCapacity() const noexcept
        {
            return m_capacity;
        }

        //  non-copyable non-movable.
        LruCache(const LruCache& other) = delete;
        LruCache(const LruCache&& other) = delete;
        LruCache& operator = (const LruCache& other) = delete;
        LruCache& operator = (const LruCache&& other) = delete;

    private:
        void UpdateKeyQueue(typename std::list<Key>::iterator& keyToUpdate);

        void RemoveLeastRecentlyUsedItem();

        void InsertNewPair(const Key& key, const Value& value);

    private:
        const size_t m_capacity;
        std::mutex m_mutex;
        std::unordered_map<Key, std::pair<Value, typename std::list<Key>::iterator>> m_map;
        std::list<Key> m_keyQueue;  //  This list is used as a queue - The front holds the newest item.
    };

    #include "LruCache.hpp"
}

#endif  /*  __LRUCACHE_H__ */
