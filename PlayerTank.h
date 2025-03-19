#ifndef PLAYER_TANK_H  // Header guard to prevent multiple inclusions
#define PLAYER_TANK_H

#include "raylib.h"  // Include the main Raylib library
#include "raymath.h"  // Include Raylib's math utilities
#include <iostream>  // Include the standard input/output library
#include <vector>  // Include the vector library for dynamic arrays
#include "GameStatus.h"  // Include the header for game status management
#include "obstacles.h"  // Include the header for obstacles
#include "EnemyTank.h"  // Include the header for enemy tanks
#include "TankShell.h"  // Include the header for tank shells

class PlayerTank {
private:
    float rotation;  // Current rotation of the tank
    float speed;  // Speed of the tank
    float rotationSpeed = 20.0f;  // Speed at which the tank rotates

    Texture2D textures[12];  // Array to store the tank's animation frames
    size_t currentFrame = 0;  // Current frame of the animation
    float animationTimer = 0;  // Timer for animation frame updates
    float frameTime;  // Time between animation frames

    Texture2D turretTexture;  // Texture for the tank's turret
    Vector2 turretPosition;  // Position of the turret
    Vector2 turretOrigin;  // Origin point for the turret's rotation
    float turretAngle;  // Current angle of the turret

    Vector2 mousePosXY;  // Position of the mouse cursor

    Vector2 shellOutPos;  // Position where shells are fired from

    float startRotation = 0.0f;  // Initial rotation of the tank
    float tankSpeed = 385;  // Base speed of the tank

#define MAX_SOUNDS 20  // Maximum number of sound instances

    Sound shootingSound;  // Sound for shooting
    Sound shootingSoundArray[MAX_SOUNDS] = { 0 };  // Array to store multiple instances of the shooting sound
    int currentShootingSound;  // Index of the current shooting sound instance

    Sound hittingSound;  // Sound for when the tank is hit
    Sound hittingSoundArray[MAX_SOUNDS] = { 0 };  // Array to store multiple instances of the hitting sound
    int currentHittingSound;  // Index of the current hitting sound instance

    Sound enemyDestroySound;  // Sound for when an enemy is destroyed
    Sound enemyDestroySoundArray[MAX_SOUNDS] = { 0 };  // Array to store multiple instances of the enemy destroy sound
    int currentEnemyDestroySoundSound;  // Index of the current enemy destroy sound instance

    Sound engineIdle;  // Sound for the tank's idle engine
    Sound engineMoving;  // Sound for the tank's moving engine

public:
    Vector2 position;  // Position of the tank
    Vector2 turretEnd;  // End position of the turret (for aiming)
    Rectangle tankRect;  // Bounding rectangle for the tank

    float health = 100.0f;  // Health of the tank

    void drawHealthBar() {  // Function to draw the tank's health bar
        DrawRectangle(tankRect.x + 1, tankRect.y - 24, health / 100.0f * tankRect.width - 2, 11.0f, MAROON);  // Draw the health bar
        DrawRectangleLinesEx({ tankRect.x, tankRect.y - 25, tankRect.width, 13.0f }, 3.0f, DARKGREEN);  // Draw the health bar border
    }

    PlayerTank() {};  // Default constructor

    void initialise(float startX, float startY) {  // Function to initialize the tank
        position = { startX, startY };  // Set the initial position
        rotation = startRotation;  // Set the initial rotation
        speed = tankSpeed;  // Set the speed

        tankRect = { position.x, position.y, (float)textures[0].width, (float)textures[0].height };  // Initialize the tank's bounding rectangle

        currentFrame = 0;  // Reset the animation frame
        animationTimer = 0.0f;  // Reset the animation timer
        frameTime = 0.01f;  // Set the time between frames

        shootingSoundArray[0] = LoadSound("sounds/shot2.mp3");  // Load the shooting sound

        for (int i = 1; i < MAX_SOUNDS; i++) {  // Create aliases for the shooting sound
            shootingSoundArray[i] = LoadSoundAlias(shootingSoundArray[0]);
        }
        currentShootingSound = 0;  // Reset the shooting sound index

        hittingSoundArray[0] = LoadSound("sounds/hit.mp3");  // Load the hitting sound

        for (int i = 1; i < MAX_SOUNDS; i++) {  // Create aliases for the hitting sound
            hittingSoundArray[i] = LoadSoundAlias(hittingSoundArray[0]);
        }
        currentHittingSound = 0;  // Reset the hitting sound index

        enemyDestroySoundArray[0] = LoadSound("sounds/enemyDestroyed.mp3");  // Load the enemy destroy sound

        for (int i = 1; i < MAX_SOUNDS; i++) {  // Create aliases for the enemy destroy sound
            enemyDestroySoundArray[i] = LoadSoundAlias(enemyDestroySoundArray[0]);
        }
        currentEnemyDestroySoundSound = 0;  // Reset the enemy destroy sound index

        engineIdle = LoadSound("sounds/engineIdle.mp3");  // Load the idle engine sound
        engineMoving = LoadSound("sounds/engineMoving.mp3");  // Load the moving engine sound
    }

    void playHitSound() {  // Function to play the hitting sound
        PlaySound(hittingSoundArray[currentHittingSound]);  // Play the current hitting sound
        currentHittingSound++;  // Move to the next sound instance
        if (currentHittingSound >= MAX_SOUNDS)  // Reset the index if it exceeds the maximum
            currentHittingSound = 0;
    }

    void playShootSound() {  // Function to play the shooting sound
        PlaySound(shootingSoundArray[currentShootingSound]);  // Play the current shooting sound
        currentShootingSound++;  // Move to the next sound instance
        if (currentShootingSound >= MAX_SOUNDS)  // Reset the index if it exceeds the maximum
            currentShootingSound = 0;
    }

    void playEnemyDestroySound() {  // Function to play the enemy destroy sound
        PlaySound(enemyDestroySoundArray[currentEnemyDestroySoundSound]);  // Play the current enemy destroy sound
        currentEnemyDestroySoundSound++;  // Move to the next sound instance
        if (currentEnemyDestroySoundSound >= MAX_SOUNDS)  // Reset the index if it exceeds the maximum
            currentEnemyDestroySoundSound = 0;
    }

    void LoadTankTexture() {  // Function to load the tank's textures
        for (int i = 0; i < 12; ++i) {  // Load all 12 animation frames
            textures[i] = LoadTexture(TextFormat("img/playerTank/pixil-frame-%01d.png", i + 1));
        }

        turretTexture = LoadTexture("img/playerTank/turret.png");  // Load the turret texture

        turretPosition = GetPosition();  // Set the turret's position
        turretOrigin = { turretTexture.width / 2.0f, turretTexture.height / 2.0f };  // Set the turret's origin
        turretOrigin.y += 17;  // Adjust the origin for proper alignment
    }

    void Update(float deltaTime, Vector2 mousePos, GameStatus& gameStatus, vector<TankShell>& playerTankShells, Camera2D& camera, int& canvasWidth, int& canvasHeight, vector<Obstacle>& obstacles, vector<EnemyTank>& allEnemyTanks) {  // Function to update the tank's state
        if (!(IsKeyDown(KEY_W) || IsKeyDown(KEY_A) || IsKeyDown(KEY_S) || IsKeyDown(KEY_D))) {  // If no movement keys are pressed
            StopSound(engineMoving);  // Stop the moving engine sound
            if (!IsSoundPlaying(engineIdle)) {  // If the idle engine sound is not playing
                PlaySound(engineIdle);  // Play the idle engine sound
            }
        }
        else {  // If movement keys are pressed
            StopSound(engineIdle);  // Stop the idle engine sound
            if (!IsSoundPlaying(engineMoving)) {  // If the moving engine sound is not playing
                PlaySound(engineMoving);  // Play the moving engine sound
            }
        }

        // Handle tank rotation based on key inputs
        if (IsKeyDown(KEY_W) && IsKeyDown(KEY_D)) {  // Move diagonally up-right
            float targetRotation = 45.0f;  // Target rotation angle
            float deltaRotation = targetRotation - rotation;  // Difference between current and target rotation
            if (deltaRotation > 180.0f) deltaRotation -= 360.0f;  // Adjust for circular rotation
            else if (deltaRotation < -180.0f) deltaRotation += 360.0f;  // Adjust for circular rotation
            if (fabs(deltaRotation) > rotationSpeed) {  // If the difference is greater than the rotation speed
                rotation += (deltaRotation > 0.0f) ? rotationSpeed : -rotationSpeed;  // Rotate towards the target
            }
            else {
                rotation = targetRotation;  // Set the rotation to the target
            }
        }
        else if (IsKeyDown(KEY_W) && IsKeyDown(KEY_A)) {  // Move diagonally up-left
            float targetRotation = 315.0f;
            float deltaRotation = targetRotation - rotation;
            if (deltaRotation > 180.0f) deltaRotation -= 360.0f;
            else if (deltaRotation < -180.0f) deltaRotation += 360.0f;
            if (fabs(deltaRotation) > rotationSpeed) {
                rotation += (deltaRotation > 0.0f) ? rotationSpeed : -rotationSpeed;
            }
            else {
                rotation = targetRotation;
            }
        }
        else if (IsKeyDown(KEY_S) && IsKeyDown(KEY_D)) {  // Move diagonally down-right
            float targetRotation = 135.0f;
            float deltaRotation = targetRotation - rotation;
            if (deltaRotation > 180.0f) deltaRotation -= 360.0f;
            else if (deltaRotation < -180.0f) deltaRotation += 360.0f;
            if (fabs(deltaRotation) > rotationSpeed) {
                rotation += (deltaRotation > 0.0f) ? rotationSpeed : -rotationSpeed;
            }
            else {
                rotation = targetRotation;
            }
        }
        else if (IsKeyDown(KEY_S) && IsKeyDown(KEY_A)) {  // Move diagonally down-left
            float targetRotation = 225.0f;
            float deltaRotation = targetRotation - rotation;
            if (deltaRotation > 180.0f) deltaRotation -= 360.0f;
            else if (deltaRotation < -180.0f) deltaRotation += 360.0f;
            if (fabs(deltaRotation) > rotationSpeed) {
                rotation += (deltaRotation > 0.0f) ? rotationSpeed : -rotationSpeed;
            }
            else {
                rotation = targetRotation;
            }
        }
        else if (IsKeyDown(KEY_W)) {  // Move up
            float targetRotation = 0.0f;
            float deltaRotation = targetRotation - rotation;
            if (deltaRotation > 180.0f) deltaRotation -= 360.0f;
            else if (deltaRotation < -180.0f) deltaRotation += 360.0f;
            if (fabs(deltaRotation) > rotationSpeed) {
                rotation += (deltaRotation > 0.0f) ? rotationSpeed : -rotationSpeed;
            }
            else {
                rotation = targetRotation;
            }
        }
        else if (IsKeyDown(KEY_A)) {  // Move left
            float targetRotation = 270.0f;
            float deltaRotation = targetRotation - rotation;
            if (deltaRotation > 180.0f) deltaRotation -= 360.0f;
            else if (deltaRotation < -180.0f) deltaRotation += 360.0f;
            if (fabs(deltaRotation) > rotationSpeed) {
                rotation += (deltaRotation > 0.0f) ? rotationSpeed : -rotationSpeed;
            }
            else {
                rotation = targetRotation;
            }
        }
        else if (IsKeyDown(KEY_S)) {  // Move down
            float targetRotation = 180.0f;
            float deltaRotation = targetRotation - rotation;
            if (deltaRotation > 180.0f) deltaRotation -= 360.0f;
            else if (deltaRotation < -180.0f) deltaRotation += 360.0f;
            if (fabs(deltaRotation) > rotationSpeed) {
                rotation += (deltaRotation > 0.0f) ? rotationSpeed : -rotationSpeed;
            }
            else {
                rotation = targetRotation;
            }
        }
        else if (IsKeyDown(KEY_D)) {  // Move right
            float targetRotation = 90.0f;
            float deltaRotation = targetRotation - rotation;
            if (deltaRotation > 180.0f) deltaRotation -= 360.0f;
            else if (deltaRotation < -180.0f) deltaRotation += 360.0f;
            if (fabs(deltaRotation) > rotationSpeed) {
                rotation += (deltaRotation > 0.0f) ? rotationSpeed : -rotationSpeed;
            }
            else {
                rotation = targetRotation;
            }
        }

        // Handle tank movement
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_A) || IsKeyDown(KEY_S) || IsKeyDown(KEY_D)) {  // If any movement key is pressed
            Vector2 potentialPosition = {  // Calculate the potential new position
                position.x + speed * cosf((rotation - 90.0f) * DEG2RAD) * deltaTime,
                position.y + speed * sinf((rotation - 90.0f) * DEG2RAD) * deltaTime
            };

            Rectangle tankBoundingBox = {  // Create a bounding box for collision detection
                potentialPosition.x - textures[currentFrame].width / 2,
                potentialPosition.y - textures[currentFrame].height / 2,
                static_cast<float>(textures[currentFrame].width),
                static_cast<float>(textures[currentFrame].height)
            };

            bool collisionDetected = false;  // Flag to check for collisions
            for (const auto& obs : obstacles) {  // Check for collisions with obstacles
                if (obs.type == BRICK || obs.type == BARRIER || obs.type == WATER) {
                    if (CheckCollisionRecs(tankBoundingBox, obs.sizeAndPosition)) {
                        collisionDetected = true;
                        break;
                    }
                }
            }

            for (auto& enemyTank : allEnemyTanks) {  // Check for collisions with enemy tanks
                if (CheckCollisionRecs(tankBoundingBox, enemyTank.posAndRect)) {
                    collisionDetected = true;
                    break;
                }
            }

            if (!collisionDetected) {  // If no collision is detected
                // Adjust position to stay within the canvas boundaries
                if (tankBoundingBox.x < 0) potentialPosition.x = textures[currentFrame].width / 2;
                if (tankBoundingBox.y < 0) potentialPosition.y = textures[currentFrame].height / 2;
                if (tankBoundingBox.x + tankBoundingBox.width > canvasWidth) potentialPosition.x = canvasWidth - textures[currentFrame].width / 2;
                if (tankBoundingBox.y + tankBoundingBox.height > canvasHeight) potentialPosition.y = canvasHeight - textures[currentFrame].height / 2;

                position = potentialPosition;  // Update the tank's position

                // Update the animation frame
                animationTimer += GetFrameTime();
                if (animationTimer >= frameTime) {
                    currentFrame = (currentFrame + 1) % 12;
                    animationTimer = 0.0f;
                }
            }
        }

        mousePosXY = mousePos;  // Update the mouse position

        turretPosition = GetPosition();  // Update the turret's position

        // Calculate the turret's angle based on the mouse position
        turretAngle = atan2(mousePosXY.y - turretPosition.y, mousePosXY.x - turretPosition.x) * RAD2DEG + 90;

        // Calculate the end point of the turret for aiming
        turretEnd = CalculateTopMidPoint(turretPosition, turretTexture.width, turretTexture.height + 32, turretAngle);

        // Handle shooting
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {  // If the left mouse button is pressed
            playerTankShells.emplace_back(turretEnd, turretAngle, mousePosXY, camera, PLAYERTANK);  // Create a new tank shell
            playShootSound();  // Play the shooting sound
        }

        // Update the tank's bounding rectangle
        tankRect = { position.x - (float)textures[0].width / 2.0f, position.y - (float)textures[0].height / 2.0f, (float)textures[0].width, (float)textures[0].height };
    }

    void Draw() {  // Function to draw the tank
        Texture2D currentTexture = textures[currentFrame];  // Get the current animation frame

        Vector2 origin = { (float)currentTexture.width / 2.0f, (float)currentTexture.height / 2.0f };  // Set the origin for rotation

        Rectangle source = { 0.0f, 0.0f, (float)currentTexture.width, (float)currentTexture.height };  // Define the source rectangle for the texture

        Rectangle dest = { position.x, position.y, (float)currentTexture.width, (float)currentTexture.height };  // Define the destination rectangle for the texture

        DrawTexturePro(currentTexture, source, dest, origin, rotation, WHITE);  // Draw the tank

        // Draw the turret
        DrawTexturePro(
            turretTexture,
            { 0, 0, (float)turretTexture.width, (float)turretTexture.height },
            { turretPosition.x, turretPosition.y, (float)turretTexture.width, (float)turretTexture.height },
            turretOrigin,
            turretAngle,
            WHITE
        );

        DrawCircleV(turretEnd, 5, BLUE);  // Draw a circle at the end of the turret for debugging
    }

    void drawDebug() {  // Function to draw debug information
        DrawText(TextFormat("FPS: %d", GetFPS()), 50, 10, 40, DARKGRAY);  // Display the current FPS
    }

    Vector2 CalculateTopMidPoint(Vector2 center, float width, float height, float angle) const {  // Function to calculate the top midpoint of the turret
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

    Vector2 GetPosition() const {  // Function to get the tank's position
        return position;
    }

    float GetRotation() const {  // Function to get the tank's rotation
        return rotation;
    }

    ~PlayerTank() {  // Destructor to clean up resources
        for (int i = 0; i < 12; ++i) {  // Unload all animation frames
            UnloadTexture(textures[i]);
        }

        UnloadTexture(turretTexture);  // Unload the turret texture

        for (int i = 1; i < MAX_SOUNDS; i++)  // Unload all shooting sound aliases
            UnloadSoundAlias(shootingSoundArray[i]);
        UnloadSound(shootingSoundArray[0]);  // Unload the original shooting sound

        for (int i = 1; i < MAX_SOUNDS; i++)  // Unload all hitting sound aliases
            UnloadSoundAlias(hittingSoundArray[i]);
        UnloadSound(hittingSoundArray[0]);  // Unload the original hitting sound

        UnloadSound(engineIdle);  // Unload the idle engine sound
        UnloadSound(engineMoving);  // Unload the moving engine sound
    }
};

#endif  // End of the header guard
