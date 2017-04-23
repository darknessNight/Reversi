#pragma once

#include <shared_mutex>
#include <atomic>

namespace darknessNight::Multithreading
{
	class shared_mutex_lock_priority
	{
		std::atomic<bool> acceptShared = true;
		std::shared_mutex mutex;
	public:
		bool try_lock()
		{
			return mutex.try_lock();
		}

		void lock()
		{
			acceptShared = false;
			mutex.lock();
		}

		void unlock()
		{
			mutex.unlock();
			acceptShared = true;
		}

		bool try_lock_shared()
		{
			if (!acceptShared) return false;
			return mutex.try_lock_shared();
		}

		void lock_shared()
		{
			while (!acceptShared)
				std::this_thread::sleep_for(std::chrono::microseconds(100));
			mutex.lock_shared();
		}

		void unlock_shared()
		{
			mutex.unlock_shared();
		}
	};
}
