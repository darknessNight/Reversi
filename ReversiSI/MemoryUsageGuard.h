#pragma once
#include "Windows.h"
#include "psapi.h"
#include <thread>

namespace darknessNight
{
	class MemoryUsageGuard
	{
	private:
		long long memoryBuffor = 1024 * 1024 * 1024;//1GB
	public:
		static long long GetMemoryLimit()
		{
			MEMORYSTATUSEX memInfo;
			memInfo.dwLength = sizeof(MEMORYSTATUSEX);
			GlobalMemoryStatusEx(&memInfo);
			auto totalVirtualMem = memInfo.ullTotalPageFile;
			return totalVirtualMem;
		}

		static long long GetAllUsedMemory()
		{
			MEMORYSTATUSEX memInfo;
			memInfo.dwLength = sizeof(MEMORYSTATUSEX);
			GlobalMemoryStatusEx(&memInfo);
			auto virtualMemUsed = memInfo.ullTotalPageFile - memInfo.ullAvailPageFile;
			return virtualMemUsed;
		}

		static long long GetAllAvailableMemory()
		{
			MEMORYSTATUSEX memInfo;
			memInfo.dwLength = sizeof(MEMORYSTATUSEX);
			GlobalMemoryStatusEx(&memInfo);
			return memInfo.ullAvailPageFile;
		}

		static long long GetMemoryUsedByCurrentProcess()
		{
			PROCESS_MEMORY_COUNTERS_EX pmc;
			GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PPROCESS_MEMORY_COUNTERS>(&pmc), sizeof(pmc));
			return pmc.PrivateUsage;
		}

		long long GetMemoryBufforSize() const
		{
			return memoryBuffor;
		}

		void SetMemoryBufforSize(long long size)
		{
			memoryBuffor = size;
		}
		
		void WaitForAvailableMemeory() const
		{
			while (GetAllAvailableMemory() < memoryBuffor)
				std::this_thread::sleep_for(std::chrono::microseconds(100));
		}
	};
}
