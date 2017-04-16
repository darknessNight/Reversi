#include "stdafx.h"

#ifdef __MEMORY_OPTIMIZED
#undef _MEMORY_OPTIMIZED
#endif

#include "CppUnitTest.h"
#include "../ReversiSI/MinMax.h"
#include <sstream>

class FakeBoardState : public SI::Reversi::BoardState {
public:
	FakeBoardState(SI::Reversi::BoardState other) {
		*reinterpret_cast<SI::Reversi::BoardState*>(this) = other;
	}

	int GetHiddenValue(unsigned x, unsigned y) const{
		return bytes[GetFlatIndex(x, y)];
	}

	void SetHiddenValue(unsigned x, unsigned y, int value) {
		bytes[GetFlatIndex(x, y)] = value;
	}
};

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
namespace Microsoft::VisualStudio::CppUnitTestFramework {
	template<> inline std::wstring ToString<FakeBoardState>(const FakeBoardState& fakeElement) {
		std::wstringstream str;
		for (int i = 0; i < fakeElement.rowsCount; i++)
			for (int j = 0; j < fakeElement.colsCount; j++)
				str << "<" << i << " " << j << ": " << static_cast<int>(fakeElement.GetHiddenValue(i, j)) << "> ";
		return str.str();
	}

	template<> inline std::wstring ToString<SI::Reversi::BoardState>(const SI::Reversi::BoardState& element) {
		FakeBoardState fakeElement = element;
		std::wstringstream str;
		for (int i = 0; i < element.rowsCount; i++)
			for (int j = 0; j < element.colsCount; j++)
				str << "<" << i << " " << j << ": " << static_cast<int>(fakeElement.GetHiddenValue(i, j)) << "> ";
		return str.str();
	}
}

namespace SI::Reversi::Tests
{

	class FakeStateGenerator :public StateGenerator {
		unsigned treeDepth = 0;
		unsigned childNodesCount = 0;
		unsigned depthOfWinning = 0;
		unsigned posOfWinning = 0;
		BoardState winning;
		std::vector<BoardState> losing;
		unsigned generated = 0;
		std::shared_ptr<std::mutex> mutex=std::make_shared<std::mutex>();
	public:
		unsigned winningForFirsts = 0;

		FakeStateGenerator() = default;
		FakeStateGenerator(unsigned depth, unsigned childCount, unsigned posOfWin, unsigned depthOfWin, BoardState win)
		{
			SetDepth(depth);
			SetCountOfChildrens(childCount);
			SetPosOfWinning(posOfWin, depthOfWin);
			SetStateOfWinning(win);
			std::vector<BoardState> losing;
			for (int i = 0; i < childCount; i++)
				losing.push_back(BoardState());
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

		void SetStateOfWinning(const BoardState& state) {
			winning = state;
		}

		void SetStatetsOfLosing(std::vector<BoardState> states) {
			losing = states;
		}

		BoardState GetNextState() override {
			std::lock_guard<std::mutex> lock(*mutex);
			FakeBoardState current = this->currentState;
			auto number = current.GetHiddenValue(6, 7)-10;
			if (number < 0)number = 0;
			int nextNumber = number*childNodesCount + generated++;
			auto depth = current.GetHiddenValue(7, 7) - 10;
			if (depth < 0)depth = 1;
			if (depth == depthOfWinning && nextNumber == posOfWinning) {
				return winning;
			}

			FakeBoardState child = winningForFirsts>nextNumber?winning:losing[nextNumber%losing.size()];
			child.SetHiddenValue(7, 7, depth + 1 + 10);
			child.SetHiddenValue(6, 7, nextNumber + 10);
			return static_cast<BoardState>(child);
		}

		bool HasNextState() override {
			std::lock_guard<std::mutex> lock(*mutex);
			FakeBoardState current = this->currentState;
			auto depth = current.GetHiddenValue(7, 7) - 10;
			if (depth < 0)depth = 0;
			if (depth < treeDepth && currentState != winning)
				return generated < childNodesCount;
			else return false;
		}

		void Reset() override {
			std::lock_guard<std::mutex> lock(*mutex);
			generated = 0;
		}

		std::shared_ptr<StateGenerator> MakeCopy()
		{
			std::shared_ptr<FakeStateGenerator> ret = std::make_shared<FakeStateGenerator>(*this);
			ret->mutex = std::make_shared<std::mutex>();
			return ret;
		}
	};


	TEST_CLASS(SemaphoreTests)
	{
	public:
		MinMax GetObject(const BoardState& initState = GetStartState(), BoardState::FieldState player = BoardState::FieldState::Player2,
			std::function<double(const BoardState&)> aprox = nullptr, unsigned depth = 5) const
		{
			if (aprox == nullptr)
				aprox = std::bind(SimpleAprox, std::placeholders::_1, player);
			return MinMax(initState, player, depth, aprox);
		}

		static double SimpleAprox(const BoardState& state, BoardState::FieldState player) {
			double result = 0;
			for (int i = 0; i < state.rowsCount; i++)
				for (int j = 0; j < state.colsCount; j++) {
					auto field = state.GetFieldState(i, j);
					if (field == player)
						result += 1;
					else if (field == BoardState::FieldState::Empty || field == BoardState::FieldState::Unknown)
						result -= 0.0;
					else result -= 1.0;
				}
			return result;
		}

		static BoardState GetStartState() {
			return BoardState();
		}

		TEST_METHOD(GetBestMove_IsSecondPlayerAndTryGetMoveAsFirst_CheckThrowException)
		{
			auto fakeGenerator = std::make_shared<FakeStateGenerator>();
			auto minmax = GetObject();
			minmax.SetStatesGenerator([&]() { return fakeGenerator->MakeCopy(); });

			auto func = [&]() {minmax.GetBestMove(); };

			Assert::ExpectException<std::exception>(func);
		}

		TEST_METHOD(SetOpponent_IsFirstPlayerAndTrySetMoveAsFirst_CheckThrowException)
		{
			auto fakeGenerator = std::make_shared<FakeStateGenerator>();
			auto minmax = GetObject(GetStartState(), BoardState::FieldState::Player1);
			minmax.SetStatesGenerator([&]() { return fakeGenerator->MakeCopy(); });

			auto func = [&]() {
				minmax.SetOpponentMove(BoardState());
			};

			Assert::ExpectException<std::exception>(func);
		}

		TEST_METHOD(SetOpponent_IsSecondPlayerAndTryTwiceSetMoveAsFirst_CheckThrowException)
		{
			auto fakeGenerator = std::make_shared<FakeStateGenerator>();
			auto minmax = GetObject();
			minmax.SetStatesGenerator([&]() { return fakeGenerator->MakeCopy(); });

			auto func = [&]() {
				minmax.SetOpponentMove(BoardState());
				minmax.SetOpponentMove(BoardState());
			};

			Assert::ExpectException<std::exception>(func);
		}

		TEST_METHOD(GetBestMove_IsFirstPlayerAndTryTwiceGetMoveAsFirst_CheckThrowException)
		{
			auto fakeGenerator = std::make_shared<FakeStateGenerator>();
			auto minmax = GetObject(GetStartState(), BoardState::FieldState::Player1);
			minmax.SetStatesGenerator([&]() { return fakeGenerator->MakeCopy(); });

			auto func = [&]() {
				minmax.GetBestMove();
				minmax.GetBestMove();
			};

			Assert::ExpectException<std::exception>(func);
		}

		TEST_METHOD(GetBestMove_HasGeneratorWithWinningOnLastPosFirstLevelAndStaring_CheckGetBest)
		{
			auto winning=GetWinning(BoardState::FieldState::Player1);
			auto fakeGenerator = std::make_shared<FakeStateGenerator>(5, 5, 4, 1, winning);
			auto minmax = GetObject(GetStartState(), BoardState::FieldState::Player1);
			minmax.SetStatesGenerator([&]() { return fakeGenerator->MakeCopy(); });

			auto result = minmax.GetBestMove();

			Assert::AreEqual(winning, result);
		}

		BoardState GetWinning(BoardState::FieldState player)
		{
			BoardState ret;
			for (int i = 0; i < ret.rowsCount; i++)
				for (int j = 0; j < ret.colsCount; j++)
					ret.SetFieldState(i, j, player);
			return ret;
		}

		TEST_METHOD(GetBestMove_HasGeneratorWithWinningOnLastPosSecondLevelAndStaring_CheckGetFirstZero)
		{
			FakeBoardState expected = GetWinning(BoardState::FieldState::Empty);
			expected.SetFieldState(1, 1, BoardState::FieldState::Player1);
			auto fakeGenerator = std::make_shared<FakeStateGenerator>(5, 5, 4, 2, expected);
			fakeGenerator->winningForFirsts = 125;

			auto minmax = GetObject(GetStartState(), BoardState::FieldState::Player1);
			minmax.SetStatesGenerator([&]() { return fakeGenerator->MakeCopy(); });

			FakeBoardState result = minmax.GetBestMove();

			expected.SetHiddenValue(7, 7, 12);
			expected.SetHiddenValue(6, 7, 10);
			Assert::AreEqual<FakeBoardState>(expected, result);
		}

		TEST_METHOD(GetBestMove_HasGeneratorWithWinningOnLastPosLastLevelAndStaring_CheckReturnWinning)
		{
			auto winning = GetWinning(BoardState::FieldState::Player1);
			auto fakeGenerator = std::make_shared<FakeStateGenerator>(5, 5, 0, 4, winning);

			fakeGenerator->winningForFirsts = 125;

			auto minmax = GetObject(GetStartState(), BoardState::FieldState::Player1);
			minmax.SetStatesGenerator([&]() { return fakeGenerator->MakeCopy(); });
			
			FakeBoardState result = minmax.GetBestMove();

			FakeBoardState expected = GetWinning(BoardState::FieldState::Player1);
			expected.SetHiddenValue(7, 7, 12);
			expected.SetHiddenValue(6, 7, 10);

			Assert::AreEqual<FakeBoardState>(expected, result);
		}
	};
}