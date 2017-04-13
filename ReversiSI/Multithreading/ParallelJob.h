#pragma once
#include <vector>
#include <list>
#include "Semaphore.h"

namespace darknessNight::Multithreading
{
	class ParallelJob
	{
		unsigned threadNumber;
		Semaphore semaphore;
		std::mutex elementsMutex;
		std::list<std::shared_ptr<std::thread>> detachedThreads;
		std::mutex threadsMutex;
	public:
		ParallelJob() : threadNumber(GetCPUNumberOfThreads() - 1), semaphore(threadNumber)
		{
		}

		template <typename T>
		void ForEach(std::function<void(T&)> func, std::vector<T> &elements)
		{
			auto it = elements.begin();
			auto end = elements.end();

			std::function<void()> task = [&]()
			{
				std::vector<std::shared_ptr<std::thread>> threads;
				while (true) {
					auto element = GetElementAndIncrementIterator<T>(it, end);

					if (element == nullptr) {
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

		template <typename T>
		void ForEachDetach(std::function<void(T&)> func, std::vector<T> &elements)
		{
			std::lock_guard<std::mutex> lock(threadsMutex);
			detachedThreads.push_back(std::make_shared<std::thread>([&elements,func,this]()
			{
				ForEach(func, elements);
				std::lock_guard<std::mutex> lock(threadsMutex);
				detachedThreads.remove_if([](std::shared_ptr<std::thread> th) {return th->get_id() == std::this_thread::get_id(); });
			}));
		}

		void Stop()
		{
			std::lock_guard<std::mutex> lock(threadsMutex);
			detachedThreads.clear();
		}

		void WaitForDetached()
		{
			threadsMutex.lock();
			auto copy = detachedThreads;
			threadsMutex.unlock();
			for (auto thread : copy)
				if (thread->joinable())
					thread->join();
		}

		void SetNumberOfThreads(unsigned newThreadNumber)
		{
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