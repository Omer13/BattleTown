#include "raylib.h"  // Include the main Raylib library
#include "raymath.h"  // Include Raylib's math utilities
#include <iostream>  // Include the standard input/output library
#include <vector>  // Include the vector library for dynamic arrays

using namespace std;  // Use the standard namespace

class menuTankShell {  // Class for the tank shell in the menu
private:
    Vector2 position;  // Position of the shell
    Vector2 direction;  // Direction of the shell
    float speed = 750.0f;  // Speed of the shell
    float rotation;  // Rotation of the shell

public:
    menuTankShell(Vector2 startPos, float angle)  // Constructor to initialize the shell
        : position(startPos), rotation(angle) {
        // Calculate the direction vector based on the angle
        direction = {
            cos((angle - 90) * DEG2RAD),
            sin((angle - 90) * DEG2RAD)
        };
    }

    void Update(float deltaTime) {  // Update the shell's position
        position.x += direction.x * speed * deltaTime;
        position.y += direction.y * speed * deltaTime;
    }

    void Draw(Texture2D shellTexture) const {  // Draw the shell
        Vector2 origin = { (float)shellTexture.width / 2.0f, (float)shellTexture.height / 2.0f };  // Set the origin for rotation
        Rectangle source = { 0.0f, 0.0f, (float)shellTexture.width, (float)shellTexture.height };  // Define the source rectangle
        Rectangle dest = { position.x, position.y, (float)shellTexture.width, (float)shellTexture.height };  // Define the destination rectangle
        DrawTexturePro(shellTexture, source, dest, origin, rotation, WHITE);  // Draw the shell texture
    }

    bool IsOutOfBounds(int screenWidth, int screenHeight) const {  // Check if the shell is out of bounds
        return position.x < 0 || position.x > screenWidth || position.y < 0 || position.y > screenHeight;
    }

    Vector2 GetPosition() const {  // Get the shell's position
        return position;
    }

    ~menuTankShell() {}  // Destructor
};

class MainMenuTank {  // Class for the tank in the main menu
private:
    Vector2 position;  // Position of the tank
    float rotation = 0.0f;  // Rotation of the tank

    Texture2D tankTexture;  // Texture for the tank

    Texture2D turretTexture;  // Texture for the tank's turret
    Vector2 turretPosition;  // Position of the turret
    Vector2 turretOrigin;  // Origin point for the turret's rotation
    float turretAngle;  // Angle of the turret

    Vector2 mousePosXY;  // Position of the mouse cursor

    Vector2 shellOutPos;  // Position where shells are fired from

    Texture2D shellTexture;  // Texture for the shell

public:
    vector<menuTankShell> shells;  // Vector to store active shells

    MainMenuTank() {};  // Default constructor

    void initialise(float startX, float startY) {  // Initialize the tank's position
        position = { startX, startY };
    }

    void loadMainMenuTankTexture() {  // Load textures for the tank, turret, and shell
        tankTexture = LoadTexture("img/playerTank/pixil-frame-2.png");  // Load the tank texture
        turretTexture = LoadTexture("img/playerTank/turret.png");  // Load the turret texture

        Image image = LoadImage("img/playerTank/fireball2.png");  // Load the shell image
        ImageResize(&image, image.width / 4, image.height / 4);  // Resize the shell image
        shellTexture = LoadTextureFromImage(image);  // Load the shell texture
        UnloadImage(image);  // Unload the shell image

        turretPosition = GetPosition();  // Set the turret's position
        turretOrigin = { turretTexture.width / 2.0f, turretTexture.height / 2.0f };  // Set the turret's origin
        turretOrigin.y += 17;  // Adjust the origin for proper alignment
    }

    void Update() {  // Update the tank's state
        turretPosition = GetPosition();  // Update the turret's position
        turretAngle = atan2(GetMousePosition().y - turretPosition.y, GetMousePosition().x - turretPosition.x) * RAD2DEG + 90;  // Calculate the turret's angle based on the mouse position
    }

    void DrawTankAndTurret() const {  // Draw the tank and turret
        Vector2 origin = { (float)tankTexture.width / 2.0f, (float)tankTexture.height / 2.0f };  // Set the origin for rotation
        Rectangle source = { 0.0f, 0.0f, (float)tankTexture.width, (float)tankTexture.height };  // Define the source rectangle
        Rectangle dest = { position.x, position.y, (float)tankTexture.width, (float)tankTexture.height };  // Define the destination rectangle
        DrawTexturePro(tankTexture, source, dest, origin, rotation, WHITE);  // Draw the tank texture

        DrawTexturePro(  // Draw the turret texture
            turretTexture,
            { 0, 0, (float)turretTexture.width, (float)turretTexture.height },
            { turretPosition.x, turretPosition.y, (float)turretTexture.width, (float)turretTexture.height },
            turretOrigin,
            turretAngle,
            WHITE
        );

        Vector2 topMidPoint = CalculateTopMidPoint(turretPosition, turretTexture.width, turretTexture.height + 32, turretAngle);  // Calculate the top midpoint of the turret
        DrawCircleV(topMidPoint, 5, RED);  // Draw a circle at the top midpoint for debugging
    }

    Vector2 CalculateTopMidPoint(Vector2 center, float width, float height, float angle) const {  // Calculate the top midpoint of the turret
        float radians = angle * DEG2RAD;  // Convert the angle to radians
        float halfWidth = width / 2.0f;  // Calculate half the width
        float halfHeight = height / 2.0f;  // Calculate half the height

        Vector2 topLeft = { -halfWidth, -halfHeight };  // Define the top-left point
        Vector2 topRight = { halfWidth, -halfHeight };  // Define the top-right point

        // Rotate the points based on the angle
        Vector2 rotatedTopLeft = {
            topLeft.x * cos(radians) - topLeft.y * sin(radians),
            topLeft.x * sin(radians) + topLeft.y * cos(radians)
        };

        Vector2 rotatedTopRight = {
            topRight.x * cos(radians) - topRight.y * sin(radians),
            topRight.x * sin(radians) + topRight.y * cos(radians)
        };

        // Adjust the rotated points to the center
        rotatedTopLeft.x += center.x;
        rotatedTopLeft.y += center.y;
        rotatedTopRight.x += center.x;
        rotatedTopRight.y += center.y;

        // Calculate the midpoint between the rotated points
        Vector2 midPoint = {
            (rotatedTopLeft.x + rotatedTopRight.x) / 2.0f,
            (rotatedTopLeft.y + rotatedTopRight.y) / 2.0f
        };

        return midPoint;  // Return the calculated midpoint
    }

    Vector2 GetPosition() const {  // Get the tank's position
        return position;
    }

    float GetRotation() const {  // Get the tank's rotation
        return rotation;
    }

    void FireShell() {  // Fire a shell from the turret
        Vector2 topMidPoint = CalculateTopMidPoint(turretPosition, turretTexture.width, turretTexture.height, turretAngle);  // Calculate the shell's starting position
        shells.emplace_back(topMidPoint, turretAngle);  // Create a new shell
    }

    void UpdateShells(float deltaTime, int screenWidth, int screenHeight) {  // Update all active shells
        for (size_t i = 0; i < shells.size(); ++i) {
            shells[i].Update(deltaTime);  // Update the shell's position
            if (shells[i].IsOutOfBounds(screenWidth, screenHeight)) {  // Check if the shell is out of bounds
                shells.erase(shells.begin() + i);  // Remove the shell
                --i;  // Adjust the index
            }
        }
    }

    void DrawShells() const {  // Draw all active shells
        for (const auto& shell : shells) {
            shell.Draw(shellTexture);  // Draw the shell
        }
    }

    float getShellSize() {  // Get the number of active shells
        return shells.size();
    }

    ~MainMenuTank() {  // Destructor to clean up resources
        UnloadTexture(tankTexture);  // Unload the tank texture
        UnloadTexture(turretTexture);  // Unload the turret texture
        UnloadTexture(shellTexture);  // Unload the shell texture
    }
};

class ExplosionAnimation {  // Class for the explosion animation
public:
    Vector2 position;  // Position of the explosion
    int currentFrame;  // Current frame of the animation

    ExplosionAnimation(Vector2 pos, int currentFrame = 0)  // Constructor to initialize the explosion
        : position(pos), currentFrame(currentFrame) {
    }
};

class Menu {  // Class for the main menu
private:
    Texture2D backgrundTexture;  // Texture for the background

    GameState nextGameState = NotSet;  // Next game state after the menu

    Rectangle buttons[3];  // Buttons for the menu
    Color buttonColors[3] = { LIGHTGRAY, LIGHTGRAY, LIGHTGRAY };  // Colors for the buttons
    const char* buttonText[3] = { "Play", "Scoreboard", "Exit" };  // Text for the buttons
    float totalHeight = 3 * 50 + 2 * 50;  // Total height of the buttons

    MainMenuTank theMainMenuTank;  // The tank in the main menu

    vector<ExplosionAnimation> explosions;  // Vector to store active explosions

    vector<Texture2D> explosionAnimationTextures;  // Textures for the explosion animation
    vector<string> framePaths = {  // Paths to the explosion animation frames
        "img/MenuExplosionAnimation/frame1.png",
        "img/MenuExplosionAnimation/frame2.png",
        "img/MenuExplosionAnimation/frame3.png",
        "img/MenuExplosionAnimation/frame4.png",
        "img/MenuExplosionAnimation/frame5.png",
        "img/MenuExplosionAnimation/frame6.png",
        "img/MenuExplosionAnimation/frame7.png",
        "img/MenuExplosionAnimation/frame8.png",
        "img/MenuExplosionAnimation/frame9.png",
        "img/MenuExplosionAnimation/frame10.png",
        "img/MenuExplosionAnimation/frame11.png",
        "img/MenuExplosionAnimation/frame12.png",
        "img/MenuExplosionAnimation/frame13.png",
        "img/MenuExplosionAnimation/frame14.png",
        "img/MenuExplosionAnimation/frame15.png",
        "img/MenuExplosionAnimation/frame16.png"
    };
    float elapsedTime = 0.0f;  // Time elapsed since the last frame
    float interval = 0.025f;  // Time interval between frames
    bool isExplosionActive = false;  // Flag to check if an explosion is active
    bool isExplosionAnimationCompleted = false;  // Flag to check if the explosion animation is completed

#define MAX_SOUNDS 20  // Maximum number of sound instances

    Sound shootingSound;  // Sound for shooting
    Sound shootingSoundArray[MAX_SOUNDS] = { 0 };  // Array to store multiple instances of the shooting sound
    int currentShootingSound;  // Index of the current shooting sound instance

public:
    Menu() {};  // Default constructor

    void initialise(int screenWidth, int screenHeight) {  // Initialize the menu
        backgrundTexture = LoadTexture("img/mainMenuBG.png");  // Load the background texture

        theMainMenuTank.initialise(screenWidth / 2, screenHeight / 4 * 3);  // Initialize the tank's position

        float startY = (screenHeight - totalHeight) / 2.0f;  // Calculate the starting Y position for the buttons

        buttons[0] = { screenWidth / 2.0f - 100, startY - 75, 200, 50 };  // Define the "Play" button
        buttons[1] = { screenWidth / 2.f + 200.0f, startY - 75, 200, 50 };  // Define the "Scoreboard" button
        buttons[2] = { screenWidth / 2.f - 400.0f, startY - 75, 200, 50 };  // Define the "Exit" button

        shootingSoundArray[0] = LoadSound("sounds/shot2.mp3");  // Load the shooting sound
        for (int i = 1; i < MAX_SOUNDS; i++) {  // Create aliases for the shooting sound
            shootingSoundArray[i] = LoadSoundAlias(shootingSoundArray[0]);
        }
        currentShootingSound = 0;  // Reset the shooting sound index
    }

    void nextFrame() {  // Advance the explosion animation to the next frame
        for (auto it = explosions.begin(); it != explosions.end(); ) {
            if (it->currentFrame < framePaths.size() - 1) {  // If the animation is not complete
                it->currentFrame++;  // Move to the next frame
                ++it;  // Move to the next explosion
            }
            else {  // If the animation is complete
                it = explosions.erase(it);  // Remove the explosion
                isExplosionActive = false;  // Set the explosion flag to false
                isExplosionAnimationCompleted = true;  // Set the animation completion flag to true
            }
        }
    }

    void LoadFrames(const vector<string>& filepaths) {  // Load the explosion animation frames
        for (const auto& filepath : filepaths) {
            Image image = LoadImage(filepath.c_str());  // Load the image
            ImageResize(&image, 50, 53);  // Resize the image
            Texture2D texture = LoadTextureFromImage(image);  // Load the texture
            explosionAnimationTextures.push_back(texture);  // Add the texture to the vector
            UnloadImage(image);  // Unload the image
        }
    }

    void update(float deltaTime, GameStatus& gameStatus) {  // Update the menu's state
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {  // If the left mouse button is pressed
            PlaySound(shootingSoundArray[currentShootingSound]);  // Play the shooting sound
            currentShootingSound++;  // Move to the next sound instance
            if (currentShootingSound >= MAX_SOUNDS)  // Reset the index if it exceeds the maximum
                currentShootingSound = 0;
        }

        for (int i = 0; i < 3; i++) {  // Check for button hover and click
            if (CheckCollisionPointRec(GetMousePosition(), buttons[i])) {  // If the mouse is over a button
                buttonColors[i] = DARKGRAY;  // Change the button color
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {  // If the button is clicked
                    if (i == 0) {  // "Play" button
                        TraceLog(LOG_INFO, "Play button clicked");
                    }
                    else if (i == 1) {  // "Scoreboard" button
                        TraceLog(LOG_INFO, "Scoreboard button clicked");
                    }
                    else if (i == 2) {  // "Exit" button
                        TraceLog(LOG_INFO, "Exit button clicked");
                    }
                }
            }
            else {
                buttonColors[i] = LIGHTGRAY;  // Reset the button color
            }
        }

        if (!isExplosionActive || isExplosionAnimationCompleted) {  // If no explosion is active or the animation is completed
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {  // If the left mouse button is pressed
                theMainMenuTank.FireShell();  // Fire a shell
            }
        }

        theMainMenuTank.Update();  // Update the tank
        theMainMenuTank.UpdateShells(deltaTime, GetScreenWidth(), GetScreenHeight());  // Update the shells

        CheckShellButtonCollision();  // Check for collisions between shells and buttons

        elapsedTime += deltaTime;  // Update the elapsed time

        if (elapsedTime >= interval) {  // If it's time to advance the animation
            nextFrame();  // Advance to the next frame
            elapsedTime = 0.0f;  // Reset the elapsed time
        }

        if (isExplosionAnimationCompleted && nextGameState != NotSet) {  // If the explosion animation is completed and a next game state is set
            switch (nextGameState) {  // Transition to the next game state
            case InGame:
                gameStatus.currentGameState = InGame;
                break;
            case Scoreboard:
                gameStatus.currentGameState = Scoreboard;
                break;
            case Exit:
                gameStatus.currentGameState = Exit;
                break;
            }
        }
    }

    void LoadTheMainMenuTankTexture() {  // Load textures for the tank and explosion animation
        theMainMenuTank.loadMainMenuTankTexture();  // Load the tank textures
        LoadFrames(framePaths);  // Load the explosion animation frames
    }

    void drawBG(int screenWidth, int screenHeight) {  // Draw the background
        DrawTexturePro(
            backgrundTexture,
            { 0, 0, (float)backgrundTexture.width, (float)backgrundTexture.height },
            { 0, 0, (float)screenWidth, (float)screenHeight },
            Vector2{ 0, 0 },
            0.0f,
            WHITE
        );
    }

    void drawMenuTankTurretShell() {  // Draw the tank, turret, shells, and buttons
        for (int i = 0; i < 3; i++) {  // Draw the buttons
            DrawRectangleRec(buttons[i], buttonColors[i]);
            DrawText(buttonText[i],
                buttons[i].x + buttons[i].width / 2 - MeasureText(buttonText[i], 20) / 2,
                buttons[i].y + buttons[i].height / 2 - 10, 20, BLACK);
        }

        theMainMenuTank.DrawTankAndTurret();  // Draw the tank and turret

        theMainMenuTank.DrawShells();  // Draw the shells

        for (const auto& explosion : explosions) {  // Draw the explosions
            DrawTexture(explosionAnimationTextures[explosion.currentFrame],
                explosion.position.x - explosionAnimationTextures[explosion.currentFrame].width / 2,
                explosion.position.y - explosionAnimationTextures[explosion.currentFrame].height, WHITE);
        }
    }

    void drawDebug() {  // Draw debug information
        DrawText(TextFormat("FPS: %d", GetFPS()), 10, 10, 40, DARKGRAY);
    }

    void startDrawing() {  // Start drawing
        BeginDrawing();
        ClearBackground(RAYWHITE);
    }

    void endDrawing() {  // End drawing
        EndDrawing();
    }

    void CheckShellButtonCollision() {  // Check for collisions between shells and buttons
        for (int i = 0; i < 3; i++) {  // Iterate through the buttons
            for (size_t j = 0; j < theMainMenuTank.shells.size(); ++j) {  // Iterate through the shells
                menuTankShell& shell = theMainMenuTank.shells[j];
                Vector2 shellPos = shell.GetPosition();
                float shellRadius = 10.0f;  // Define the shell's collision radius

                if (CheckCollisionCircleRec(shellPos, shellRadius, buttons[i])) {  // If the shell collides with a button
                    switch (i) {  // Handle the collision based on the button
                    case 0:
                        TraceLog(LOG_INFO, "Shell hit 'Play' button!");
                        nextGameState = InGame;
                        break;
                    case 1:
                        TraceLog(LOG_INFO, "Shell hit 'Scoreboard' button!");
                        nextGameState = Scoreboard;
                        break;
                    case 2:
                        TraceLog(LOG_INFO, "Shell hit 'Exit' button!");
                        nextGameState = Exit;
                        break;
                    }

                    explosions.emplace_back(shellPos, 0);  // Create an explosion at the shell's position

                    isExplosionActive = true;  // Set the explosion flag to true
                    isExplosionAnimationCompleted = false;  // Set the animation completion flag to false

                    theMainMenuTank.shells.erase(theMainMenuTank.shells.begin() + j);  // Remove the shell
                    --j;  // Adjust the index
                }
            }
        }
    }

    ~Menu() {  // Destructor to clean up resources
        UnloadTexture(backgrundTexture);  // Unload the background texture

        for (auto& texture : explosionAnimationTextures) {  // Unload the explosion animation textures
            UnloadTexture(texture);
        }
        explosionAnimationTextures.clear();  // Clear the explosion animation textures vector

        explosions.clear();  // Clear the explosions vector

        for (int i = 1; i < MAX_SOUNDS; i++)  // Unload all shooting sound aliases
            UnloadSoundAlias(shootingSoundArray[i]);
        UnloadSound(shootingSoundArray[0]);  // Unload the original shooting sound
    }
};
