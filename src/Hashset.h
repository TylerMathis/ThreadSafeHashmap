#include <vector>
#include "LinkedList.h"

// Hashset abstract
template<class T>
class IHashset {
	public:
		virtual void insert(const T &item) = 0;
		virtual bool contains(T item) const = 0;
};

// Thread safe hashset
namespace tshs {

	/* Thread Safe Hashmap
	 *
	 * Operations are sequentially consistent, but behavior
	 * between close gets and sets is not defined
	 */
	template<
		class T,
			  template<class> class Container = ll::AddOnlyLockFreeLL,
			  class F = std::hash<T>>
				  class Hashset : IHashset<T> {
					  private:
						  // Private member variables
						  uint capacity;
						  F hash;
						  std::vector<Container<T>> hashset;

						  // Wrapper method to extract index from key
						  size_t getHashedIndex(const T &item) const {
							  return hash(item) % capacity;
						  }

					  public:
						  // Construct hashmap
						  // Use a prime capcacity
						  Hashset(uint capacity) : capacity(capacity), hashset(capacity) {}

						  // Nothing really interesting about the destructor
						  virtual ~Hashset() {}

						  // Associate specified key with specified value
						  void insert(const T &item) {
							  size_t index = getHashedIndex(item);
							  hashset[index].add(item);
						  }

						  // Returns whether the item is in the Hashset
						  bool contains(T item) const {
							  size_t index = getHashedIndex(item);
							  return hashset[index].find(item);
						  }
				  };
};
