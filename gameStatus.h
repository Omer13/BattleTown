#ifndef GAMESTATUS_H  // Header guard to prevent multiple inclusions
#define GAMESTATUS_H

enum GameState {  // Enumeration to define the possible states of the game
    RaylibAnimation,  // State for the Raylib logo animation
    MainMenu,         // State for the main menu
    InGame,           // State for when the game is being played
    GameOver,         // State for when the game is over
    Scoreboard,       // State for displaying the scoreboard
    Exit,             // State for exiting the game
    NotSet            // Default state (not set)
};

class GameStatus {  // Class to manage the current state of the game
public:
    GameState currentGameState = RaylibAnimation;  // Variable to store the current game state, initialized to RaylibAnimation

    GameState getGameState() {  // Function to get the current game state
        return currentGameState;  // Return the current game state
    }

    void setGameState(GameState gameState) {  // Function to set the current game state
        currentGameState = gameState;  // Update the current game state
    }
};

#endif  // End of the header guard
