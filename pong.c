#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h> // For fabsf()
// Enum for game states
typedef enum GameState {
    MENU,
    MODE_SELECT,
    DIFFICULTY_SELECT,
    PLAYING,
    GAME_OVER
} GameState;
// Enum for game modes
typedef enum GameMode {
    PVP,  // Player vs Player
    PVC   // Player vs Computer
} GameMode;
// Enum for difficulty levels
typedef enum Difficulty {
    EASY,
    MEDIUM,
    HARD
} Difficulty;
// Particle struct for effects
typedef struct Particle {
    Vector2 position;
    Vector2 velocity;
    float lifetime;
    Color color;
} Particle;
// Raindrop struct for menu animation
typedef struct Raindrop {
    Vector2 position;
    float speed;
    float length;
    Color color;
} Raindrop;

int main() {
    const int screen_width = 1280;
    const int screen_height = 800;
    InitWindow(screen_width, screen_height, "LAVA VS ICE");
      
    
    // Initialize audio system
    InitAudioDevice();
    
    
    // Load sounds and music
    Music backgroundMusic = LoadMusicStream("background_music.mp3");
    Sound gameOverSound = LoadSound("game_over.wav");
    Sound enterSound = LoadSound("enter.wav");
    Sound backSound = LoadSound("back.wav");
    Sound arrowSound = LoadSound("arrow.wav");
    Sound paddleHitSound = LoadSound("paddle_hit.wav");  
    Sound wallHitSound = LoadSound("paddle_hit.wav");     
    
    // Set background music volume to high (1.0f) initially for main menu
    SetMusicVolume(backgroundMusic, 1.5f);
    
    // Play background music in loop
    PlayMusicStream(backgroundMusic);
    
    // Background colors
    Color menuBackgroundColor = BLACK;
    Color pvpBackgroundColor = (Color){50, 0, 50, 255}; // Dark Purple for PvP
    Color pvcBackgroundColor = (Color){0, 60, 60, 255}; // Dark Teal for PvC
    Color gameOverBackgroundColor = (Color){139, 0, 0, 255};  // Dark Red for Game Over
    
    // New split background colors for playing state
    Color leftBackgroundColor = (Color){255, 200, 200, 255}; // Mild red
    Color rightBackgroundColor = (Color){220, 254, 255, 255}; // Very mild sky blue (almost white with blue tint)
    
    // Minecraft-style block colors for menu background
    const int blockSize = 40;
    int gridWidth, gridHeight;
    
    // High opacity block colors for main menu
    Color leftBlockColorsHigh[4] = {
        (Color){255, 200, 200, 200}, // Mild red with higher opacity
        (Color){255, 180, 180, 200}, // Slightly darker red
        (Color){255, 160, 160, 200}, // Even darker red
        (Color){255, 140, 140, 200}  // Darkest red
    };
    
    Color rightBlockColorsHigh[4] = {
        (Color){220, 254, 255, 200}, // Very mild sky blue with higher opacity
        (Color){200, 244, 255, 200}, // Slightly darker blue
        (Color){180, 234, 255, 200}, // Even darker blue
        (Color){160, 224, 255, 200}  // Darkest blue
    };
    
    // Low opacity block colors for secondary menus
    Color leftBlockColors[4] = {
        (Color){255, 200, 200, 60}, // Mild red with low opacity
        (Color){255, 180, 180, 60}, // Slightly darker red
        (Color){255, 160, 160, 60}, // Even darker red
        (Color){255, 140, 140, 60}  // Darkest red
    };
    
    Color rightBlockColors[4] = {
        (Color){220, 254, 255, 60}, // Very mild sky blue with low opacity
        (Color){200, 244, 255, 60}, // Slightly darker blue
        (Color){180, 234, 255, 60}, // Even darker blue
        (Color){160, 224, 255, 60}  // Darkest blue
    };
    
    // Calculate grid dimensions
    gridWidth = screen_width / blockSize + 1;
    gridHeight = screen_height / blockSize + 1;
    
    // Score colors - Lava (left) and Ice (right)
    Color leftScoreColor = (Color){255, 50, 50, 255};   // Bright red-orange for Lava player
    Color rightScoreColor = (Color){50, 150, 255, 255};  // Bright cyan-blue for Ice player
    
    // Back button rectangle
    Rectangle backButton = { screen_width - 150, 20, 120, 40 };
    
    // Ball settings
    Vector2 ballPosition = { screen_width / 2.0f, screen_height / 2.0f };
    Vector2 ballSpeed = { 5.0f, 5.0f };
    int ballRadius = 20;
    Color ballColor = (Color){100, 300, 10, 255}; 
    Color ballGlow = (Color){255, 50, 0, 255}; 
    
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
    Difficulty currentDifficulty = EASY;
    
    // Menu selection variables
    int modeSelection = 0; // 0: PvP, 1: PvAI
    int difficultySelection = 0; // 0: Easy, 1: Medium, 2: Hard
    
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
    
    // Raindrop animation for menu
    const int numRaindrops = 100;
    Raindrop lavaRaindrops[numRaindrops];
    Raindrop iceRaindrops[numRaindrops];
    
    // Initialize raindrops
    for (int i = 0; i < numRaindrops; i++) {
        // Lava raindrops (will be on the right side in main menu)
        lavaRaindrops[i].position = (Vector2){
            (float)GetRandomValue(screen_width / 2, screen_width),
            (float)GetRandomValue(-screen_height, 0)
        };
        lavaRaindrops[i].speed = (float)GetRandomValue(5, 15);
        lavaRaindrops[i].length = (float)GetRandomValue(15, 30); // Increased length
        lavaRaindrops[i].color = (Color){255, 100, 0, 180}; // Orange with more opacity
        
        // Ice raindrops (will be on the left side in main menu)
        iceRaindrops[i].position = (Vector2){
            (float)GetRandomValue(0, screen_width / 2),
            (float)GetRandomValue(-screen_height, 0)
        };
        iceRaindrops[i].speed = (float)GetRandomValue(5, 15);
        iceRaindrops[i].length = (float)GetRandomValue(15, 30); // Increased length
        iceRaindrops[i].color = (Color){0, 150, 255, 180}; // Light blue with more opacity
    }
    
    SetTargetFPS(60);
    
    while (!WindowShouldClose()) {
        // Update music stream
        UpdateMusicStream(backgroundMusic);
        
        switch (currentState) {
            case MENU:
                if (IsKeyPressed(KEY_ENTER)) {
                    PlaySound(enterSound); // Play enter sound
                    // Set volume to medium for mode selection
                    SetMusicVolume(backgroundMusic, 0.6f);
                    currentState = MODE_SELECT;
                }
                // Back button check (though not needed in main menu)
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), backButton)) {
                    // No action needed in main menu
                }
                break;
                
            case MODE_SELECT:
                if (IsKeyPressed(KEY_DOWN)) {
                    PlaySound(arrowSound); // Play arrow sound
                    modeSelection = (modeSelection + 1) % 2;
                }
                if (IsKeyPressed(KEY_UP)) {
                    PlaySound(arrowSound); // Play arrow sound
                    modeSelection = (modeSelection - 1 + 2) % 2;
                }
                if (IsKeyPressed(KEY_ENTER)) {
                    PlaySound(enterSound); // Play enter sound
                    currentMode = (modeSelection == 0) ? PVP : PVC;
                    currentState = DIFFICULTY_SELECT;
                    difficultySelection = 0; // Reset to Easy
                }
                if (IsKeyPressed(KEY_BACKSPACE)) {
                    PlaySound(backSound); // Play back sound
                    // Set volume to high for main menu
                    SetMusicVolume(backgroundMusic, 1.0f);
                    currentState = MENU;
                }
                // Back button mouse click
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), backButton)) {
                    PlaySound(backSound); // Play back sound
                    // Set volume to high for main menu
                    SetMusicVolume(backgroundMusic, 1.0f);
                    currentState = MENU;
                }
                break;
                
            case DIFFICULTY_SELECT:
                if (IsKeyPressed(KEY_DOWN)) {
                    PlaySound(arrowSound); // Play arrow sound
                    difficultySelection = (difficultySelection + 1) % 3;
                }
                if (IsKeyPressed(KEY_UP)) {
                    PlaySound(arrowSound); // Play arrow sound
                    difficultySelection = (difficultySelection - 1 + 3) % 3;
                }
                if (IsKeyPressed(KEY_ENTER)) {
                    PlaySound(enterSound); // Play enter sound
                    currentDifficulty = (Difficulty)difficultySelection;
                    
                    // Set parameters by difficulty
                    if (currentDifficulty == EASY) {
                        ballSpeed = (Vector2){4.0f, 4.0f};
                        maxSpeed = 8.0f;
                        paddleSpeed = 7.0f;
                        aiSpeed = 3.0f;
                        leftPaddle.height = 150;
                        rightPaddle.height = 150;
                    } else if (currentDifficulty == MEDIUM) {
                        ballSpeed = (Vector2){5.0f, 5.0f};
                        maxSpeed = 12.0f;
                        paddleSpeed = 6.0f;
                        aiSpeed = 5.0f;
                        leftPaddle.height = 100;
                        rightPaddle.height = 100;
                    } else if (currentDifficulty == HARD) {
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
                    
                    // Set volume to low for gameplay
                    SetMusicVolume(backgroundMusic, 1.0f);
                    
                    currentState = PLAYING;
                }
                if (IsKeyPressed(KEY_BACKSPACE)) {
                    PlaySound(backSound); // Play back sound
                    currentState = MODE_SELECT;
                }
                // Back button mouse click
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), backButton)) {
                    PlaySound(backSound); // Play back sound
                    currentState = MODE_SELECT;
                }
                break;
                
            case PLAYING:
                ballPosition.x += ballSpeed.x;
                ballPosition.y += ballSpeed.y;
                
                if (ballPosition.x >= screen_width - ballRadius) {
                    leftScore++;
                    if (leftScore >= winScore) {
                        PlaySound(gameOverSound); // Play game over sound
                        StopMusicStream(backgroundMusic); // Stop background music
                        currentState = GAME_OVER;
                        winnerText = "Lava Wins!";
                    } else {
                        ballPosition = (Vector2){ screen_width / 2.0f, screen_height / 2.0f };
                        int dirX = (rand() % 2 == 0) ? 1 : -1;
                        int dirY = (rand() % 2 == 0) ? 1 : -1;
                        float speed = (currentDifficulty == EASY ? 4.0f : 
                                      (currentDifficulty == MEDIUM ? 5.0f : 7.0f));
                        ballSpeed.x = speed * dirX;
                        ballSpeed.y = speed * dirY;
                    }
                }
                
                if (ballPosition.x <= ballRadius) {
                    rightScore++;
                    if (rightScore >= winScore) {
                        PlaySound(gameOverSound); // Play game over sound
                        StopMusicStream(backgroundMusic); // Stop background music
                        currentState = GAME_OVER;
                        winnerText = "Ice Wins!";
                    } else {
                        ballPosition = (Vector2){ screen_width / 2.0f, screen_height / 2.0f };
                        int dirX = (rand() % 2 == 0) ? 1 : -1;
                        int dirY = (rand() % 2 == 0) ? 1 : -1;
                        float speed = (currentDifficulty == EASY ? 4.0f : 
                                      (currentDifficulty == MEDIUM ? 5.0f : 7.0f));
                        ballSpeed.x = speed * dirX;
                        ballSpeed.y = speed * dirY;
                    }
                }
                
                // Wall collision (top and bottom)
                if (ballPosition.y >= screen_height - ballRadius || ballPosition.y <= ballRadius) {
                    ballSpeed.y *= -1;
                    PlaySound(wallHitSound); // Play wall hit sound
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
                    PlaySound(paddleHitSound); // Play paddle hit sound
                }
                
                if (CheckCollisionCircleRec(ballPosition, ballRadius, rightPaddle)) {
                    ballSpeed.x *= -1;
                    ballPosition.x = rightPaddle.x - ballRadius;
                    ballSpeed.y += (ballSpeed.y > 0 ? 0.5f : -0.5f);
                    PlaySound(paddleHitSound); // Play paddle hit sound
                }
                
                // Ball-obstacle collision
                for (int i = 0; i < 3; i++) {
                    if (CheckCollisionCircleRec(ballPosition, ballRadius, obstacles[i])) {
                        ballSpeed.x *= -1;
                        ballSpeed.y *= -1;
                        ballPosition.x += ballSpeed.x;
                        ballPosition.y += ballSpeed.y;
                        PlaySound(wallHitSound); // Play wall hit sound for obstacles too
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
                
                // Back button - Return to difficulty selection instead of main menu
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), backButton)) {
                    PlaySound(backSound); // Play back sound
                    currentState = DIFFICULTY_SELECT;
                }
                break;
                
            case GAME_OVER:
                // Enter key returns to mode selection instead of main menu
                if (IsKeyPressed(KEY_ENTER)) {
                    PlaySound(enterSound); // Play enter sound
                    PlayMusicStream(backgroundMusic); // Restart background music
                    // Set volume to medium for mode selection
                    SetMusicVolume(backgroundMusic, 0.65f);
                    currentState = MODE_SELECT;
                    winnerText = NULL;
                }
                // Back button still returns to main menu
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), backButton)) {
                    PlaySound(backSound); // Play back sound
                    PlayMusicStream(backgroundMusic); // Restart background music
                    // Set volume to high for main menu
                    SetMusicVolume(backgroundMusic, 1.0f);
                    currentState = MENU;
                    winnerText = NULL;
                }
                break;
        }
        
        BeginDrawing();
        
        // Draw background based on game state
        if (currentState == MENU || currentState == MODE_SELECT || currentState == DIFFICULTY_SELECT) {
            // Draw Minecraft-style block background for menu screens
            ClearBackground(BLACK);
            
            // Draw block background with different opacity based on state
            for (int y = 0; y < gridHeight; y++) {
                for (int x = 0; x < gridWidth; x++) {
                    int blockX = x * blockSize;
                    int blockY = y * blockSize;
                    
                    // Create a pattern with blocks
                    int pattern = (x + y) % 4;
                    
                    if (blockX < screen_width / 2) {
                        // Left side - lava theme
                        if (currentState == MENU) {
                            DrawRectangle(blockX, blockY, blockSize, blockSize, leftBlockColorsHigh[pattern]);
                        } else {
                            DrawRectangle(blockX, blockY, blockSize, blockSize, leftBlockColors[pattern]);
                        }
                        DrawRectangleLines(blockX, blockY, blockSize, blockSize, Fade(BLACK, 0.2f));
                    } else {
                        // Right side - ice theme
                        if (currentState == MENU) {
                            DrawRectangle(blockX, blockY, blockSize, blockSize, rightBlockColorsHigh[pattern]);
                        } else {
                            DrawRectangle(blockX, blockY, blockSize, blockSize, rightBlockColors[pattern]);
                        }
                        DrawRectangleLines(blockX, blockY, blockSize, blockSize, Fade(BLACK, 0.2f));
                    }
                }
            }
        } else if (currentState == PLAYING) {
            // Draw split background with mild red on left and very mild sky blue on right
            DrawRectangle(0, 0, screen_width / 2, screen_height, leftBackgroundColor);
            DrawRectangle(screen_width / 2, 0, screen_width / 2, screen_height, rightBackgroundColor);
        } else if (currentState == GAME_OVER) {
            ClearBackground(gameOverBackgroundColor);
        }
        
        // Draw stars only in PLAYING and GAME_OVER states
        if (currentState == PLAYING || currentState == GAME_OVER) {
            for (int i = 0; i < numStars; i++) {
                Color starColor;
                if (currentState == PLAYING) {
                    if (stars[i].x < screen_width / 2) {
                        starColor = Fade((Color){255, 165, 0, 255}, 1.0f);
                    } else {
                        starColor = Fade((Color){135, 206, 235, 255}, 1.0f);
                    }
                } else { // GAME_OVER
                    starColor = Fade(WHITE, 0.5f);
                }
                DrawPixelV(stars[i], starColor);
            }
        }
        
        // Draw raindrops in MENU, MODE_SELECT, and DIFFICULTY_SELECT states
        if (currentState == MENU || currentState == MODE_SELECT || currentState == DIFFICULTY_SELECT) {
            if (currentState == MENU) {
                // In main menu: swap sides - lava raindrops on right, ice raindrops on left
                // Use largest raindrops for main menu
                
                // Update and draw lava raindrops (right side)
                for (int i = 0; i < numRaindrops; i++) {
                    // Update position
                    lavaRaindrops[i].position.y += lavaRaindrops[i].speed;
                    
                    // Reset if off screen
                    if (lavaRaindrops[i].position.y > screen_height) {
                        lavaRaindrops[i].position = (Vector2){
                            (float)GetRandomValue(screen_width / 2, screen_width),
                            (float)GetRandomValue(-50, -10)
                        };
                        lavaRaindrops[i].speed = (float)GetRandomValue(5, 15);
                        lavaRaindrops[i].length = (float)GetRandomValue(15, 30);
                    }
                    
                    // Draw raindrop as a line with largest thickness for main menu
                    DrawLineEx(
                        (Vector2){lavaRaindrops[i].position.x, lavaRaindrops[i].position.y},
                        (Vector2){lavaRaindrops[i].position.x, lavaRaindrops[i].position.y + lavaRaindrops[i].length},
                        5.0f, // Largest thickness for main menu
                        lavaRaindrops[i].color
                    );
                }
                
                // Update and draw ice raindrops (left side)
                for (int i = 0; i < numRaindrops; i++) {
                    // Update position
                    iceRaindrops[i].position.y += iceRaindrops[i].speed;
                    
                    // Reset if off screen
                    if (iceRaindrops[i].position.y > screen_height) {
                        iceRaindrops[i].position = (Vector2){
                            (float)GetRandomValue(0, screen_width / 2),
                            (float)GetRandomValue(-50, -10)
                        };
                        iceRaindrops[i].speed = (float)GetRandomValue(5, 15);
                        iceRaindrops[i].length = (float)GetRandomValue(15, 30);
                    }
                    
                    // Draw raindrop as a line with largest thickness for main menu
                    DrawLineEx(
                        (Vector2){iceRaindrops[i].position.x, iceRaindrops[i].position.y},
                        (Vector2){iceRaindrops[i].position.x, iceRaindrops[i].position.y + iceRaindrops[i].length},
                        5.0f, // Largest thickness for main menu
                        iceRaindrops[i].color
                    );
                }
            } else {
                // In mode and difficulty selection: mixed raindrops on both sides
                // Use medium-sized raindrops for other menus
                
                // Draw lava raindrops on both sides
                for (int i = 0; i < numRaindrops; i++) {
                    // Update position
                    lavaRaindrops[i].position.y += lavaRaindrops[i].speed;
                    
                    // Reset if off screen
                    if (lavaRaindrops[i].position.y > screen_height) {
                        lavaRaindrops[i].position = (Vector2){
                            (float)GetRandomValue(0, screen_width),
                            (float)GetRandomValue(-50, -10)
                        };
                        lavaRaindrops[i].speed = (float)GetRandomValue(5, 15);
                        lavaRaindrops[i].length = (float)GetRandomValue(12, 25); // Increased length
                    }
                    
                    // Draw raindrop as a line with medium thickness
                    DrawLineEx(
                        (Vector2){lavaRaindrops[i].position.x, lavaRaindrops[i].position.y},
                        (Vector2){lavaRaindrops[i].position.x, lavaRaindrops[i].position.y + lavaRaindrops[i].length},
                        4.0f, // Increased thickness for mode and difficulty selection
                        lavaRaindrops[i].color
                    );
                }
                
                // Draw ice raindrops on both sides
                for (int i = 0; i < numRaindrops; i++) {
                    // Update position
                    iceRaindrops[i].position.y += iceRaindrops[i].speed;
                    
                    // Reset if off screen
                    if (iceRaindrops[i].position.y > screen_height) {
                        iceRaindrops[i].position = (Vector2){
                            (float)GetRandomValue(0, screen_width),
                            (float)GetRandomValue(-50, -10)
                        };
                        iceRaindrops[i].speed = (float)GetRandomValue(5, 15);
                        iceRaindrops[i].length = (float)GetRandomValue(12, 25); // Increased length
                    }
                    
                    // Draw raindrop as a line with medium thickness
                    DrawLineEx(
                        (Vector2){iceRaindrops[i].position.x, iceRaindrops[i].position.y},
                        (Vector2){iceRaindrops[i].position.x, iceRaindrops[i].position.y + iceRaindrops[i].length},
                        4.0f, // Increased thickness for mode and difficulty selection
                        iceRaindrops[i].color
                    );
                }
            }
        }
        
        // Draw back button in all screens except main menu
        if (currentState == MODE_SELECT || currentState == DIFFICULTY_SELECT || currentState == GAME_OVER || currentState == PLAYING) {
            // Use a mild color for the back button in playing state
            Color buttonColor = GRAY;
            if (currentState == PLAYING) {
                buttonColor = (Color){80, 80, 80, 180}; // Semi-transparent gray
            }
            DrawRectangleRec(backButton, buttonColor);
            DrawText("Back", backButton.x + 20, backButton.y + 10, 20, WHITE);
        }
        
        // Draw game elements
        if (currentState == MENU) {
            // Draw colorful title with gradient effect
            int titleFontSize = 80;
            int titleY = screen_height / 3;
            
            // Calculate positions for each part of the title
            int lavaWidth = MeasureText("LAVA", titleFontSize);
            int vsWidth = MeasureText(" VS ", titleFontSize);
            int iceWidth = MeasureText("ICE", titleFontSize);
            int totalWidth = lavaWidth + vsWidth + iceWidth;
            int startX = screen_width / 2 - totalWidth / 2;
            
            // Draw LAVA with fiery gradient
            for (int i = 0; i < 5; i++) {
                Color lavaColor = ColorLerp((Color){255, 50, 0, 255}, (Color){255, 200, 0, 255}, i / 4.0f);
                DrawText("LAVA", startX - i, titleY - i, titleFontSize, Fade(lavaColor, 1.0f - i * 0.15f));
            }
            
            // Draw " VS " in white
            DrawText(" VS ", startX + lavaWidth, titleY, titleFontSize, WHITE);
            
            // Draw ICE with icy gradient
            for (int i = 0; i < 5; i++) {
                Color iceColor = ColorLerp((Color){0, 150, 255, 255}, (Color){150, 220, 255, 255}, i / 4.0f);
                DrawText("ICE", startX + lavaWidth + vsWidth - i, titleY - i, titleFontSize, Fade(iceColor, 1.0f - i * 0.15f));
            }
            
            // Draw instruction text with better visibility
            DrawText("Press ENTER to start", screen_width / 2 - MeasureText("Press ENTER to start", 40) / 2, screen_height / 2, 40, WHITE);
        } else if (currentState == MODE_SELECT) {
            DrawText("Select Game Mode", screen_width / 2 - MeasureText("Select Game Mode", 60) / 2, 100, 60, WHITE);
            DrawText(modeSelection == 0 ? "> Player Vs Player" : "Player Vs Player", screen_width / 2 - 180, 250, 40, (modeSelection == 0) ? YELLOW : WHITE);
            DrawText(modeSelection == 1 ? "> Player Vs AI" : "Player Vs AI", screen_width / 2 - 180, 320, 40, (modeSelection == 1) ? YELLOW : WHITE);
            DrawText("Use Up/Down Arrows to select, Enter to choose", screen_width / 2 - MeasureText("Use Up/Down Arrows to select, Enter to choose", 25) / 2, 450, 25, GRAY);
            DrawText("Press Backspace or click Back to go back", screen_width / 2 - MeasureText("Press Backspace or click Back to go back", 25) / 2, 490, 25, GRAY);
        } else if (currentState == DIFFICULTY_SELECT) {
            DrawText("Select Difficulty", screen_width / 2 - MeasureText("Select Difficulty", 60) / 2, 100, 60, WHITE);
            DrawText(difficultySelection == 0 ? "> Easy" : "Easy", screen_width / 2 - 180, 250, 40, (difficultySelection == 0) ? YELLOW : WHITE);
            DrawText(difficultySelection == 1 ? "> Medium" : "Medium", screen_width / 2 - 180, 320, 40, (difficultySelection == 1) ? YELLOW : WHITE);
            DrawText(difficultySelection == 2 ? "> Hard" : "Hard", screen_width / 2 - 180, 390, 40, (difficultySelection == 2) ? YELLOW : WHITE);
            DrawText("Use Up/Down Arrows to select, Enter to start", screen_width / 2 - MeasureText("Use Up/Down Arrows to select, Enter to start", 25) / 2, 450, 25, GRAY);
            DrawText("Press Backspace or click Back to go back", screen_width / 2 - MeasureText("Press Backspace or click Back to go back", 25) / 2, 490, 25, GRAY);
        } else {
            // Draw gold ball with yellow glow that suits the LAVA VS ICE theme
            DrawCircleGradient((int)ballPosition.x, (int)ballPosition.y, ballRadius, ballColor, ballGlow);
            // Add an extra glow ring for stronger effect
            DrawCircleLines((int)ballPosition.x, (int)ballPosition.y, ballRadius + 2, ballGlow);
            
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
            
            // Draw scores with theme-appropriate colors
            DrawText(TextFormat("%d", leftScore), screen_width / 4, 20, 40, leftScoreColor);
            DrawText(TextFormat("%d", rightScore), 3 * screen_width / 4, 20, 40, rightScoreColor);
            
            if (currentState == GAME_OVER) {
                DrawText(winnerText, screen_width / 2 - MeasureText(winnerText, 60) / 2, screen_height / 2 - 30, 60, YELLOW);
                DrawText("Press Enter to return to mode selection", screen_width / 2 - MeasureText("Press Enter to return to mode selection", 30) / 2, screen_height / 2 + 50, 30, GRAY);
                DrawText("or click Back to return to main menu", screen_width / 2 - MeasureText("or click Back to return to main menu", 30) / 2, screen_height / 2 + 80, 30, GRAY);
            }
        }
        
        EndDrawing();
    }
    
    // Unload sounds and music
    UnloadMusicStream(backgroundMusic);
    UnloadSound(gameOverSound);
    UnloadSound(enterSound);
    UnloadSound(backSound);
    UnloadSound(arrowSound);
    UnloadSound(paddleHitSound);  // Unload paddle hit sound
    UnloadSound(wallHitSound);    // Unload wall hit sound
    
    // Close audio system
    CloseAudioDevice();
    
    CloseWindow();
    return 0;
}
