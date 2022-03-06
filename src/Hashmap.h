#include <thread>
#include <vector>
#include <atomic>
#include <assert.h>
#include "Semaphore.h"
#include "LinkedList.h"

using std::hash;
using std::thread;
using std::vector;
using std::atomic_uint;
using std::atomic_int;
using ll::LockableLinkedList;
using semaphore::CountingSemaphore;

// Wait-free Hashmap
namespace wfhm {

	// Entry in the hashmap
	template<class K, class V>
	struct Entry {
		K key;
		V value;
		Entry() {}
		// Just key constructor for lookups
		Entry(K key) : key(key) {}
		Entry(K key, V value) : key(key), value(value) {}

		/*
		 * We equate entries on key so that we don't have duplicates.
		 * This also interfaces nicely with the underlying data structure
		 * as we can easy replace items on equality
		 */
		bool operator==(const Entry &a) const {
			return key == a.key;
		}
	};

    /* Hashmap where we do *not* manage threads for the user
     *
     * Operations are sequentially consistent, but behavior
     * between close gets and sets is not defined
     */
    template<class K, class V, class F = hash<K>>
    class Hashmap {
        // Less typing later
        typedef Entry<K, V> TypedEntry;

    private:
        // Private member variables
        uint capacity;
        F hash;
        vector<LockableLinkedList<TypedEntry>> hashmap;

        // Wrapper method to extract index from key
        size_t getHashedIndex(K key) { return hash(key) % capacity; }

    public:
        // Construct hashmap
        // Use a prime capcacity
        Hashmap(uint capacity, uint maxWorkerThreads = 4) : capacity(capacity), hashmap(capacity) {}

        // Associate specified key with specified value
        void put(const K key, const V value) {
            size_t index = getHashedIndex(key);
            hashmap[index].add(TypedEntry(key, value));
        }

        // Get a pointer to the value in the map
        V *get(const K key) {
			// Get item
			size_t index = getHashedIndex(key);
			auto entry = hashmap[index].get(TypedEntry(key));

			// Return if it exists
			return entry == nullptr ? nullptr : &(entry->value);
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
	template<class K, class V, class F = hash<K>>
	class ManagedHashmap {
		// Less typing later
		typedef Entry<K, V> TypedEntry;

	private:
		// Private member variables
		uint capacity;
		F hash;
		vector<LockableLinkedList<TypedEntry>> hashmap;

		// We will using a counting semaphore to limit our job count
		CountingSemaphore threadLock;

		// Wrapper method to extract index from key
		size_t getHashedIndex(K key) { return hash(key) % capacity; }

	public:
		// Construct a new managed hashmap
		// Use a prime capacity
		ManagedHashmap(
			uint capacity, uint maxWorkerThreads = 4
		) : capacity(capacity), hashmap(capacity),
			threadLock(maxWorkerThreads) {}

		// On destruct, wait for all operations to finish
		~ManagedHashmap() { while (threadLock.active); }

		// Associate specified key with specified value
		void put(const K key, const V value) {
			// Acquire the thread semaphore
			threadLock.acquire();

			// Spawn new thread
			thread t([&] (TypedEntry entry) {
				size_t index = getHashedIndex(entry.key);
				hashmap[index].add(entry);
				threadLock.release();
			}, TypedEntry(key, value));

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
		V *getSequential(const K key) {
			// Spin until all puts are done
			while (threadLock.active);

			// Get item
			size_t index = getHashedIndex(key);
			auto entry = hashmap[index].get(TypedEntry(key));

			// Return if it exists
			return entry == nullptr ? nullptr : &(entry->value);
		}
	};
};
