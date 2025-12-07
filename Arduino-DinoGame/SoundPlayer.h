#ifndef SOUND_PLAYER_H
#define SOUND_PLAYER_H

#include "Config.h"

class SoundPlayer {
private:
  void playTone(int frequency, int duration) {
    const int sampleRate = 8000;
    const int samples = (sampleRate * duration) / 1000;
    
    for (int i = 0; i < samples; i++) {
      float angle = (2.0 * PI * frequency * i) / sampleRate;
      int value = (int)(128 + 127 * sin(angle));
      
      dacWrite(DAC_SOUND_PIN, value);
      delayMicroseconds(1000000 / sampleRate);
    }
    
    dacWrite(DAC_SOUND_PIN, 128); // Silencio
  }
  
public:
  SoundPlayer() {}
  
  void begin() {
    pinMode(DAC_SOUND_PIN, OUTPUT);
    dacWrite(DAC_SOUND_PIN, 128);
    Serial.println("DAC configurado en GPIO 25");
  }
  
  void playJumpSound() {
    // Sonido de salto - tono ascendente rápido
    playTone(600, 50);
    playTone(800, 50);
  }
  
  void playScoreSound() {
    // Sonido de puntuación - beep corto
    playTone(1200, 30);
  }
  
  void playDeathSound() {
    // Sonido de muerte - tono descendente
    playTone(800, 100);
    playTone(600, 100);
    playTone(400, 150);
    playTone(200, 200);
  }
  
  void playStartSound() {
    // Sonido de inicio - melodía corta
    playTone(523, 80);   // C5
    playTone(659, 80);   // E5
    playTone(784, 120);  // G5
  }
};

#endif
