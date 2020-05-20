/*
 * keypad.cpp
 * Copyright 2020 - Juan C. Bryksa (jcbryksa@gmail.com)
 */

#include "keypad.hpp"

const uint8_t *Keypad::_pins;
uint8_t Keypad::_trigger;
uint8_t Keypad::_quantity;
uint8_t Keypad::_pressed[MAX_BUTTONS];
unsigned long Keypad::_timestamp = 0;
unsigned long Keypad::_debounceInterval;

void (*Keypad::_handler)(uint8_t) = NULL;
void (**Keypad::_handlers)(void) = NULL;


void Keypad::init(const uint8_t *pins, uint8_t quantity, void (*handler)(uint8_t), uint8_t trigger, unsigned long debounceInterval) {

  _pins = pins;
  _quantity = quantity;
  _handler = handler;
  _trigger = trigger;
  _debounceInterval = debounceInterval;

  memset(_pressed, 0, sizeof(_pressed));

  // Inicializa los pines establecidos para los switches
  for ( int i = 0 ; i < _quantity ; i++ )
    if ( _trigger == LOW )
      pinMode(_pins[i], INPUT_PULLUP);
    else
      pinMode(_pins[i], INPUT);

}


void Keypad::scan() {

  // Recorre cada uno de los switches e invoca su funcion
  // handler correspondiente con cada liberacion de tecla
  // con una logica para la eliminacion de rebote
  for ( int i = 0 ; i < _quantity ; i++ )
    if ( digitalRead(_pins[i]) == _trigger ) {
      _pressed[i] = 1;
    } else {
      if ( _pressed[i] && millis() - _timestamp > _debounceInterval) {
        _handler(i);
        _timestamp = millis();
      }
      _pressed[i] = 0;
    }

}
