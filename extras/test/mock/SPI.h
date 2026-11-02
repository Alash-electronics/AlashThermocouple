// Имитация аппаратного SPI: байты берутся из того же кадра, что и у программной шины.
#pragma once
#include "Arduino.h"
struct SPISettings { uint32_t clock; uint8_t order, mode; SPISettings(uint32_t c = 1000000, uint8_t o = MSBFIRST, uint8_t m = SPI_MODE0) : clock(c), order(o), mode(m) {} };
inline SPISettings g_lastSettings;
inline int g_spiBegins = 0, g_spiTransactions = 0;
class SPIClass {
 public:
  void begin() { g_spiBegins++; }
  void beginTransaction(SPISettings s) { g_lastSettings = s; g_spiTransactions++; _pos = 0; }
  void endTransaction() {}
  uint8_t transfer(uint8_t) {
    int nbytes = g_bus.nbits / 8;
    if (!g_bus.csLow || _pos >= nbytes) return 0xFF;
    uint8_t b = (g_bus.frame >> (8 * (nbytes - 1 - _pos))) & 0xFF; _pos++; return b;
  }
 private:
  int _pos = 0;
};
inline SPIClass SPI;
