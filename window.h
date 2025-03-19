#include "raylibLogoAnimation.h"    // Include the header for the Raylib logo animation
#include "raylib.h"                 // Include the main Raylib library
#include <vector>                   // Include the vector library for dynamic arrays
#include <any>                      // Include the any library for type-safe container for single values of any type
#include "PlayerTank.h"             // Include the header for the player tank
#include "gameStatus.h"             // Include the header for game status management
#include "menu.h"                   // Include the header for the game menu
#include "game.h"                   // Include the header for the main game logic

using namespace std;  // Use the standard namespace

class Window {
private:
    int screenWidth, screenHeight;  // Variables to store the screen width and height

    int canvasWidth = 1560;  // Width of the game canvas

    int canvasHeight = 54600;  // Height of the game canvas
    Camera2D camera;  // 2D camera for the game
    float deltaTime;  // Variable to store the time between frames
    Menu menu;  // Instance of the Menu class

    GameStatus gameStatus;  // Instance of the GameStatus class

    Game game;  // Instance of the Game class

    Texture2D crosshair;  // Texture for the crosshair

    RaylibLogoAnimation RaylibLogoAnimation;  // Instance of the RaylibLogoAnimation class

    Vector2 mousePosition;  // Variable to store the mouse position

    const char* gameOverText = "Game Over!";  // Text to display when the game is over

public:
    int initWindowWidth = 500, initWindowHeight = 500;  // Initial window dimensions

    Music backgroundMusic;  // Background music for the game

    Window() : game(&canvasWidth, &canvasHeight, &camera) {  // Constructor for the Window class

        SetConfigFlags(FLAG_VSYNC_HINT);  // Enable VSync to synchronize the frame rate with the monitor refresh rate

        InitWindow(initWindowWidth, initWindowHeight, "BattleTown V4");  // Initialize the game window

        InitAudioDevice();  // Initialize the audio device

        backgroundMusic = LoadMusicStream("sounds/MainMenu.mp3");  // Load the background music

        RaylibLogoAnimation.initialise(initWindowWidth, initWindowHeight);  // Initialize the Raylib logo animation

        SetWindowState(FLAG_WINDOW_UNDECORATED);  // Set the window to be undecorated (no title bar, etc.)
        DisableCursor();  // Disable the default cursor
        crosshair = LoadTexture("img/playerTank/crosshair.png");  // Load the crosshair texture

        screenWidth = GetMonitorWidth(0), screenHeight = GetMonitorHeight(0);  // Get the screen dimensions of the primary monitor

        menu.initialise(screenWidth, screenHeight);  // Initialize the menu with the screen dimensions
        menu.LoadTheMainMenuTankTexture();  // Load the main menu tank texture

        SetTargetFPS(60);  // Set the target frames per second to 60

        camera = { 0 };  // Initialize the camera
        camera.target = { (float)canvasWidth / 2.0f, (float)canvasHeight / 2.0f };  // Set the camera target to the center of the canvas
        camera.offset = { (float)screenWidth / 2.0f, (float)screenHeight / 2.0f };  // Set the camera offset to the center of the screen
        camera.rotation = 0.0f;  // Set the camera rotation to 0
        camera.zoom = 1.0f;  // Set the camera zoom to 1

        game.initialise(canvasWidth / 2.0f, canvasHeight - 50, screenWidth, screenHeight);  // Initialize the game
        game.LoadTextures();  // Load the game textures

    }

    void run() {  // Main game loop

        while (!WindowShouldClose()) {  // Continue running until the window is closed
            clipMouseToWindow();  // Clip the mouse to the window boundaries
            deltaTime = GetFrameTime();  // Get the time between frames

            switch (gameStatus.currentGameState) {  // Switch based on the current game state
            case RaylibAnimation:  // If the current state is the Raylib animation
                if (!RaylibLogoAnimation.animationCompleted) {  // If the animation is not completed
                    RaylibLogoAnimation.Update();  // Update the animation
                    RaylibLogoAnimation.Draw();  // Draw the animation
                }
                else {  // If the animation is completed
                    gameStatus.currentGameState = MainMenu;  // Change the game state to the main menu

                    SetWindowSize(screenWidth, screenHeight);  // Set the window size to the screen dimensions

                    SetWindowState(FLAG_FULLSCREEN_MODE);  // Set the window to fullscreen mode
                }
                break;
            case MainMenu:  // If the current state is the main menu
                playBgMusic();  // Play the background music

                menu.update(deltaTime, gameStatus);  // Update the menu
                menu.startDrawing();  // Start drawing the menu
                menu.drawBG(screenWidth, screenHeight);  // Draw the menu background
                menu.drawDebug();  // Draw debug information
                menu.drawMenuTankTurretShell();  // Draw the menu tank turret shell
                drawMouseCorsshair();  // Draw the mouse crosshair
                menu.endDrawing();  // End drawing the menu
                break;

            case InGame:  // If the current state is in-game

                game.checkCollisions();  // Check for collisions in the game

                game.update(deltaTime, gameStatus, screenWidth, screenHeight, canvasWidth, canvasHeight);  // Update the game
                game.startDrawing();  // Start drawing the game

                game.beginCamera2D();  // Begin the 2D camera

                game.DrawBackgroundWithTiles(canvasWidth, canvasHeight, 260);  // Draw the background with tiles

                game.endCamera2D();  // End the 2D camera

                game.drawDebug();  // Draw debug information

                drawMouseCorsshair();  // Draw the mouse crosshair
                game.endDrawing();  // End drawing the game
                break;
            case Scoreboard:  // If the current state is the scoreboard

                BeginDrawing();  // Start drawing
                ClearBackground(RAYWHITE);  // Clear the background to white
                drawMouseCorsshair();  // Draw the mouse crosshair
                EndDrawing();  // End drawing
                break;
            case GameOver:  // If the current state is game over

                playBgMusic();  // Play the background music

                BeginDrawing();  // Start drawing
                ClearBackground(RAYWHITE);  // Clear the background to white
                menu.drawBG(screenWidth, screenHeight);  // Draw the menu background

                DrawText(gameOverText, (screenWidth - MeasureText(gameOverText, 150)) / 2, (screenHeight - MeasureText(gameOverText, 150)) / 2, 150, MAROON);  // Draw the "Game Over" text
                drawMouseCorsshair();  // Draw the mouse crosshair
                EndDrawing();  // End drawing
                break;
            case Exit:  // If the current state is exit
                exit(EXIT_SUCCESS);  // Exit the game
            }

        }
    }

    void drawMouseCorsshair() {  // Function to draw the mouse crosshair
        DrawTexture(crosshair, GetMousePosition().x - crosshair.width / 2, GetMousePosition().y - crosshair.height / 2, WHITE);  // Draw the crosshair texture at the mouse position
    }

    ~Window() {  // Destructor for the Window class
        UnloadTexture(crosshair);  // Unload the crosshair texture
        CloseAudioDevice();  // Close the audio device
        CloseWindow();  // Close the game window
    }

    void clipMouseToWindow() {  // Function to clip the mouse to the window boundaries

        mousePosition = GetMousePosition();  // Get the current mouse position

        const int MOUSE_BOUND = 25;  // Define the boundary for the mouse

        if (mousePosition.x < MOUSE_BOUND) mousePosition.x = 25;  // Clip the mouse to the left boundary
        if (mousePosition.x > GetScreenWidth() - MOUSE_BOUND) mousePosition.x = GetScreenWidth() - MOUSE_BOUND;  // Clip the mouse to the right boundary
        if (mousePosition.y < MOUSE_BOUND) mousePosition.y = MOUSE_BOUND;  // Clip the mouse to the top boundary
        if (mousePosition.y > GetScreenHeight() - MOUSE_BOUND) mousePosition.y = GetScreenHeight() - MOUSE_BOUND;  // Clip the mouse to the bottom boundary

        SetMousePosition(mousePosition.x, mousePosition.y);  // Set the mouse position to the clipped position
    }

    void playBgMusic() {  // Function to play the background music

        UpdateMusicStream(backgroundMusic);  // Update the music stream

        if (!IsMusicStreamPlaying(backgroundMusic)) {  // If the music is not playing
            PlayMusicStream(backgroundMusic);  // Play the background music
        }
    }

};
