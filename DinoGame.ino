#include <TFT_eSPI.h>
#include "Config.h"
#include "Point.h"
#include "BLEController.h"
#include "SoundPlayer.h"
#include "DinoGame.h"

TFT_eSPI tft = TFT_eSPI();
BLEController bleController;
SoundPlayer soundPlayer;
DinoGame game;

// Variables para optimización de renderizado
int lastDinoY = -1;
int lastObstaclePositions[3] = {-1, -1, -1};
int lastBirdPositions[2] = {-1, -1};

void drawGround() {
  tft.drawLine(0, GROUND_Y, SCREEN_WIDTH, GROUND_Y, COLOR_GROUND);
  
  for (int x = 0; x < SCREEN_WIDTH; x += 20) {
    tft.drawPixel(x, GROUND_Y + 1, COLOR_GROUND);
  }
}

void clearDino(int y) {
  tft.fillRect(DINO_X - 5, y - 5, DINO_WIDTH + 13, DINO_HEIGHT + 15, COLOR_BG);
}

void drawDino() {
  int y = game.getDinoY();
  
  if (y != lastDinoY && lastDinoY != -1) {
    clearDino(lastDinoY);
  }
  lastDinoY = y;
  
  // Cuerpo
  tft.fillRect(DINO_X, y, DINO_WIDTH, DINO_HEIGHT, COLOR_DINO);
  
  // Cabeza
  tft.fillRect(DINO_X + DINO_WIDTH - 5, y - 5, 8, 8, COLOR_DINO);
  
  // Ojo
  tft.fillCircle(DINO_X + DINO_WIDTH - 2, y - 2, 2, TFT_WHITE);
  tft.fillCircle(DINO_X + DINO_WIDTH - 2, y - 2, 1, TFT_BLACK);
  
  // Patas animadas
  if (!game.isDinoJumping()) {
    static bool legToggle = false;
    static unsigned long lastLegUpdate = 0;
    
    if (millis() - lastLegUpdate > 100) {
      legToggle = !legToggle;
      lastLegUpdate = millis();
    }
    
    if (legToggle) {
      tft.fillRect(DINO_X + 3, y + DINO_HEIGHT, 4, 5, COLOR_DINO);
      tft.fillRect(DINO_X + 13, y + DINO_HEIGHT + 2, 4, 3, COLOR_DINO);
    } else {
      tft.fillRect(DINO_X + 3, y + DINO_HEIGHT + 2, 4, 3, COLOR_DINO);
      tft.fillRect(DINO_X + 13, y + DINO_HEIGHT, 4, 5, COLOR_DINO);
    }
  }
  
  // Cola
  tft.fillRect(DINO_X - 5, y + 5, 6, 3, COLOR_DINO);
}

void clearObstacle(int x, int y) {
  tft.fillRect(x - 5, y, OBSTACLE_WIDTH + 10, OBSTACLE_HEIGHT + 5, COLOR_BG);
}

void drawObstacles() {
  const Obstacle* obstacles = game.getObstacles();
  
  for (int i = 0; i < 3; i++) {
    if (lastObstaclePositions[i] != -1 && 
        (!obstacles[i].active || obstacles[i].x != lastObstaclePositions[i])) {
      clearObstacle(lastObstaclePositions[i], obstacles[i].y);
    }
    
    if (obstacles[i].active) {
      lastObstaclePositions[i] = obstacles[i].x;
      
      // Cactus
      tft.fillRect(obstacles[i].x, obstacles[i].y, 
                   OBSTACLE_WIDTH, OBSTACLE_HEIGHT, COLOR_OBSTACLE);
      
      // Brazos
      tft.fillRect(obstacles[i].x - 3, obstacles[i].y + 8, 3, 8, COLOR_OBSTACLE);
      tft.fillRect(obstacles[i].x + OBSTACLE_WIDTH, obstacles[i].y + 12, 3, 8, COLOR_OBSTACLE);
    } else {
      lastObstaclePositions[i] = -1;
    }
  }
}

void clearBird(int x, int y) {
  tft.fillRect(x - 2, y - 2, 24, 19, COLOR_BG);
}

void drawBirds() {
  if (!game.areBirdsEnabled()) return;
  
  const Bird* birds = game.getBirds();
  
  for (int i = 0; i < 2; i++) {
    if (lastBirdPositions[i] != -1 && 
        (!birds[i].active || birds[i].x != lastBirdPositions[i])) {
      clearBird(lastBirdPositions[i], birds[i].y);
    }
    
    if (birds[i].active) {
      lastBirdPositions[i] = birds[i].x;
      
      int x = birds[i].x;
      int y = birds[i].y;
      
      // Cuerpo del ave (color diferente para visibilidad)
      tft.fillEllipse(x + 10, y + 7, 8, 5, TFT_ORANGE);
      
      // Cabeza
      tft.fillCircle(x + 4, y + 5, 4, TFT_ORANGE);
      
      // Ojo
      tft.fillCircle(x + 3, y + 4, 1, TFT_BLACK);
      
      // Pico
      tft.fillTriangle(x, y + 5, x + 3, y + 4, x + 3, y + 6, TFT_YELLOW);
      
      // Alas (animadas)
      if (birds[i].wingFrame == 0) {
        // Alas arriba
        tft.fillTriangle(x + 8, y + 5, x + 6, y, x + 12, y + 3, TFT_DARKCYAN);
        tft.fillTriangle(x + 12, y + 5, x + 14, y, x + 18, y + 3, TFT_DARKCYAN);
      } else {
        // Alas abajo
        tft.fillTriangle(x + 8, y + 9, x + 6, y + 14, x + 12, y + 11, TFT_DARKCYAN);
        tft.fillTriangle(x + 12, y + 9, x + 14, y + 14, x + 18, y + 11, TFT_DARKCYAN);
      }
    } else {
      lastBirdPositions[i] = -1;
    }
  }
}

void drawHUD() {
  static int lastScore = -1;
  static int lastHighScore = -1;
  static int lastSpeed = -1;
  static bool lastBirdsEnabled = false;
  
  int currentScore = game.getScore();
  int currentHighScore = game.getHighScore();
  int currentSpeed = game.getObstacleSpeed();
  bool birdsEnabled = game.areBirdsEnabled();
  
  if (currentScore != lastScore) {
    tft.fillRect(10, 15, 200, 20, COLOR_BG);
    tft.setTextColor(COLOR_TEXT, COLOR_BG);
    tft.setTextSize(2);
    tft.setCursor(10, 15);
    tft.printf("Score: %d", currentScore);
    lastScore = currentScore;
  }
  
  if (currentHighScore != lastHighScore) {
    tft.fillRect(10, 35, 200, 20, COLOR_BG);
    tft.setTextColor(COLOR_TEXT, COLOR_BG);
    tft.setTextSize(2);
    tft.setCursor(10, 35);
    tft.printf("HI: %d", currentHighScore);
    lastHighScore = currentHighScore;
  }
  
  if (currentSpeed != lastSpeed) {
    tft.fillRect(250, 15, 100, 20, COLOR_BG);
    tft.setTextColor(COLOR_TEXT, COLOR_BG);
    tft.setTextSize(2);
    tft.setCursor(250, 15);
    tft.printf("Speed: %d", currentSpeed);
    lastSpeed = currentSpeed;
  }
  
  // Indicador de aves
  if (birdsEnabled != lastBirdsEnabled) {
    tft.fillRect(250, 35, 100, 20, COLOR_BG);
    if (birdsEnabled) {
      tft.setTextColor(TFT_ORANGE, COLOR_BG);
      tft.setTextSize(2);
      tft.setCursor(250, 35);
      tft.print("AVES!");
    }
    lastBirdsEnabled = birdsEnabled;
  }
  
  // Estado BLE
  static unsigned long lastBLEUpdate = 0;
  static bool lastConnected = false;
  
  if (millis() - lastBLEUpdate > 1000 || bleController.isConnected() != lastConnected) {
    lastBLEUpdate = millis();
    lastConnected = bleController.isConnected();
    
    tft.fillRect(370, 20, 100, 25, COLOR_BG);
    if (bleController.isConnected()) {
      tft.setTextColor(TFT_GREEN, COLOR_BG);
      tft.setCursor(370, 25);
      tft.print("ONLINE");
    } else {
      tft.setTextColor(TFT_RED, COLOR_BG);
      tft.setCursor(365, 25);
      tft.print("OFFLINE");
    }
  }
}

void drawGameFull() {
  tft.fillRect(0, HUD_HEIGHT, SCREEN_WIDTH, GAME_AREA_HEIGHT, COLOR_BG);
  drawGround();
  
  lastDinoY = -1;
  for (int i = 0; i < 3; i++) {
    lastObstaclePositions[i] = -1;
  }
  for (int i = 0; i < 2; i++) {
    lastBirdPositions[i] = -1;
  }
}

void drawGameIncremental() {
  drawObstacles();
  drawBirds(); // Nuevo
  drawDino();
  drawGround();
  drawHUD();
}

void drawGameOver() {
  tft.fillScreen(COLOR_BG);
  
  tft.setTextColor(TFT_RED, COLOR_BG);
  tft.setTextSize(5);
  tft.setCursor(50, 80);
  tft.println("GAME OVER");
  
  tft.setTextColor(COLOR_TEXT, COLOR_BG);
  tft.setTextSize(3);
  tft.setCursor(120, 150);
  tft.printf("Score: %d", game.getScore());
  
  if (game.getScore() == game.getHighScore() && game.getScore() > 0) {
    tft.setTextColor(TFT_YELLOW, COLOR_BG);
    tft.setTextSize(2);
    tft.setCursor(140, 190);
    tft.println("NEW RECORD!");
  }
  
  tft.setTextColor(COLOR_TEXT, COLOR_BG);
  tft.setTextSize(3);
  tft.setCursor(80, 220);
  tft.printf("Best: %d", game.getHighScore());
  
  tft.setTextSize(2);
  tft.setTextColor(TFT_DARKGREY, COLOR_BG);
  tft.setCursor(80, 280);
  tft.println("Presiona para reiniciar");
}

void drawWaitingScreen() {
  static unsigned long lastUpdate = 0;
  static int dots = 0;
  static int dinoAnim = 0;
  
  if (millis() - lastUpdate > 500) {
    lastUpdate = millis();
    dots = (dots + 1) % 4;
    dinoAnim = (dinoAnim + 1) % 2;
    
    tft.fillScreen(COLOR_BG);
    
    tft.setTextColor(TFT_CYAN);
    tft.setTextSize(5);
    tft.setCursor(50, 60);
    tft.println("DINO GAME");
    
    // Dinosaurio animado
    int animDinoY = 150;
    tft.fillRect(200, animDinoY, 30, 40, COLOR_DINO);
    tft.fillCircle(210, animDinoY + 10, 3, TFT_WHITE);
    
    if (dinoAnim == 0) {
      tft.fillRect(205, animDinoY + 40, 5, 8, COLOR_DINO);
      tft.fillRect(220, animDinoY + 43, 5, 5, COLOR_DINO);
    } else {
      tft.fillRect(205, animDinoY + 43, 5, 5, COLOR_DINO);
      tft.fillRect(220, animDinoY + 40, 5, 8, COLOR_DINO);
    }
    
    tft.setTextColor(COLOR_TEXT);
    tft.setTextSize(2);
    tft.setCursor(130, 210);
    tft.print("Esperando");
    for (int i = 0; i < dots; i++) {
      tft.print(".");
    }
    for (int i = dots; i < 3; i++) {
      tft.print(" ");
    }
    
    tft.setCursor(140, 240);
    tft.setTextColor(bleController.isConnected() ? TFT_GREEN : TFT_ORANGE);
    tft.print(bleController.isConnected() ? "CONECTADO" : "DESCONECTADO");
    
    tft.setTextColor(TFT_DARKGREY);
    tft.setTextSize(1);
    tft.setCursor(125, 290);
    tft.println("Presiona el boton para saltar e iniciar");
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);
  
  Serial.println("\n=====================================");
  Serial.println("===   DINO GAME - ESP32 + TFT    ===");
  Serial.println("===   Con AVES (Score > 500)     ===");
  Serial.println("=====================================");
  
  Serial.println("[1/3] Inicializando TFT...");
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(COLOR_BG);
  tft.setTextColor(TFT_WHITE, COLOR_BG);
  tft.setTextSize(2);
  tft.setCursor(100, 150);
  tft.println("Cargando...");
  Serial.println("      TFT OK");
  delay(300);
  
  Serial.println("[2/3] Inicializando audio DAC...");
  soundPlayer.begin();
  Serial.println("      Audio OK");
  delay(300);
  
  Serial.println("[3/3] Inicializando BLE...");
  bleController.begin();
  Serial.println("      BLE OK");
  delay(300);
  
  Serial.println("Inicializando juego...");
  game.init();
  Serial.println("Juego OK");
  
  tft.fillScreen(COLOR_BG);
  
  Serial.println("\n=====================================");
  Serial.println("Sistema listo - 30 FPS");
  Serial.println("AVES aparecen con Score > 500!");
  Serial.println("=====================================\n");
}

void loop() {
  static unsigned long lastFrameTime = 0;
  unsigned long currentTime = millis();
  
  if (currentTime - lastFrameTime < 33) {
    delay(1);
    return;
  }
  lastFrameTime = currentTime;
  
  Command cmd = bleController.getCommand();
  
  if (cmd == CMD_JUMP) {
    if (!game.hasStarted()) {
      game.start();
      Serial.println("*** JUEGO INICIADO ***");
      drawGameFull();
      
    } else if (game.hasEnded()) {
      game.init();
      Serial.println("*** REINICIANDO ***");
      
    } else {
      game.jump();
      soundPlayer.playJumpSound();
    }
  }
  
  if (game.hasStarted() && !game.hasEnded()) {
    game.update();
    drawGameIncremental();
    
    if (game.hasEnded()) {
      Serial.println("\n=================================");
      Serial.println("          GAME OVER");
      Serial.println("=================================");
      Serial.printf("Score: %d\n", game.getScore());
      Serial.printf("Best: %d\n", game.getHighScore());
      Serial.println("=================================\n");
      
      soundPlayer.playDeathSound();
      drawGameOver();
      delay(2000);
    }
    
  } else {
    drawWaitingScreen();
  }
}
