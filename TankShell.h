#ifndef TANK_SHELL_H
#define TANK_SHELL_H

// Define an enumeration for the type of shooter (player or enemy)
enum bulletShooterType {
    PLAYERTANK,  // Represents a shell shot by the player tank
    ENEMYTANK    // Represents a shell shot by an enemy tank
};

using namespace std;  // Use the standard namespace

// Define the TankShell class
class TankShell {
private:
    float rotation;  // Rotation angle of the shell in degrees

public:
    float shellDeltaTime;  // Delta time for the shell's movement
    Vector2 position;      // Current position of the shell

    Vector2 shellTip;      // The tip of the shell (used for collision detection)

    Vector2 direction;     // Direction vector of the shell's movement
    float speed = 750.0f;  // Speed of the shell
    Vector2 StartPosition; // Initial position of the shell
    float distanceTraveled; // Distance traveled by the shell
    float maxDistance;     // Maximum distance the shell can travel
    Vector2 target;        // Target position of the shell

    Vector2 potentialPosition; // Potential next position of the shell

    bulletShooterType shooter; // Type of shooter (player or enemy)

    // Set the position of the shell
    void setPosition(Vector2 newPos) {
        position = newPos;
    }

    // Get the current position of the shell
    Vector2 getPosition() {
        return position;
    }

    // Get the potential next position of the shell
    Vector2 GetPotentialPosition() {
        return potentialPosition;
    }

    // Constructor for the TankShell class
    TankShell(Vector2 startPos, float angle, Vector2 mousePos, Camera2D& camera, bulletShooterType whoShot)
        : position(startPos), StartPosition(startPos), rotation(angle), target(mousePos), shooter(whoShot) {

        // Calculate the direction vector based on the angle
        direction = {
            cos((angle - 90) * DEG2RAD),  // Convert angle to radians and calculate x-component
            sin((angle - 90) * DEG2RAD)   // Convert angle to radians and calculate y-component
        };

        // Calculate the maximum distance the shell can travel
        maxDistance = sqrtf(
            pow(StartPosition.x - GetScreenToWorld2D(target, camera).x, 2) +  // Calculate x-distance
            pow(StartPosition.y - GetScreenToWorld2D(target, camera).y, 2)    // Calculate y-distance
        );
    }

    // Update the shell's position and state
    void Update(float& deltaTime, Camera2D& camera) {

        shellDeltaTime = deltaTime;  // Update delta time

        // Calculate the potential next position of the shell
        potentialPosition = {
            position.x + direction.x * speed * deltaTime,  // Update x-position
            position.y + direction.y * speed * deltaTime   // Update y-position
        };

        // Calculate the new distance traveled by the shell
        float newDistanceTraveled = sqrtf(
            pow(StartPosition.x - potentialPosition.x, 2) +  // Calculate x-distance
            pow(StartPosition.y - potentialPosition.y, 2)    // Calculate y-distance
        );

        // Check if the shell has exceeded its maximum travel distance
        if (newDistanceTraveled >= maxDistance) {
            // Calculate the overshoot distance
            float overshoot = newDistanceTraveled - maxDistance;
            // Calculate the adjustment factor to stop the shell at the max distance
            float adjustmentFactor = (newDistanceTraveled - overshoot) / newDistanceTraveled;
            // Adjust the position to stop at the max distance
            position = {
                StartPosition.x + (potentialPosition.x - StartPosition.x) * adjustmentFactor,
                StartPosition.y + (potentialPosition.y - StartPosition.y) * adjustmentFactor
            };
            distanceTraveled = maxDistance;  // Set distance traveled to max distance
            return;  // Exit the function
        }

        // Update the shell's position and distance traveled
        position = potentialPosition;
        distanceTraveled = newDistanceTraveled;
    }

    // Draw the shell on the screen
    void Draw(Texture2D shellTexture) {
        Rectangle source = { 0.0f, 0.0f, (float)shellTexture.width, (float)shellTexture.height };  // Source rectangle for the texture
        Rectangle dest = { position.x, position.y, (float)shellTexture.width, (float)shellTexture.height };  // Destination rectangle for the texture
        Vector2 origin = { (float)shellTexture.width / 2.0f, (float)shellTexture.height / 2.0f };  // Origin for rotation
        DrawTexturePro(shellTexture, source, dest, origin, rotation, WHITE);  // Draw the rotated texture

        shellTip = GetTopEdgeMidpoint(shellTexture);  // Update the shell tip position
    }

    // Calculate the midpoint of the top edge of the shell texture
    Vector2 GetTopEdgeMidpoint(Texture2D shellTexture) const {
        Vector2 topEdgeMid = { position.x, position.y - shellTexture.height / 2.0f };  // Calculate the midpoint of the top edge

        float radians = rotation * DEG2RAD;  // Convert rotation to radians
        float cosTheta = cos(radians);       // Calculate cosine of the rotation angle
        float sinTheta = sin(radians);       // Calculate sine of the rotation angle

        // Rotate the midpoint to match the shell's rotation
        Vector2 rotatedMidpoint = {
            position.x + (topEdgeMid.x - position.x) * cosTheta - (topEdgeMid.y - position.y) * sinTheta,
            position.y + (topEdgeMid.x - position.x) * sinTheta + (topEdgeMid.y - position.y) * cosTheta
        };

        return rotatedMidpoint;  // Return the rotated midpoint
    }

    // Get the current position of the shell
    Vector2 GetPosition() const {
        return position;
    }

    // Destructor for the TankShell class
    ~TankShell() {}
};

#endif
