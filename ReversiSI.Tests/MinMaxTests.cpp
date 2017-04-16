#include "stdafx.h"
#include "CppUnitTest.h"
#include "../ReversiSI/MinMax.h"
#include <sstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SI::Reversi::Tests
{
	class FakeStateGenerator :public StateGenerator {
	public:
		virtual MapState GetNextState() override {
			return MapState();
		}

		virtual bool HasNextState() override {
			return false;
		}

		virtual void Reset() override {

		}
	};


	TEST_CLASS(SemaphoreTests)
	{
	public:
		MinMax GetObject(const MapState& initState=GetStartState(), MapState::State player=MapState::State::Player2, 
			std::function<double(const MapState&)> aprox = nullptr, unsigned depth=5) {
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