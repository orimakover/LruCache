template<typename Key, typename Value>
LruCache<Key, Value>::LruCache(const int capacity)
: m_capacity(capacity)
{
    if ( 0 >= capacity)
    {
        throw std::runtime_error("Invalid capacity.");
    }
}

template<typename Key, typename Value>
[[nodiscard]] Value LruCache<Key, Value>::Get(const Key& key)
{
    std::scoped_lock lock(m_mutex);
    const auto& findResult = m_map.find(key);
    if (m_map.end() == findResult)
    {
        throw std::runtime_error("Requested key does not currently exist in the cache.");
    }

    UpdateKeyQueue(findResult->second.second);
    return findResult->second.first;
}

template<typename Key, typename Value>
void LruCache<Key, Value>::Set(const Key& key, const Value& value)
{
    std::scoped_lock lock(m_mutex);
    auto findResult = m_map.find(key); 
    if (m_map.end() != findResult)
    {
        UpdateKeyQueue(findResult->second.second);
        findResult->second.first = value;
    }
    else
    {
        if (m_map.size() == m_capacity)
        {
            RemoveLeastRecentlyUsedItem();
        }

        InsertNewPair(key, value);
    }
}

template<typename Key, typename Value>
void LruCache<Key, Value>::UpdateKeyQueue(typename std::list<Key>::iterator& keyToUpdate)
{
    m_keyQueue.erase(keyToUpdate);
    if (m_keyQueue.end() != keyToUpdate)
    {
        m_keyQueue.push_front(*keyToUpdate);
    }

    keyToUpdate = m_keyQueue.begin();
}

template<typename Key, typename Value>
void LruCache<Key, Value>::RemoveLeastRecentlyUsedItem()
{
    const auto& backKey = m_keyQueue.back();
    const auto& findResult = m_map.find(backKey);
    if (m_map.end() == findResult)
    {
        throw std::runtime_error("Underlying containers content mismatch - logic error.");
    }
    
    if (1 != m_map.erase(backKey))
    {
        throw std::runtime_error("After reaching capacity, failed to erase element from underlying container.");
    }

    m_keyQueue.pop_back();
}

template<typename Key, typename Value>
void LruCache<Key, Value>::InsertNewPair(const Key& key, const Value& value)
{
    m_keyQueue.push_front(key);
    std::pair<Value, typename std::list<Key>::iterator> valueIteratorPair = std::make_pair(value, m_keyQueue.begin());
    const auto& insertResult = m_map.insert(std::make_pair(key, valueIteratorPair));
    if (!insertResult.second)
    {
        m_keyQueue.pop_front();
        throw std::runtime_error("Failed to insert objects into the underlying container.");
    }
}