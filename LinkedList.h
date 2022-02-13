#include <cstddef>

namespace ll {
	// Simple Linked-List node
	template<class T>
	class Node {
	public:
		Node<T> *next = nullptr;
		T val;
		Node(T val) : val(val) {}
	};

	// Linked-List implementation
	template<class T>
	class LinkedList {
	private:
		Node<T> *head = nullptr;
		size_t curSize;
	public:
		// Construct a new Linked-List
		LinkedList() : curSize(0) {}

		// Add element to head
		// Returns new head
		Node<T> *add(T val) {
			Node<T> *newHead = new Node<T>(val);
			newHead->next = head;
			head = newHead;
			curSize++;
			return head;
		}

		// Remove an element by value
		// Returns whether or not the value was successfully removed
		bool remove(T val) {
			// If we are empty, exit early
			if (curSize == 0) return false;

			// If we are removing just the head, do it
			if (head->val == val) {
				Node<T> *oldHead = head;
				head = head->next;
				delete oldHead;
				curSize--;
				return true;
			}

			// Normal deletion protocol
			Node<T> *curNode = head;
			while (curNode->next != nullptr && curNode->next->val != val)
				curNode = curNode->next;

			if (curNode->next == nullptr) return false;

			Node<T> *removedNode = curNode->next;
			curNode->next = curNode->next->next;
			delete removedNode;
			curSize--;
			return true;
		}

		// Returns the value if it exists, null otherwise
		T *get(T val) {
			// If we are empty, exit early
			if (curSize == 0) return nullptr;

			// Check for existence
			Node<T> *curNode = head;
			while (curNode != nullptr && curNode->val != val)
				curNode = curNode->next;

			return curNode == nullptr ? nullptr : &(curNode->val);
		}

		// Get the current size
		size_t size() { return curSize; }
	};
};
