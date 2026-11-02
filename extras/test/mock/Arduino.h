// Имитация Arduino.h и шины с микросхемой MAX6675 / MAX31855 для тестов AlashThermocouple.
// Микросхема отдаёт старший бит при CS = LOW и сдвигает следующий по спаду SCK, как настоящая.
#pragma once
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
class __FlashStringHelper;
#define F(x) (reinterpret_cast<const __FlashStringHelper *>(x))
#define PROGMEM
#define HEX 16
#define OUTPUT 1
#define INPUT 0
#define LOW 0
#define HIGH 1
#define MSBFIRST 1
#define SPI_MODE0 0
inline uint32_t g_ms = 1000;
inline uint32_t millis() { return g_ms; }
inline void delayMicroseconds(unsigned) {}
struct Bus {
  uint8_t cs = 0, clk = 0, miso = 0;
  uint32_t frame = 0; int nbits = 16;
  bool csLow = false; int idx = 0; int lastClk = 0; bool floating = false; bool floatHigh = true;
  int frames = 0; int clockEdgesWhileCsHigh = 0;
  void reset() { frame = 0; nbits = 16; csLow = false; idx = 0; lastClk = 0; floating = false; floatHigh = true; frames = 0; clockEdgesWhileCsHigh = 0; }
  int bit() const { return idx < nbits ? (frame >> (nbits - 1 - idx)) & 1 : 0; }
};
inline Bus g_bus;
inline void pinMode(uint8_t, uint8_t) {}
inline void digitalWrite(uint8_t p, uint8_t v) {
  if (p == g_bus.cs) { if (v == LOW && !g_bus.csLow) { g_bus.csLow = true; g_bus.idx = 0; g_bus.frames++; } else if (v == HIGH) g_bus.csLow = false; }
  else if (p == g_bus.clk) {
    if (!g_bus.csLow && v != g_bus.lastClk) g_bus.clockEdgesWhileCsHigh++;
    if (g_bus.csLow && g_bus.lastClk == HIGH && v == LOW) g_bus.idx++;      // следующий бит по спаду
    g_bus.lastClk = v;
  }
}
inline int digitalRead(uint8_t p) { if (p != g_bus.miso) return 0; if (g_bus.floating) return g_bus.floatHigh; return g_bus.csLow ? g_bus.bit() : 1; }
class Print {
 public:
  std::string text;
  void put(const char *s) { printf("%s", s); text += s; }
  void print(const char *s) { put(s); }
  void print(const __FlashStringHelper *s) { put((const char *)s); }
  void println(const __FlashStringHelper *s) { put((const char *)s); put("\n"); }
  void print(long v) { char b[24]; snprintf(b, sizeof b, "%ld", v); put(b); }
  void println(long v) { print(v); put("\n"); }
  void print(double v, int d = 2) { char b[40]; snprintf(b, sizeof b, "%.*f", d, v); put(b); }
  void println(double v, int d = 2) { print(v, d); put("\n"); }
  void println(unsigned long v, int base) { char b[24]; snprintf(b, sizeof b, base == 16 ? "%lX" : "%lu", v); put(b); put("\n"); }
  void println(const char *s = "") { put(s); put("\n"); }
};
