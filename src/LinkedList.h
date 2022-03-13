#include <iostream>
#include <cstddef>
#include <mutex>
#include <atomic>
#include <vector>
#include <climits>
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

        // Member vars
        MNode head;
        F hash;
		atomic_size_t curSize;

    public:
        // Constructor
        LockFreeLinkedList() : head(new Node(T{}, INT_MIN)), curSize(0) {
            // Make a head and tail with min and max values for easy inserting
            head.getRef()->next = MNode(new Node(T{}, INT_MAX));
        }

		// Destructor, free all nodes
		~LockFreeLinkedList() {
            MNode curr = head;
            while (curr.getRef() != nullptr) {
                MNode next = curr.getRef()->next;
                delete curr.getRef();
                curr = next;
            }
		}

		// Returns the head. Not thread safe
		MNode DANGEROUS_getHead() { return head; }

        // Find a desired window, fixing logically deleted nodes along the way
        Window find(MNode head, int key) {
            MNode pred, curr, succ;

            // Traverse from beginning of list
            retry: while (true) {
                // Guaranteed to exist, at least two nodes in list
                pred = head;
                curr = pred.getRef()->next;

                while (true) {
                    // Move forwards, and delete if necessary
                    succ = curr.getRef()->next;

                    // Might not exist yet, if it does check it's mark
                    while (succ.getMark()) {

                        // Try to physically delete the logically deleted node
                        Node *expectedRef = curr.getRef();
                        bool expectedMark = false;
                        Node *requiredRef = succ.getRef();
                        bool requiredMark = false;
                        bool snip = pred.getRef()->next.compareExchangeBothWeak(
                            expectedRef,
                            expectedMark,
                            requiredRef,
                            requiredMark
                        );

                        if (!snip) goto retry;

                        Node *toDelete = curr.getRef();

                        curr = curr.getRef()->next;
                        succ = succ.getRef()->next;

                        // Delete the old node
                        delete toDelete;
                    }

                    if (curr.getRef()->key >= key)
                        return Window(pred, curr);

                    pred = curr;
                    curr = succ;
                }
            }
        }

        // Add new item to list
        bool add(T item) {
            int key = hash(item);
            Node *toBeAdded = new Node(item, key);

            while (true) {
                // Find the node
                Window window = find(head, key);
                MNode pred = window.pred, curr = window.curr;

                // If it already exists, exit
                if (curr.getRef()->key == key) return false;

                // Attempt to link new node it in with CAS
                MNode node = MNode(toBeAdded);
                node.getRef()->next = curr;

                Node *expectedRef = curr.getRef();
                bool expectedMark = false;
                Node *requiredRef = node.getRef();
                bool requiredMark = false;

                if (pred.getRef()->next.compareExchangeBothWeak(
                    expectedRef,
                    expectedMark,
                    requiredRef,
                    requiredMark
                )) {
                    curSize++;
                    return true;
                }
            }
        }

        // Remove item from list
        bool remove(T item) {
            int key = hash(item);

            bool snip = false;
            while (true) {
                // Find the item
                Window window = find(head, key);
                // MNode *pred = window.pred;
                MNode curr = window.curr;

                // If we didn't find it, stop
                if (curr.getRef()->key != key) return false;

                // Attempt to logically delete the node
                MNode succ = curr.getRef()->next;

                Node *expectedRef = succ.getRef();
                bool expectedMark = false;
                Node *requiredRef = succ.getRef();
                bool requiredMark = true;

                snip = curr.getRef()->next.compareExchangeBothWeak(
                    expectedRef,
                    expectedMark,
                    requiredRef,
                    requiredMark
                );

                if (!snip) continue;

                // TODO: MAKE THIS WORK!
                //
                // For some reason I don't understand,
                // we cannot physically delete these yet
                // They're reference seems to be used later
                //
                //

                // On logical deletion success, attempt a physical deletion
                // expectedRef = curr->getRef();
                // expectedMark = false;
                // requiredRef = succ->getRef();
                // requiredMark = false;

                // Attempt physcial deletion
                // snip = pred->getRef()->next->compareExchangeBothWeak(
                    // expectedRef,
                    // expectedMark,
                    // requiredRef,
                    // requiredMark
                // );

                // If physical was successful, free the mem
                // if (snip) delete expectedRef;

                curSize--;
                return true;
            }
        }

        // Returns true if the item is in the list. Stores found item in
        // argument
        bool contains(T &item) {
            int key = hash(item);
            MNode curr = head;

            while (curr.getRef()->key < key)
                curr = curr.getRef()->next;
            bool marked = curr.getRef()->next.getMark();

            if (curr.getRef()->key == key && !marked) {
                item = curr.getRef()->val;
                return true;
            } return false;
        }

		// Get the current size
		size_t size() { return curSize; }
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
