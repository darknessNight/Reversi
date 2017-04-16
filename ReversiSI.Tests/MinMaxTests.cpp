#include "stdafx.h"

#ifdef __MEMORY_OPTIMIZED
#undef _MEMORY_OPTIMIZED
#endif

#include "CppUnitTest.h"
#include "../ReversiSI/MinMax.h"
#include <sstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SI::Reversi::Tests
{
	class FakeMapState : public MapState {
	public:
		FakeMapState(MapState other) {
			*reinterpret_cast<MapState*>(this) = other;
		}

		char GetHiddenValue(unsigned x, unsigned y) {
			return bytes[GetFlatIndex(x, y)];
		}

		virtual State GetFieldState(unsigned x, unsigned y)const override
		{
			auto flatIndex = GetFlatIndex(x, y);
			if (bytes[flatIndex] > State::Unknown)
				return State::Empty;
			return static_cast<State>(bytes[flatIndex]);
		}

		void SetHiddenValue(unsigned x, unsigned y, char value) {
			bytes[GetFlatIndex(x, y)] = value;
		}
	};

	class FakeStateGenerator :public StateGenerator {
		unsigned treeDepth=0;
		unsigned childNodesCount = 0;
		unsigned depthOfWinning = 0;
		unsigned posOfWinning = 0;
		MapState winning;
		std::vector<MapState> losing;
		unsigned generated = 0;
	public:
		FakeStateGenerator() = default;
		FakeStateGenerator(unsigned depth, unsigned childCount, unsigned posOfWin, unsigned depthOfWin, MapState win)
		{
			SetDepth(depth);
			SetCountOfChildrens(childCount);
			SetPosOfWinning(posOfWin, depthOfWin);
			SetStateOfWinning(win);
			std::vector<MapState> losing;
			for (int i = 0; i < childCount; i++)
				losing.push_back(MapState());
			SetStatetsOfLosing(losing);
		}

		void SetDepth(unsigned depth) {
			treeDepth = depth;
		}

		void SetCountOfChildrens(unsigned count) {
			childNodesCount = count;
		}

		void SetPosOfWinning(unsigned pos, unsigned depth) {
			posOfWinning = pos;
			depthOfWinning = depth;
		}

		void SetStateOfWinning(const MapState& state) {
			winning = state;
		}

		void SetStatetsOfLosing(std::vector<MapState> states) {
			losing = states;
		}

		MapState GetNextState() override {
			FakeMapState current = this->currentState;
			auto number=current.GetHiddenValue(8, 9);
			auto nextNumber = number*childNodesCount + generated;
			auto depth = current.GetHiddenValue(9, 9);
			if (depth == depthOfWinning && nextNumber == posOfWinning)
				return winning;

			FakeMapState child=losing[generated++];
			child.SetHiddenValue(9,9,depth + 1);
			child.SetHiddenValue(8, 9, nextNumber);
			return static_cast<MapState>(child);
		}

		bool HasNextState() override {
			FakeMapState current = this->currentState;
			if(current.GetHiddenValue(9,9)<treeDepth && currentState!=winning)
				return generated < childNodesCount;
			else return false;
		}

		void Reset() override {
			generated = 0;
		}
	};


	TEST_CLASS(SemaphoreTests)
	{
	public:
		MinMax GetObject(const MapState& initState=GetStartState(), MapState::State player=MapState::State::Player2, 
			std::function<double(const MapState&)> aprox = nullptr, unsigned depth=5) const
		{
			if (aprox == nullptr)
				aprox = std::bind(SimpleAprox, std::placeholders::_1, player);
			return MinMax(initState, player, depth, aprox);
		}

		static double SimpleAprox(const MapState& state, MapState::State player) {
			double result = 0;
			for(int i=0;i<state.rowsCount;i++)
				for (int j = 0; j < state.colsCount; j++) {
					auto field = state.GetFieldState(i, j);
					if (field == player)
						result += 1;
					else if (field == MapState::State::Empty)
						result -= 0.0;
					else result -= 1.0;
				}
			return result;
		}

		static MapState GetStartState() {
			return MapState();
		}

		TEST_METHOD(GetBestMove_IsSecondPlayerAndTryGetMoveAsFirst_CheckThrowException)
		{
			auto fakeGenerator = std::make_shared<FakeStateGenerator>();
			auto minmax = GetObject();
			minmax.SetStatesGenerator(fakeGenerator);

			auto func = [&]() {minmax.GetBestMove(); };

			Assert::ExpectException<std::exception>(func);
		}

		TEST_METHOD(SetOpponent_IsFirstPlayerAndTrySetMoveAsFirst_CheckThrowException)
		{
			auto fakeGenerator = std::make_shared<FakeStateGenerator>();
			auto minmax = GetObject(GetStartState(),MapState::State::Player1);
			minmax.SetStatesGenerator(fakeGenerator);

			auto func = [&]() {
				minmax.SetOpponentMove(MapState()); 
			};

			Assert::ExpectException<std::exception>(func);
		}

		TEST_METHOD(SetOpponent_IsSecondPlayerAndTryTwiceSetMoveAsFirst_CheckThrowException)
		{
			auto fakeGenerator = std::make_shared<FakeStateGenerator>();
			auto minmax = GetObject();
			minmax.SetStatesGenerator(fakeGenerator);

			auto func = [&]() {
				minmax.SetOpponentMove(MapState());
				minmax.SetOpponentMove(MapState());
			};

			Assert::ExpectException<std::exception>(func);
		}

		TEST_METHOD(SetOpponent_IsFirstPlayerAndTryTwiceGetMoveAsFirst_CheckThrowException)
		{
			auto fakeGenerator = std::make_shared<FakeStateGenerator>();
			auto minmax = GetObject(GetStartState(), MapState::State::Player1);
			minmax.SetStatesGenerator(fakeGenerator);

			auto func = [&]() {
				minmax.GetBestMove();
				minmax.GetBestMove();
			};

			Assert::ExpectException<std::exception>(func);
		}
	};
}