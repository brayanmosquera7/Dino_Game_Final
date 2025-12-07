#ifndef DINO_GAME_H
#define DINO_GAME_H

#include "Config.h"
#include "Point.h"

class Obstacle {
public:
  int x;
  int y;
  bool active;
  
  Obstacle() : x(SCREEN_WIDTH), y(GROUND_Y - OBSTACLE_HEIGHT), active(true) {}
  
  void reset() {
    x = SCREEN_WIDTH;
    active = true;
  }
  
  void move(int speed) {
    x -= speed;
    if (x < -OBSTACLE_WIDTH) {
      active = false;
    }
  }
};

class Bird {
public:
  int x;
  int y;
  bool active;
  int wingFrame;
  
  Bird() : x(SCREEN_WIDTH), y(GROUND_Y - 60), active(false), wingFrame(0) {}
  
  void reset(int height) {
    x = SCREEN_WIDTH + random(100, 300);
    y = GROUND_Y - height;
    active = true;
    wingFrame = 0;
  }
  
  void move(int speed) {
    x -= speed + 2;
    wingFrame = (wingFrame + 1) % 2;
    
    if (x < -25) {
      active = false;
    }
  }
  
  int getWidth() const { return 20; }
  int getHeight() const { return 15; }
};

class DinoGame {
private:
  int dinoY;
  int dinoVelocityY;
  bool isJumping;
  bool isOnGround;
  
  Obstacle obstacles[3];
  Bird birds[2];
  
  int obstacleSpeed;
  unsigned long lastObstacleSpawn;
  unsigned long lastBirdSpawn;
  
  int score;
  int highScore;
  bool gameOver;
  bool isStarted;
  bool birdsEnabled;
  
  void resetDino() {
    dinoY = GROUND_Y - DINO_HEIGHT;
    dinoVelocityY = 0;
    isJumping = false;
    isOnGround = true;
  }
  
  void spawnObstacle() {
    for (int i = 0; i < 3; i++) {
      if (!obstacles[i].active) {
        obstacles[i].reset();
        obstacles[i].x = SCREEN_WIDTH + random(50, 200);
        lastObstacleSpawn = millis();
        return;
      }
    }
  }
  
  void spawnBird() {
    if (!birdsEnabled) return;
    
    for (int i = 0; i < 2; i++) {
      if (!birds[i].active) {
        // ===== MODIFICADO: Alturas a nivel de salto del dino =====
        // Altura del salto del dino es ~100-110 píxeles
        // Las aves aparecen entre 70-110 píxeles de altura
        int heights[] = {70, 90, 110}; // Altura baja, media y alta del salto
        int randomHeight = heights[random(0, 3)];
        // ==========================================================
        
        birds[i].reset(randomHeight);
        lastBirdSpawn = millis();
        return;
      }
    }
  }
  
  bool checkCollision() {
    int dinoLeft = DINO_X;
    int dinoRight = DINO_X + DINO_WIDTH;
    int dinoTop = dinoY;
    int dinoBottom = dinoY + DINO_HEIGHT;
    
    // Colisión con cactus
    for (int i = 0; i < 3; i++) {
      if (obstacles[i].active) {
        int obsLeft = obstacles[i].x;
        int obsRight = obstacles[i].x + OBSTACLE_WIDTH;
        int obsTop = obstacles[i].y;
        int obsBottom = obstacles[i].y + OBSTACLE_HEIGHT;
        
        if (dinoRight > obsLeft && dinoLeft < obsRight &&
            dinoBottom > obsTop && dinoTop < obsBottom) {
          return true;
        }
      }
    }
    
    // Colisión con aves
    if (birdsEnabled) {
      for (int i = 0; i < 2; i++) {
        if (birds[i].active) {
          int birdLeft = birds[i].x;
          int birdRight = birds[i].x + birds[i].getWidth();
          int birdTop = birds[i].y;
          int birdBottom = birds[i].y + birds[i].getHeight();
          
          // Hitbox ligeramente reducida para fairness
          if (dinoRight > birdLeft + 2 && dinoLeft < birdRight - 2 &&
              dinoBottom > birdTop + 2 && dinoTop < birdBottom - 2) {
            return true;
          }
        }
      }
    }
    
    return false;
  }

public:
  DinoGame() : dinoY(GROUND_Y - DINO_HEIGHT), dinoVelocityY(0), 
               isJumping(false), isOnGround(true),
               obstacleSpeed(OBSTACLE_SPEED_INITIAL),
               lastObstacleSpawn(0), lastBirdSpawn(0),
               score(0), highScore(0),
               gameOver(false), isStarted(false), birdsEnabled(false) {}
  
  void init() {
    resetDino();
    
    for (int i = 0; i < 3; i++) {
      obstacles[i].active = false;
    }
    
    for (int i = 0; i < 2; i++) {
      birds[i].active = false;
    }
    
    obstacleSpeed = OBSTACLE_SPEED_INITIAL;
    lastObstacleSpawn = 0;
    lastBirdSpawn = 0;
    score = 0;
    gameOver = false;
    isStarted = false;
    birdsEnabled = false;
  }
  
  void start() {
    isStarted = true;
    lastObstacleSpawn = millis();
    lastBirdSpawn = millis();
  }
  
  void jump() {
    if (isOnGround && !gameOver) {
      dinoVelocityY = -JUMP_FORCE;
      isJumping = true;
      isOnGround = false;
    }
  }
  
  void update() {
    if (!isStarted || gameOver) return;
    
    // Física del dinosaurio
    dinoVelocityY += GRAVITY;
    dinoY += dinoVelocityY;
    
    // Limitar al suelo
    if (dinoY >= GROUND_Y - DINO_HEIGHT) {
      dinoY = GROUND_Y - DINO_HEIGHT;
      dinoVelocityY = 0;
      isJumping = false;
      isOnGround = true;
    }
    
    // Mover obstáculos
    for (int i = 0; i < 3; i++) {
      if (obstacles[i].active) {
        obstacles[i].move(obstacleSpeed);
      }
    }
    
    // Mover aves
    if (birdsEnabled) {
      for (int i = 0; i < 2; i++) {
        if (birds[i].active) {
          birds[i].move(obstacleSpeed);
        }
      }
    }
    
    // Generar nuevo obstáculo (cactus)
    if (millis() - lastObstacleSpawn > 1800) {
      spawnObstacle();
    }
    
    // Generar nueva ave (solo si score > 500)
    if (birdsEnabled && millis() - lastBirdSpawn > 2500) {
      spawnBird();
    }
    
    // Incrementar puntuación
    score++;
    
    // Activar aves cuando score > 500
    if (score > 500 && !birdsEnabled) {
      birdsEnabled = true;
      Serial.println("*** AVES ACTIVADAS! ***");
    }
    
    // Aumentar velocidad gradualmente
    if (score % SPEED_INCREASE_INTERVAL == 0 && obstacleSpeed < OBSTACLE_SPEED_MAX) {
      obstacleSpeed++;
    }
    
    // Verificar colisión
    if (checkCollision()) {
      gameOver = true;
      if (score > highScore) {
        highScore = score;
      }
    }
  }
  
  // Getters
  bool hasStarted() const { return isStarted; }
  bool hasEnded() const { return gameOver; }
  int getScore() const { return score; }
  int getHighScore() const { return highScore; }
  int getDinoY() const { return dinoY; }
  bool isDinoJumping() const { return isJumping; }
  int getObstacleSpeed() const { return obstacleSpeed; }
  bool areBirdsEnabled() const { return birdsEnabled; }
  
  const Obstacle* getObstacles() const { return obstacles; }
  const Bird* getBirds() const { return birds; }
};

#endif
