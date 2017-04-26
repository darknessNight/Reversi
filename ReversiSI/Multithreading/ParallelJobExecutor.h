#pragma once
#include <vector>
#include <list>
#include "Semaphore.h"

namespace darknessNight::Multithreading
{
	class ParallelJobExecutor
	{
		unsigned threadNumber;
		Semaphore semaphore;
		std::mutex elementsMutex;
		std::mutex threadsMutex;
		bool working;
	public:
		ParallelJobExecutor() : threadNumber(GetCPUNumberOfThreads() - 1), semaphore(threadNumber)
		{
		}

		ParallelJobExecutor(unsigned number) : threadNumber(number - 1), semaphore(threadNumber)
		{
		}

		template <typename T>
		void ForEach(std::function<void(T&)> func, std::vector<T> &elements)
		{
			auto it = elements.begin();
			auto end = elements.end();
			working = true;

			std::function<void()> task = [&]()
			{
				std::vector<std::shared_ptr<std::thread>> threads;
				while (true) {
					auto element = GetElementAndIncrementIterator<T>(it, end);

					if (element == nullptr || working==false) {
						WaitForMyThreads(threads);
						return;
					}

					if (semaphore.try_lock())
					{
						threads.push_back(std::make_shared<std::thread>(GetNewTaskProc(task,func), element));
					}
					else
					{
						func(*element);
					}
				}
			};

			task();
		}
	private:
		template <typename T> T* GetElementAndIncrementIterator(typename std::vector<T>::iterator &it, typename std::vector<T>::iterator &end)
		{
			T* element = nullptr;
			elementsMutex.lock();
			if (it != end) {
				element = &*it;
				++it;
			}
			elementsMutex.unlock();
			return element;
		}

		void WaitForMyThreads(std::vector<std::shared_ptr<std::thread>> &threads) const
		{
			for (auto thread : threads)
				if (thread->joinable())
					thread->join();
		}

		template <typename T> std::function<void(T*)> GetNewTaskProc(std::function<void()> &task, std::function<void(T&)> &func)
		{
			return [&](T* el) {
				SemaphoreUnlocker unlocker(semaphore);
				func(*el);
				task();
			};
		}

	public:

		void Stop()
		{
			working = false;
		}

		void SetNumberOfThreads(unsigned newThreadNumber)
		{
			if ( newThreadNumber <= 0 )newThreadNumber = 1;
			threadNumber = newThreadNumber;
			semaphore.ChangeAccessLimit(threadNumber - 1);
		}

		void SetNumberOfThreadsToMax()
		{
			SetNumberOfThreads(GetCPUNumberOfThreads());
		}

		unsigned GetNumberOfThreads()const
		{
			return threadNumber;
		}

		unsigned GetCountOfFreeThreads() const
		{
			return semaphore.GetFreeSlotNumber() + 1;
		}

		static unsigned GetCPUNumberOfThreads()
		{
			auto Number = std::thread::hardware_concurrency();
			return Number == 0 ? 1 : Number;
		}
	};
}