#include "raylib.h"  // Include the main Raylib library
#include "PlayerTank.h"  // Include the PlayerTank class
#include <string>  // Include the string library
#include <vector>  // Include the vector library for dynamic arrays
#include "obstacles.h"  // Include the obstacles class
#include "EnemyTank.h"  // Include the EnemyTank class
#include <map>  // Include the map library for key-value pairs
#include <random>  // Include the random library for random number generation

class gameShellExplosionAnimation {  // Class for the explosion animation of shells
public:
    Vector2 position;  // Position of the explosion
    int currentFrame;  // Current frame of the animation

    gameShellExplosionAnimation(Vector2 pos, int currentFrame = 0)  // Constructor to initialize the explosion
        : position(pos), currentFrame(currentFrame) {
    }
};

class Game {  // Main game class
private:
    Texture2D backgroundTexture;  // Texture for the background

    Texture2D enemyTankBasic;  // Texture for the basic enemy tank

    PlayerTank playerTank;  // Instance of the PlayerTank class

    vector<Obstacle> obstacles;  // Vector to store obstacles
    vector<Texture2D> waterTextures;  // Vector to store water animation textures
    Texture2D treeTexture;  // Texture for trees
    Texture2D barrierTexture;  // Texture for barriers
    Texture2D brickTexture;  // Texture for bricks

    int currentWaterFrame = 0;  // Current frame of the water animation
    float waterAnimationTimer = 0.0f;  // Timer for water animation
    float waterFrameTime = 0.05f;  // Time between water animation frames

    Vector2 defaultTileWidthHeight = { 30, 30 };  // Default size of tiles
    float TileSpacer = defaultTileWidthHeight.x;  // Spacing between tiles

    float maxTimeBeforeNextWave = 60.0f;  // Maximum time before the next wave of enemies

    int levelHeight = TileSpacer * 4 * 13;  // Height of each level
    int currentLevel = 0;  // Current level

    float levelData[35][3] = {  // Data for each level (number of enemies, elapsed time, max time)
        { 3.0f, 0.0f, 60.0f }, { 3.0f, 0.0f, 60.0f }, { 3.0f, 0.0f, 60.0f }, { 3.0f, 0.0f, 60.0f },
        { 3.0f, 0.0f, 60.0f }, { 3.0f, 0.0f, 60.0f }, { 3.0f, 0.0f, 60.0f }, { 3.0f, 0.0f, 60.0f },
        { 3.0f, 0.0f, 60.0f }, { 3.0f, 0.0f, 60.0f }, { 3.0f, 0.0f, 60.0f }, { 3.0f, 0.0f, 60.0f },
        { 3.0f, 0.0f, 60.0f }, { 3.0f, 0.0f, 60.0f }, { 3.0f, 0.0f, 60.0f }, { 3.0f, 0.0f, 60.0f },
        { 3.0f, 0.0f, 60.0f }, { 3.0f, 0.0f, 60.0f }, { 3.0f, 0.0f, 60.0f }, { 3.0f, 0.0f, 60.0f },
        { 3.0f, 0.0f, 60.0f }, { 3.0f, 0.0f, 60.0f }, { 3.0f, 0.0f, 60.0f }, { 3.0f, 0.0f, 60.0f },
        { 3.0f, 0.0f, 60.0f }, { 3.0f, 0.0f, 60.0f }, { 3.0f, 0.0f, 60.0f }, { 3.0f, 0.0f, 60.0f },
        { 3.0f, 0.0f, 60.0f }, { 3.0f, 0.0f, 60.0f }, { 3.0f, 0.0f, 60.0f }, { 3.0f, 0.0f, 60.0f },
        { 3.0f, 0.0f, 60.0f }, { 3.0f, 0.0f, 60.0f }, { 3.0f, 0.0f, 60.0f }
    };

public:
    float myElapsedTime = 0.0;  // Elapsed time for the game

    Music inGame;  // Background music for the game

    struct Canvas {  // Struct to store canvas dimensions
        int* width;
        int* height;
    };

    map<int, vector<Obstacle>> levelSpawnPoints;  // Map to store spawn points for each level

    vector<EnemyTank> allEnemyTanks;  // Vector to store all enemy tanks
    vector<EnemyTank> enemiesToBeSpawned;  // Vector to store enemies waiting to be spawned

    vector<Obstacle> randomlyPickedSpawnPoints;  // Vector to store randomly picked spawn points

    int highestLevelReached = 0;  // Highest level reached by the player

    Texture2D shellTexture;  // Texture for the tank shells

    void loadShellTexture() {  // Load the shell texture
        Image image = LoadImage("img/playerTank/fireball2.png");  // Load the shell image
        ImageResize(&image, image.width / 4, image.height / 4);  // Resize the shell image
        shellTexture = LoadTextureFromImage(image);  // Load the shell texture
        UnloadImage(image);  // Unload the shell image
    }

    vector<gameShellExplosionAnimation> explosions;  // Vector to store explosion animations
    vector<Texture2D> explosionAnimationTextures;  // Vector to store explosion animation textures
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
    float elapsedTime = 0.0f;  // Elapsed time for the explosion animation
    float interval = 0.025f;  // Time interval between explosion frames
    bool isExplosionActive = false;  // Flag to check if an explosion is active
    bool isExplosionAnimationCompleted = false;  // Flag to check if the explosion animation is completed

    void animationUpdater(float& deltaTime) {  // Update the explosion animation
        elapsedTime += deltaTime;

        if (elapsedTime >= interval) {
            nextFrame();  // Advance to the next frame
            elapsedTime = 0.0f;  // Reset the elapsed time
        }
    }

    void nextFrame() {  // Advance the explosion animation to the next frame
        for (auto it = explosions.begin(); it != explosions.end(); ) {
            if (it->currentFrame < framePaths.size() - 1) {  // If the animation is not complete
                it->currentFrame++;  // Move to the next frame
                ++it;  // Move to the next explosion
            } else {  // If the animation is complete
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

    void LoadExplosionAnimationTextures() {  // Load all explosion animation textures
        LoadFrames(framePaths);
    }

    void drawExplosionAnimations() {  // Draw all explosion animations
        for (const auto& explosion : explosions) {
            DrawTexture(explosionAnimationTextures[explosion.currentFrame],
                explosion.position.x - explosionAnimationTextures[explosion.currentFrame].width / 2,
                explosion.position.y - explosionAnimationTextures[explosion.currentFrame].height, WHITE);
        }
    }

    Canvas canvas;  // Canvas dimensions
    Camera2D* camera;  // Camera for the game

    vector<EnemyTank> enemyTanks;  // Vector to store enemy tanks

    vector<TankShell> playerTankShells;  // Vector to store player tank shells

    Game(int* canvasWidth, int* canvasHeight, Camera2D* mainCamera) {  // Constructor to initialize the game
        canvas.width = canvasWidth;
        canvas.height = canvasHeight;
        camera = mainCamera;
    }

    ~Game() {}  // Destructor

    void initialise(float playerTankPosX, float playerTankPosY, int& screenWidth, int& screenHeight) {  // Initialize the game
        playerTank.initialise(playerTankPosX, playerTankPosY);  // Initialize the player tank
        initialiseObstacles(obstacles, levelSpawnPoints);  // Initialize the obstacles
    }

    void LoadTextures() {  // Load all textures
        backgroundTexture = LoadTexture("img/bg2.png");  // Load the background texture
        loadShellTexture();  // Load the shell texture
        enemyTankBasic = LoadTexture("img/enemyTank/enemyTankBasic.png");  // Load the enemy tank texture
        playerTank.LoadTankTexture();  // Load the player tank textures
        LoadExplosionAnimationTextures();  // Load the explosion animation textures
        loadWaterTextures();  // Load the water textures
        loadTreeTexture();  // Load the tree texture
        loadBarrierTexture();  // Load the barrier texture
        loadBrickTexture();  // Load the brick texture
    }

    void DrawPlayerTankAndTurret() {  // Draw the player tank and turret
        playerTank.Draw();
    }

    void drawDebug() {  // Draw debug information
        DrawText(TextFormat("FPS: %d", GetFPS()), 50, 10, 40, DARKGRAY);  // Display the FPS

        int yPosition = 20;  // Y position for debug text

        for (int i = 0; i < 35; i++) {  // Display level data
            if (levelData[i][1] != 0.0f) {
                char text[100];
                sprintf_s(text, "Level Data [%d]: { %.1f, %.1f, %.1f }", i, levelData[i][0], levelData[i][1], levelData[i][2]);
                yPosition += 40 + 5;  // Adjust the Y position
            }
        }
    }

    void update(float& deltaTime, GameStatus& gameStatus, int& screenWidth, int& screenHeight, int& canvasWidth, int& canvasHeight) {  // Update the game state
        myElapsedTime += deltaTime;  // Update the elapsed time

        loadAndPlayBgMusic();  // Load and play the background music

        if (playerTank.health <= 0) {  // Check if the player tank is destroyed
            gameStatus.currentGameState = GameOver;  // Set the game state to GameOver
        }

        static mt19937 gen(random_device{}());  // Random number generator

        spawnEnemyTanks(playerTank.tankRect);  // Spawn enemy tanks

        updateEnemyTanks(allEnemyTanks, playerTank.tankRect, playerTank.position.y, obstacles, *canvas.width, *canvas.height, deltaTime, playerTankShells, *camera);  // Update enemy tanks

        int newLevel = (canvasHeight - playerTank.GetPosition().y) / levelHeight;  // Calculate the current level

        if (newLevel != currentLevel) {  // If the level has changed
            currentLevel = newLevel;  // Update the current level

            if (currentLevel > highestLevelReached) {  // Update the highest level reached
                highestLevelReached = currentLevel;
            }

            if (currentLevel - 1 != -1) {  // Update the max time before the next wave
                maxTimeBeforeNextWave = levelData[currentLevel - 1][2];
                levelData[currentLevel][2] = maxTimeBeforeNextWave;
            }
        }

        for (int lvl = 0; lvl <= highestLevelReached; lvl++) {  // Update the elapsed time for each level
            levelData[lvl][1] += deltaTime;
        }

        for (int lvl = 0; lvl <= highestLevelReached; lvl++) {  // Spawn enemies for each level
            if (levelData[lvl][1] >= levelData[lvl][2]) {  // If it's time to spawn enemies
                if (!(levelData[lvl][2] < 10.0f)) {  // Adjust the max time before the next wave
                    levelData[lvl][2]--;
                }
                levelData[lvl][1] = 0.0f;  // Reset the elapsed time

                vector<int> usedSpawnPoints;  // Vector to store used spawn points
                for (size_t i = 0; i < levelData[lvl][0]; i++) {  // Pick random spawn points
                    while (true) {
                        int random_index = uniform_int_distribution<int>{ 0, int(levelSpawnPoints[lvl + 1].size()) - 1 }(gen);
                        if (find(usedSpawnPoints.begin(), usedSpawnPoints.end(), random_index) != usedSpawnPoints.end()) {
                            continue;
                        } else {
                            usedSpawnPoints.push_back(random_index);
                            break;
                        }
                    }
                }

                for (auto spawnpoint : usedSpawnPoints) {  // Spawn enemies at the selected spawn points
                    int temp = 0;
                    if (spawnpoint == 0) {
                        temp = 10;
                    }
                    int random_index = uniform_int_distribution<int>{ 1, 5 }(gen);
                    enemiesToBeSpawned.push_back(EnemyTank(BASIC,
                        { (float)enemyTankBasic.width , (float)enemyTankBasic.height },
                        { levelSpawnPoints[lvl + 1].at(spawnpoint).sizeAndPosition.x + temp,
                          levelSpawnPoints[lvl + 1].at(spawnpoint).sizeAndPosition.y + (float)enemyTankBasic.height / 5 }
                    , random_index));
                }

                levelData[lvl][0]++;  // Increase the number of enemies for the next wave
            }
        }

        animationUpdater(deltaTime);  // Update the explosion animation

        playerTank.Update(deltaTime, GetScreenToWorld2D(GetMousePosition(), *camera), gameStatus, playerTankShells, *camera, canvasWidth, canvasHeight, obstacles, allEnemyTanks);  // Update the player tank

        UpdateShells(deltaTime, screenWidth, screenHeight);  // Update the player tank shells

        updateCameraToPlayerTankPosition();  // Update the camera to follow the player tank

        updateWaterAnimation(deltaTime);  // Update the water animation
    }

    void UpdateShells(float& deltaTime, int& screenWidth, int& screenHeight) {  // Update the player tank shells
        for (size_t i = 0; i < playerTankShells.size(); ) {
            playerTankShells[i].Update(deltaTime, *camera);  // Update the shell's position

            if (playerTankShells[i].distanceTraveled >= playerTankShells[i].maxDistance) {  // If the shell has traveled its max distance
                playerTankShells[i] = playerTankShells.back();  // Remove the shell
                playerTankShells.pop_back();
            } else {
                ++i;  // Move to the next shell
            }
        }
    }

    void DrawShells() {  // Draw the player tank shells
        for (auto& shell : playerTankShells) {
            shell.Draw(shellTexture);  // Draw the shell
        }
    }

    void updateCameraToPlayerTankPosition() {  // Update the camera to follow the player tank
        camera->target = playerTank.GetPosition();
    }

    void beginCamera2D() {  // Begin 2D camera mode
        BeginMode2D(*camera);
    }

    void endCamera2D() {  // End 2D camera mode
        DrawPlayerTankAndTurret();  // Draw the player tank and turret
        drawObstacles(obstacles, currentWaterFrame, waterTextures, treeTexture, barrierTexture, brickTexture);  // Draw the obstacles
        playerTank.drawHealthBar();  // Draw the player tank's health bar
        drawEnemyTanks(allEnemyTanks, enemyTankBasic);  // Draw the enemy tanks
        DrawShells();  // Draw the player tank shells
        drawExplosionAnimations();  // Draw the explosion animations
        enemyTracker();  // Draw enemy tracking circles
        EndMode2D();  // End 2D camera mode
    }

    void startDrawing() {  // Start drawing
        BeginDrawing();
        ClearBackground(RAYWHITE);
    }

    void endDrawing() {  // End drawing
        EndDrawing();
    }

    void loadWaterTextures() {  // Load the water textures
        for (int i = 1; i <= 16; ++i) {
            Image image = LoadImage(("img/obstacles/water/" + to_string(i) + ".png").c_str());  // Load the water image
            ImageResize(&image, defaultTileWidthHeight.x, defaultTileWidthHeight.y);  // Resize the water image
            waterTextures.push_back(LoadTextureFromImage(image));  // Load the water texture
            UnloadImage(image);  // Unload the water image
        }
    }

    void updateWaterAnimation(float deltaTime) {  // Update the water animation
        waterAnimationTimer += deltaTime;

        if (waterAnimationTimer >= waterFrameTime) {  // If it's time to advance the water animation
            waterAnimationTimer -= waterFrameTime;
            currentWaterFrame = (currentWaterFrame + 1) % waterTextures.size();  // Move to the next frame
        }
    }

    void loadTreeTexture() {  // Load the tree texture
        Image image = LoadImage("img/obstacles/tree/tree.png");  // Load the tree image
        ImageResize(&image, defaultTileWidthHeight.x, defaultTileWidthHeight.y);  // Resize the tree image
        treeTexture = LoadTextureFromImage(image);  // Load the tree texture
        UnloadImage(image);  // Unload the tree image
    }

    void loadBarrierTexture() {  // Load the barrier texture
        Image image = LoadImage("img/obstacles/barrier/barrier.png");  // Load the barrier image
        ImageResize(&image, defaultTileWidthHeight.x, defaultTileWidthHeight.y);  // Resize the barrier image
        barrierTexture = LoadTextureFromImage(image);  // Load the barrier texture
        UnloadImage(image);  // Unload the barrier image
    }

    void loadBrickTexture() {  // Load the brick texture
        Image image = LoadImage("img/obstacles/brick/brick.png");  // Load the brick image
        ImageResize(&image, defaultTileWidthHeight.x, defaultTileWidthHeight.y);  // Resize the brick image
        brickTexture = LoadTextureFromImage(image);  // Load the brick texture
        UnloadImage(image);  // Unload the brick image
    }

    void DrawBackgroundAndGrid(int canvasWidth, int canvasHeight, int gridSize) {  // Draw the background and grid
        DrawRectangle(0, 0, canvasWidth, canvasHeight, LIGHTGRAY);  // Draw the background

        for (int x = 0; x < canvasWidth; x += gridSize) {  // Draw vertical grid lines
            DrawLine(x, 0, x, canvasHeight, DARKGRAY);
        }
        for (int y = 0; y < canvasHeight; y += gridSize) {  // Draw horizontal grid lines
            DrawLine(0, y, canvasWidth, y, DARKGRAY);
        }
    }

    void DrawBackgroundWithTiles(int canvasWidth, int canvasHeight, int gridSize) {  // Draw the background with tiles
        DrawRectangle(0, 0, canvasWidth, canvasHeight, LIGHTGRAY);  // Draw the background

        float scale = (float)gridSize / backgroundTexture.width;  // Calculate the scale for the background texture

        for (int y = 0; y < canvasHeight; y += gridSize) {  // Draw the background tiles
            for (int x = 0; x < canvasWidth; x += gridSize) {
                Rectangle sourceRec = { 0.0f, 0.0f, (float)backgroundTexture.width, (float)backgroundTexture.height };  // Define the source rectangle
                Rectangle destRec = { (float)x, (float)y, (float)gridSize, (float)gridSize };  // Define the destination rectangle
                Vector2 origin = { 0.0f, 0.0f };  // Define the origin
                DrawTexturePro(backgroundTexture, sourceRec, destRec, origin, 0.0f, WHITE);  // Draw the background tile
            }
        }
    }

    int countSpawnPoints() {  // Count the number of spawn points
        int temp = 0;
        for (const auto& obstacle : obstacles) {
            if (obstacle.type == SPAWN_POINT) {
                temp++;
            }
        }
        return temp;
    }

    void checkCollisions() {  // Check for collisions between shells and obstacles/enemies
        for (auto shellIt = playerTankShells.begin(); shellIt != playerTankShells.end();) {
            Rectangle shellRect = {  // Define the shell's bounding rectangle
                shellIt->GetPosition().x - shellTexture.width / 2.0f,
                shellIt->GetPosition().y - shellTexture.height / 2.0f,
                (float)shellTexture.width,
                (float)shellTexture.height
            };

            bool shellCollided = false;  // Flag to check if the shell has collided

            for (auto obstacleIt = obstacles.begin(); obstacleIt != obstacles.end();) {  // Check for collisions with obstacles
                if ((obstacleIt->type == BRICK || obstacleIt->type == BARRIER) &&
                    CheckCollisionRecs(shellRect, obstacleIt->sizeAndPosition)) {

                    if (shellIt->shooter == PLAYERTANK) {  // If the shell was fired by the player
                        playerTank.playHitSound();  // Play the hit sound
                    }

                    explosions.emplace_back(shellIt->GetPosition(), 0);  // Create an explosion
                    shellIt = playerTankShells.erase(shellIt);  // Remove the shell

                    if (obstacleIt->type != BARRIER) {  // If the obstacle is not a barrier
                        obstacleIt = obstacles.erase(obstacleIt);  // Remove the obstacle
                    } else {
                        ++obstacleIt;  // Move to the next obstacle
                    }

                    shellCollided = true;  // Set the collision flag to true
                    break;  // Exit the loop
                } else {
                    ++obstacleIt;  // Move to the next obstacle
                }
            }

            if (shellCollided) {  // If the shell has collided
                continue;  // Skip to the next shell
            }

            if (shellIt->shooter == PLAYERTANK) {  // If the shell was fired by the player
                for (auto it = allEnemyTanks.begin(); it != allEnemyTanks.end(); ) {  // Check for collisions with enemy tanks
                    if (CheckCollisionRecs(shellRect, it->posAndRect)) {  // If the shell collides with an enemy tank
                        playerTank.playEnemyDestroySound();  // Play the enemy destroy sound
                        explosions.emplace_back(Vector2{ it->centre.x , it->centre.y + 20 }, 0);  // Create an explosion
                        shellIt = playerTankShells.erase(shellIt);  // Remove the shell
                        it = allEnemyTanks.erase(it);  // Remove the enemy tank
                        shellCollided = true;  // Set the collision flag to true
                        break;  // Exit the loop
                    } else {
                        ++it;  // Move to the next enemy tank
                    }
                }
            } else {  // If the shell was fired by an enemy
                for (auto& enemyTank : allEnemyTanks) {  // Check for collisions with the player tank
                    if (CheckCollisionRecs(shellRect, playerTank.tankRect)) {  // If the shell collides with the player tank
                        playerTank.health -= 5;  // Reduce the player tank's health
                        playerTank.playHitSound();  // Play the hit sound
                        explosions.emplace_back(shellIt->GetPosition(), 0);  // Create an explosion
                        shellIt = playerTankShells.erase(shellIt);  // Remove the shell
                        shellCollided = true;  // Set the collision flag to true
                        break;  // Exit the loop
                    }
                }
            }
            if (!shellCollided) {  // If the shell has not collided
                ++shellIt;  // Move to the next shell
            }
        }
    }

    void enemyTracker() {  // Draw enemy tracking circles
        if (allEnemyTanks.size() > 0) {  // If there are enemy tanks
            for (const auto& enemyTank : allEnemyTanks) {  // Draw tracking circles for each enemy tank
                DrawCircle(enemyTank.centre.x, enemyTank.centre.y, 5, RED);  // Draw the center circle

                switch (enemyTank.currentDirection) {  // Draw the direction circle
                case UP:
                    DrawCircle(enemyTank.centre.x, enemyTank.posAndRect.y, 5, BLUE);
                    break;
                case RIGHT:
                    DrawCircle(enemyTank.posAndRect.x + enemyTank.posAndRect.width, enemyTank.centre.y, 5, BLUE);
                    break;
                case DOWN:
                    DrawCircle(enemyTank.centre.x, enemyTank.posAndRect.y + enemyTank.posAndRect.height, 5, BLUE);
                    break;
                case LEFT:
                    DrawCircle(enemyTank.posAndRect.x, enemyTank.centre.y, 5, BLUE);
                    break;
                }
            }
        }
    }

    void spawnEnemyTanks(Rectangle playerTankRect) {  // Spawn enemy tanks
        if (enemiesToBeSpawned.empty()) return;  // If there are no enemies to spawn, return

        auto it = enemiesToBeSpawned.begin();
        while (it != enemiesToBeSpawned.end()) {
            bool collides = false;  // Flag to check for collisions

            for (const auto& existingTank : allEnemyTanks) {  // Check for collisions with existing enemy tanks
                if (CheckCollisionRecs(it->posAndRect, existingTank.posAndRect)) {
                    collides = true;
                    break;  // Exit the loop
                }
            }

            if (CheckCollisionRecs(it->posAndRect, playerTankRect)) {  // Check for collisions with the player tank
                collides = true;
            }

            if (!collides) {  // If there are no collisions
                allEnemyTanks.push_back(*it);  // Add the enemy tank to the list
                it = enemiesToBeSpawned.erase(it);  // Remove the enemy tank from the spawn list
            } else {
                ++it;  // Move to the next enemy tank
            }
        }
    }

    void loadAndPlayBgMusic() {  // Load and play the background music
        if (inGame.stream.buffer == NULL) {  // If the music is not loaded
            inGame = LoadMusicStream("sounds/inGame.mp3");  // Load the music
        }

        UpdateMusicStream(inGame);  // Update the music stream

        SetMusicVolume(inGame, 0.4f);  // Set the music volume

        if (!IsMusicStreamPlaying(inGame)) {  // If the music is not playing
            PlayMusicStream(inGame);  // Play the music
        }
    }
};
