/*
 * keypad.hpp
 * Copyright 2020 - Juan C. Bryksa (jcbryksa@gmail.com)
 */

#ifndef keypad_h
#define keypad_h

#include "common.hpp"

#define MAX_BUTTONS 10

class Keypad {

  static const uint8_t *_pins;            // array de pines correspondiente a los switches
  static uint8_t _trigger;                // nivel de disparo (LOW o HIGH)
  static uint8_t _quantity;               // cantidad total de switches
  static uint8_t _pressed[MAX_BUTTONS];   // array de flags que determinan si se encuentran presionados (lo utiliza la logica de eliminacion de rebote)
  static unsigned long _timestamp;        // almacena un timestamp (lo utiliza la la eliminacion de rebote)
  static unsigned long _debounceInterval; // valor de espera en milisegundos para la eliminacion de rebote

  static void (*_handler)(uint8_t);
  static void (**_handlers)(void);

public:

  static void init(const uint8_t *pins, uint8_t quantity, void (*handler)(uint8_t), uint8_t trigger = LOW, unsigned long debounceInterval = 100);
  static void scan(void);

};

#endif
