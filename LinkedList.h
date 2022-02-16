#include <iostream>
#include <cstddef>
#include <mutex>
#include <atomic>

using std::mutex;
using std::atomic_size_t;

namespace ll {

	// Simple Linked-List node
	template<class T>
	class Node {
	private:
		// Lock
		mutex mtx;

	public:
		// Member variables
		Node<T> *next = nullptr;
		T val;

		// Construct
		Node() {} // Dummy node for head
		Node(T val) : val(val) {}

		// Wrappers for thread control
		void lock() { mtx.lock(); }
		void unlock() { mtx.unlock(); }

		// Lock the next node and return it
		Node<T> *getNextAndLock() {
			if (next == nullptr)
				return nullptr;
			next->mtx.lock();
			return next;
		}
	};

	// Linked-List implementation
	template<class T>
	class LinkedList {
	private:
		// Member variables
		Node<T> *head = new Node<T>();
		atomic_size_t curSize;

	public:
		// Construct a new Linked-List
		LinkedList() : curSize(0) {}

		// Returns the head. Not thread safe
		Node<T> *DANGEROUS_getHead() { return head; }

		// Add new element to the linked list
		void add(T val) {
			// Maintain lock on cur node
			Node<T> *mover = head;
			mover->lock();

			// Traverse the list
			bool isHead = true;
			while (true) {
				// If we find it, update
				if (!isHead && mover->val == val) {
					mover->val = val;
					mover->unlock();
					curSize++;
					return;
				}

				// Lock our next node (if it exists)
				Node<T> *next = mover->getNextAndLock();
				// Stop if we've reached the end
				if (next == nullptr)
					break;

				// Unlock and move
				mover->unlock();
				mover = next;
				isHead = false;
			}

			// Insert at end
			mover->next = new Node<T>(val);
			mover->unlock();
			curSize++;
		}

		// Remove an element
		// Returns whether or not the value was successfully removed
		bool remove(T val) {
			// Maintain lock on current node
			Node<T> *mover = head;
			mover->lock();

			// Traverse, look for node to remove
			while (true) {
				// Get the next node
				Node<T> *next = mover->getNextAndLock();

				// Break if we're done
				if (next == nullptr) {
					mover->unlock();
					return false;
				}

				// Found it, remove
				if (next->val == val) {
					mover->next = next->next;
					mover->unlock();
					next->unlock();

					delete next;
					curSize--;

					return true;
				}

				// Unlock and move
				mover->unlock();
				mover = next;
			}

			mover->unlock();
			return false;
		}

		// Returns the value if it exists, null otherwise
		T *get(T val) {
			// Empty list
			if (head->next == nullptr)
				return nullptr;

			// Maintain lock on current node
			Node<T> *mover = head->next;
			mover->lock();

			// Check for existence
			while (true) {
				if (mover->val == val) {
					T *ret = &(mover->val);
					mover->unlock();
					return ret;
				}

				// Get next and break if done
				Node<T> *next = mover->getNextAndLock();

				if (next == nullptr) {
					mover->unlock();
					return nullptr;
				}

				// Move
				mover->unlock();
				mover = next;
			}
		}

		// Get the current size
		size_t size() { return curSize; }
	};
};
