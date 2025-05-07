#include "raylib.h"
#include <cmath>
#include <cstdlib>
#include <ctime>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

const int MAX_OBSTACLES = 20;
const int MAX_POWERUPS = 5;

enum GameScreen { MENU, GAMEPLAY, GAME_OVER };

typedef struct Obstacle {
    Rectangle rect;
    int fallSpeed;
    bool active;
    bool isZigzag;
    float zigzagSpeed;
    float zigzagOffset;
    int shapeType; // 0: kare, 1: daire, 2: üçgen
} Obstacle;

typedef struct PowerUp {
    Rectangle rect;
    int type;
    bool active;
    float duration;
} PowerUp;

typedef struct Particle {
    Vector2 position;
    Vector2 velocity;
    float lifetime;  // Ne kadar süre yaşayacak
    Color color;
    bool active;
} Particle;

const int MAX_PARTICLES = 200;
Particle particles[MAX_PARTICLES];


// Oyun objeleri
Obstacle obstacles[MAX_OBSTACLES];
PowerUp powerUps[MAX_POWERUPS];
Rectangle player;

// Oyuncu özellikleri
float playerSpeed = 5.0f;
float originalPlayerSpeed = 5.0f;
int lives = 3;

// Seviye ve skor sistemleri
int level = 1;
float gameTimer = 0.0f;
float timePerLevel = 15.0f;
float totalSurvivalTime = 0.0f;
int score = 0;
float levelScore = 0.0f;

// Spawn zamanlayıcıları
int obstacleSpawnTimer = 0;
int powerUpSpawnTimer = 0;

// Power-up durumları
bool isInvisible = false;
float invisibilityTimer = 0.0f;

bool hasSpeedBoost = false;
float speedBoostTimer = 0.0f;

bool hasBlockReset = false;
float blockResetTimer = 0.0f;

bool hasTimeSlow = false;
float timeSlowTimer = 0.0f;

bool hasShield = false;
float shieldTimer = 0.0f;

// Diğer yardımcı değişkenler
float timeSlowFactor = 1.0f;



GameScreen currentScreen = MENU;

void ResetGame() {
    player = { SCREEN_WIDTH / 2.0f - 25, SCREEN_HEIGHT - 60, 50, 50 };
    lives = 3;
    level = 1;
    gameTimer = 0.0f;
    totalSurvivalTime = 0.0f;
    score = 0;
    levelScore = 0.0f; 
    obstacleSpawnTimer = 0;
    powerUpSpawnTimer = 0;
    isInvisible = false;
    invisibilityTimer = 0;
    hasSpeedBoost = false;
    speedBoostTimer = 0;
    hasTimeSlow = false;
    timeSlowTimer = 0;
    hasShield = false;

    for (int i = 0; i < MAX_OBSTACLES; i++) obstacles[i].active = false;
    for (int i = 0; i < MAX_POWERUPS; i++) powerUps[i].active = false;

    for (int i = 0; i < MAX_PARTICLES; i++) {
        particles[i].active = false;
    }
    
}


void SpawnObstacle() {
    obstacleSpawnTimer++;
    int spawnThreshold = hasTimeSlow ? 50 : 30;
    if (obstacleSpawnTimer >= spawnThreshold) {
        obstacleSpawnTimer = 0;
        for (int i = 0; i < MAX_OBSTACLES; i++) {
            if (!obstacles[i].active) {
                float width = 40, height = 40;
                obstacles[i].rect = { (float)(rand() % (SCREEN_WIDTH - (int)width)), -height, width, height };
                int baseSpeed = 1 + level;
                obstacles[i].fallSpeed = hasTimeSlow ? baseSpeed : baseSpeed + 1;

                if (level >= 4 ) {
                    obstacles[i].isZigzag = true;
                    obstacles[i].zigzagSpeed = 0.1f + (float)(rand() % 5) / 50.0f;
                    obstacles[i].zigzagOffset = 0;
                } else {
                    obstacles[i].isZigzag = false;
                }

                obstacles[i].shapeType = rand() % 3;
                obstacles[i].active = true;
                break;
            }
        }
    }
}

void DrawObstacle(const Obstacle& obs) {
    float cx = obs.rect.x + obs.rect.width / 2;
    float cy = obs.rect.y + obs.rect.height / 2;
    Color color = RED;

    switch (obs.shapeType) {
        case 0: // kare
            DrawRectangleRec(obs.rect, color);
            break;
        case 1: // daire
            DrawCircle((int)cx, (int)cy, obs.rect.width / 2, color);
            break;
        case 2: // üçgen
            DrawTriangle(
                (Vector2){ cx, obs.rect.y },
                (Vector2){ obs.rect.x, obs.rect.y + obs.rect.height },
                (Vector2){ obs.rect.x + obs.rect.width, obs.rect.y + obs.rect.height },
                color
            );
            break;
    }
}





void drawPowerUp(const PowerUp& powerUp) {
    float r = 0.0f, g = 0.0f, b = 0.0f;
    switch (powerUp.type) {
        case 1: // Speed Boost - Yeşil
            r = 0.0f; g = 1.0f; b = 0.0f;
            break;
        case 2: // Block Reset - Mavi
            r = 0.0f; g = 0.0f; b = 1.0f;
            break;
        case 3: // Invisibility - Sarı
            r = 1.0f; g = 1.0f; b = 0.0f;
            break;
        case 4: // Time Slow - Cyan (Açık Mavi)
            r = 0.0f; g = 1.0f; b = 1.0f;
            break;
        case 5: // Shield - Magenta
            r = 1.0f; g = 0.0f; b = 1.0f;
            break;
        case 6: // Extra Life - Parlak Pembe/Mor
            r = 1.0f; g = 0.2f; b = 0.8f;
            break;
    }

    
    DrawRectangleRec(
        (Rectangle){ powerUp.rect.x, powerUp.rect.y, 40, 40 },
        ColorFromNormalized({ r, g, b, 1.0f })
    );
}




void blocksClearAndRespawn() {
    // Eski blokları kapat
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        obstacles[i].active = false;
    }

    // Seviyeye göre yeni bloklar doğur
    for (int i = 0; i < level && i < MAX_OBSTACLES; i++) {
        float width = 40, height = 40;
        obstacles[i].rect = { (float)(rand() % (SCREEN_WIDTH - (int)width)), -height, width, height };
        obstacles[i].fallSpeed = 2 + level;
        obstacles[i].isZigzag = (level >= 4) && (rand() % 2 == 0);
        obstacles[i].zigzagSpeed = 0.1f + (float)(rand() % 5) / 50.0f;
        obstacles[i].zigzagOffset = 0;
        obstacles[i].shapeType = rand() % 3;
        obstacles[i].active = true;
    }
}



void createHealthParticles() {
    Vector2 center = { player.x + player.width / 2, player.y + player.height / 2 };
    for (int i = 0; i < 30; i++) { // 30 tane partikül
        for (int j = 0; j < MAX_PARTICLES; j++) {
            if (!particles[j].active) {
                float angle = (rand() % 360) * DEG2RAD;
                float speed = 100.0f + rand() % 100;
                particles[j].position = center;
                particles[j].velocity = { cosf(angle) * speed, sinf(angle) * speed };
                particles[j].lifetime = 1.0f; // 1 saniye yaşasın
                particles[j].color = PINK; 
                particles[j].active = true;
                break;
            }
        }
    }
}



void CollectPowerUp(int type) {
    switch (type) {
        case 1: // Speed Boost
            hasSpeedBoost = true;
            speedBoostTimer = 20.0f;
            playerSpeed = originalPlayerSpeed + 3.0f; // Hızı artırmak için
            break;

        case 2: // Block Reset
            hasBlockReset = true;
            blockResetTimer = 20.0f;
            blocksClearAndRespawn();
            break;

        case 3: // Invisibility
            isInvisible = true;
            invisibilityTimer = 20.0f;
            break;

        case 4: // Time Slow
            hasTimeSlow = true;
            timeSlowTimer = 15.0f;
            break;

        case 5: // Shield
            hasShield = true;
            shieldTimer = 10.0f;
            break;

        case 6: // Extra Life
            if (lives < 5) lives++;
            createHealthParticles();
            break;
    }
}


void UpdatePowerUpEffects() {
    float dt = GetFrameTime(); // Frame süresi

    // SpeedBoost efekti
    if (hasSpeedBoost) {
        speedBoostTimer -= dt;
        if (speedBoostTimer <= 0) {
            hasSpeedBoost = false;
            playerSpeed = originalPlayerSpeed;
        }
    }

    // Block Reset efekti
    if (hasBlockReset) {
        blockResetTimer -= dt;
        if (blockResetTimer <= 0) {
            hasBlockReset = false;
        }
    }

    // Invisibility efekti
    if (isInvisible) {
        invisibilityTimer -= dt;
        if (invisibilityTimer <= 0) {
            isInvisible = false;
        }
    }

    // Time Slow efekti
    if (hasTimeSlow) {
        timeSlowTimer -= dt;
        if (timeSlowTimer <= 0) {
            hasTimeSlow = false;
            timeSlowFactor = 1.0f; // Zamanı normale döndür
        }
    }

    // Shield efekti
    if (hasShield) {
        shieldTimer -= dt;
        if (shieldTimer <= 0) {
            hasShield = false;
        }
    }
}



void SpawnPowerUp() {
    if (!isInvisible && !hasSpeedBoost) { 
        
        int activePowerUps = 0;
        for (int i = 0; i < MAX_POWERUPS; i++) {
            if (powerUps[i].active) activePowerUps++;
        }

        if (activePowerUps >= MAX_POWERUPS) return; // 5'ten fazla power-up olmasın
        
        int powerUpChance;
        if (level <= 3) {
            powerUpChance = 500;
        } else if (level <= 5) {
            powerUpChance = 300;
        } else if (level <= 8) {
            powerUpChance = 200;
        } else {
            powerUpChance = 100;
        }

        if (rand() % powerUpChance == 0) { // Şans mekanizması

            for (int i = 0; i < MAX_POWERUPS; i++) {
                if (!powerUps[i].active) {
                    int r = rand() % 100;
                    int powerUpType;
                    
                    if (level <= 3) {
                        powerUpType = (r < 80) ? (rand() % 3 + 1) : (rand() % 3 + 4);
                    } else if (level <= 6) {
                        powerUpType = (r < 60) ? (rand() % 3 + 1) : (rand() % 3 + 4);
                    } else {
                        powerUpType = (r < 40) ? (rand() % 3 + 1) : (rand() % 3 + 4);
                    }

                    powerUps[i].rect = { (float)(rand() % (SCREEN_WIDTH - 40)), -40.0f, 40.0f, 40.0f };
                    powerUps[i].type = powerUpType;
                    powerUps[i].duration = 5.0f;
                    powerUps[i].active = true;
                    break;
                }
            }
        }
    }
}



void CheckPowerUpCollisions() {
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (powerUps[i].active && CheckCollisionRecs(player, powerUps[i].rect)) {
            CollectPowerUp(powerUps[i].type);
            powerUps[i].active = false;
        }
    }
}

void UpdatePowerUps() {
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (powerUps[i].active) {
            powerUps[i].rect.y += 2 + (level * 0.5f);

            if (CheckCollisionRecs(player, powerUps[i].rect)) {
                CollectPowerUp(powerUps[i].type);
                powerUps[i].active = false;
            }

            // Eğer ekranın dışına çıkarsa, pasif hale getir
            if (powerUps[i].rect.y > SCREEN_HEIGHT) {
                powerUps[i].active = false;
            }
        }
    }
}

void CreateShieldBreakParticles(Vector2 center) {
    for (int i = 0; i < 30; i++) {
        for (int j = 0; j < MAX_PARTICLES; j++) {
            if (!particles[j].active) {
                float angle = (rand() % 360) * DEG2RAD;
                float speed = 200.0f + rand() % 200;
                particles[j].position = center;
                particles[j].velocity = { cosf(angle) * speed, sinf(angle) * speed };
                particles[j].lifetime = 1.0f; 
                particles[j].color = SKYBLUE;
                particles[j].active = true;
                break;
            }
        }
    }
}
void UpdateParticles() {
    float dt = GetFrameTime();
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].active) {
            particles[i].position.x += particles[i].velocity.x * dt;
            particles[i].position.y += particles[i].velocity.y * dt;
            particles[i].lifetime -= dt;
            if (particles[i].lifetime <= 0) {
                particles[i].active = false;
            }
        }
    }
}

void DrawParticles() {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].active) {
            DrawCircleV(particles[i].position, 3, particles[i].color);
        }
    }
}

void UpdateObstacles() {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (obstacles[i].active) {
            if (obstacles[i].isZigzag) {
                obstacles[i].zigzagOffset += obstacles[i].zigzagSpeed;
                obstacles[i].rect.x += sinf(obstacles[i].zigzagOffset) * 2.5f;
            }
            obstacles[i].rect.y += obstacles[i].fallSpeed;

            if (!isInvisible && CheckCollisionRecs(player, obstacles[i].rect)) {
                if (hasShield) {
                    CreateShieldBreakParticles((Vector2){ player.x + player.width / 2, player.y + player.height / 2 });
                    hasShield = false;
                    obstacles[i].active = false;
                    
                } else {
                    lives--;
                    if (lives <= 0) currentScreen = GAME_OVER;
                    obstacles[i].active = false;
                }
            }
            
            if (obstacles[i].rect.y > SCREEN_HEIGHT) {
                obstacles[i].active = false;
                score += 1; // Blok kaçtı, oyuncuya puan!
            }
        }
    }
}






int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Avoidance Game");
    SetTargetFPS(60);
    srand(time(0));

    ResetGame();

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        float delta = 0.0f; 
        switch (currentScreen) {
            case MENU:
                DrawText("AVOIDANCE GAME", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 3, 40, WHITE);
                DrawText("Press [ENTER] to Start", SCREEN_WIDTH / 2 - 140, SCREEN_HEIGHT / 2, 20, GRAY);
                if (IsKeyPressed(KEY_ENTER)) {
                    ResetGame();
                    currentScreen = GAMEPLAY;
                }
                break;

                case GAMEPLAY:
                gameTimer += GetFrameTime();
                
            
                if (gameTimer >= timePerLevel) {
                    level++;
                    gameTimer = 0;
                
                
                    for (int i = 0; i < MAX_OBSTACLES; i++) {
                        if (obstacles[i].active) {
                            obstacles[i].fallSpeed += 1; 
                        }
                    }
                }
            
                if (IsKeyDown(KEY_LEFT)) player.x -= hasSpeedBoost ? 8 : 5;
                if (IsKeyDown(KEY_RIGHT)) player.x += hasSpeedBoost ? 8 : 5;
            
                if (player.x < 0) player.x = 0;
                if (player.x + player.width > SCREEN_WIDTH) player.x = SCREEN_WIDTH - player.width;
            
                SpawnObstacle();
                SpawnPowerUp();
            
                UpdateObstacles();
                UpdatePowerUps();
                UpdatePowerUpEffects();
                UpdateParticles();


                if (hasShield) {
                    bool shieldBroken = false; // Kalkanın kırılıp kırılmadığını takip etmek için bir bayrak
                    float shieldBreakEffectTimer = 0.0f; // Efekt süresi için bir zamanlayıcı ekle
                    for (int i = 0; i < MAX_OBSTACLES; i++) {
                        if (obstacles[i].active && CheckCollisionRecs(player, obstacles[i].rect)) {
                            hasShield = false;  // Kalkan kırıldı
                            shieldBroken = true;  // Kalkanın kırıldığını işaretle
                            break;  // Kalkanın kırıldığını tespit ettik, daha fazla kontrol etmeye gerek yok
                        }
                    }
                
                    // Eğer kalkan kırıldıysa efekt oluştur
                    if (shieldBroken) {
                        CreateShieldBreakParticles((Vector2){ player.x + player.width / 2, player.y + player.height / 2 });
                    }
                
                    // Kalkanı çiz
                    DrawCircleLines((int)(player.x + player.width / 2), (int)(player.y + player.height / 2), player.width, SKYBLUE);
                    DrawCircle((int)(player.x + player.width / 2), (int)(player.y + player.height / 2), player.width + 5, Fade(SKYBLUE, 0.2f));
                }
                
            
                DrawRectangleRec(player, isInvisible ? DARKGRAY : GREEN);
                for (int i = 0; i < MAX_OBSTACLES; i++)
                    if (obstacles[i].active) DrawObstacle(obstacles[i]);
            
                for (int i = 0; i < MAX_POWERUPS; i++)
                    if (powerUps[i].active) drawPowerUp(powerUps[i]);

                DrawParticles();
            
                DrawText(TextFormat("Lives: %d", lives), 10, 10, 20, RAYWHITE);
                DrawText(TextFormat("Level: %d", level), 10, 35, 20, ORANGE);
                DrawText(TextFormat("Score: %d", score), 10, 60, 20, YELLOW); // Buraya dikkat: anlık score
                break;
            
            case GAME_OVER:
                DrawText("GAME OVER", SCREEN_WIDTH / 2 - 120, SCREEN_HEIGHT / 3, 40, RED);
                DrawText(TextFormat("Final Score: %d", score), SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 20, 20, RAYWHITE);
                DrawText("Press [ENTER] to Return to Menu", SCREEN_WIDTH / 2 - 180, SCREEN_HEIGHT / 2 + 20, 20, GRAY);
                if (IsKeyPressed(KEY_ENTER)) currentScreen = MENU;
                break;
        }
        

        EndDrawing();
    }

    CloseWindow();
    return 0;
}



