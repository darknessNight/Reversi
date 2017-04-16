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
			state.SetFieldState(4, 4, MapState::State::Player1);				//		0 0 0 0
			state.SetFieldState(3, 3, MapState::State::Player1);				//		0 1 2 0
			state.SetFieldState(3, 4, MapState::State::Player2);				//		0 2 1 0
			state.SetFieldState(4, 3, MapState::State::Player2);				//		0 0 0 0
			StateGenerator generator(state, MapState::State::Player1);			
																				
			generator.GetAllNextStates(state, MapState::State::Player1);		
																				
			std::vector<MapState> result = std::vector<MapState>();
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

			MapState expectedState2;
			expectedState2.SetFieldState(4, 4, MapState::State::Player1);
			expectedState2.SetFieldState(4, 3, MapState::State::Player2);
			expectedState2.SetFieldState(2, 4, MapState::State::Player1);
			expectedState2.SetFieldState(3, 3, MapState::State::Player1);
			expectedState2.SetFieldState(3, 4, MapState::State::Player1);

			expected.push_back(expectedState2);

			MapState expectedState3;
			expectedState3.SetFieldState(3, 3, MapState::State::Player1);
			expectedState3.SetFieldState(4, 3, MapState::State::Player1);
			expectedState3.SetFieldState(5, 3, MapState::State::Player1);
			expectedState3.SetFieldState(4, 4, MapState::State::Player1);
			expectedState3.SetFieldState(3, 4, MapState::State::Player2);

			expected.push_back(expectedState3);

			MapState expectedState4;
			expectedState4.SetFieldState(3, 3, MapState::State::Player1);
			expectedState4.SetFieldState(4, 3, MapState::State::Player2);
			expectedState4.SetFieldState(3, 4, MapState::State::Player1);
			expectedState4.SetFieldState(4, 4, MapState::State::Player1);
			expectedState4.SetFieldState(3, 5, MapState::State::Player1);

			expected.push_back(expectedState4);

			MatchNextStates(expected, result);
		}

		TEST_METHOD(Check_if_duplicates_are_correctly_recognized) {

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