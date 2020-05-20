/*
 * led-indicator.hpp
 * Copyright 2020 - Juan C. Bryksa (jcbryksa@gmail.com)
 */

#ifndef LED_INDICATOR_H
#define LED_INDICATOR_H

#include "common.hpp"

#define MAX_LEDS 20

class LedIndicator {

public:

  // Define el estado de los leds
  typedef enum {OFF, ON, BLINK_SLOW, BLINK_MEDIUM, BLINK_FAST} LedStatus;

  static void init(const uint8_t *ledIndicatorPins, uint8_t quantity, uint8_t commonPinLevel = HIGH);
  static void on(uint8_t ind);
  static void off(uint8_t ind);
  static uint8_t toggle(uint8_t ind);
  static void blink(uint8_t ind, LedIndicator::LedStatus blinkStatus = LedIndicator::BLINK_MEDIUM);
  static LedIndicator::LedStatus read(uint8_t ind);

private:

  static const uint8_t * _pins;       // array de pines conectado a cada led
  static uint8_t _quantity;           // cantidad total de leds
  static LedStatus _status[MAX_LEDS]; // array de estado de cada led
  static uint8_t _common;             // terminal comun (puede ser LOW o HIGH)

  static void _blink(LedIndicator::LedStatus blinkStatus);
  static void _blinkSlow(void);   // establece el efecto blink con velocidad baja
  static void _blinkMedium(void); // idem con velocidad media
  static void _blinkFast(void);   // idem con velocidad alta

};

#endif
