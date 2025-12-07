#ifndef CONFIG_H
#define CONFIG_H

// BLE
#define BLE_NAME "ESP32-DINO-GAME"
#define SERVICE_UUID "fc96f65e-318a-4001-84bd-77e9d12af44b"
#define CHARACTERISTIC_UUID_RX "04d3552e-b9b3-4be6-a8b4-aa43c4507c4d"

// DAC para efectos de sonido
#define DAC_SOUND_PIN 25

// Pantalla TFT
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 320

// Área del juego
#define HUD_HEIGHT 60
#define GAME_AREA_HEIGHT (SCREEN_HEIGHT - HUD_HEIGHT)
#define GROUND_Y 240  // Altura del suelo

// Dinosaurio
#define DINO_X 50
#define DINO_WIDTH 20
#define DINO_HEIGHT 30
#define JUMP_FORCE 15
#define GRAVITY 1

// Obstáculos
#define OBSTACLE_WIDTH 15
#define OBSTACLE_HEIGHT 30
#define OBSTACLE_SPEED_INITIAL 5
#define OBSTACLE_SPEED_MAX 12

// Juego
#define SPEED_INCREASE_INTERVAL 500

// Colores (RGB565)
#define COLOR_BG 0x0000
#define COLOR_DINO 0x07E0
#define COLOR_OBSTACLE 0xF800
#define COLOR_GROUND 0xFFFF
#define COLOR_TEXT 0xFFFF
#define COLOR_CLOUD 0x7BEF

#endif
