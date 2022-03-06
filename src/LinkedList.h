#include <iostream>
#include <cstddef>
#include <mutex>
#include <atomic>

using std::mutex;
using std::atomic_size_t;

namespace ll {

	// Linked-List implementation
	template<class T>
	class LockableLinkedList {

	private:
        // Lockable Linked-List node
        class LockableNode {
        private:
            // Lock
            mutex mtx;

        public:
            // Member variables
            LockableNode *next = nullptr;
            T val;

            // Construct
            LockableNode() {} // Dummy node for head
            LockableNode(T val) : val(val) {}

            // Wrappers for thread control
            void lock() { mtx.lock(); }
            void unlock() { mtx.unlock(); }

            // Lock the next node and return it
            LockableNode *getNextAndLock() {
                if (next == nullptr)
                    return nullptr;
                next->mtx.lock();
                return next;
            }
        };

		// Member variables
		LockableNode *head = new LockableNode();
		atomic_size_t curSize;

	public:
		// Construct a new Linked-List
		LockableLinkedList() : curSize(0) {}

		// Destructor, free all nodes
		~LockableLinkedList() {
			// Obtain lock on head
			LockableNode *mover = head;
			mover->lock();

			// Free everything
			while (mover != nullptr) {
				LockableNode *next = mover->getNextAndLock();

				mover->unlock();
				delete mover;

				mover = next;
			}
		}

		// Returns the head. Not thread safe
		LockableNode *DANGEROUS_getHead() { return head; }

		// Add new element to the linked list
		void add(T val) {
			// Maintain lock on cur node
			LockableNode *mover = head;
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
				LockableNode *next = mover->getNextAndLock();
				// Stop if we've reached the end
				if (next == nullptr)
					break;

				// Unlock and move
				mover->unlock();
				mover = next;
				isHead = false;
			}

			// Insert at end
			mover->next = new LockableNode(val);
			mover->unlock();
			curSize++;
		}

		// Remove an element
		// Returns whether or not the value was successfully removed
		bool remove(T val) {
			// Maintain lock on current node
			LockableNode *mover = head;
			mover->lock();

			// Traverse, look for node to remove
			while (true) {
				// Get the next node
			    LockableNode *next = mover->getNextAndLock();

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
		    LockableNode *mover = head->next;
			mover->lock();

			// Check for existence
			while (true) {
				if (mover->val == val) {
					T *ret = &(mover->val);
					mover->unlock();
					return ret;
				}

				// Get next and break if done
		        LockableNode *next = mover->getNextAndLock();

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
