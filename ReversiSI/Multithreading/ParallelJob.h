#pragma once
#include <vector>
#include "Semaphore.h"

namespace darknessNight::Multithreading
{
	class ParallerJob
	{
		unsigned threadCount;
		unsigned CPUThreadNumber;
		Semaphore semaphore;
		std::mutex elementsMutex;
	public:
		ParallerJob()
		{}
		typedef int T;
		//template <typename T>
		void DoForEach(std::function<void(T&)> func, std::vector<T> elements)
		{

		}

		void SetCountOfThreads(unsigned newThreadCount)
		{
			threadCount = newThreadCount;
		}

		void SetCountOfThreadsToMax()
		{

		}

		unsigned GetCountOfThreads()
		{

		}

		unsigned GetCPUCountOfThreads()
		{

		}

		unsigned GetGPUCountOfThreads()
		{
			return 0;
		}

		void SwitchToGPU()
		{
			throw std::runtime_error("GPU not supported");
		}

		void SwitchToCPU() {}
	};
}