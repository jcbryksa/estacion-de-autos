/*
 * display.hpp
 * Copyright 2020 - Juan C. Bryksa (jcbryksa@gmail.com)
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include "common.hpp"


class Display {

public:

  // Define un tipo de ejfecto que puede estar ejecutando el display
  typedef enum {NONE, BLINK, RIGHT_ROTATION, LEFT_ROTATION, SHIFT_UP, SHIFT_DOWN} Effect;

  static void init(const uint8_t *displayPins, uint8_t commonPinLevel = HIGH);
  static void show(uint8_t value);
  static void effect(Effect effect);
  static void clearEffect(void);

private:

  static const uint8_t *pins;    // array de pines
  static uint8_t common;         // terminal comun (puede ser LOW o HIGH)
  static Effect _activeEffect;   // efecto actualmente activo
  static uint8_t _value;         // valor decimal que muestra el display
  static uint8_t _effectStep;    // numero de secuencia o escena que se esta ejecutando en un efecto
  static uint8_t _blinkCounter;  // contador utilizado para el efecto blink

  static void _setSegment(uint8_t segment);
  static void _clearSegment(uint8_t segment);
  static void _playEffect(void);
  static void _setSegmentsByte(uint8_t value);

};


#endif
