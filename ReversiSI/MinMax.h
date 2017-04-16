#pragma once
#include "StateGenerator.h"
#include "Multithreading/ParallelJobExecutor.h"
#include <functional>
#include <iostream>
#include <fstream>

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
		std::shared_ptr<ParallelJobExecutor> executor;
		std::fstream file;
		std::shared_ptr<std::mutex> out=std::make_shared<std::mutex>();

	public:
		MinMax(BoardState startState, BoardState::FieldState siPlayer, unsigned minDepth, std::function<double(const BoardState&)> aprox)
			: currentState(startState), siPlayer(siPlayer), heur(aprox), generatorFabric([]() {return std::make_shared<StateGenerator>(); }),
			minimumDepth(minDepth), executor(std::make_shared<ParallelJobExecutor>())
		{
			file.open("D:\\desktop\\CLog.log", std::ios::out);
			if (file.good())
				std::clog.set_rdbuf(file.rdbuf());
		}

		void SetStatesGenerator(std::function<std::shared_ptr<StateGenerator>()> stateGeneratorFabric)
		{
			generatorFabric = stateGeneratorFabric;
		}

		void SetParallelJobExecutor(std::shared_ptr<ParallelJobExecutor> executor)
		{
			this->executor = executor;
		}

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

		BoardState FindBestMove(std::shared_ptr<StateGenerator> generator)
		{
			std::mutex mutex;
			BoardState bestState;
			auto bestValue = -std::numeric_limits<double>::max();
			auto nextStates = generator->GetAllNextStates();

			executor->ForEach<BoardState>([&](BoardState& nextState)
			{
				auto res = minimax(nextState, minimumDepth, false);
				{
					std::lock_guard<std::mutex> lock(*out);
					std::clog << 6<< " my " << " " << res << "\n";
				}
				std::lock_guard<std::mutex> lock(mutex);
				if (res > bestValue)
				{
					bestValue = res;
					bestState = nextState;
				}
			}, nextStates);

			currentState = bestState;
			return bestState;
		}

		double minimax(BoardState node, unsigned depth, bool maximizingPlayer)
		{
			auto generator = generatorFabric();
			generator->SetCurrentState(node);
			if (depth == 0 || !generator->HasNextState()) {
				std::lock_guard<std::mutex> lock(*out);
				std::clog << 0 << (maximizingPlayer?" my":" him") << " " << heur(node)<<" "<< (int) node.GetFieldState(6,7)<<" " <<(int) node.GetFieldState(7,7) << "\n";
				return heur(node);
			}

			if (maximizingPlayer) {
				return MinMaxRecursive(depth, maximizingPlayer, generator, -std::numeric_limits<double>::max(), max);
			}
			else {
				return MinMaxRecursive(depth, maximizingPlayer, generator, std::numeric_limits<double>::max(), min);
			}
		}
		double MinMaxRecursive(unsigned depth, bool maximizingPlayer, std::shared_ptr<StateGenerator> generator, double bestValue, std::function<double(double, double)> selector)
		{
			std::mutex mutex;
			auto nextStates = generator->GetAllNextStates();
			/*executor->ForEach<BoardState>(
				[&](BoardState& child)*/
			for(auto child: nextStates){
					auto v = minimax(child, depth - 1, !maximizingPlayer);
					std::lock_guard<std::mutex> lock(mutex);
					bestValue = selector(bestValue, v);
			}/*, nextStates);*/
			std::lock_guard<std::mutex> lock(*out);
			std::clog << depth << (maximizingPlayer ? " my " : " him ") << " " << bestValue << "\n";
			return bestValue;
		}

		static double max(double a, double b)
		{
			return b > a ? b : a;
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