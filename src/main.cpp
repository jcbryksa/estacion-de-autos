/*
 * main.cpp
 * Copyright 2020 - Juan C. Bryksa (jcbryksa@gmail.com)
 * 
 * Estacion de autos de juguete
 * https://youtu.be/DeJ5emJt6Y4
 * 
 */

#include "common.hpp"
#include "display.hpp"
#include "keypad.hpp"
#include "led-indicator.hpp"
#include "light.hpp"
#include "elevator.hpp"

// Light (pin de datos WS2811)
#define LIGHT_DATA_PIN 12

// Elevator engine
#define ELEVATOR_ENGINE_PIN_A 9
#define ELEVATOR_ENGINE_PIN_B 10
#define ELEVATOR_BUZZER_PIN   13

// Keypad
#define FLOOR_1  0
#define FLOOR_2  1
#define FLOOR_3  2
#define LIGHT    3


                     // segmentos ->  a  b  c  d  e  f   g
const uint8_t displayPins[]       = { 7, 8, 4, 3, 2, 99, 5 };
const uint8_t keypadPins[]        = { 17, 16, 14, 15 };
const uint8_t ledIndicatorPins[]  = { 11 };
const uint8_t elevatorFloorPins[] = { 18, 19, 6 };


void keypadHandler(uint8_t n);
void elevatorEnd(uint8_t floor);


void setup()
{
  Light::init(LIGHT_DATA_PIN);
  Display::init(displayPins);
  Keypad::init(keypadPins, arrayLength(keypadPins), keypadHandler);
  LedIndicator::init(ledIndicatorPins, arrayLength(ledIndicatorPins));
  Elevator::init(elevatorFloorPins, arrayLength(elevatorFloorPins), ELEVATOR_ENGINE_PIN_A,
                 ELEVATOR_ENGINE_PIN_B, ELEVATOR_BUZZER_PIN, elevatorEnd);

  uint8_t floor = Elevator::floor();

  /* Si el ascensor se encuentra entre pisos
   * se lo desplaza hacia el primero
   */
  if ( floor != NO_FLOOR )
    Display::show(Elevator::floor() + 1);
  else {
    Display::effect(Display::SHIFT_DOWN);
    Elevator::goTo(FLOOR_1);
  }
  /**/

}


void loop()
{
  // Escanea el estado de los switches
  Keypad::scan();
}


/**
 * Funcion invocada cuando concluye
 * cualqquier recorrido del ascensror
 */
void elevatorEnd(uint8_t floor) {
  Display::clearEffect();
  Display::show(floor + 1);
}


/**
 * Manejo de switches
 * pisos del 1 al 3 y luces
 */
void keypadHandler(uint8_t key) {

  // On/Off luces
  if ( key == LIGHT ) {

    if ( LedIndicator::toggle(0) )
      Light::on();
    else
      Light::off();
  }
  //

  // Obtiene piso actual donde esta el ascensor
  uint8_t floor = Elevator::floor();

  /* Si el switch presionado es el de alguno de
   * los pisos distintos del actual y el
   * ascensor se encuentra listo
   */
  if ( key != LIGHT && Elevator::status() == Elevator::READY && key != floor ) {

    // Ascensor entre pisos
    if ( floor == NO_FLOOR )
      Display::effect(Display::RIGHT_ROTATION);
    else if ( key < floor ) // solicitud de bajada
      Display::effect(Display::SHIFT_DOWN);
    else // solicitud de subida
      Display::effect(Display::SHIFT_UP);

    // Ir al piso key
    Elevator::goTo(key);

  }
  /**/

}
