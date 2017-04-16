#pragma once
#include "StateGenerator.h"
#include "Multithreading\ParallelJob.h"
#include <functional>

using darknessNight::Multithreading::ParallelJob;

namespace SI::Reversi {
	class MinMax {
	protected:
		MapState::State siPlayer;
		MapState::State currentPlayer = MapState::State::Player1;
	public:
		MinMax(MapState startState, MapState::State siPlayer, unsigned minDepth, std::function<double(const MapState&)> aprox)
			: siPlayer(siPlayer)
		{}

		void SetStatesGenerator(std::shared_ptr<StateGenerator> stateGenerator){}
		void SetParallelJobExecutor(std::shared_ptr<ParallelJob> executor){}

		MapState GetBestMove() {
			if (currentPlayer != siPlayer)
				throw std::exception("It's no move of SI");
			IncrementPlayer();
			return MapState();
		}
		MapState GetBestMoveAsync(std::function<void(const MapState&)> callback) {
			return MapState();
		}

		void SetOpponentMove(const MapState& opponentMove){
			if (currentPlayer == siPlayer)
				throw std::exception("It's move of SI");
			IncrementPlayer();
		}

	private:
		void IncrementPlayer()
		{
			currentPlayer = (MapState::State) (((currentPlayer + 1) % MapState::State::Unknow));
			if (currentPlayer == MapState::State::Empty)
				currentPlayer = MapState::State::Player1;
		}
	};
}