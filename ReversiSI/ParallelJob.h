#pragma once
#include <shared_mutex>

namespace darknessNight::Multithreading
{
	class ParallerJob
	{
		unsigned threadCount;
		unsigned CPUThreadNumber;
		bool runOnGPU = false;
		std::shared_mutex mutex;
	public:
		ParallerJob()
		{}
	};
}