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
			bool root = false;

			MinMaxNode(const BoardState& state, bool maximizing = false)
				:value((maximizing ? -1 : 1) * std::numeric_limits<double>::max()), maximizing(maximizing), state(state)
			{}

			MinMaxNode(const MinMaxNode& other)
				:maximizing(other.maximizing),state(other.state),parent(other.parent),children(other.children)
			{
				value = static_cast<double>(other.value);
			}

			void SetAsRoot()
			{
				root = true;
			}

			bool IsRoot() const
			{
				return root;
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
		std::function<std::shared_ptr<StateGenerator>(const BoardState&, BoardState::FieldState)> generatorFabric;
		unsigned minimumDepth;
		unsigned currentDepth=1;
		std::shared_ptr<ParallelJobExecutor> executor;
		std::shared_ptr<std::shared_mutex> currentStateMutex = std::make_shared<std::shared_mutex>();
		std::shared_ptr<std::thread> algorithmThread;
		bool working = true;

	public:
		MinMax(BoardState startState, BoardState::FieldState siPlayer, unsigned minDepth, std::function<double(const BoardState&)> aprox)
			: currentState(std::make_shared<MinMaxNode>(startState, siPlayer == BoardState::FieldState::Player1)), 
				siPlayer(siPlayer), heur(aprox), generatorFabric([](const BoardState& state, BoardState::FieldState player) {return std::make_shared<StateGenerator>(state,player); }),
				minimumDepth(minDepth), executor(std::make_shared<ParallelJobExecutor>())
		{
			currentState->SetAsRoot();
			algorithmThread = std::make_shared<std::thread>([&]() {FindBestMove(); });
		}

		~MinMax()
		{
			working = false;
			if (algorithmThread->joinable())
				algorithmThread->join();
		}

		void SetStatesGenerator(std::function<std::shared_ptr<StateGenerator>(const BoardState&, BoardState::FieldState)> stateGeneratorFabric)
		{
			generatorFabric = stateGeneratorFabric;
		}

		void SetParallelJobExecutor(std::shared_ptr<ParallelJobExecutor> executor)
		{
			this->executor = executor;
		}

		BoardState GetBestMove() {
			CheckAndPrepare();

			while(currentDepth<minimumDepth)
			{
				std::this_thread::sleep_for(std::chrono::microseconds(100));
			}

			auto bestValue = -std::numeric_limits<double>::max();
			auto bestState = currentState;
			for (auto el : currentState->children)
			{
				if (bestValue<el->value)
				{
					bestState = el;
					bestValue = el->value;
				}
			}

			currentStateMutex->lock();
			currentState = bestState;
			currentState->SetAsRoot();
			currentStateMutex->unlock();
			return bestState->state;
		}

	protected:
		void CheckAndPrepare()
		{
			if (currentPlayer != siPlayer)
				throw std::exception("It's no move of SI");
			IncrementPlayer();
		}

		void FindBestMove()
		{
			std::vector<std::shared_ptr<MinMaxNode>> levels[2];
			int parent = 0;
			int child = 1;
			std::mutex mutex;
			levels[parent].push_back(currentState);

			while (working) {
				executor->ForEach<std::shared_ptr<MinMaxNode>>([&](std::shared_ptr<MinMaxNode>& next)
				{
					std::shared_lock<std::shared_mutex> lock(*currentStateMutex);
					minmax(next, levels[child], mutex);
				}, levels[parent]);

				currentDepth++;
				parent = (parent + 1) % 2;
				child = (child + 1) % 2;

				std::shared_lock<std::shared_mutex> lock(*currentStateMutex);
				levels[child].clear();
			}
		}

		void minmax(std::shared_ptr<MinMaxNode> node, std::vector<std::shared_ptr<MinMaxNode>> &children, std::mutex &mutex) const
		{
			auto generator = generatorFabric(node->state, node->maximizing?siPlayer:GetNotSiPlayer());

			if (node->parent.expired() && !node->IsRoot())
				return;

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

		BoardState::FieldState GetNotSiPlayer() const
		{
			auto result = static_cast<BoardState::FieldState>((currentPlayer + 1) % BoardState::FieldState::Unknown);
			if (result == BoardState::FieldState::Empty)
				result = BoardState::FieldState::Player1;
			return result;
		}

		void RefreshParent(std::shared_ptr<MinMaxNode> node) const
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
			std::thread th([&]() {callback(GetBestMove()); });
			th.detach();
		}

		void SetOpponentMove(const BoardState& opponentMove) {
			if (currentPlayer == siPlayer)
				throw std::exception("It's move of SI");
			IncrementPlayer();

			std::lock_guard<std::shared_mutex> lock(*currentStateMutex);
			for (auto el : currentState->children)
			{
				if (el->state==opponentMove)
				{
					currentState = el;
					currentState->SetAsRoot();
					return;
				}
			}
			std::exception("Undefined move");			
		}

	private:
		void IncrementPlayer()
		{
			currentPlayer = static_cast<BoardState::FieldState>((currentPlayer + 1) % BoardState::FieldState::Unknown);
			if (currentPlayer == BoardState::FieldState::Empty)
				currentPlayer = BoardState::FieldState::Player1;
		}
	public:
		unsigned GetCurrentDepth() const
		{
			return currentDepth;
		}
	};
}