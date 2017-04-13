#include "stdafx.h"
#include "CppUnitTest.h"
#include "../ReversiSI/Multithreading/Semaphore.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace darknessNight::Multithreading;

namespace SI::Reversi::Tests
{
	TEST_CLASS(SemaphoreTests)
	{
	public:
		TEST_METHOD(Lock_HasTwoAccessAndThreeTries_CheckReturnFalse)
		{
			Semaphore sem(2);
			sem.try_lock();
			sem.try_lock();
			auto result = sem.try_lock();
			Assert::IsFalse(result);
		}

		TEST_METHOD(Lock_HasThreeAccessWithTwoAccessSemaphore_CheckStack)
		{
			Semaphore sem(2);
			sem.lock();
			sem.unlock();
			sem.lock();
			sem.lock();
			std::thread th([&] () {
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
				sem.unlock();
			});
			bool flag = false;
			std::thread th2([&] () {
				sem.lock();
				flag = true;
				sem.unlock();
			});
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
			bool result = flag;
			sem.unlock();
			sem.unlock();
			if ( th.joinable() ) th.join();
			if ( th2.joinable() ) th2.join();
			Assert::IsFalse(result);
		}

		TEST_METHOD(Lock_HasThreeAccessAndOneReleaseWithTwoAccessSemaphore_CheckNotStack)
		{
			Semaphore sem(2);
			sem.lock();
			sem.unlock();
			sem.lock();
			sem.lock();
			std::thread th([&] () {
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
				sem.unlock();
			});
			bool flag = false;
			std::thread th2([&] () {
				sem.lock();
				flag = true;
				sem.unlock();
			});
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			bool result = flag;
			sem.unlock();
			sem.unlock();
			if ( th.joinable() ) th.join();
			if ( th2.joinable() ) th2.join();
			Assert::IsTrue(result);
		}

		TEST_METHOD(ChangeLimit_HasEmptySemaphore_CheckChanged)
		{
			Semaphore sem(2);
			sem.ChangeAccessLimit(4);
			int result = 0;
			while ( sem.try_lock() )result++;
			Assert::AreEqual(4,result);
		}

		TEST_METHOD(ChangeLimit_HasOneAccessedSemaphore_CheckChangedToNewLimitMinusOne)
		{
			Semaphore sem(2);
			sem.try_lock();
			sem.ChangeAccessLimit(4);
			int result = 0;
			while ( sem.try_lock() )result++;
			Assert::AreEqual(3, result);
		}

		TEST_METHOD(ChangeLimit_HasAllAccessedSemaphore_CheckChangedToZero)
		{
			Semaphore sem(4);
			for(int i=0;i<4;i++ )
				sem.try_lock();
			sem.ChangeAccessLimit(2);
			int result = 0;
			while ( sem.try_lock() )result++;
			Assert::AreEqual(0, result);
		}

		TEST_METHOD(ChangeLimit_HasAllAccessedSemaphore_CheckReleaseDidntChangeMaxCount)
		{
			Semaphore sem(4);
			for ( int i = 0; i<4; i++ )
				sem.try_lock();
			sem.ChangeAccessLimit(2);
			for ( int i = 0; i<4; i++ )
				sem.unlock();
			int result = 0;
			while ( sem.try_lock() )result++;
			Assert::AreEqual(2, result);
		}

		TEST_METHOD(Copy_HasTwoAccessedSemaphore_CheckCopyHasSameDatas)
		{
			Semaphore sem(4);
			for ( int i = 0; i<2; i++ )
				sem.try_lock();
			auto copy = sem;

			int result = 0;
			while ( copy.try_lock() )result++;
			Assert::AreEqual(2, result);
		}

		TEST_METHOD(Copy_HasTwoAccessedSemaphore_CheckOriginHasSameDataLikeCopy)
		{
			Semaphore sem(4);
			for ( int i = 0; i<2; i++ )
				sem.try_lock();
			auto copy = sem;
			for ( int i = 0; i<2; i++ )
				sem.unlock();

			int result = 0;
			while ( sem.try_lock() )result++;
			Assert::AreEqual(4, result);
		}

		TEST_METHOD(Copy_FreeSemaphoreAndCopyWithChagedLimit_CheckOriginHasSameDataLikeCopy)
		{
			Semaphore sem(4);
			auto copy = sem;
			copy.ChangeAccessLimit(8);

			int result = 0;
			while ( sem.try_lock() )result++;
			Assert::AreEqual(8, result);
		}
	};
}