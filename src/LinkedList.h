#include <iostream>
#include <cstddef>
#include <mutex>
#include <atomic>
#include <vector>
#include <limits>
#include "MarkableReference.h"

using std::mutex;
using std::atomic_size_t;
using std::uintptr_t;
using std::hash;

namespace ll {

    // Lock Free Linked-List implementation
    // Inspired by The Art of Multiprocessor Programming
    // Chapter 9.8
    template<class T, class F = hash<T>>
    class LockFreeLinkedList {

    private:
        // Regular Linked-List node
        class Node {

        public:
            MarkableReference<Node> next;
            T val;
            int key;

            // Construct
            Node() {} // Dummy node for head
            Node(T val, int key) : val(val), key(key) {}
        };

        // Less typing later
        typedef MarkableReference<Node> MNode;

        // Holds a pair of pointers
        class Window {
            public:
                MNode pred, curr;
                Window(MNode pred, MNode curr)
                    : pred(pred), curr(curr) {}
        };

        // Member variables
        F hash;
        MNode head;

    public:
        // Constructor
        LockFreeLinkedList() {
            // Make a head and tail with min and max values for easy inserting
            head = MNode(new Node(T{}, INT_MIN));
            head.getRef()->next = MNode(new Node(T{}, INT_MAX));
        }

        // Find a desired window, fixing logically deleted nodes along the way
        Window find(MNode head, int key) {
            MNode pred, curr, succ;

            bool marked;
            bool snip;
            retry: while (true) {
                pred = head;
                curr = pred.getRef()->next.getRef();
                while (true) {
                    succ = curr.getRef()->next.getBoth(marked);
                    while (marked) {
                        snip = pred.getRef()->next.compareExchangeBothWeak(curr.getRef(), succ.getRef(), false, false);

                        if (!snip) goto retry;

                        curr = succ;
                        succ = curr.getRef()->next.getBoth(marked);
                    }
                    if (curr.getRef()->key >= key)
                        return Window(pred, curr);
                    pred = curr;
                    curr = succ;
                }
            }
        }

        bool add(T item) {
            int key = hash(item);
            while (true) {
                Window window = find(head, key);
                MNode pred = window.pred, curr = window.curr;
                if (curr.getRef()->key == key) return false;

                MNode node(new Node(item, key));
                node.getRef()->next = MNode(curr);
                if (pred.getRef()->next.compareExchangeBothWeak(
                    curr.getRef(),
                    node.getRef(),
                    false,
                    false
                )) return true;
            }
        }

        bool remove(T item) {
            int key = hash(item);
            bool snip = false;
            while (true) {
                Window window = find(head, key);
                MNode pred = window.pred, curr = window.curr;
                if (curr.getRef()->key != key) return false;

                MNode succ = curr.getRef()->next.getRef();
                snip = curr.getRef()->next.compareExchangeBothWeak(
                    succ.getRef(),
                    succ.getRef(),
                    false,
                    true
                );

                if (!snip) continue;

                pred.getRef()->next.compareExchangeBothWeak(
                    curr.getRef(),
                    succ.getRef(),
                    false,
                    false
                );

                return true;
            }
        }

        bool contains(T item) {
            bool marked = false;
            int key = hash(item);
            MNode curr = head;

            while (curr.getRef()->key < key) {
                curr = curr.getRef()->next.getRef();
                MNode succ = curr.getRef()->next.getBoth(marked);
            }

            return (curr.getRef()->key == key && !marked);
        }
    };

	// Lockable Linked-List implementation
	template<class T>
	class LockableLinkedList {

	private:
        // Lockable Linked-List node
        class LockableNode {
        private:
            // Lock
            mutex mtx;

        public:
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
