/*
 * elevator.cpp
 * Copyright 2020 - Juan C. Bryksa (jcbryksa@gmail.com)
 */

#include "elevator.hpp"

const uint8_t * Elevator::_floorPins;
uint8_t Elevator::_floors;
uint8_t Elevator::_enginePinA;
uint8_t Elevator::_enginePinB;
uint8_t Elevator::_buzzerPin;
uint8_t Elevator::_pwmValue;
uint8_t Elevator::_currentFloor;
uint8_t Elevator::_goToFloor;
Elevator::Direction Elevator::_currentMovement;
void (*Elevator::_endCallback)(uint8_t);
Elevator::Status Elevator::_status;


void Elevator::init(const uint8_t *floorPins, const uint8_t floors, const uint8_t enginePinA,
                    const uint8_t enginePinB, const uint8_t buzzerPin, void (*endCallback)(uint8_t) ) {

  _floorPins  = floorPins;
  _floors     = floors;
  _enginePinA = enginePinA;
  _enginePinB = enginePinB;
  _buzzerPin  = buzzerPin;

  _pwmValue = 200;         // no utilizado en la logica actualmente programada
  _goToFloor = NO_FLOOR;   // ningun piso solicitado
  _currentMovement = NONE; // ningun movimiento actual
  _endCallback = NULL;     // ningun callback de finalizacion de recorrido
  _status = READY;         // estado actual = listo

  /* Inicializa cada uno de los pines
   * correspondiente a los switches de
   * final de carrera de cada piso
   */
  for ( int i = 0 ; i < _floors ; i++ )
    pinMode(_floorPins[i], INPUT_PULLUP);

  // Inicializa los pines de salida (motor y buzzer)
  pinMode(_enginePinA, OUTPUT);
  pinMode(_enginePinB, OUTPUT);
  pinMode(_buzzerPin, OUTPUT);

  // Verifica en que piso esta actualmente
  _checkCurrentFloor();

  // Si tiene seteada una funcion de final de recorrido la ejecuta
  if ( endCallback )
    _endCallback = endCallback;

  // Establece el escaneo ciclico
  setInterval(_scan, 1);

}


void Elevator::goTo(uint8_t floor, void (*endCallback)(uint8_t) ) {

  _goToFloor = floor;

  _status = WAITING;

  /* Posterga el estado ready para que el ascensor
   * permanezca inmovil un tiempo y luego
   * avance hacia el piso solicitado
   */
  setTimeout(_setReady, WAIT_TIME);

  // Establece la funcion callback si se paso por argumento
  if ( endCallback )
    _endCallback = endCallback;
  
  // Hace sonar el buzzer
  _beep();

}


Elevator::Status Elevator::status() {
  return _status;
}


uint8_t Elevator::floor() {
  return _currentFloor;
}


void Elevator::_playBuzzer() {
  digitalWrite(_buzzerPin, ! digitalRead(_buzzerPin));
}


void Elevator::_setReady() {
  _status = READY;
}


void Elevator::_beep(uint8_t status) {

  static AsynchLoop::LoopId loopId; // id correspondiente al ciclo "beep"

  if ( status == ON )
    loopId = setInterval(_playBuzzer, BUZZER_PERIOD);
  else {
    clearInterval(loopId);
    digitalWrite(_buzzerPin, LOW);
  }

}


void Elevator::_move(Elevator::Direction direction) {

  _currentMovement = direction;

  // Giro del motor para movimiento ascendente
  if ( direction == UP ) {
    //analogWrite(_enginePinA, _pwmValue);
    digitalWrite(_enginePinA, HIGH);
    digitalWrite(_enginePinB, LOW);
  }
  else if ( direction == DOWN ) { // giro para movimiento descendente
    digitalWrite(_enginePinA, LOW);
    digitalWrite(_enginePinB, HIGH);
    //analogWrite(_enginePinB, _pwmValue);
  }

}


void Elevator::_stop() {
  _currentMovement = NONE;
  digitalWrite(_enginePinA, LOW);
  digitalWrite(_enginePinB, LOW);
  _beep(OFF);
}


void Elevator::_brake() {

  // Establece un movimiento inverso
  if ( _currentMovement == UP )
    _move(DOWN);
  else if ( _currentMovement == DOWN )
    _move(UP);

  // Mantiene ese movimiento inverso durante algunos milisegundos
  setTimeout(_stop, BRAKE_TIME);
}


void Elevator::_checkCurrentFloor() {

  _currentFloor = NO_FLOOR;

  // Escanea el estado de los switches final de carrera de cada piso
  for ( int i = 0 ; i < _floors ; i++ )
    if ( digitalRead(_floorPins[i]) == LOW ) {
      _currentFloor = i;
      break;
    }

}


void Elevator::_scan() {

  // Verifica cual es el piso actual (si es entre pisos determina NO_FLOOR)
  _checkCurrentFloor();

  // Si se ha solicitado ir a algun piso  y ademas 
  // el ascensor se encuetra detenido (movimiento NONE) entre pisos (piso NO_FLOOR)
  if ( _goToFloor != NO_FLOOR && _currentFloor == NO_FLOOR && _currentMovement == NONE ) {
    if ( _goToFloor == 0 ) // si se pide el primer piso
      _currentFloor = 1;
    else if ( _goToFloor == (_floors - 1) ) // si se pide el ultimo
      _currentFloor = 0;
    else {  // ninguno de los anteriores (alguno del medio), manda al primero
      _currentFloor = 1;
      _goToFloor = 0;
    }
  }
  //

  // Si el ascensor se encuentra detenido y se ha solicitado ir a algun piso
  if ( _currentMovement == NONE && _goToFloor != NO_FLOOR  && _status == READY ) {

    // Determina si subir o bajar en funcion del piso actual y el solicitado
    if ( _goToFloor > _currentFloor )
      _move(UP);
    else if ( _goToFloor < _currentFloor )
      _move(DOWN);

    _status = BUSY;
  }
  //

  // Detiene el ascensor cuando este ha llegado al piso solicitado
  if ( _goToFloor == _currentFloor && _currentFloor != NO_FLOOR && _status == BUSY ) {

    // Aplica el freno (movimiento inverso durante unos pocos milisegundos)
    _brake();

    // Resetea: establece que el nuevo piso solicitado es ninguno
    _goToFloor = NO_FLOOR;

    if ( _endCallback )
      _endCallback(_currentFloor);

    // De nuevo vuelve a estar disponible
    _status = READY;
  }
  //

}
