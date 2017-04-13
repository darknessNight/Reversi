#include "stdafx.h"
#include "CppUnitTest.h"
#include "../ReversiSI/Multithreading/ParallelJob.h"
#include <sstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace darknessNight::Multithreading;

namespace Microsoft::VisualStudio::CppUnitTestFramework
{
	template<> inline std::wstring ToString<std::vector<int>>(const std::vector<int>&el) {
		std::wstringstream stream;
		for (auto i : el)
		{
			stream << L"(" << i << L"), ";
		}
		return stream.str();
	}
}

namespace SI::Reversi::Tests
{
	TEST_CLASS(SemaphoreTests)
	{
	public:
		TEST_METHOD(GetCPUCountOfThreads_CheckReturnSomePositiveNumber)
		{
			auto result = ParallelJob::GetCPUNumberOfThreads();
			Assert::AreNotEqual(0u, result);
		}

		TEST_METHOD(SetNumberOfThreads_CheckReturnCorrectNumber)
		{
			ParallelJob jobs;
			jobs.SetNumberOfThreads(8);
			auto result = jobs.GetNumberOfThreads();
			Assert::AreEqual(8u, result);
		}

		TEST_METHOD(SetNumberOfThreadsToMax_CheckReturnCorrectNumber)
		{
			ParallelJob jobs;
			jobs.SetNumberOfThreads(8);
			auto result = jobs.GetNumberOfThreads();
			auto expected = ParallelJob::GetCPUNumberOfThreads();
			Assert::AreEqual(expected, result);
		}

		TEST_METHOD(ForEach_HasOneThreadParallelJob_CheckDoActionForEachElement)
		{
			ParallelJob jobs;
			jobs.SetNumberOfThreads(1);
			std::vector<int> elements = { 1,2,3,4,5,6,7,8,9,10 };
			jobs.ForEach([](int &el) { el++; }, elements);
			Assert::AreEqual(std::vector<int>({ 2,3,4,5,6,7,8,9,10,11 }), elements);
		}

		TEST_METHOD(ForEach_HasTwoThreadParallelJobAndSleepAction_CheckDoActionForTwoElements)
		{
			ParallelJob jobs;
			jobs.SetNumberOfThreads(1);
			std::vector<int> elements = { 1,2,3,4 };
			
				std::thread thread([&]() {jobs.ForEach([](int &el)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(10));
					el++;
					std::this_thread::sleep_for(std::chrono::milliseconds(50));
				}, elements);
				});

				std::this_thread::sleep_for(std::chrono::milliseconds(15));
			


			auto result = elements;
			std::vector<int> expected({ 2,3,4,5 });

			auto resultMatch = 0;
			for (auto i = 0; i < expected.size(); i++)
				if (expected[i] == result[i])
					resultMatch++;

			if (thread.joinable())
				thread.join();
			Assert::AreEqual(2, resultMatch);
		}
	};
}