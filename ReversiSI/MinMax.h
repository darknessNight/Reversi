#pragma once
#include "StateGenerator.h"
#include "Multithreading/ParallelJobExecutor.h"
#include "Multithreading/shared_mutex_lock_priority.h"
#include "MemoryUsageGuard.h"
#include <functional>
#include <atomic>
#include <iostream>

typedef bool POZIOM_ZAJEBISTOSCI;
#define ZAJEBISTOSC true
#define NIE_ZAJEBISTOSC false

#define ZAJEBISTOSCI_NIE_ZMIENISZ const

using namespace darknessNight::Multithreading;

namespace SI::Reversi {
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
			bool deleted = false;
			int depth = 0;

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
		std::function<bool(bool, double, BoardState, double)> betaHeur;
		unsigned minimumDepth;
		std::atomic<unsigned > currentDepth = 0;
		std::shared_ptr<ParallelJobExecutor> executor;
		std::shared_ptr<shared_mutex_lock_priority> currentStateMutex = std::make_shared<shared_mutex_lock_priority>();
		std::shared_ptr<std::thread> algorithmThread;
		bool working = true;
		bool restart = false;
		bool reachEnd = false;
		bool alphaBetaAlgorithm = SUPER_EXTRA_ZAJEBISTY_CONST;
		darknessNight::MemoryUsageGuard memoryGuard;

	public:
		MinMax(BoardState startState, BoardState::FieldState siPlayer, unsigned minDepth, std::function<double(const BoardState&)> aprox)
			: currentState(std::make_shared<MinMaxNode>(startState, siPlayer == BoardState::FieldState::Player1)), 
				siPlayer(siPlayer), heur(aprox), generatorFabric([](const BoardState& state, BoardState::FieldState player) {return std::make_shared<StateGenerator>(state,player); }),
				minimumDepth(minDepth), executor(std::make_shared<ParallelJobExecutor>())
		{
			currentState->SetAsRoot();
			algorithmThread = std::make_shared<std::thread>([&]() {CalcNextLevels(); });
			executor->SetNumberOfThreads(executor->GetCPUNumberOfThreads() - 2);
			betaHeur=[&](bool b1, double value, BoardState b2, double next) ->bool {return DefaultBetaHeur(b1, value, b2, next); };
		}

		~MinMax()
		{
			currentState = nullptr;
			working = false;
			if (algorithmThread->joinable())
				algorithmThread->join();
		}

		void SetBetaHeur(std::function<bool(bool, double, BoardState, double)> heur)
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
			CheckAndPrepare();

			WaitForMinDepth();

			auto bestState = FindBestNextMove();

			ChangeCurrentState(bestState);
			return bestState->state;
		}

		void ChangeCurrentState(std::shared_ptr<MinMaxNode> &bestState)
		{
			currentStateMutex->lock();
			currentState = bestState;
			currentState->SetAsRoot();
			ResetMinMax();
			currentStateMutex->unlock();
		}

		std::shared_ptr<MinMaxNode> FindBestNextMove()
		{
			auto bestValue = -std::numeric_limits<double>::max();
			auto bestState = currentState;

			for ( auto el : currentState->children )
			{
				if ( bestValue<el->value )
				{
					bestState = el;
					bestValue = el->value;
				}
			}
			return bestState;
		}

		void WaitForMinDepth()
		{
			while ( (unsigned)currentDepth<minimumDepth && !reachEnd )
			{
				std::this_thread::sleep_for(std::chrono::microseconds(100));
			}

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

		void CalcNextLevels()
		{
			std::vector<std::shared_ptr<MinMaxNode>> levels[2];
			int parent = 0;
			int child = 1;
			std::mutex mutex;
			levels[parent].push_back(currentState);
			while (working) {
				CalcNextLevel(levels, child, mutex, parent);
				parent = (parent + 1) % 2;
				child = (child + 1) % 2;
				levels[child].clear();

				if (levels[parent].empty()) reachEnd = true;
				do {
					RestartCalcing(levels, parent);
					std::this_thread::sleep_for(std::chrono::microseconds(100));
				}
				while (levels[parent].empty() && working);
			}
			std::cout << "Ended game\n";
		}

		void CalcNextLevel(std::vector<std::shared_ptr<SI::Reversi::MinMax::MinMaxNode>>  *levels, int child, std::mutex &mutex, int parent)
		{
			auto start = std::chrono::high_resolution_clock::now();
			executor->ForEach<std::shared_ptr<MinMaxNode>>([&] (std::shared_ptr<MinMaxNode>& next) {
				std::shared_lock<shared_mutex_lock_priority> lock(*currentStateMutex);
				minmax(next, levels[child], mutex);
			}, levels[parent]);
			auto time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start);

			currentDepth = currentDepth + 1;

			std::cout << "Count of childrens: " << levels[child].size() << "; Level: " << (unsigned)currentDepth
				<< " Available memory:" << memoryGuard.GetAllAvailableMemory() / 1024.0 / 1024.0 << " MB Render time:" << time.count() / 1000.f << "ms\n";

		}

		void RestartCalcing(std::vector<std::shared_ptr<SI::Reversi::MinMax::MinMaxNode>> *levels, int parent)
		{
			if ( restart )
			{
				reachEnd = false;
				std::shared_lock<shared_mutex_lock_priority> lock(*currentStateMutex);
				levels[parent].clear();
				AppendAllChildren(levels[parent]);
				restart = false;
				std::cout << "Count of childrens: " << levels[parent].size() << "; Current level after restart: " << (unsigned)currentDepth
					<< " Available memory:" << memoryGuard.GetAllAvailableMemory() / 1024.0 / 1024.0 << " MB" << "\n";
			}
		}

		void AppendAllChildren(std::vector<std::shared_ptr<MinMaxNode>> &parents)
		{
			std::vector<std::shared_ptr<MinMaxNode>> levels[2];
			int parentLevel = 0;
			int childLevel = 1;
			std::mutex mutex;
			currentState->depth = 0;
			levels[parentLevel].push_back(currentState);
			currentDepth = std::numeric_limits<unsigned>::max();
			while ( !levels[parentLevel].empty() )
			{
				AppendNodes(levels, parentLevel, parents, childLevel);
				currentDepth = currentDepth + 1;
				parentLevel = (parentLevel + 1) % 2;
				childLevel = (childLevel + 1) % 2;
				levels[childLevel].clear();
			}
		}

		void AppendNodes(std::vector<std::shared_ptr<SI::Reversi::MinMax::MinMaxNode>>  *levels, int parentLevel, std::vector<std::shared_ptr<SI::Reversi::MinMax::MinMaxNode>> & parents, int childLevel)
		{
			for ( auto node : levels[parentLevel] )
			{
				if ( node->children.empty() || node->deleted )
				{
					node->deleted = false;
					currentDepth = min(currentDepth, node->depth);
					parents.push_back(node);
				}
				for ( auto child : node->children )
				{
					child->depth = node->depth + 1;
					levels[childLevel].push_back(child);
				}
			}
		}

		void minmax(std::shared_ptr<MinMaxNode> node, std::vector<std::shared_ptr<MinMaxNode>> &children, std::mutex &mutex) const
		{
			if ( node->parent.expired() && !node->IsRoot() )
				return;

			auto parent = node->parent.lock();

			auto generator = generatorFabric(node->state, node->maximizing?siPlayer:GetNotSiPlayer());


			if (!generator->HasNextState()) {
				if ( node->parent.expired() )
					return;
				{
					auto parentGenerator = generatorFabric(parent->state, parent->maximizing ? siPlayer : GetNotSiPlayer());
					if (!parentGenerator->HasNextState())
						return;
				}

				AddNodeForPassMove(mutex, node, children);
				return;
			}
			
			if (IsNodeCutted(node, parent))
				return;

			AppendAllChildrens(generator, node, mutex, children);
		}

		void AddNodeForPassMove(std::mutex & mutex, std::shared_ptr<MinMaxNode>& node, std::vector<std::shared_ptr<MinMaxNode>>& children) const
		{
			std::lock_guard<std::mutex> lock(mutex);
			auto newNode = std::make_shared<MinMaxNode>(*node);
			newNode->parent = node;
			newNode->maximizing = !node->maximizing;
			newNode->depth = node->depth + 1;
			node->children.push_back(newNode);
			children.push_back(newNode);
			RefreshParent(newNode);
		}

		bool IsNodeCutted(std::shared_ptr<MinMaxNode>& node, std::shared_ptr<MinMaxNode>& parent) const{
			if (alphaBetaAlgorithm && !node->root && node->depth>4)
				if (betaHeur(parent->maximizing, parent->value, node->state, node->value)) {
					node->deleted = true;
					return true;
				}
			return false;
		}

		void AppendAllChildrens(std::shared_ptr<SI::Reversi::StateGenerator> &generator, std::shared_ptr<MinMaxNode> node, std::mutex & mutex, std::vector<std::shared_ptr<MinMaxNode>>& children) const
		{
			auto nexts = generator->GetAllNextStates();
			for (auto el : nexts)
			{
				memoryGuard.WaitForAvailableMemeory();
				auto child = std::make_shared<MinMaxNode>(el, !node->maximizing);
				child->parent = node;
				child->depth = node->depth + 1;
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

			std::lock_guard<shared_mutex_lock_priority> lock(*currentStateMutex);
			for (auto el : currentState->children)
			{
				if (el->state==opponentMove)
				{
					currentState = el;
					currentState->SetAsRoot();
					IncrementPlayer();
					ResetMinMax();
					return;
				}
			}

			throw std::exception("Undefined move");			
		}

	private:
		void IncrementPlayer()
		{
			currentPlayer = static_cast<BoardState::FieldState>((currentPlayer + 1) % BoardState::FieldState::Unknown);
			if (currentPlayer == BoardState::FieldState::Empty)
				currentPlayer = BoardState::FieldState::Player1;
			currentDepth=currentDepth-1;
			std::cout << "Decrease level of tree. New level " << (unsigned)currentDepth <<"\n";
		}
	public:
		unsigned GetCurrentDepth() const
		{
			return (unsigned)currentDepth;
		}
		protected:
			bool DefaultBetaHeur(bool maximizing, double value, BoardState next, double nextValue)
			{
				auto result=value > nextValue*nextValue;
				return maximizing ? result : !result;
			}
	};
}