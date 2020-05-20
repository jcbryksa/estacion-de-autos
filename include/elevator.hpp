/*
 * elevator.hpp
 * Copyright 2020 - Juan C. Bryksa (jcbryksa@gmail.com)
 */

#ifndef ELEVATOR_H
#define ELEVATOR_H

#include "common.hpp"

#define NO_FLOOR 255
#define ON       1
#define OFF      0

#define WAIT_TIME     1500
#define BRAKE_TIME    30
#define BUZZER_PERIOD 300

class Elevator {

public:

  // Define el estado del ascensor
  typedef enum {READY, BUSY, WAITING, ERROR} Status;

  static void init(const uint8_t *floorPins, const uint8_t floors, const uint8_t enginePinA,
                   const uint8_t enginePinB, const uint8_t buzzerPin, void (*endCallback)(uint8_t) = NULL );
  
  /**
   * Determina a que piso debe dirigirse el ascensor
   * El segundo argumento (opcional) establece una funcion
   * callback que sera ejecutada cuando finalice el recorrido solicitado
   */
  static void goTo(uint8_t floor, void (*endCallback)(uint8_t) = NULL );

  /**
   * Retorna el estado actual del ascensor
   */
  static Status status(void);

  /**
   * Retorna el piso en el cual se encuentra el ascensor
   */
  static uint8_t floor(void);

private:

  // Define la direccion (o sentido) hacia la cual se desplaza el ascensor
  typedef enum {NONE, UP, DOWN} Direction;

  static const uint8_t *_floorPins;      // pines donde se conectan los finales de carrera de cada piso
  static uint8_t _floors;                // cantidad total de pisos
  static uint8_t _enginePinA;            // pin A del motor
  static uint8_t _enginePinB;            // pin B del motor
  static uint8_t _buzzerPin;             // pin conectado al buzzer (beep)
  static uint8_t _pwmValue;              // No utilizado en la solucion programada
  static uint8_t _currentFloor;          // piso en el cual se encuentra el ascensor actualmente
  static uint8_t _goToFloor;             // piso solicitado (cuando concluye el recorrido toma el valor NO_FLOOR)
  static Direction _currentMovement;     // movimiento actual del ascensor (arriba, abajo o ninguno)
  static void (*_endCallback)(uint8_t);  // funcion callback a invocar cuando finaliza un recorrido
  static Status _status;                 // estado actual del ascensor (listo, ocupado, en espera)

  static void _move(Direction direction);
  static void _stop(void);
  static void _brake(void);
  static void _scan(void);
  static void _checkCurrentFloor(void);
  static void _beep(uint8_t status = ON);
  static void _playBuzzer(void);
  static void _setReady(void);

};


#endif
