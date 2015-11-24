#include "AtomicHashMap.h"

namespace Utils 
{
    inline std::uint32_t AtomicHashIntMap::IntegerHash(const std::uint32_t inputVal) const 
    {
        std::uint32_t hashKeyComputed(inputVal);

        hashKeyComputed ^= hashKeyComputed >> 16;
        hashKeyComputed *= 0x85ebca6b;
        hashKeyComputed ^= hashKeyComputed >> 13;
        hashKeyComputed *= 0xc2b2ae35;
        hashKeyComputed ^= hashKeyComputed >> 16;

        return hashKeyComputed;
    }

    AtomicHashIntMap::AtomicHashIntMap(const std::uint32_t hashSize)
    {
        // Initialize cells
        
        assert((hashSize & (hashSize - 1)) == 0);   
        // Must be a power of 2
        
        m_HashSize = hashSize;

        m_HashTable = std::unique_ptr<HashEntry[]>(new HashEntry[hashSize]);
        
        Clear();
    }

    void AtomicHashIntMap::Clear()
    {
        for (std::uint32_t idx = 0; idx < m_HashSize; idx++)
        {
            m_HashTable[idx].first.store(0, std::memory_order_relaxed);
            m_HashTable[idx].second.store(0, std::memory_order_relaxed);
        }
    }

    AtomicHashIntMap::~AtomicHashIntMap()
    {
    }

    void AtomicHashIntMap::SetItem(const std::uint32_t key, const std::uint32_t value)
    {
        assert(key != 0);

        for (std::uint32_t idx = IntegerHash(key);; idx++)
        {
            idx &= m_HashSize - 1;

            uint32_t prevKey = 0; 
            // We expected the cell to be 0 if unused, or if it is filled, to be with the key we look for

            // Make a CAS
            m_HashTable[idx].first.compare_exchange_strong(prevKey, key, std::memory_order_relaxed);
            
            if ((prevKey == 0) || (prevKey == key))
            {
                // Either we have added the key, or it already existed.
                m_HashTable[idx].second.store(value, std::memory_order_relaxed);
                return;
            }

            // keep looking..
        }
    }

    std::uint32_t AtomicHashIntMap::GetItem(const std::uint32_t key) const
    {
        assert(key != 0);

        for (std::uint32_t idx = IntegerHash(key);; idx++)
        {
            idx &= m_HashSize - 1;

            const std::uint32_t probedKey = m_HashTable[idx].first.load(std::memory_order_relaxed);

            if (probedKey == key) 
            {
                // return the stored value for this key
                return m_HashTable[idx].second.load(std::memory_order_relaxed);
            }
                
            if (probedKey == 0) 
            {
                // key doesn't exist
                return 0;
            }
        }
    }

    std::vector<std::pair<std::uint32_t, std::uint32_t>> AtomicHashIntMap::GetPrivateCopyOfContent() const
    {
        std::vector<std::pair<std::uint32_t, std::uint32_t>> privateCopy;

        for (std::uint32_t idx = 0; idx < m_HashSize; idx++)
        {
            const std::uint32_t keyAtPos(m_HashTable[idx].first.load(std::memory_order_relaxed));
            const std::uint32_t valAtPos(m_HashTable[idx].second.load(std::memory_order_relaxed));

            const bool existsAnEntryAtThisPos(
                keyAtPos != 0
             && valAtPos != 0);
            
            if (existsAnEntryAtThisPos)
            {
                privateCopy.push_back(std::make_pair(keyAtPos, valAtPos));
            }
        }

        privateCopy.shrink_to_fit();

        return privateCopy;
    }
}