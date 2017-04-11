#include "stdafx.h"
#include "CppUnitTest.h"
#include "../ReversiSI/MapState.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SI::Reversi::Tests
{		
	TEST_CLASS(MapStateTests)
	{
	public:
		
		TEST_METHOD(GetFieldState_HasEmptyMap_CheckReturnEmptyForAllFields)
		{
			MapStateMemoryOptimized map;
			for ( unsigned i = 0; i < MapStateMemoryOptimized::rowsCount; i++ )
			{
				for ( unsigned j = 0; j < MapStateMemoryOptimized::colsCount; j++ )
					Assert::AreEqual((int)MapStateMemoryOptimized::State::Empty, (int)map.GetFieldState(i, j));
			}
		}

		TEST_METHOD(GetFieldState_HasMapWithFirstElements_CheckReturnEmptyForOthers)
		{
			char bytes[16] = { 0b10010000,0b10010000,0,0,0,0,0,0 };
			MapStateMemoryOptimized map(bytes);
			for ( unsigned i = 1; i < MapStateMemoryOptimized::rowsCount; i++ )
			{
				for ( unsigned j = 1; j < MapStateMemoryOptimized::colsCount; j++ )
					Assert::AreEqual((int)MapStateMemoryOptimized::State::Empty, (int)map.GetFieldState(i, j));
			}
		}

		TEST_METHOD(GetFieldState_HasMapWithFirstElements_CheckReturnCorrectFirsts)
		{
			char bytes[16] = { 0b10010000,0,0b10010000,0,0,0,0,0,0,0,0,0,0,0,0,1 };
			MapStateMemoryOptimized map(bytes);
			int result[5];
			result[0]=map.GetFieldState(0, 0);
			result[1] = map.GetFieldState(0, 1);
			result[2] = map.GetFieldState(1, 0);
			result[3] = map.GetFieldState(1, 1);
			result[4] = map.GetFieldState(7, 7);

			Assert::AreEqual<int>(0b10, result[0], std::to_wstring(0).c_str());
			Assert::AreEqual<int>(0b01, result[1], std::to_wstring(1).c_str());
			Assert::AreEqual<int>(0b10, result[2], std::to_wstring(2).c_str());
			Assert::AreEqual<int>(0b01, result[3], std::to_wstring(3).c_str());
			Assert::AreEqual<int>(0b01, result[4], std::to_wstring(4).c_str());
		}

		TEST_METHOD(SetFieldState_HasMapWithFirstElements_CheckReturnCorrectFirsts)
		{
			MapStateMemoryOptimized map;
			map.SetFieldState(0, 0,(MapStateMemoryOptimized::State)2);
			map.SetFieldState(0, 1, (MapStateMemoryOptimized::State)1);
			map.SetFieldState(1, 0, (MapStateMemoryOptimized::State)2);
			map.SetFieldState(1, 1, (MapStateMemoryOptimized::State)1);
			map.SetFieldState(7, 7, (MapStateMemoryOptimized::State)1);

			int result[5];
			result[0] = map.GetFieldState(0, 0);
			result[1] = map.GetFieldState(0, 1);
			result[2] = map.GetFieldState(1, 0);
			result[3] = map.GetFieldState(1, 1);
			result[4] = map.GetFieldState(7, 7);

			Assert::AreEqual<int>(0b10, result[0], std::to_wstring(0).c_str());
			Assert::AreEqual<int>(0b01, result[1], std::to_wstring(1).c_str());
			Assert::AreEqual<int>(0b10, result[2], std::to_wstring(2).c_str());
			Assert::AreEqual<int>(0b01, result[3], std::to_wstring(3).c_str());
			Assert::AreEqual<int>(0b01, result[4], std::to_wstring(4).c_str());
		}
	};
}