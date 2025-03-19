#include "raylib.h"  // Include the main Raylib library

class RaylibLogoAnimation {
private:
    int logoPositionX, logoPositionY;  // Variables to store the position of the logo
    int framesCounter = 0, lettersCount = 0;  // Counters for animation frames and letters
    int topSideRecWidth = 16, leftSideRecHeight = 16, bottomSideRecWidth = 16, rightSideRecHeight = 16;  // Dimensions for the logo's rectangles
    int state = 0;  // Variable to track the current state of the animation

    float alpha = 1.0f;  // Variable to control the transparency of the logo

public:
    bool animationCompleted = false;  // Flag to indicate if the animation is completed

    RaylibLogoAnimation() {}  // Default constructor

    void initialise(int& screenWidth, int& screenHeight) {  // Function to initialize the logo position
        logoPositionX = screenWidth / 2 - 128;  // Set the X position of the logo to the center of the screen
        logoPositionY = screenHeight / 2 - 128;  // Set the Y position of the logo to the center of the screen
    }

    void Update() {  // Function to update the animation
        if (state == 0) {  // State 0: Initial delay before the animation starts
            framesCounter++;  // Increment the frame counter
            if (framesCounter == 120) {  // After 120 frames, move to the next state
                state = 1;
                framesCounter = 0;  // Reset the frame counter
            }
        }
        else if (state == 1) {  // State 1: Expand the top and left rectangles
            topSideRecWidth += 4;  // Increase the width of the top rectangle
            leftSideRecHeight += 4;  // Increase the height of the left rectangle

            if (topSideRecWidth == 256) state = 2;  // When the top rectangle reaches full width, move to the next state
        }
        else if (state == 2) {  // State 2: Expand the bottom and right rectangles
            bottomSideRecWidth += 4;  // Increase the width of the bottom rectangle
            rightSideRecHeight += 4;  // Increase the height of the right rectangle

            if (bottomSideRecWidth == 256) state = 3;  // When the bottom rectangle reaches full width, move to the next state
        }
        else if (state == 3) {  // State 3: Fade out the logo and display the text
            framesCounter++;  // Increment the frame counter
            if (framesCounter % 12 == 0) {  // Every 12 frames, reveal one more letter
                lettersCount++;
                framesCounter = 0;  // Reset the frame counter
            }

            if (lettersCount >= 10) {  // When all letters are revealed, start fading out
                alpha -= 0.02f;  // Decrease the transparency
                if (alpha <= 0.0f) {  // When fully transparent, move to the next state
                    alpha = 0.0f;
                    state = 4;
                }
            }
        }
        else if (state == 4) {  // State 4: Animation completed, wait for user input to restart
            if (IsKeyPressed(KEY_R)) {  // If the 'R' key is pressed, reset the animation
                framesCounter = 0;
                lettersCount = 0;
                topSideRecWidth = 16;
                leftSideRecHeight = 16;
                bottomSideRecWidth = 16;
                rightSideRecHeight = 16;
                alpha = 1.0f;
                state = 0;  // Reset to the initial state
            }
        }
    }

    void Draw() {  // Function to draw the animation
        BeginDrawing();  // Start drawing
        ClearBackground(RAYWHITE);  // Clear the background to white

        DrawText("Powered by...", 50, 50, 30, Fade(BLACK, alpha));  // Draw the "Powered by..." text

        if (state == 0) {  // State 0: Draw a blinking rectangle
            if ((framesCounter / 15) % 2) DrawRectangle(logoPositionX, logoPositionY, 16, 16, BLACK);  // Blink the rectangle
        }
        else if (state == 1) {  // State 1: Draw the expanding top and left rectangles
            DrawRectangle(logoPositionX, logoPositionY, topSideRecWidth, 16, BLACK);  // Draw the top rectangle
            DrawRectangle(logoPositionX, logoPositionY, 16, leftSideRecHeight, BLACK);  // Draw the left rectangle
        }
        else if (state == 2) {  // State 2: Draw all four expanding rectangles
            DrawRectangle(logoPositionX, logoPositionY, topSideRecWidth, 16, BLACK);  // Draw the top rectangle
            DrawRectangle(logoPositionX, logoPositionY, 16, leftSideRecHeight, BLACK);  // Draw the left rectangle
            DrawRectangle(logoPositionX + 240, logoPositionY, 16, rightSideRecHeight, BLACK);  // Draw the right rectangle
            DrawRectangle(logoPositionX, logoPositionY + 240, bottomSideRecWidth, 16, BLACK);  // Draw the bottom rectangle
        }
        else if (state == 3) {  // State 3: Draw the fading logo and text
            DrawRectangle(logoPositionX, logoPositionY, topSideRecWidth, 16, Fade(BLACK, alpha));  // Draw the top rectangle with fading
            DrawRectangle(logoPositionX, logoPositionY + 16, 16, leftSideRecHeight - 32, Fade(BLACK, alpha));  // Draw the left rectangle with fading
            DrawRectangle(logoPositionX + 240, logoPositionY + 16, 16, rightSideRecHeight - 32, Fade(BLACK, alpha));  // Draw the right rectangle with fading
            DrawRectangle(logoPositionX, logoPositionY + 240, bottomSideRecWidth, 16, Fade(BLACK, alpha));  // Draw the bottom rectangle with fading
            DrawRectangle(GetScreenWidth() / 2 - 112, GetScreenHeight() / 2 - 112, 224, 224, Fade(RAYWHITE, alpha));  // Draw a white rectangle with fading
            DrawText(TextSubtext("raylib", 0, lettersCount), GetScreenWidth() / 2 - 44, GetScreenHeight() / 2 + 48, 50, Fade(BLACK, alpha));  // Draw the "raylib" text with fading
        }
        else if (state == 4) {  // State 4: Animation completed
            animationCompleted = true;  // Set the animation completed flag to true
        }

        EndDrawing();  // End drawing
    }
};
