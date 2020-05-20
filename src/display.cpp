/*
 * display.cpp
 * Copyright 2020 - Juan C. Bryksa (jcbryksa@gmail.com)
 */

#include "display.hpp"

const uint8_t * Display::pins;
uint8_t Display::common;
Display::Effect Display::_activeEffect = NONE;
uint8_t Display::_value = 0;
uint8_t Display::_effectStep = 0;
uint8_t Display::_blinkCounter = 4;

void Display::_setSegment(uint8_t segment) {
  digitalWrite(pins[segment], (common==LOW)? HIGH : LOW);
}


void Display::_clearSegment(uint8_t segment) {
  digitalWrite(pins[segment], (common==LOW)? LOW : HIGH);
}


void Display::init(const uint8_t *displayPins, uint8_t commonPinLevel) {

  common = commonPinLevel;
  pins = displayPins;

  for ( int i = 0 ; i < 7 ; i++ ) {
    pinMode(pins[i], OUTPUT);
    _clearSegment(i);
  }

  setInterval(_playEffect, 70);

}


void Display::show(uint8_t value) {

  const uint8_t numbers[] = {
   //-gfedcba
    B00111111, //0
    B00000110, //1
    B01011011, //2
    B01001111, //3
    B01100110, //4
    B01101101, //5
    B01111101, //6
    B00000111, //7
    B01111111, //8
    B01101111, //9
  };

  _setSegmentsByte(numbers[value]);

  _value = value;

}


void Display::_setSegmentsByte(uint8_t value) {

  uint8_t mask = 0x01;

  for ( int i = 0 ; i < 7 ; i++ ) {
    if ( (value & mask) )
      _setSegment(i);
    else
      _clearSegment(i);

    mask <<= 1;// mask &= 0xFE;
  }

}


void Display::_playEffect() {

  switch(_activeEffect) {

    case NONE: break;

    case BLINK: {

      if (!_blinkCounter) {
        if (_effectStep) {
          _setSegmentsByte(0);
          _effectStep = 0;
        } else {
          show(_value);
          _effectStep = 1;
        }
        _blinkCounter = 4;
      }
      else
        _blinkCounter--;

      break;
    }

    case RIGHT_ROTATION: {

      if (_effectStep == B01000000)
        _effectStep = B00000001;

      _setSegmentsByte(_effectStep);

      _effectStep <<=1;

      break;
    }

    case LEFT_ROTATION: {

      if (_effectStep == 0)
        _effectStep = B0100000;

      _setSegmentsByte(_effectStep);

      _effectStep >>=1;

      break;
    }

    case SHIFT_UP:
    case SHIFT_DOWN: {

      const uint8_t up[] = {
       //-gfedcba
        B00001000, //0
        B01000000, //1
        B00000001, //2
      };
      const uint8_t down[] = {
       //-gfedcba
        B00000001, //0
        B01000000, //1
        B00001000, //2
      };

      if (!_blinkCounter) {

        if (_effectStep == 3)
          _effectStep = 0;

        if ( _activeEffect == SHIFT_UP )
          _setSegmentsByte(up[_effectStep]);
        else
          _setSegmentsByte(down[_effectStep]);

        _effectStep++;

        _blinkCounter = 2;
      }
      else
        _blinkCounter--;

      break;
    }

  }

}


void Display::effect(Display::Effect effect) {

  _activeEffect = effect;

  switch(_activeEffect) {
    case NONE: {
      show(_value);
      break;
    }
    case BLINK: {
      _effectStep = 1;
      break;
    }
    case RIGHT_ROTATION: {
      _effectStep = B00000001;
      break;
    }
    case LEFT_ROTATION: {
      _effectStep = B00100000;
      break;
    }
    case SHIFT_UP:
    case SHIFT_DOWN: {
      _effectStep = 0;
      break;
    }
  }

}


void Display::clearEffect() {
  _activeEffect = NONE;
  show(_value);
}
