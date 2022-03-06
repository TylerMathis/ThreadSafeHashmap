#include <assert.h>
#include <mutex>
#include <condition_variable>
#include <atomic>

using std::mutex;
using std::unique_lock;
using std::lock_guard;
using std::condition_variable;
using std::atomic_uint;

namespace semaphore {
	class CountingSemaphore {
	private:
		mutex mtx;
		condition_variable cnd;
		atomic_uint count = 0;
	public:
		atomic_uint active = 0;

		CountingSemaphore(unsigned count = 0) :
			count(count) {}

		void acquire() {
			unique_lock<decltype(mtx)> lock(mtx);
			while (!count)
				cnd.wait(lock);
			--count;
			++active;
		}

		void release() {
			lock_guard<decltype(mtx)> lock(mtx);
			++count;
			--active;
			cnd.notify_one();
		}
	};
};
