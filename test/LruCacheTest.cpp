#include <gtest/gtest.h>
#include "LruCache.h"

class MyKeyClass 
{
public:
    explicit MyKeyClass(int key) 
    : m_key(key) 
    {}

    int GetKey() const 
    {
        return m_key;
    }

    // Overload the equality operator (operator==)
    bool operator==(const MyKeyClass& other) const 
    {
        return m_key == other.m_key;
    }

private:
    const int m_key;  // Private data member

};

namespace std 
{
    template <>
    struct hash<MyKeyClass> 
    {
        std::size_t operator()(const MyKeyClass& k) const 
        {
            return std::hash<int>()(k.GetKey());  // Hash the int key
        }
    };
}


namespace lru_cache
{
    namespace lru_cache_tests
    {
        TEST(lru_cache_test, most_basic_sanity) 
        {
            const size_t capacity = 2;
            LruCache<int, int> myCache(capacity);
            EXPECT_EQ(myCache.GetCapacity(), capacity);
            myCache.Set(0,0);
            myCache.Set(1,1);
            EXPECT_EQ(0, myCache.Get(0));
            EXPECT_EQ(1, myCache.Get(1));
        }

        TEST(lru_cache_test, sanity_with_custom_key) 
        {
            const size_t capacity = 2;
            LruCache<MyKeyClass, int> myCache(capacity);
            EXPECT_EQ(myCache.GetCapacity(), capacity);
            const MyKeyClass key0(0), key1(1), key2(2);
            const std::pair<MyKeyClass, int> pair0(key0, 0), pair1(key1, 1), pair2(key2, 2);
            myCache.Set(pair0.first, pair0.second);
            myCache.Set(pair1.first, pair1.second);
            EXPECT_EQ(pair0.first.GetKey(), myCache.Get(pair0.first));
            myCache.Set(pair2.first, pair2.second);
            try
            {
                EXPECT_EQ(pair1.second, myCache.Get(pair1.first));
            }
            catch(const std::exception& e)
            {
                std::cerr << "Exception caught in test: [" << e.what() << "]" << '\n';
                return;
            }
            
            FAIL() << "Test should not reach here. Exception should have been thrown earlier.";
        }

        TEST(lru_cache_test, sanity_with_shared_pointers) 
        {
            const int capacity = 10;
            LruCache<std::shared_ptr<int>, std::shared_ptr<int>> myCache(capacity);
            EXPECT_EQ(myCache.GetCapacity(), capacity);
            std::vector<std::pair<std::shared_ptr<int>, std::shared_ptr<int>>> dataContainer;
            for (int i = 0 ; i < capacity ; ++i)
            {
                dataContainer.push_back(std::make_pair(std::make_shared<int>(i), std::make_shared<int>(i)));
            }

            for (const auto& dataItem: dataContainer)
            {
                myCache.Set(dataItem.first, dataItem.second);
            }

            for (int i = 0 ; i < capacity ; ++i)
            {
                EXPECT_EQ(dataContainer[i].second, myCache.Get(dataContainer[i].first));
            }

            myCache.Set(std::make_shared<int>(capacity + 1), std::make_shared<int>(capacity + 1));
            try
            {
                EXPECT_EQ(dataContainer[0].second, myCache.Get(dataContainer[0].first));
            }
            catch(const std::exception& e)
            {
                std::cerr << "Exception caught in test: [" << e.what() << "]" << '\n';
                return;
            }

            FAIL() << "Test should not reach here. Exception should have been thrown earlier.";
        }

        TEST(lru_cache_test, zero_capacity_is_invalid) 
        {
            const size_t capacity = 0;
            try
            {
                LruCache<int, int> myCache(capacity);          
            }
            catch(const std::runtime_error& e)
            {
                std::cerr << "Exception caught in test: [" << e.what() << "]" << '\n';
                return;
                
            }
            catch(...)
            {
                FAIL() << "Unexpected error.";
            }

            FAIL() << "Test should not reach here. Exception should have been thrown earlier.";
        }

        TEST(lru_cache_test, negative_capacity_is_invalid) 
        {
            const int capacity = -1;
            try
            {
                LruCache<int, int> myCache(capacity);          
            }
            catch(const std::runtime_error& e)
            {
                std::cerr << "Exception caught in test: [" << e.what() << "]" << '\n';
                return;
            }
            catch(...)
            {
                FAIL() << "Unexpected error.";
            }

             FAIL() << "Test should not reach here. Exception should have been thrown earlier.";
        }

        TEST(lru_cache_test, maximal_int_capacity_should_pass) 
        {
            // const size_t capacity = std::numeric_limits<size_t>::max();
            const size_t capacity = INT_MAX;
            try
            {
                LruCache<int, int> myCache(capacity);          
            }
            catch(const std::runtime_error& e)
            {
                FAIL() << "Unexpected error.";
            }
            catch(...)
            {
                FAIL() << "Unexpected error.";
            }
        }

        TEST(lru_cache_test, outdated_item_should_not_be_found_in_cache) 
        {
            const size_t capacity = 2;
            LruCache<int, int> myCache(capacity);
            EXPECT_EQ(myCache.GetCapacity(), capacity);
            myCache.Set(0,0);
            myCache.Set(1,1);
            myCache.Set(2,2);
            EXPECT_EQ(1, myCache.Get(1));
            try
            {
                EXPECT_EQ(0, myCache.Get(0));
            }
            catch(const std::exception& e)
            {
                std::cerr << "Exception caught in test: [" << e.what() << "]" << '\n';
                return;
            }
            
            FAIL() << "Test should not reach here. Exception should have been thrown earlier.";
        }

        TEST(lru_cache_test, item_outdated_by_get_function_call_should_not_be_found_in_cache) 
        {
            const size_t capacity = 2;
            LruCache<int, int> myCache(capacity);
            EXPECT_EQ(myCache.GetCapacity(), capacity);
            myCache.Set(0,0);
            myCache.Set(1,1);
            EXPECT_EQ(0, myCache.Get(0));
            myCache.Set(2,2);
            try
            {
                EXPECT_EQ(1, myCache.Get(1));
            }
            catch(const std::exception& e)
            {
                std::cerr << "Exception caught in test: [" << e.what() << "]" << '\n';
                return;
            }
            
            FAIL() << "Test should not reach here. Exception should have been thrown earlier.";
        }

        TEST(lru_cache_test, item_outdated_by_get_function_call_should_not_be_found_in_cache_with_larger_cache_capacity) 
        {
            const int capacity = 100, requestedKey = 71;
            if (requestedKey >= capacity)
            {
                FAIL() << "This test relies on capacity being larger than requested key.";
            }

            try
            {
                LruCache<int, int> myCache(capacity);
                EXPECT_EQ(myCache.GetCapacity(), capacity);
                for (int i = 0 ; i < capacity ; ++i)
                {
                    myCache.Set(i,i);
                }

                for (int j = 0 ; j < requestedKey ; ++j)
                {
                    EXPECT_EQ(j, myCache.Get(j));
                }
                
                myCache.Set(capacity, capacity);
                EXPECT_EQ(requestedKey, myCache.Get(requestedKey));
            }
            catch(const std::exception& e)
            {
                std::cerr << "Exception caught in test: [" << e.what() << "]" << '\n';
                return;
            }
            
            FAIL() << "Test should not reach here. Exception should have been thrown earlier.";
        }

        void DoSet(LruCache<int, int>& cache, const std::vector<std::pair<int,int>>& data)
        {
            for (const auto& item: data)
            {
                try
                {
                    cache.Set(item.first, item.second);
                }
                catch(const std::exception& e)
                {
                    FAIL() << "UNEXPECTED ERROR IN MULTITHREAD TEST: " << e.what() << '\n';
                }
            }
        }

        void DoGet(LruCache<int, int>& cache, const std::vector<std::pair<int,int>>& pairsToGetAndCompare)
        {
            const int nofKeys = pairsToGetAndCompare.size();
            int i = 0;
            while (i < nofKeys)
            {
                try
                {
                    EXPECT_EQ(pairsToGetAndCompare[i].second, cache.Get(pairsToGetAndCompare[i].first));
                    ++i;
                }
                catch(const std::exception& e)
                {
                    // Do not increment i. Re-try until getting the key
                }
            }
        }

        [[nodiscard]] std::vector<std::pair<int, int>> GenerateKeyValuePairs(const size_t numberOfPairs) noexcept
        {
            std::vector<std::pair<int, int>> result;
            for (size_t i = 0 ; i < numberOfPairs ; ++i)
            {
                result.push_back(std::make_pair<int, int>(i, i));
            }

            return result;
        }

        TEST(lru_cache_test, multi_thread_basic_sanity) 
        {
            const int capacity = 1000;
            const auto sourcePairs = GenerateKeyValuePairs(capacity);

            try
            {
                LruCache<int, int> myCache(capacity);
                EXPECT_EQ(myCache.GetCapacity(), capacity);
                std::thread consumer(&DoGet, std::ref(myCache), std::ref(sourcePairs));
                std::thread producer(&DoSet, std::ref(myCache), std::ref(sourcePairs));
                producer.join();
                consumer.join();
            }
            catch(const std::exception& e)
            {
                FAIL() << "Exception should not have been thrown.";
            }
        }

        TEST(lru_cache_test, multi_thread_multiple_consumers_single_producer) 
        {
            const int capacity = 1000, numberOfConsumers = 10;
            const auto sourcePairs = GenerateKeyValuePairs(capacity);

            try
            {
                LruCache<int, int> myCache(capacity);
                EXPECT_EQ(myCache.GetCapacity(), capacity);
                std::vector<std::thread> consumerContainer;
                for (int i = 0 ; i < numberOfConsumers ; ++i)
                {
                    consumerContainer.push_back(std::thread(&DoGet, std::ref(myCache), std::ref(sourcePairs)));
                }

                std::thread producer(&DoSet, std::ref(myCache), std::ref(sourcePairs));
                producer.join();
                for (auto& consumerThread: consumerContainer)
                {
                    consumerThread.join();
                }
            }
            catch(const std::exception& e)
            {
                FAIL() << "Exception should not have been thrown.";
            }
        }
        
        TEST(lru_cache_test, multi_thread_multiple_producers_multiple_consumers) 
        {
            const int capacity = 10000, numberOfThreadsOfEachType = 100;
            const auto sourcePairs = GenerateKeyValuePairs(capacity);
            try
            {
                LruCache<int, int> myCache(capacity);
                EXPECT_EQ(myCache.GetCapacity(), capacity);
                std::vector<std::thread> consumerContainer, producerContainer;
                for (int i = 0 ; i < numberOfThreadsOfEachType ; ++i)
                {
                    consumerContainer.push_back(std::thread(&DoGet, std::ref(myCache), std::ref(sourcePairs)));
                }

                for (int i = 0 ; i < numberOfThreadsOfEachType ; ++i)
                {
                    producerContainer.push_back(std::thread(&DoSet, std::ref(myCache), std::ref(sourcePairs)));
                }

                for (auto& producerThread: producerContainer)
                {
                    producerThread.join();
                }

                for (auto& consumerThread: consumerContainer)
                {
                    consumerThread.join();
                }
            }
            catch(const std::exception& e)
            {
                FAIL() << "Exception should not have been thrown.";
            }
        }
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
