#include <thread>
#include <vector>
#include <atomic>
#include <assert.h>
#include "Semaphore.h"
#include "LinkedList.h"

// Hashmap abstract
template<class K, class V>
class IHashmap {
public:
    virtual void put(const K &key, const V &val) = 0;
    virtual std::pair<bool, V> get(const K &key) const = 0;
};

// Thread safe hashmap
namespace tshm {

	// Entry in the hashmap
	template<class K, class V>
	struct Entry {
		K key;
		V val;
		Entry() {}
		// Just key constructor for lookups
		Entry(K key) : key(key) {}
		Entry(K key, V val) : key(key), val(val) {}

		/*
		 * We equate entries on key so that we don't have duplicates.
		 * This also interfaces nicely with the underlying data structure
		 * as we can easy replace items on equality
		 */
		bool operator==(const Entry &a) const { return key == a.key; }
	};

    /* Hashmap where we do *not* manage threads for the user
     *
     * Operations are sequentially consistent, but behavior
     * between close gets and sets is not defined
     */
    template<class K, class V, class F = std::hash<K>>
    class Hashmap : IHashmap<K, V> {
        // Less typing later
        typedef Entry<K, V> TypedEntry;

    private:
        // Private member variables
        uint capacity;
        F hash;
        std::vector<ll::LockableLL<TypedEntry>> hashmap;

        // Wrapper method to extract index from key
        size_t getHashedIndex(const K &key) const {
            return hash(key) % capacity;
        }

    public:
        // Construct hashmap
        // Use a prime capcacity
        Hashmap(uint capacity) : capacity(capacity), hashmap(capacity) {}

        // Nothing really interesting about the destructor
        virtual ~Hashmap() {}

        // Associate specified key with specified value
        void put(const K &key, const V &val) {
            size_t index = getHashedIndex(key);
            hashmap[index].add(TypedEntry(key, val));
        }

        // Return the status of containment and value
        std::pair<bool, V> get(const K &key) const {
			size_t index = getHashedIndex(key);

            TypedEntry entry(key);
			if (hashmap[index].find(entry))
                return {true, entry.val};

            return {false, V{}};
        }
    };

	/* Hashmap with managed threads
	 *
	 * We will allow the user to queue up
	 * as they would like to, but upon context switching from
	 * 'put's to 'get's, we require that all actions of the previous
	 * context fully finish execution.
	 */
	// types<Key, Value, HashFunction>
	template<class K, class V, class F = std::hash<K>>
	class ManagedHashmap : IHashmap<K, V> {
		// Less typing later
		typedef Entry<K, V> TypedEntry;

	private:
		// Private member variables
		uint capacity;
		F hash;
        std::vector<ll::LockableLL<TypedEntry>> hashmap;

		// We will using a counting semaphore to limit our job count
        semaphore::CountingSemaphore threadLock;

		// Wrapper method to extract index from key
		size_t getHashedIndex(const K &key) const {
            return hash(key) % capacity;
        }

	public:
		// Construct a new managed hashmap
		// Use a prime capacity
		ManagedHashmap(
			uint capacity, uint maxWorkerThreads = 4
		) : capacity(capacity), hashmap(capacity),
			threadLock(maxWorkerThreads) {}

		// On destruct, wait for all operations to finish
		virtual ~ManagedHashmap() { while (threadLock.active); }

		// Associate specified key with specified value
		void put(const K &key, const V &val) {
			// Acquire the thread semaphore
			threadLock.acquire();

			// Spawn new thread
            std::thread t([&] (TypedEntry entry) {
				size_t index = getHashedIndex(entry.key);
				hashmap[index].add(entry);
				threadLock.release();
			}, TypedEntry(key, val));

			// Detach the thread
			t.detach();
		}

		/*
		 * Get the pointer to the value associated with a key.
		 * Note that this is sequential for now, as it doesn't make much
		 * sense for a managed object to be fully async. We would need to implement
		 * promises, which might happen at a later date.

		 * TODO: Implement promises
		 */
        std::pair<bool, V> get(const K &key) const {
			// Spin until all puts are done
			while (threadLock.active);

			// Get item
			size_t index = getHashedIndex(key);
            TypedEntry entry(key);
            if (hashmap[index].find(entry))
                return {true, entry.val};

            return {false, V{}};
		}
	};
};
