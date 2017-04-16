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
		struct MinMaxNode
		{
			double value;
			bool maximizing = false;
			BoardState state;
			std::weak_ptr<MinMaxNode> parent;
			std::vector<std::shared_ptr<MinMaxNode>> children;

			MinMaxNode(const BoardState& state, bool maximizing = false)
				:state(state), maximizing(maximizing), value(maximizing ? -1 : 1 * std::numeric_limits<double>::max())
			{}
		};
	protected:
		MinMaxNode currentState;
		BoardState::FieldState siPlayer;
		BoardState::FieldState currentPlayer = BoardState::FieldState::Player1;
		std::function<double(const BoardState&)> heur;
		std::function<std::shared_ptr<StateGenerator>()> generatorFabric;
		unsigned minimumDepth;
		std::shared_ptr<ParallelJobExecutor> executor;
		std::fstream file;
		std::shared_ptr<std::mutex> out = std::make_shared<std::mutex>();

	public:
		MinMax(BoardState startState, BoardState::FieldState siPlayer, unsigned minDepth, std::function<double(const BoardState&)> aprox)
			: currentState(startState, siPlayer == BoardState::FieldState::Player1), siPlayer(siPlayer), heur(aprox), generatorFabric([]() {return std::make_shared<StateGenerator>(); }),
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
				return currentState.state;

			return FindBestMove(generator);
		}

	protected:
		std::shared_ptr<StateGenerator> CheckAndPrepare()
		{
			if (currentPlayer != siPlayer)
				throw std::exception("It's no move of SI");
			IncrementPlayer();

			auto generator = generatorFabric();
			generator->StateGenerator::SetCurrentState(currentState.state);
			return generator;
		}

		BoardState FindBestMove(std::shared_ptr<StateGenerator> generator)
		{
			std::mutex mutex;
			BoardState bestState = currentState.state;
			auto bestValue = -std::numeric_limits<double>::max();
			auto nextStates = generator->GetAllNextStates();

			executor->ForEach<BoardState>([&](BoardState& nextState)
			{
				auto next = std::make_shared<MinMaxNode>(nextState, false);
				next->parent = std::make_shared<MinMaxNode>(currentState);
				{
					std::lock_guard<std::mutex> lock(mutex);
					currentState.children.push_back(next);
				}
				minimax(next, false, minimumDepth);
			}, nextStates);

			for(auto el:currentState.children)
			{
				if(bestValue<el->value)
				{
					bestState = el->state;
					bestValue = el->value;
				}
			}

			currentState = bestState;
			return bestState;
		}

		void minimax(std::shared_ptr<MinMaxNode> node, bool maximizingPlayer, unsigned depth)
		{
			auto generator = generatorFabric();
			generator->SetCurrentState(node->state);
			if (!generator->HasNextState() || depth==0) {
				RefreshParent(heur(node->state), node);
				return;
			}

			MinMaxRecursive(node, maximizingPlayer, generator, depth);
		}

		void RefreshParent(double val, std::shared_ptr<MinMaxNode> node)
		{
			while(auto parent = node->parent.lock())
			{
				auto copy = parent->value;
				if (parent->maximizing)
					parent->value = max(parent->value, val);
				else parent->value = min(parent->value, val);
				if (copy == parent->value)
					break;
				node = parent;
			}
		}

		void MinMaxRecursive(std::shared_ptr<MinMaxNode> node, bool maximizingPlayer, std::shared_ptr<StateGenerator> generator, unsigned depth)
		{
			std::mutex mutex;
			auto nextStates = generator->GetAllNextStates();
			/*executor->ForEach<BoardState>(
				[&](BoardState& child){*/
			for (auto child : nextStates) {
				auto next = std::make_shared<MinMaxNode>(child, !maximizingPlayer);
				next->parent = node;
				{
					std::lock_guard<std::mutex> lock(mutex);
					node->children.push_back(next);
				}
				RefreshParent(heur(child), next);
				minimax(node, !maximizingPlayer, depth-1);
			}//, nextStates);
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