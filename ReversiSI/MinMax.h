#pragma once
#include "StateGenerator.h"
#include "Multithreading/ParallelJobExecutor.h"
#include <functional>

using darknessNight::Multithreading::ParallelJobExecutor;

namespace SI::Reversi {
	class MinMax {
	protected:
		BoardState currentState;
		BoardState::FieldState siPlayer;
		BoardState::FieldState currentPlayer = BoardState::FieldState::Player1;
		std::function<double(const BoardState&)> heur;
		std::function<std::shared_ptr<StateGenerator>()> generatorFabric;
		unsigned minimumDepth;

		struct MinMaxResult
		{
			BoardState state;
			double value;

			MinMaxResult(double val) :value(val) {}
			MinMaxResult(double val, BoardState st) :state(st), value(val) {}

			bool operator<(const MinMaxResult& other) const {
				return value < other.value;
			}

			bool operator>(const MinMaxResult& other) const {
				return value > other.value;
			}
		};

	public:
		MinMax(BoardState startState, BoardState::FieldState siPlayer, unsigned minDepth, std::function<double(const BoardState&)> aprox)
			: currentState(startState), siPlayer(siPlayer), heur(aprox), generatorFabric([]() {return std::make_shared<StateGenerator>(); }),
			minimumDepth(minDepth)
		{}

		void SetStatesGenerator(std::function<std::shared_ptr<StateGenerator>()> stateGeneratorFabric)
		{
			generatorFabric = stateGeneratorFabric;
		}

		void SetParallelJobExecutor(std::shared_ptr<ParallelJobExecutor> executor) {}

		BoardState GetBestMove() {
			auto generator = CheckAndPrepare();

			if (!generator->HasNextState())
				return currentState;

			return FindBestMove(generator);
		}

	protected:
		std::shared_ptr<StateGenerator> CheckAndPrepare()
		{
			if (currentPlayer != siPlayer)
				throw std::exception("It's no move of SI");
			IncrementPlayer();

			auto generator = generatorFabric();
			generator->StateGenerator::SetCurrentState(currentState);
			return generator;
		}

		BoardState FindBestMove(std::shared_ptr<StateGenerator> generator) const
		{
			BoardState bestState;
			auto bestValue = -std::numeric_limits<double>::max();
			for (; generator->HasNextState();) {
				auto nextState = generator->GetNextState();
				auto res = minimax(nextState, minimumDepth, false);
				if (res > bestValue)
				{
					bestValue = res;
					bestState = nextState;
				}
			}
			return bestState;
		}

		double minimax(BoardState node, unsigned depth, bool maximizingPlayer) const
		{
			auto generator = generatorFabric();
			generator->SetCurrentState(node);
			if (depth == 0 || !generator->HasNextState())
				return heur(node);
			if (maximizingPlayer) {
				auto bestValue = -std::numeric_limits<double>::max();

				for (; generator->HasNextState();) {
					auto child = generator->GetNextState();
					auto v = minimax(child, depth - 1, !maximizingPlayer);
					bestValue = max(bestValue, v);
				}
				return bestValue;
			}
			else {
				auto bestValue = std::numeric_limits<double>::max();
				for (; generator->HasNextState();) {
					auto child = generator->GetNextState();
					auto v = minimax(child, depth - 1, !maximizingPlayer);
					bestValue = min(bestValue, v);
				}
				return bestValue;
			}
		}

		static double max(double a, double b)
		{
			return a > b ? a : b;
		}

		static double min(double a, double b)
		{
			return a < b ? a : b;
		}

	public:

		BoardState GetBestMoveAsync(std::function<void(const BoardState&)> callback) {
			return BoardState();
		}

		void SetOpponentMove(const BoardState& opponentMove) {
			if (currentPlayer == siPlayer)
				throw std::exception("It's move of SI");
			IncrementPlayer();
		}

	private:
		void IncrementPlayer()
		{
			currentPlayer = (BoardState::FieldState) (((currentPlayer + 1) % BoardState::FieldState::Unknown));
			if (currentPlayer == BoardState::FieldState::Empty)
				currentPlayer = BoardState::FieldState::Player1;
		}
	};
}