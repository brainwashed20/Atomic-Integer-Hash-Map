#pragma once

#include <atomic>
#include <cstdint>
#include <vector>
#include <memory>

namespace Utils
{
    /**
     *  Maps 32-bit integers to 32-bit integers.
     *  The hash size must be a power of 2
     *  Uses open addressing with linear probing.
     *  You can call SetItem and GetItem from several threads simultaneously.
     *  GetItem is wait-free.
     *  SetItem is lock-free.
     *  You can't assign any value to key = 0, as is reserved to indicate an unused cell.
     *  The hash table never grows in size.
     *  You can't delete individual items from the hash table.
     *  You can Clear the hash table, but only at a time when there are no other calls being made from other threads.
     */
    class AtomicHashIntMap
    {
    private:

        // aliasing
        typedef std::pair<std::atomic<std::uint32_t>, std::atomic<std::uint32_t>> HashEntry;

        /**
         * from code.google.com/p/smhasher/wiki/MurmurHash3
         * The hash function used to distribue the keys inside the array
         */
        inline std::uint32_t IntegerHash(const std::uint32_t) const;

        /**
         * Method iterates through the hash and makes all the key-pair-values (0,0)
         */
        void Clear();

        /**
         * The array which contains all the entries from the hash
         */
        std::unique_ptr<HashEntry[]> m_HashTable;

        /**
         * We require that the size is a power of 2 because bitwise-and is an inexpensive operation, whereas integer modulo (%) is quite heavy.
         */
        std::uint32_t m_HashSize;
     

    public:
        /**
         * Constructor which takes as a parameter the size that the hash table will have
         * MUST be a powert of 2
         */
        AtomicHashIntMap(const std::uint32_t);

        ~AtomicHashIntMap();

        // Basic operations

        /**
         * GetItem and SetItem use a circular search, which means that if SetItem or GetItem reaches the end of the array,
         * it simply wraps back around to index 0 and continues searching.
         * As long as the array never fills completely, every search is guaranteed to finish either by locating the desired key, or by locating an entry whose key is zero,
         * which means that the desired key does not exist in the hash table.
         * This technique is referred to as open addressing with linear probing:
         * https://en.wikipedia.org/wiki/Open_addressing
         * https://en.wikipedia.org/wiki/Linear_probing
         */
        void SetItem(const std::uint32_t key, const std::uint32_t value);
        std::uint32_t GetItem(const std::uint32_t key) const;
    
        /**
         * Method returns a full copy of the filled hash entries 
         */
        std::vector<std::pair<std::uint32_t, std::uint32_t>> GetPrivateCopyOfContent() const;
    };

}