/*
 * light.cpp
 * Copyright 2020 - Juan C. Bryksa (jcbryksa@gmail.com)
 */

#include "light.hpp"

Adafruit_NeoPixel * Light::_pixels = NULL;
Light::Zone Light::_zones[TOTAL_ZONES] = {{0,12}, {13,25}, {26,38}};
Light::Scene Light::_scene = NONE;
uint32_t Light::_step = 0;
uint8_t Light::_intervalScaler = SCALER_SLOW_SPEED;
uint8_t Light::_intervalScalerCounter = _intervalScaler;
AsynchLoop::LoopId Light::_autoOffInterval;
long Light::_onTimeSeconds;
Light::Status Light::_status;

// Array que define todas las posibles conbinatorias de secuencias on/off
// con cada encendido/apagado iran rotando
Light::ChangeType Light::_changeTypes[] = {
  {_sequentialOn, _sequentialOff},
  {_sequentialFadeOn, _sequentialFadeOff},
  {_fadeOn, _sequentialOff},
  {_sequentialOn, _off},
  {_on, _sequentialOff},
  {_sequentialOn, _off}
};
uint8_t Light::_activeChangeType = 0;


void Light::init(const uint8_t dataPin) {

  // Evita algunos milisegundos de destellos indeseados
  pinMode(dataPin, INPUT_PULLUP);

  _pixels = new Adafruit_NeoPixel(TOTAL_PIXELS, dataPin, NEO_GRB + NEO_KHZ800);
  _pixels->begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  _pixels->clear(); // Set all pixel colors to 'off'
  setAll(ZERO_BRIGHT, ZERO_BRIGHT, ZERO_BRIGHT);
  setInterval(_runInterval, 2);

  _onTimeSeconds = ON_TIME_SECONDS;

  _status = OFF;

}


void Light::setAll(int red, int green, int blue) {

  for( int i=0; i < TOTAL_PIXELS ; i++ ) {
    _pixels->setPixelColor(i, _pixels->Color(red, blue, green));
  }

  _pixels->show();

}


void Light::_runInterval() {

  if ( _intervalScalerCounter ) {
    _intervalScalerCounter--;
    return;
  }
  else
    _intervalScalerCounter = _intervalScaler;

  if ( ! _step )
    return;

  switch (_scene)
  {

    case SEQUENTIAL_ON: {

      _setZone(_step-1, MAX_BRIGHT, MAX_BRIGHT, MAX_BRIGHT);

      if ( _step == 1 )
        _scene = NONE;

      break;
    }

    case SEQUENTIAL_OFF: {

      _setZone(_step-1, ZERO_BRIGHT, ZERO_BRIGHT, ZERO_BRIGHT);

      if ( _step == 1 )
        _scene = NONE;

      break;
    }

    case FADE_ON: {

      uint8_t value = (uint8_t) 256 - _step;

      setAll(value, value, value);

      if ( _step == 1 )
        _scene = NONE;

      break;
    }


    case FADE_OFF: {

      uint8_t value = (uint8_t) _step - 1;

      setAll(value, value, value);

      if ( _step == 1 )
        _scene = NONE;

      break;
    }

    case SEQUENTIAL_FADE_ON: {

      uint8_t value, zone;

      if ( _step > 510 ) {
        value = (uint8_t) 766 - _step;
        zone = 0;
      }
      else if ( _step > 254 ) {
        value = (uint8_t) 510 - _step;
        zone = 1;
      } else {
        value = (uint8_t) 255 - _step;
        zone = 2;
      }

      _setZone(zone, value, value, value);

      if ( _step == 1 )
        _scene = NONE;

      break;
    }

    case SEQUENTIAL_FADE_OFF: {

      uint8_t value, zone;

      if ( _step > 510 ) {
        value = (uint8_t) _step - 510 - 1;
        zone = 0;
      }
      else if ( _step > 255 ) {
        value = (uint8_t) _step - 255 - 1;
        zone = 1;
      } else {
        value = (uint8_t) _step - 1;
        zone = 2;
      }

      _setZone(zone, value, value, value);

      if ( _step == 1 )
        _scene = NONE;

      break;
    }

    default:
      break;
  }

  _step--;

}


void Light::_setIntervalScaler(uint8_t intervalScaler) {
  _intervalScaler = intervalScaler;
  _intervalScalerCounter = _intervalScaler;
}


void Light::_resetInterval() {
  _intervalScaler = 0;
  _intervalScalerCounter = _intervalScaler;
  _scene = NONE;
}


void Light::_setZone(uint8_t zone, int red, int green, int blue) {
  for( int i=_zones[zone].begin; i <= _zones[zone].end ; i++ ) {
    _pixels->setPixelColor(i, _pixels->Color(red, blue, green));
  }
  _pixels->show();
}


void Light::_on() {
  _resetInterval();
  setAll(MAX_BRIGHT, MAX_BRIGHT, MAX_BRIGHT);
}


void Light::_off() {
  _resetInterval();
  setAll(ZERO_BRIGHT, ZERO_BRIGHT, ZERO_BRIGHT);
}


void Light::_sequentialOn() {
  _setIntervalScaler(SCALER_SLOW_SPEED);
  _scene = SEQUENTIAL_ON;
  _step = 3;
}


void Light::_sequentialOff() {
  _setIntervalScaler(SCALER_SLOW_SPEED);
  _scene = SEQUENTIAL_OFF;
  _step = 3;
}


void Light::_fadeOn() {
  _setIntervalScaler(SCALER_MEDIUM_SPEED);
  _scene = FADE_ON;
  _step = 256;
}


void Light::_fadeOff() {
  _setIntervalScaler(SCALER_MEDIUM_SPEED);
  _scene = FADE_OFF;
  _step = 256;
}


void Light::_sequentialFadeOn() {
  _setIntervalScaler(SCALER_FAST_SPEED);
  _scene = SEQUENTIAL_FADE_ON;
  _step = 766; // 255 * 3 + 1
}


void Light::_sequentialFadeOff() {
  _setIntervalScaler(SCALER_FAST_SPEED);
  _scene = SEQUENTIAL_FADE_OFF;
  _step = 766; // 255 * 3 + 1
}


void Light::_decreaseOnTimeSeconds() {

  _onTimeSeconds--;

  if ( ! _onTimeSeconds ) {
    off();
    clearInterval(_autoOffInterval);
    _onTimeSeconds = ON_TIME_SECONDS;
  }

}


void Light::on() {

  _changeTypes[_activeChangeType].on();
  
  // Invoca cada 1 segundo la funcion encargada de controlar el apagado
  // automatico al transcurrir ON_TIME_SECONDS segundos de encendido
  _autoOffInterval = setInterval(_decreaseOnTimeSeconds, 1000);

  _status = ON;

}


void Light::off() {

  if ( _status == OFF )
    return;

  _changeTypes[_activeChangeType].off();

  _activeChangeType++;

  if ( _activeChangeType == arrayLength(_changeTypes) )
    _activeChangeType = 0;

  // Elimina el intervalo establecido para apagado automatico
  clearInterval(_autoOffInterval);

  _status = OFF;

}
