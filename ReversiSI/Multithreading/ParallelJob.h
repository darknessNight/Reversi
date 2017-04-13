#pragma once
#include <vector>
#include "Semaphore.h"

namespace darknessNight::Multithreading
{
	class ParallelJob
	{
		unsigned threadNumber;
		Semaphore semaphore;
		std::mutex elementsMutex;
	public:
		ParallelJob() : threadNumber(GetCPUNumberOfThreads() - 1), semaphore(threadNumber)
		{
		}

		typedef int T;
		//template <typename T>
		void ForEach(std::function<void(T&)> func, std::vector<T> &elements)
		{
			auto it = elements.begin();
			auto end = elements.end();

			std::function<void()> task = [&]()
			{
				std::vector<std::shared_ptr<std::thread>> threads;
				while (true) {
					elementsMutex.lock();
					T* element = nullptr;
					if (it != end) {
						element = &*it;
						it++;
					}
					elementsMutex.unlock();

					if (element == nullptr) {
						for (auto thread : threads)
							if (thread->joinable())
								thread->join();
						return;
					}

					if (semaphore.try_lock())
					{
						threads.push_back(std::make_shared<std::thread>([&](T* el) { func(*el);  task(); }, element));
					}
					else
					{
						func(*element);
					}
				}

			};

			task();
		}
	public:

		//template <typename T>
		void ForEachDetach(std::function<void(T&)> func, std::vector<T> &elements)
		{

		}

		void Stop()
		{

		}

		void SetNumberOfThreads(unsigned newThreadNumber)
		{
			threadNumber = newThreadNumber;
			semaphore.ChangeAccessLimit(threadNumber);
		}

		void SetNumberOfThreadsToMax()
		{
			SetNumberOfThreads(GetCPUNumberOfThreads());
		}

		unsigned GetNumberOfThreads()const
		{
			return threadNumber;
		}

		static unsigned GetCPUNumberOfThreads()
		{
			auto Number = std::thread::hardware_concurrency();
			return Number == 0 ? 1 : Number;
		}
	};
}