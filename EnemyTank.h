#ifndef ENEMYTANK_H
#define ENEMYTANK_H

// Include necessary libraries and headers
#include "raylib.h"  // Raylib library for graphics and input handling
#include "TankShell.h"  // Header for TankShell class
#include <random>  // Standard library for random number generation
#include <array>  // Standard library for array container
#include <string>  // Standard library for string handling
#include "PlayerTank.h"  // Header for PlayerTank class

using namespace std;  // Use the standard namespace

// Define an enumeration for different directions
enum Direction {
    UP,    // Represents the upward direction
    RIGHT, // Represents the rightward direction
    DOWN,  // Represents the downward direction
    LEFT   // Represents the leftward direction
};

// Array to map direction enum values to their string representations
string directionString[4] = { "UP", "RIGHT", "DOWN", "LEFT" };

// Define an enumeration for different types of enemy tanks
enum EnemyType {
    BASIC,    // Represents a basic enemy tank
    ADVANCED  // Represents an advanced enemy tank
};

// Define the EnemyTank class
class EnemyTank {
public:
    Rectangle posAndRect;  // Position and rectangle of the enemy tank
    EnemyType Type;        // Type of the enemy tank (BASIC or ADVANCED)
    Rectangle Rect;        // Rectangle representing the size of the tank
    bool hasSpawned = false;  // Whether the tank has spawned
    Vector2 Position;      // Position of the tank
    float Speed = 100.0f;  // Speed of the tank

    Direction guidedDirection = DOWN;  // Direction guided by player position
    Direction currentDirection = DOWN; // Current movement direction

    Direction Rotation;    // Rotation of the tank

    Vector2 centre;        // Center position of the tank

    float timeLimit;       // Time limit for direction change
    float elapsedTime;     // Elapsed time since last direction change

    float shootingInterval = 1.30f;  // Interval between shots

    float bufferDistance = 10.0f;    // Buffer distance for collision handling

    // Constructor for the EnemyTank class
    EnemyTank(EnemyType enemyType, Vector2 WH, Vector2 Pos, int timeUntilNextDirectionChange) {
        Type = enemyType;  // Set the type of the tank
        Rect = { WH.x , WH.y };  // Set the size of the tank
        Position = { Pos.x , Pos.y };  // Set the position of the tank

        centre = { Pos.x + WH.x / 2, Pos.y + WH.y / 2 };  // Calculate the center position
        timeLimit = timeUntilNextDirectionChange;  // Set the time limit for direction change
        posAndRect = { Pos.x, Pos.y, WH.x, WH.y };  // Set the position and rectangle
    }
};

// Function to check if two rectangles are different
bool areRectanglesDifferent(const Rectangle& rect1, const Rectangle& rect2) {
    return rect1.x != rect2.x || rect1.y != rect2.y || rect1.width != rect2.width || rect1.height != rect2.height;
}

// Function to update enemy tanks
void updateEnemyTanks(vector<EnemyTank>& allEnemyTanks, Rectangle& playerTankRect, float& playerTankPositionY, vector<Obstacle>& obstacles, int& CanvasWidth, int& canvasHeight, float& deltaTime, vector<TankShell>& playerTankShells, Camera2D& camera) {

    static mt19937 gen(random_device{}());  // Random number generator
    static float accumulatedTime = 0.0f;   // Accumulated time for updates
    accumulatedTime += deltaTime;          // Add deltaTime to accumulated time

    if (allEnemyTanks.empty()) return;     // If no enemy tanks, return

    // Update the center position of each enemy tank
    for (auto& enemyTank : allEnemyTanks) {
        enemyTank.centre = { enemyTank.posAndRect.x + enemyTank.posAndRect.width / 2,
                             enemyTank.posAndRect.y + enemyTank.posAndRect.height / 2 };
    }

    // Update elapsed time for each enemy tank
    if (accumulatedTime >= 0.1f) {
        for (auto& enemyTank : allEnemyTanks) {
            enemyTank.elapsedTime += accumulatedTime;
        }
        accumulatedTime = 0.0f;  // Reset accumulated time
    }

    // Handle shooting logic for enemy tanks
    for (auto& enemyTank : allEnemyTanks) {
        if (enemyTank.shootingInterval - 0.015f > 0) {
            enemyTank.shootingInterval -= 0.015f;  // Decrease shooting interval
        }
        else {
            enemyTank.shootingInterval = 1.30f;  // Reset shooting interval

            // Create a new tank shell based on the current direction
            switch (enemyTank.currentDirection) {
            case UP:
                playerTankShells.emplace_back(Vector2{ enemyTank.centre.x, enemyTank.posAndRect.y }, 0.0f, Vector2{ 0, 0 }, camera, ENEMYTANK);
                break;
            case RIGHT:
                playerTankShells.emplace_back(Vector2{ enemyTank.posAndRect.x + enemyTank.posAndRect.width, enemyTank.centre.y }, 90.0f, Vector2{ 0, 0 }, camera, ENEMYTANK);
                break;
            case DOWN:
                playerTankShells.emplace_back(Vector2{ enemyTank.centre.x, enemyTank.posAndRect.y + enemyTank.posAndRect.height }, 180.0f, Vector2{ 0, 0 }, camera, ENEMYTANK);
                break;
            case LEFT:
                playerTankShells.emplace_back(Vector2{ enemyTank.posAndRect.x, enemyTank.centre.y }, 270.0f, Vector2{ 0, 0 }, camera, ENEMYTANK);
                break;
            }
        }
    }

    // Update guided direction based on player position
    for (auto& enemyTank : allEnemyTanks) {
        enemyTank.guidedDirection = (enemyTank.posAndRect.y > playerTankPositionY) ? UP : DOWN;
    }

    // Update direction for each enemy tank
    for (auto& enemyTank : allEnemyTanks) {
        if (enemyTank.elapsedTime >= enemyTank.timeLimit) {
            enemyTank.elapsedTime = 0;  // Reset elapsed time
            enemyTank.timeLimit = uniform_int_distribution<int>{ 2, 5 }(gen);  // Set new time limit

            // Randomly choose a new direction based on guided direction
            if (enemyTank.guidedDirection == UP) {
                enemyTank.currentDirection = array{ RIGHT, LEFT, UP }[uniform_int_distribution<int>(0, 2)(gen)];
            }
            else {
                enemyTank.currentDirection = array{ RIGHT, LEFT, DOWN }[uniform_int_distribution<int>(0, 2)(gen)];
            }
        }
    }

    // Update position of each enemy tank
    for (auto& enemyTank : allEnemyTanks) {
        Rectangle newPosAndRect = enemyTank.posAndRect;  // New position and rectangle

        bool hasRotationChanged = false;  // Whether rotation has changed
        bool changeXorY = false;         // Whether to change X or Y position
        float nextMove;                  // Next move position

        // Calculate new position based on current direction
        if (enemyTank.currentDirection == UP && enemyTank.posAndRect.y > 10) {
            newPosAndRect.y -= enemyTank.Speed * deltaTime;
            nextMove = newPosAndRect.y;
            changeXorY = true;
        }
        else if (enemyTank.currentDirection == DOWN && enemyTank.posAndRect.y < canvasHeight - enemyTank.posAndRect.height - 10) {
            newPosAndRect.y += enemyTank.Speed * deltaTime;
            nextMove = newPosAndRect.y;
            changeXorY = true;
        }
        else if (enemyTank.currentDirection == LEFT && enemyTank.posAndRect.x > 10) {
            newPosAndRect.x -= enemyTank.Speed * deltaTime;
            nextMove = newPosAndRect.x;
        }
        else if (enemyTank.currentDirection == RIGHT && enemyTank.posAndRect.x < CanvasWidth - enemyTank.posAndRect.width - 10) {
            newPosAndRect.x += enemyTank.Speed * deltaTime;
            nextMove = newPosAndRect.x;
        }

        bool collisionDetected = false;  // Whether collision is detected

        // Check for collisions with obstacles
        for (const auto& obs : obstacles) {
            if (obs.type == BRICK || obs.type == BARRIER || obs.type == WATER) {
                if (CheckCollisionRecs(newPosAndRect, obs.sizeAndPosition)) {
                    collisionDetected = true;
                    break;
                }
            }
        }

        // Check for collision with player tank
        if (CheckCollisionRecs(newPosAndRect, playerTankRect)) {
            collisionDetected = true;
            continue;
        }

        // Check for collisions with other enemy tanks
        if (!collisionDetected) {
            for (auto& nextEnemyTank : allEnemyTanks) {
                if (areRectanglesDifferent(enemyTank.posAndRect, nextEnemyTank.posAndRect) && CheckCollisionRecs(newPosAndRect, nextEnemyTank.posAndRect)) {
                    Vector2 separationForce = Vector2{ nextEnemyTank.posAndRect.x - enemyTank.posAndRect.x, nextEnemyTank.posAndRect.y - enemyTank.posAndRect.y };
                    float distance = sqrt(separationForce.x * separationForce.x + separationForce.y * separationForce.y);
                    if (distance < 20.0f) {  // Apply separation force if too close
                        newPosAndRect.x -= separationForce.x * 0.1f;
                        newPosAndRect.y -= separationForce.y * 0.1f;
                    }
                    collisionDetected = true;
                    break;
                }
            }
        }

        // Update position if no collision detected
        if (!collisionDetected) {
            enemyTank.posAndRect = newPosAndRect;
        }
    }
}

// Function to draw enemy tanks on the screen
void drawEnemyTanks(vector<EnemyTank>& allEnemyTanks, Texture2D& enemyTankTexture) {
    if (allEnemyTanks.size() > 0) {
        for (auto& enemyTank : allEnemyTanks) {
            // Draw the tank based on its current direction
            switch (enemyTank.currentDirection) {
            case UP:
                DrawTexturePro(enemyTankTexture,
                    { 0.0f, 0.0f, (float)enemyTankTexture.width, (float)enemyTankTexture.height },
                    { enemyTank.posAndRect.x + enemyTank.posAndRect.width / 2, enemyTank.posAndRect.y + enemyTank.posAndRect.height / 2, (float)enemyTankTexture.width, (float)enemyTankTexture.height },
                    { (float)enemyTankTexture.width / 2.0f, (float)enemyTankTexture.height / 2.0f },
                    0, WHITE);
                break;
            case DOWN:
                DrawTexturePro(enemyTankTexture,
                    { 0.0f, 0.0f, (float)enemyTankTexture.width, (float)enemyTankTexture.height },
                    { enemyTank.posAndRect.x + enemyTank.posAndRect.width / 2, enemyTank.posAndRect.y + enemyTank.posAndRect.height / 2, (float)enemyTankTexture.width, (float)enemyTankTexture.height },
                    { (float)enemyTankTexture.width / 2.0f, (float)enemyTankTexture.height / 2.0f },
                    180, WHITE);
                break;
            case LEFT:
                DrawTexturePro(enemyTankTexture,
                    { 0.0f, 0.0f, (float)enemyTankTexture.width, (float)enemyTankTexture.height },
                    { enemyTank.posAndRect.x + enemyTank.posAndRect.width / 2, enemyTank.posAndRect.y + enemyTank.posAndRect.height / 2, (float)enemyTankTexture.width, (float)enemyTankTexture.height },
                    { (float)enemyTankTexture.width / 2.0f, (float)enemyTankTexture.height / 2.0f },
                    270, WHITE);
                break;
            case RIGHT:
                DrawTexturePro(enemyTankTexture,
                    { 0.0f, 0.0f, (float)enemyTankTexture.width, (float)enemyTankTexture.height },
                    { enemyTank.posAndRect.x + enemyTank.posAndRect.width / 2, enemyTank.posAndRect.y + enemyTank.posAndRect.height / 2, (float)enemyTankTexture.width, (float)enemyTankTexture.height },
                    { (float)enemyTankTexture.width / 2.0f, (float)enemyTankTexture.height / 2.0f },
                    90, WHITE);
                break;
            }

            // Draw direction text for debugging
            DrawText(directionString[enemyTank.guidedDirection].c_str(), enemyTank.centre.x - 50, enemyTank.centre.y - 100, 20, BLACK);
            DrawText((directionString[enemyTank.currentDirection]).c_str(), enemyTank.centre.x - 50, enemyTank.centre.y - 80, 20, BLACK);
        }
    }
}

#endif
