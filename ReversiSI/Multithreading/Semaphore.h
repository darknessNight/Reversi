#pragma once
#include <shared_mutex>
#include <memory>

namespace darknessNight::Multithreading
{
	class Semaphore
	{
		std::shared_ptr<unsigned> counter;
		std::shared_ptr<unsigned> maxCount;
		std::shared_ptr<std::mutex> counterMutex;
	public:
		Semaphore():Semaphore(4){}

		Semaphore(unsigned count)
		{
			counter = std::make_shared<unsigned>(count);
			maxCount = std::make_shared<unsigned>(count);
			counterMutex = std::make_shared<std::mutex>();
		}

		void ChangeAccessLimit(unsigned limit)
		{
			counterMutex->lock();
			auto diff = *maxCount - *counter;
			*maxCount = limit;
			if ( limit >= diff )
				*counter = *maxCount - diff;
			else *counter = 0;
			counterMutex->unlock();
		}

		bool try_lock()
		{
			bool result = false;
			counterMutex->lock();
			if ( *counter > 0 )
			{
				(*counter)--;
				result = true;
			}
			counterMutex->unlock();
			return result;
		}

		void lock()
		{
			bool result = false;
			while ( true )
			{
				if ( try_lock() )
					break;
				std::this_thread::sleep_for(std::chrono::microseconds(100));
			}
		}

		void unlock()
		{
			counterMutex->lock();
			(*counter)=min(*counter+1,*maxCount);
			counterMutex->unlock();
		}

		unsigned GetFreeSlotNumber()const
		{
			std::lock_guard<std::mutex> lock(*counterMutex);
			return *counter;
		}

	private:
		unsigned min(unsigned a, unsigned b)
		{
			return a > b ? b : a;
		}
	};


	class SemaphoreUnlocker
	{
		Semaphore sem;
	public:
		explicit SemaphoreUnlocker(Semaphore& semaphore)
		{
			sem = semaphore;
		}
		~SemaphoreUnlocker()
		{
			sem.unlock();
		}
	};
}