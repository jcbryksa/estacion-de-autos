/*
 * light.hpp
 * Copyright 2020 - Juan C. Bryksa (jcbryksa@gmail.com)
 */

#ifndef LIGHT_H
#define LIGHT_H

#include "common.hpp"

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif


#define TOTAL_PIXELS          39  // cada uno de los WS2811
#define TOTAL_ZONES           3   // cada uno de los pisos
#define MAX_BRIGHT            255
#define ZERO_BRIGHT           0
#define SCALER_SLOW_SPEED     130
#define SCALER_MEDIUM_SPEED   12
#define SCALER_FAST_SPEED     1
#define ON_TIME_SECONDS       900  // tiempo de espera en segundos para el apagado automatico (15 min)

class Light {

public:

  // Define las posibles escenas o efectos de apagado o encendido
  typedef enum {
    NONE,
    SEQUENTIAL_ON,
    SEQUENTIAL_OFF,
    FADE_ON,
    FADE_OFF,
    SEQUENTIAL_FADE_ON,
    SEQUENTIAL_FADE_OFF
  } Scene;

  // Define los posibles estados
  typedef enum {
    ON,
    OFF
  } Status;

  static void init(const uint8_t dataPin);
  static void setAll(int red, int green, int blue);
  static void on(void);
  static void off(void);

private:

  // Define zonas, que son tramos iniciados por el nodo begin y finalizado por end
  typedef struct { int begin; int end; } Zone;

  // Define un conjunto de escenas de apagado y encendido
  typedef struct { void (*on)(); void (*off)(); } ChangeType;

  static Adafruit_NeoPixel *_pixels;
  static Zone _zones[TOTAL_ZONES];
  static Scene _scene;
  static uint32_t _step;
  static uint8_t _intervalScalerCounter;
  static uint8_t _intervalScaler;
  static ChangeType _changeTypes[];
  static uint8_t _activeChangeType;
  static AsynchLoop::LoopId _autoOffInterval;
  static long _onTimeSeconds;
  static Status _status;

  static void _setIntervalScaler(uint8_t intervalScaler);
  static void _resetInterval(void);
  static void _runInterval(void);
  static void _setZone(uint8_t zone, int red, int green, int blue);
  static void _on(void);
  static void _off(void);
  static void _sequentialOn(void);
  static void _sequentialOff(void);
  static void _fadeOn(void);
  static void _fadeOff(void);
  static void _sequentialFadeOn(void);
  static void _sequentialFadeOff(void);
  static void _decreaseOnTimeSeconds(void);

};


#endif
