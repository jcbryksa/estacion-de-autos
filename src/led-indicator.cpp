/*
 * led-indicator.cpp
 * Copyright 2020 - Juan C. Bryksa (jcbryksa@gmail.com)
 */

#include "led-indicator.hpp"

const uint8_t * LedIndicator::_pins;
uint8_t LedIndicator::_quantity;
LedIndicator::LedStatus LedIndicator::_status[MAX_LEDS];
uint8_t LedIndicator::_common;


void LedIndicator::init(const uint8_t *ledIndicatorPins, uint8_t quantity, uint8_t commonPinLevel) {

  _common = commonPinLevel;
  _pins = ledIndicatorPins;
  _quantity = quantity;

  // Inicializa cada uno de los pines de leds
  for ( int i = 0 ; i < _quantity ; i++ ) {
    pinMode(_pins[i], OUTPUT);
    digitalWrite(_pins[i], _common);
  }

  // Establece cada ciclo para blick con velocidades baja, media y alta
  AsyncLoop.attach(_blinkSlow, 800);
  AsyncLoop.attach(_blinkMedium, 200);
  AsyncLoop.attach(_blinkFast, 100);

}


void LedIndicator::on(uint8_t ind) {
  _status[ind] = ON;
  digitalWrite(_pins[ind], !_common);
}


void LedIndicator::off(uint8_t ind) {
  _status[ind] = OFF;
  digitalWrite(_pins[ind], _common);
}


void LedIndicator::blink(uint8_t ind, LedIndicator::LedStatus blinkStatus) {
  _status[ind] = blinkStatus;
}


uint8_t LedIndicator::toggle(uint8_t ind) {

  uint8_t status = !digitalRead(_pins[ind]);
  digitalWrite(_pins[ind], status);

  if(_common)
    status = !status;

  _status[ind] = (status)? ON : OFF;

  return status;
}


void LedIndicator::_blink(LedIndicator::LedStatus blinkStatus) {

  for ( int i = 0 ; i < _quantity ; i++ )
    if ( _status[i] == blinkStatus )
      digitalWrite(_pins[i], !digitalRead(_pins[i]));

}


void LedIndicator::_blinkSlow() {
  _blink(BLINK_SLOW);
}


void LedIndicator::_blinkMedium() {
  _blink(BLINK_MEDIUM);
}


void LedIndicator::_blinkFast() {
  _blink(BLINK_FAST);
}


LedIndicator::LedStatus LedIndicator::read(uint8_t ind) {
  return _status[ind];
}
