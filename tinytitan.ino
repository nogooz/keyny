#include <DigiKeyboard.h>
#include "blake2s.h"

#define MIC A1
#define LDR A0
#define BUTTON 0
#define BUFFER_SIZE 32

uint8_t entropyBuffer[BUFFER_SIZE];
uint8_t writeIndex = 0;
unsigned long lastButtonMicros = 0;
uint8_t key[32];
char hex[3] = {0, 0, 0};  // globale, riusato

void writeToBuffer(uint8_t sample){
  entropyBuffer[writeIndex] ^= sample;
  writeIndex = (writeIndex + 1) % BUFFER_SIZE;
}

void setup() {
  pinMode(BUTTON, INPUT_PULLUP);
}

void loop() {
  writeToBuffer(analogRead(MIC) & 0xFF);
  writeToBuffer(analogRead(LDR) & 0xFF);

  if (digitalRead(BUTTON) == LOW && millis() > 3000){
    unsigned long now = micros();
    unsigned long delta = now - lastButtonMicros;
    lastButtonMicros = now;
    writeToBuffer((uint8_t)(delta & 0xFF));
    writeToBuffer((uint8_t)((delta >> 8) & 0xFF));

    blake2s(key, entropyBuffer, BUFFER_SIZE);

    for (uint8_t i = 0; i < 32; i++) {
      hex[0] = "0123456789abcdef"[key[i] >> 4];
      hex[1] = "0123456789abcdef"[key[i] & 0x0F];
      DigiKeyboard.print(hex);
    }
    DigiKeyboard.println();

    // Aspetta rilascio pulsante
    while (digitalRead(BUTTON) == LOW) {
      DigiKeyboard.delay(10);
    }
    DigiKeyboard.delay(300);
  }

  DigiKeyboard.delay(10);
}