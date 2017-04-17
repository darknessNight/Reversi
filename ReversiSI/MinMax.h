#pragma once
#include "StateGenerator.h"
#include "Multithreading/ParallelJobExecutor.h"
#include <functional>
#include <iostream>
#include <fstream>
#include <atomic>

using darknessNight::Multithreading::ParallelJobExecutor;

namespace SI::Reversi {
	class MinMax {
	protected:
		struct MinMaxNode
		{
			std::atomic<double> value;
			bool maximizing = false;
			BoardState state;
			std::weak_ptr<MinMaxNode> parent;
			std::vector<std::shared_ptr<MinMaxNode>> children;

			MinMaxNode(const BoardState& state, bool maximizing = false)
				:value(maximizing ? -1 : 1 * std::numeric_limits<double>::max()), maximizing(maximizing), state(state)
			{}

			MinMaxNode(const MinMaxNode& other)
				:maximizing(other.maximizing),state(other.state),parent(other.parent),children(other.children)
			{
				value = static_cast<double>(other.value);
			}

			MinMaxNode& operator=(const MinMaxNode& other){
				value = static_cast<double>(other.value);
				maximizing = other.maximizing;
				state = other.state;
				parent = other.parent;
				children = other.children;
				return *this;
			}
		};
	protected:
		std::shared_ptr<MinMaxNode> currentState;
		BoardState::FieldState siPlayer;
		BoardState::FieldState currentPlayer = BoardState::FieldState::Player1;
		std::function<double(const BoardState&)> heur;
		std::function<std::shared_ptr<StateGenerator>()> generatorFabric;
		unsigned minimumDepth;
		unsigned currentDepth=1;
		std::shared_ptr<ParallelJobExecutor> executor;
		std::fstream file;
		std::shared_ptr<std::mutex> out = std::make_shared<std::mutex>();

	public:
		MinMax(BoardState startState, BoardState::FieldState siPlayer, unsigned minDepth, std::function<double(const BoardState&)> aprox)
			: currentState(std::make_shared<MinMaxNode>(startState, siPlayer == BoardState::FieldState::Player1)), 
				siPlayer(siPlayer), heur(aprox), generatorFabric([]() {return std::make_shared<StateGenerator>(); }),
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
				return currentState->state;

			return FindBestMove();
		}

	protected:
		std::shared_ptr<StateGenerator> CheckAndPrepare()
		{
			if (currentPlayer != siPlayer)
				throw std::exception("It's no move of SI");
			IncrementPlayer();

			auto generator = generatorFabric();
			generator->StateGenerator::SetCurrentState(currentState->state);
			return generator;
		}

		BoardState FindBestMove()
		{
			std::vector<std::shared_ptr<MinMaxNode>> levels[2];
			int parent = 0;
			int child = 1;
			std::mutex mutex;
			levels[parent].push_back(currentState);

			while (currentDepth < minimumDepth) {

				executor->ForEach<std::shared_ptr<MinMaxNode>>([&](std::shared_ptr<MinMaxNode>& next)
				{
					minmax(next, levels[child], mutex);
				}, levels[parent]);

				currentDepth++;
				levels[parent].clear();
				parent = (parent + 1) % 2;
				child = (child + 1) % 2;
			}


			double bestValue = -std::numeric_limits<double>::max();
			auto bestState = currentState;
			for(auto el:currentState->children)
			{
				if(bestValue<el->value)
				{
					bestState = el;
					bestValue = el->value;
				}
			}

			currentState = bestState;
			return bestState->state;
		}

		void minmax(std::shared_ptr<MinMaxNode> node, std::vector<std::shared_ptr<MinMaxNode>> &children, std::mutex &mutex)
		{
			auto generator = generatorFabric();
			generator->SetCurrentState(node->state);

			if (!generator->HasNextState()) {
				std::lock_guard<std::mutex> lock(mutex);
				children.push_back(node);
				return;
			}

			auto nexts=generator->GetAllNextStates();
			
			for(auto el: nexts)
			{
				auto child = std::make_shared<MinMaxNode>(el, !node->maximizing);
				child->parent = node;
				node->children.push_back(child);
				mutex.lock();
				children.push_back(child);
				mutex.unlock();
				RefreshParent(child);
			}
		}

		void RefreshParent(std::shared_ptr<MinMaxNode> node)
		{
			auto val = heur(node->state);
			node->value = val;
			while(auto parent = node->parent.lock())
			{
				double copy = parent->value;
				if (parent->maximizing)
					parent->value = max(parent->value, val);
				else parent->value = min(parent->value, val);
				if (copy == parent->value)
					break;
				node = parent;
			}
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