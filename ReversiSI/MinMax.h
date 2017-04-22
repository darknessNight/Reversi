﻿#pragma once
#include "StateGenerator.h"
#include "Multithreading/ParallelJobExecutor.h"
#include "MemoryUsageGuard.h"
#include <functional>
#include <atomic>
#include <iostream>

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
		darknessNight::MemoryUsageGuard memoryGuard;

	public:
		MinMax(BoardState startState, BoardState::FieldState siPlayer, unsigned minDepth, std::function<double(const BoardState&)> aprox)
			: currentState(std::make_shared<MinMaxNode>(startState, siPlayer == BoardState::FieldState::Player1)), 
				siPlayer(siPlayer), heur(aprox), generatorFabric([](const BoardState& state, BoardState::FieldState player) {return std::make_shared<StateGenerator>(state,player); }),
				minimumDepth(minDepth), executor(std::make_shared<ParallelJobExecutor>())
		{
			currentState->SetAsRoot();
			algorithmThread = std::make_shared<std::thread>([&]() {FindBestMove(); });
			executor->SetNumberOfThreads(0);
		}

		~MinMax()
		{
			currentState = nullptr;
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

			std::cout << "Waiting for depth\n";
			std::cout << "Current depth: " << currentDepth << "\n";

			while(currentDepth<minimumDepth)
			{
				std::this_thread::sleep_for(std::chrono::microseconds(100));
			}

			std::cout << "Try find best move\n";
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

			std::cout << "Dealloc unreachable moves\n";
			currentStateMutex->lock();
			currentState = bestState;
			currentState->SetAsRoot();
			currentStateMutex->unlock();
			std::cout << "End of minmax\n";
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
			int darkSoulIt;
			while (working) {
				darkSoulIt = 0;
				executor->ForEach<std::shared_ptr<MinMaxNode>>([&](std::shared_ptr<MinMaxNode>& next)
				{
					std::shared_lock<std::shared_mutex> lock(*currentStateMutex);
					mutex.lock();
					if ( next->parent.expired() ) darkSoulIt++;
					mutex.unlock();
					minmax(next, levels[child], mutex);
				}, levels[parent]);

				std::cout <<"Unnecesary iterations: "<< darkSoulIt << " Count of childrens: " << levels[child].size() << "; Level: "<<currentDepth
					<<" Available memory:" << memoryGuard.GetAllAvailableMemory()/1024.0/1024.0<<" MB"<<"\n";

				currentDepth++;
				parent = (parent + 1) % 2;
				child = (child + 1) % 2;

				std::shared_lock<std::shared_mutex> lock(*currentStateMutex);
				levels[child].clear();
			}
		}

		void minmax(std::shared_ptr<MinMaxNode> node, std::vector<std::shared_ptr<MinMaxNode>> &children, std::mutex &mutex) const
		{
			if ( node->parent.expired() && !node->IsRoot() )
				return;

			auto generator = generatorFabric(node->state, node->maximizing?siPlayer:GetNotSiPlayer());


			if (!generator->HasNextState()) {
				std::lock_guard<std::mutex> lock(mutex);
				auto newNode = std::make_shared<MinMaxNode>(*node);
				newNode->maximizing = !node->maximizing;
				children.push_back(newNode);
				return;
			}

			auto nexts=generator->GetAllNextStates();
			auto safeGenerator = generatorFabric(node->state, node->maximizing ? siPlayer : GetNotSiPlayer());
			auto nextasdfasdfasfasfasd = safeGenerator->GetAllNextStates();
			auto nextragsgadfgadfgasdfwegasrgerg = safeGenerator->GetAllNextStates();
			
			for(auto el: nexts)
			{
				memoryGuard.WaitForAvailableMemeory();
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
			auto result = static_cast<BoardState::FieldState>((siPlayer + 1) % BoardState::FieldState::Unknown);
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

		void GetBestMoveAsync(std::function<void(const BoardState&)> callback) {
			std::thread th([callback,this]() {
				callback(GetBestMove()); 
			});
			th.detach();
		}

		void SetOpponentMove(const BoardState& opponentMove) {
			if (currentPlayer == siPlayer)
				throw std::exception("It's move of SI");

			std::lock_guard<std::shared_mutex> lock(*currentStateMutex);
			for (auto el : currentState->children)
			{
				if (el->state==opponentMove)
				{
					std::cout << "Found passing opponet move\nTry dealloc\n";
					currentState = el;
					std::cout << "Deallocated\n";
					currentState->SetAsRoot();
					IncrementPlayer();
					return;
				}
			}

			auto nextStates = generatorFabric(currentState->state, GetNotSiPlayer())->GetAllNextStates();
			for ( auto el : nextStates )
			{
				if ( el == opponentMove )
				{
					for ( int i = 0; i < opponentMove.rowsCount; i++ )
					{
						for ( int j = 0; j < opponentMove.colsCount; j++ )
							std::cout << (int)el.GetFieldState(j, i);
						std::cout << "\n";
					}
					std::cout << "----------------------\n";
				}
			}

			std::cout << "Opponent move:-----------\n";
			for ( int i = 0; i < opponentMove.rowsCount; i++ )
			{
				for ( int j = 0; j < opponentMove.colsCount; j++ )
					std::cout << (int)opponentMove.GetFieldState(j,i);
				std::cout << "\n";
			}
			std::cout << "Current move:-----------\n";
			for ( int i = 0; i < opponentMove.rowsCount; i++ )
			{
				for ( int j = 0; j < opponentMove.colsCount; j++ )
					std::cout << (int)currentState->state.GetFieldState(j,i);
				std::cout << "\n";
			}
			std::cout << "Child moves:-----------\n";
			for ( auto el : currentState->children )
			{
				for ( int i = 0; i < opponentMove.rowsCount; i++ )
				{
					for ( int j = 0; j < opponentMove.colsCount; j++ )
						std::cout << (int)el->state.GetFieldState(j,i);
					std::cout << "\n";
				}
				std::cout << "----------------------\n";
			}
			throw std::exception("Undefined move");			
		}

	private:
		void IncrementPlayer()
		{
			currentPlayer = static_cast<BoardState::FieldState>((currentPlayer + 1) % BoardState::FieldState::Unknown);
			if (currentPlayer == BoardState::FieldState::Empty)
				currentPlayer = BoardState::FieldState::Player1;
			currentDepth--;
		}
	public:
		unsigned GetCurrentDepth() const
		{
			return currentDepth;
		}
	};
}