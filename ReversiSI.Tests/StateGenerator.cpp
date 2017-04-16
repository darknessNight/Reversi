#include "stdafx.h"
#include "CppUnitTest.h"
#include "../ReversiSI/StateGenerator.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SI::Reversi::Tests
{
	TEST_CLASS(StateGeneratorTests)
	{
	public:

		//przyk³adowy test, który trzeba zmieniæ lub usun¹æ
		TEST_METHOD(generatingStates_HasFirstStateAndPlayer1_CheckReturnAllNextStates)
		{
			MapState state;
			state.SetFieldState(4, 4, MapState::State::Player1);
			state.SetFieldState(3, 3, MapState::State::Player1);
			state.SetFieldState(3, 4, MapState::State::Player2);
			state.SetFieldState(4, 3, MapState::State::Player2);
			StateGenerator generator(state, MapState::State::Player1);

			std::vector<MapState> result = generator.GetAllNextStates(state, MapState::State::Player1);
			while ( generator.HasNextState() )
				result.push_back(generator.GetNextState());
			std::vector<MapState> expected = std::vector<MapState>();

			MapState expectedState;
			expectedState.SetFieldState(4, 4, MapState::State::Player1);
			expectedState.SetFieldState(4, 3, MapState::State::Player1);
			expectedState.SetFieldState(4, 2, MapState::State::Player1);
			expectedState.SetFieldState(3, 3, MapState::State::Player1);
			expectedState.SetFieldState(3, 4, MapState::State::Player2);

			expected.push_back(expectedState);

			expectedState.SetFieldState(4, 4, MapState::State::Player1);
			expectedState.SetFieldState(4, 3, MapState::State::Player2);
			expectedState.SetFieldState(2, 4, MapState::State::Player1);
			expectedState.SetFieldState(3, 3, MapState::State::Player1);
			expectedState.SetFieldState(3, 4, MapState::State::Player1);

			expected.push_back(expectedState);

			expectedState.SetFieldState(3, 3, MapState::State::Player1);
			expectedState.SetFieldState(4, 3, MapState::State::Player1);
			expectedState.SetFieldState(5, 3, MapState::State::Player1);
			expectedState.SetFieldState(4, 4, MapState::State::Player1);
			expectedState.SetFieldState(3, 4, MapState::State::Player2);

			expected.push_back(expectedState);

			expectedState.SetFieldState(3, 3, MapState::State::Player1);
			expectedState.SetFieldState(4, 3, MapState::State::Player2);
			expectedState.SetFieldState(3, 4, MapState::State::Player1);
			expectedState.SetFieldState(4, 4, MapState::State::Player1);
			expectedState.SetFieldState(3, 5, MapState::State::Player1);

			expected.push_back(expectedState);

			MatchNextStates(expected, result);
		}

		void MatchNextStates(std::vector<SI::Reversi::MapState> &expected, std::vector<SI::Reversi::MapState> &result)
		{
			if ( expected.size() != result.size() )
				Assert::Fail(L"Expected and result states number mismatch");
			for ( auto el : expected )
			{
				bool found = false;
				for ( auto i : result )
				{
					if ( i == el )
					{
						found = true;
						break;
					}
				}
				if ( !found )
					Assert::Fail(L"Didn\'t find one of next states");//mo¿na zmieniæ ¿eby wyœwietla³o coœ sensownego
			}
		}
	};
}