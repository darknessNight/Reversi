#pragma once
#include "StateGenerator.h"
#include "Multithreading/ParallelJobExecutor.h"
#include "MemoryUsageGuard.h"
#include <functional>
#include <atomic>
#include <iostream>

/*namespace std {
	typedef shared_timed_mutex shared_mutex;
}*/

typedef bool POZIOM_ZAJEBISTOSCI;
#define ZAJEBISTOSC true
#define NIE_ZAJEBISTOSC false

#define ZAJEBISTOSCI_NIE_ZMIENISZ const

//using darknessNight_Multithreading::ParallelJobExecutor;
using darknessNight::Multithreading::ParallelJobExecutor;

namespace SI::Reversi {
//namespace SI_Reversi {
	class MinMax {
		ZAJEBISTOSCI_NIE_ZMIENISZ POZIOM_ZAJEBISTOSCI SUPER_EXTRA_ZAJEBISTY_CONST = ZAJEBISTOSC;//TUTAJ JEST ZAJEBISTY CONST
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
		std::function<bool(BoardState, BoardState)> betaHeur;
		unsigned minimumDepth;
		unsigned currentDepth=1;
		std::shared_ptr<ParallelJobExecutor> executor;
		std::shared_ptr<std::shared_mutex> currentStateMutex = std::make_shared<std::shared_mutex>();
		std::shared_ptr<std::thread> algorithmThread;
		bool working = true;
		bool restart = false;
		bool alphaBetaAlgorithm = SUPER_EXTRA_ZAJEBISTY_CONST;
		darknessNight::MemoryUsageGuard memoryGuard;

	public:
		MinMax(BoardState startState, BoardState::FieldState siPlayer, unsigned minDepth, std::function<double(const BoardState&)> aprox)
			: currentState(std::make_shared<MinMaxNode>(startState, siPlayer == BoardState::FieldState::Player1)), 
				siPlayer(siPlayer), heur(aprox), generatorFabric([](const BoardState& state, BoardState::FieldState player) {return std::make_shared<StateGenerator>(state,player); }),
				minimumDepth(minDepth), executor(std::make_shared<ParallelJobExecutor>())
		{
			currentState->SetAsRoot();
			algorithmThread = std::make_shared<std::thread>([&]() {FindBestMove(); });
			executor->SetNumberOfThreads(executor->GetCPUNumberOfThreads() - 2);
			betaHeur=[&](BoardState b1, BoardState b2) ->bool {return DefaultBetaHeur(b1, b2); };
		}

		~MinMax()
		{
			currentState = nullptr;
			working = false;
			if (algorithmThread->joinable())
				algorithmThread->join();
		}

		void SetBetaHeur(std::function<bool(BoardState, BoardState)> heur)
		{
			betaHeur = heur;
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
			std::cout << ">>Checem ruch od SI\n;";
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
			ResetMinMax();
			currentStateMutex->unlock();
			std::cout << "End of minmax\n";
			std::cout << ">>Mam ruch od SI\n;";
			return bestState->state;
		}

	protected:
		void ResetMinMax()
		{
			restart = true;
			executor->Stop();
		}

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

				{
					std::shared_lock<std::shared_mutex> lock(*currentStateMutex);
					levels[child].clear();
				}

				do
				if ( restart )
				{
					std::cout << "Start restarting\n";
					std::shared_lock<std::shared_mutex> lock(*currentStateMutex);
					AppendAllChildren(levels[parent]);
					restart = false;
					std::cout << "Ended restarting\n";
				}
				while (levels[parent].empty() && working);
			}
			std::cout << "Ended game\n";
		}

		void AppendAllChildren(std::vector<std::shared_ptr<MinMaxNode>> &parents)
		{
			std::vector<std::shared_ptr<MinMaxNode>> levels[2];
			int parentLevel = 0;
			int childLevel = 1;
			std::mutex mutex;
			levels[parentLevel].push_back(currentState);
			while ( !levels[parentLevel].empty() )
			{
				for ( auto node : levels[parentLevel] )
				{
					if ( node->children.empty() )
						parents.push_back(node);
					for ( auto child : node->children )
					{
						levels[childLevel].push_back(child);
					}
				}

				parentLevel = (parentLevel + 1) % 2;
				childLevel = (childLevel + 1) % 2;
				levels[childLevel].clear();
			}
		}

		void minmax(std::shared_ptr<MinMaxNode> node, std::vector<std::shared_ptr<MinMaxNode>> &children, std::mutex &mutex) const
		{
			if ( node->parent.expired() && !node->IsRoot() )
				return;

			auto generator = generatorFabric(node->state, node->maximizing?siPlayer:GetNotSiPlayer());


			if (!generator->HasNextState()) {
				if ( node->parent.expired() )
					return;
				auto parentGenerator = generatorFabric(node->parent.lock()->state, node->parent.lock()->maximizing ? siPlayer : GetNotSiPlayer());
				if ( !parentGenerator->HasNextState() )
					return;

				std::lock_guard<std::mutex> lock(mutex);
				auto newNode = std::make_shared<MinMaxNode>(*node);
				newNode->parent = node;
				newNode->maximizing = !node->maximizing;
				node->children.push_back(newNode);
				children.push_back(newNode);
				RefreshParent(newNode);
				return;
			}

			auto nexts=generator->GetAllNextStates();
			
			if (alphaBetaAlgorithm &&!node->root && currentDepth>2 && betaHeur(node->state, node->parent.lock()->state)) return;
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
			std::cout << ">>Wpycham mu ruch do gardla\n;";
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
					ResetMinMax();
					std::cout << ">>Wepchnalem mu ruch do gardla\n;";
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
		protected:
			bool DefaultBetaHeur(BoardState current, BoardState next)
			{
				return heur(current) > heur(next)*heur(next);
			}
	};
}