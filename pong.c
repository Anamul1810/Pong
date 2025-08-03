#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h> // For fabsf()

// Enum for game states
typedef enum GameState {
    MENU,
    PLAYING,
    GAME_OVER
} GameState;

// Enum for game modes
typedef enum GameMode {
    PVP,  // Player vs Player
    PVC   // Player vs Computer
} GameMode;

// Particle struct for effects
typedef struct Particle {
    Vector2 position;
    Vector2 velocity;
    float lifetime;
    Color color;
} Particle;

int main() {
    const int screen_width = 1280;
    const int screen_height = 800;
    InitWindow(screen_width, screen_height, "Pong: Lava vs Ice");

    // Background colors
    Color menuBackgroundColor = BLACK;
    Color pvpBackgroundColor = (Color){50, 0, 50, 255}; // Dark Purple for PvP
    Color pvcBackgroundColor = (Color){0, 60, 60, 255}; // Dark Teal for PvC
    Color gameOverBackgroundColor = (Color){139, 0, 0, 255};  // Dark Red for Game Over

    // Back button rectangle
    Rectangle backButton = { screen_width - 150, 20, 120, 40 };

    // Ball settings
    Vector2 ballPosition = { screen_width / 2.0f, screen_height / 2.0f };
    Vector2 ballSpeed = { 5.0f, 5.0f };
    int ballRadius = 20;
    Color ballColor = WHITE;

    // Paddle settings
    Rectangle leftPaddle = { 50, screen_height / 2 - 50, 20, 100 };
    Rectangle rightPaddle = { screen_width - 70, screen_height / 2 - 50, 20, 100 };
    Color leftColor = (Color){255, 69, 0, 255};   // Fiery orange
    Color rightColor = (Color){0, 191, 255, 255}; // Icy blue
    float paddleSpeed = 6.0f;
    float aiSpeed = 5.0f;

    // Obstacles in the middle
    Rectangle obstacles[3] = {
        { screen_width / 2 - 10, 100, 20, 50 },
        { screen_width / 2 - 10, 300, 20, 50 },
        { screen_width / 2 - 10, 500, 20, 50 }
    };

    int leftScore = 0;
    int rightScore = 0;
    const int winScore = 5;
    float maxSpeed = 12.0f;

    srand(time(NULL));

    GameState currentState = MENU;
    GameMode currentMode = PVP;
    int menuSelection = 0;
    int gameLevel = 1; // 1=easy, 2=medium, 3=hard
    const char* winnerText = NULL;

    // Star background
    const int numStars = 200;
    Vector2 stars[numStars];
    for (int i = 0; i < numStars; i++) {
        stars[i].x = (float)GetRandomValue(0, screen_width);
        stars[i].y = (float)GetRandomValue(0, screen_height);
    }

    // Particle effects
    const int maxParticles = 10;
    Particle lavaParticles[maxParticles];
    Particle iceParticles[maxParticles];
    for (int i = 0; i < maxParticles; i++) {
        lavaParticles[i].lifetime = 0;
        iceParticles[i].lifetime = 0;
    }

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        switch (currentState) {
            case MENU:
                if (IsKeyPressed(KEY_DOWN)) menuSelection = (menuSelection + 1) % 6;
                if (IsKeyPressed(KEY_UP)) menuSelection = (menuSelection - 1 + 6) % 6;
                if (IsKeyPressed(KEY_ENTER)) {
                    if (menuSelection == 0) { currentMode = PVP; gameLevel = 1; }
                    else if (menuSelection == 1) { currentMode = PVP; gameLevel = 2; }
                    else if (menuSelection == 2) { currentMode = PVP; gameLevel = 3; }
                    else if (menuSelection == 3) { currentMode = PVC; gameLevel = 1; }
                    else if (menuSelection == 4) { currentMode = PVC; gameLevel = 2; }
                    else if (menuSelection == 5) { currentMode = PVC; gameLevel = 3; }
                    // Set parameters by level
                    if (gameLevel == 1) {
                        ballSpeed = (Vector2){4.0f, 4.0f};
                        maxSpeed = 8.0f;
                        paddleSpeed = 7.0f;
                        aiSpeed = 3.0f;
                        leftPaddle.height = 150;
                        rightPaddle.height = 150;
                    } else if (gameLevel == 2) {
                        ballSpeed = (Vector2){5.0f, 5.0f};
                        maxSpeed = 12.0f;
                        paddleSpeed = 6.0f;
                        aiSpeed = 5.0f;
                        leftPaddle.height = 100;
                        rightPaddle.height = 100;
                    } else if (gameLevel == 3) {
                        ballSpeed = (Vector2){7.0f, 7.0f};
                        maxSpeed = 15.0f;
                        paddleSpeed = 5.0f;
                        aiSpeed = 7.0f;
                        leftPaddle.height = 70;
                        rightPaddle.height = 70;
                    }
                    leftScore = 0;
                    rightScore = 0;
                    ballPosition = (Vector2){ screen_width / 2.0f, screen_height / 2.0f };
                    int dirX = (rand() % 2 == 0) ? 1 : -1;
                    int dirY = (rand() % 2 == 0) ? 1 : -1;
                    ballSpeed.x = fabsf(ballSpeed.x) * dirX;
                    ballSpeed.y = fabsf(ballSpeed.y) * dirY;
                    leftPaddle.y = screen_height / 2 - leftPaddle.height / 2;
                    rightPaddle.y = screen_height / 2 - rightPaddle.height / 2;
                    currentState = PLAYING;
                }
                break;

            case PLAYING:
                ballPosition.x += ballSpeed.x;
                ballPosition.y += ballSpeed.y;

                if (ballPosition.x >= screen_width - ballRadius) {
                    leftScore++;
                    if (leftScore >= winScore) {
                        currentState = GAME_OVER;
                        winnerText = "Lava Wins!";
                    } else {
                        ballPosition = (Vector2){ screen_width / 2.0f, screen_height / 2.0f };
                        int dirX = (rand() % 2 == 0) ? 1 : -1;
                        int dirY = (rand() % 2 == 0) ? 1 : -1;
                        float speed = (gameLevel == 1 ? 4.0f : (gameLevel == 2 ? 5.0f : 7.0f));
                        ballSpeed.x = speed * dirX;
                        ballSpeed.y = speed * dirY;
                    }
                }
                if (ballPosition.x <= ballRadius) {
                    rightScore++;
                    if (rightScore >= winScore) {
                        currentState = GAME_OVER;
                        winnerText = "Ice Wins!";
                    } else {
                        ballPosition = (Vector2){ screen_width / 2.0f, screen_height / 2.0f };
                        int dirX = (rand() % 2 == 0) ? 1 : -1;
                        int dirY = (rand() % 2 == 0) ? 1 : -1;
                        float speed = (gameLevel == 1 ? 4.0f : (gameLevel == 2 ? 5.0f : 7.0f));
                        ballSpeed.x = speed * dirX;
                        ballSpeed.y = speed * dirY;
                    }
                }
                if (ballPosition.y >= screen_height - ballRadius || ballPosition.y <= ballRadius) {
                    ballSpeed.y *= -1;
                }

                // --- Paddle Controls ---
                if (currentMode == PVP) {
                    // Player 1: Mouse controls left paddle
                    leftPaddle.y = GetMouseY() - leftPaddle.height / 2;
                    // Player 2: Arrow keys control right paddle
                    if (IsKeyDown(KEY_UP)) rightPaddle.y -= paddleSpeed;
                    if (IsKeyDown(KEY_DOWN)) rightPaddle.y += paddleSpeed;
                } else if (currentMode == PVC) {
                    // Player: W/S keys for left paddle
                    if (IsKeyDown(KEY_W)) leftPaddle.y -= paddleSpeed;
                    if (IsKeyDown(KEY_S)) leftPaddle.y += paddleSpeed;
                    // AI for right paddle
                    if (rightPaddle.y + rightPaddle.height / 2 < ballPosition.y) {
                        rightPaddle.y += aiSpeed;
                    } else if (rightPaddle.y + rightPaddle.height / 2 > ballPosition.y) {
                        rightPaddle.y -= aiSpeed;
                    }
                }

                // Clamp paddles
                if (leftPaddle.y < 0) leftPaddle.y = 0;
                if (leftPaddle.y + leftPaddle.height > screen_height) leftPaddle.y = screen_height - leftPaddle.height;
                if (rightPaddle.y < 0) rightPaddle.y = 0;
                if (rightPaddle.y + rightPaddle.height > screen_height) rightPaddle.y = screen_height - rightPaddle.height;

                // Ball-paddle collision
                if (CheckCollisionCircleRec(ballPosition, ballRadius, leftPaddle)) {
                    ballSpeed.x *= -1;
                    ballPosition.x = leftPaddle.x + leftPaddle.width + ballRadius;
                    ballSpeed.y += (ballSpeed.y > 0 ? 0.5f : -0.5f);
                }
                if (CheckCollisionCircleRec(ballPosition, ballRadius, rightPaddle)) {
                    ballSpeed.x *= -1;
                    ballPosition.x = rightPaddle.x - ballRadius;
                    ballSpeed.y += (ballSpeed.y > 0 ? 0.5f : -0.5f);
                }
                for (int i = 0; i < 3; i++) {
                    if (CheckCollisionCircleRec(ballPosition, ballRadius, obstacles[i])) {
                        ballSpeed.x *= -1;
                        ballSpeed.y *= -1;
                        ballPosition.x += ballSpeed.x;
                        ballPosition.y += ballSpeed.y;
                    }
                }
                if (ballSpeed.y > maxSpeed) ballSpeed.y = maxSpeed;
                if (ballSpeed.y < -maxSpeed) ballSpeed.y = -maxSpeed;

                // Particle effects
                for (int i = 0; i < maxParticles; i++) {
                    if (lavaParticles[i].lifetime > 0) {
                        lavaParticles[i].position.x += lavaParticles[i].velocity.x;
                        lavaParticles[i].position.y += lavaParticles[i].velocity.y;
                        lavaParticles[i].lifetime -= GetFrameTime();
                    } else if (GetRandomValue(0, 100) < 5) {
                        lavaParticles[i].position = (Vector2){leftPaddle.x + leftPaddle.width, leftPaddle.y + leftPaddle.height / 2};
                        lavaParticles[i].velocity = (Vector2){GetRandomValue(-2, 2), GetRandomValue(-2, 2)};
                        lavaParticles[i].lifetime = GetRandomValue(5, 10) / 10.0f;
                        lavaParticles[i].color = leftColor;
                    }
                    if (iceParticles[i].lifetime > 0) {
                        iceParticles[i].position.x += iceParticles[i].velocity.x;
                        iceParticles[i].position.y += iceParticles[i].velocity.y;
                        iceParticles[i].lifetime -= GetFrameTime();
                    } else if (GetRandomValue(0, 100) < 5) {
                        iceParticles[i].position = (Vector2){rightPaddle.x, rightPaddle.y + rightPaddle.height / 2};
                        iceParticles[i].velocity = (Vector2){GetRandomValue(-2, 2), GetRandomValue(-2, 2)};
                        iceParticles[i].lifetime = GetRandomValue(5, 10) / 10.0f;
                        iceParticles[i].color = rightColor;
                    }
                }
                // Back button
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), backButton)) {
                    currentState = MENU;
                }
                break;

            case GAME_OVER:
                if (IsKeyPressed(KEY_ENTER)) {
                    currentState = MENU;
                    winnerText = NULL;
                }
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), backButton)) {
                    currentState = MENU;
                    winnerText = NULL;
                }
                break;
        }

        BeginDrawing();
        // Set background color based on game state
        if (currentState == MENU) {
            ClearBackground(menuBackgroundColor);
        } else if (currentState == PLAYING) {
            if (currentMode == PVP) {
                ClearBackground(pvpBackgroundColor);
            } else {
                ClearBackground(pvcBackgroundColor);
            }
        } else if (currentState == GAME_OVER) {
            ClearBackground(gameOverBackgroundColor); // Explicitly set dark red for Game Over
        }

        // Draw stars
        for (int i = 0; i < numStars; i++) {
            Color starColor;
            if (currentState == MENU || currentState == GAME_OVER) {
                starColor = Fade(WHITE, 0.5f);
            } else if (currentState == PLAYING) {
                if (currentMode == PVP) {
                    if (stars[i].x < screen_width / 2) {
                        starColor = Fade((Color){255, 165, 0, 255}, 0.5f);
                    } else {
                        starColor = Fade((Color){135, 206, 235, 255}, 0.5f);
                    }
                } else {
                    if (stars[i].x < screen_width / 2) {
                        starColor = Fade((Color){255, 165, 0, 255}, 0.5f);
                    } else {
                        starColor = Fade((Color){0, 255, 255, 255}, 0.5f);
                    }
                }
            }
            DrawPixelV(stars[i], starColor);
        }

        // Draw game elements
        if (currentState == MENU) {
            // Modified to show full game title
            DrawText("Pong: Lava vs Ice", screen_width / 2 - MeasureText("Pong: Lava vs Ice", 40) / 2, 100, 40, WHITE);
            DrawText("Select Mode and Level:", screen_width / 2 - MeasureText("Select Mode and Level:", 30) / 2, 200, 30, WHITE);
            DrawText(menuSelection == 0 ? "> PvP Easy" : "PvP Easy", screen_width / 2 - 150, 250, 30, (menuSelection == 0) ? YELLOW : WHITE);
            DrawText(menuSelection == 1 ? "> PvP Medium" : "PvP Medium", screen_width / 2 - 150, 280, 30, (menuSelection == 1) ? YELLOW : WHITE);
            DrawText(menuSelection == 2 ? "> PvP Hard" : "PvP Hard", screen_width / 2 - 150, 310, 30, (menuSelection == 2) ? YELLOW : WHITE);
            DrawText(menuSelection == 3 ? "> PvC Easy" : "PvC Easy", screen_width / 2 - 150, 350, 30, (menuSelection == 3) ? YELLOW : WHITE);
            DrawText(menuSelection == 4 ? "> PvC Medium" : "PvC Medium", screen_width / 2 - 150, 380, 30, (menuSelection == 4) ? YELLOW : WHITE);
            DrawText(menuSelection == 5 ? "> PvC Hard" : "PvC Hard", screen_width / 2 - 150, 410, 30, (menuSelection == 5) ? YELLOW : WHITE);
            DrawText("Use Up/Down Arrows to select, Enter to start", screen_width / 2 - MeasureText("Use Up/Down Arrows to select, Enter to start", 20) / 2, 460, 20, GRAY);
        } else {
            DrawCircleGradient((int)ballPosition.x, (int)ballPosition.y, ballRadius, ballColor, Fade(ballColor, 0.3f));
            DrawRectangleRec(leftPaddle, leftColor);
            DrawRectangleRec(rightPaddle, rightColor);
            for (int i = 0; i < 3; i++) {
                Color obsColor = (obstacles[i].x < screen_width / 2) ? Fade(leftColor, 0.5f) : Fade(rightColor, 0.5f);
                DrawRectangleRec(obstacles[i], obsColor);
            }
            for (int i = 0; i < screen_height; i += 20) {
                DrawLine(screen_width / 2, i, screen_width / 2, i + 10, Fade(WHITE, 0.2f));
            }
            for (int i = 0; i < maxParticles; i++) {
                if (lavaParticles[i].lifetime > 0) {
                    DrawCircleV(lavaParticles[i].position, 3, Fade(lavaParticles[i].color, lavaParticles[i].lifetime));
                }
                if (iceParticles[i].lifetime > 0) {
                    DrawCircleV(iceParticles[i].position, 3, Fade(iceParticles[i].color, iceParticles[i].lifetime));
                }
            }
            DrawText(TextFormat("%d", leftScore), screen_width / 4, 20, 40, WHITE);
            DrawText(TextFormat("%d", rightScore), 3 * screen_width / 4, 20, 40, WHITE);
            DrawRectangleRec(backButton, GRAY);
            DrawText("Back", backButton.x + 20, backButton.y + 10, 20, WHITE);
            if (currentState == GAME_OVER) {
                DrawText(winnerText, screen_width / 2 - MeasureText(winnerText, 40) / 2, screen_height / 2 - 20, 40, YELLOW);
                DrawText("Press Enter or click Back to return to menu", screen_width / 2 - MeasureText("Press Enter or click Back to return to menu", 20) / 2, screen_height / 2 + 40, 20, GRAY);
            }
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
