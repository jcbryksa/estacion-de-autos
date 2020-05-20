#include <Arduino.h>

#include "async-loop.hpp"

#ifndef ASYNC_LOOP_CPP
#define ASYNC_LOOP_CPP

AsynchLoop AsyncLoop;      // preinstatiate

ISR(TIMER1_OVF_vect)       // interrupt service routine that wraps a user defined function supplied by attachInterrupt
{

  AsyncLoop.callAsyncLoops();

}


void AsynchLoop::_init(long microseconds)
{
  if ( ! _initialized ) {

    _initialized = 1;
    TCCR1A = 0;                 // clear control register A
    TCCR1B = _BV(WGM13);        // set mode 8: phase and frequency correct pwm, stop the timer
    setPeriod(microseconds);

    memset(_loops, 0x00, sizeof(_loops));

    TIMSK1 = _BV(TOIE1);  // sets the timer overflow interrupt enable bit

    resume();

  }

}


void AsynchLoop::setPeriod(long microseconds)		// AR modified for atomic access
{

  long cycles = (F_CPU / 2000000) * microseconds;                                // the counter runs backwards after TOP, interrupt is at BOTTOM so divide microseconds by 2
  if(cycles < RESOLUTION)              clockSelectBits = _BV(CS10);              // no prescale, full xtal
  else if((cycles >>= 3) < RESOLUTION) clockSelectBits = _BV(CS11);              // prescale by /8
  else if((cycles >>= 3) < RESOLUTION) clockSelectBits = _BV(CS11) | _BV(CS10);  // prescale by /64
  else if((cycles >>= 2) < RESOLUTION) clockSelectBits = _BV(CS12);              // prescale by /256
  else if((cycles >>= 2) < RESOLUTION) clockSelectBits = _BV(CS12) | _BV(CS10);  // prescale by /1024
  else        cycles = RESOLUTION - 1, clockSelectBits = _BV(CS12) | _BV(CS10);  // request was out of bounds, set as maximum

  oldSREG = SREG;
  cli();							// Disable interrupts for 16 bit register access
  ICR1 = cycles;                                          // ICR1 is TOP in p & f correct pwm mode
  SREG = oldSREG;

  TCCR1B &= ~(_BV(CS10) | _BV(CS11) | _BV(CS12));
  TCCR1B |= clockSelectBits;                                          // reset clock select register, and starts the clock
}


AsynchLoop::LoopId AsynchLoop::attach(void (*isr)(), long milliseconds, AsynchLoop::LoopType loopType)
{
  int id;

  if ( ! _initialized )
    _init();

  for ( id = 0 ; id < MAX_ASYNC_LOOPS ; id++ )
    if (_loops[id].handlerFunction == NULL)
      break;

  _loops[id].loopType = loopType;
  _loops[id].period = milliseconds;
  _loops[id].counter = milliseconds;
  _loops[id].handlerFunction = isr;

  return id;
}


void AsynchLoop::detach(AsynchLoop::LoopId loopId)
{
  //TIMSK1 &= ~_BV(TOIE1);    // clears the timer overflow interrupt enable bit
															// timer continues to count without calling the isr
  memset( (void *) &(_loops[loopId]), 0x00, sizeof(Loop));

}


void AsynchLoop::resume()				// AR suggested
{
  TCCR1B |= clockSelectBits;
}


void AsynchLoop::restart()		// Depricated - Public interface to start at zero - Lex 10/9/2011
{
	start();
}


void AsynchLoop::start()	// AR addition, renamed by Lex to reflect it's actual role
{
  unsigned int tcnt1;

  TIMSK1 &= ~_BV(TOIE1);        // AR added
  GTCCR |= _BV(PSRSYNC);   		// AR added - reset prescaler (NB: shared with all 16 bit timers);

  oldSREG = SREG;				// AR - save status register
  cli();						// AR - Disable interrupts
  TCNT1 = 0;
  SREG = oldSREG;          		// AR - Restore status register
	resume();
  do {	// Nothing -- wait until timer moved on from zero - otherwise get a phantom interrupt
	oldSREG = SREG;
	cli();
	tcnt1 = TCNT1;
	SREG = oldSREG;
  } while (tcnt1==0);

//  TIFR1 = 0xff;              		// AR - Clear interrupt flags
//  TIMSK1 = _BV(TOIE1);              // sets the timer overflow interrupt enable bit
}


void AsynchLoop::stop()
{
  TCCR1B &= ~(_BV(CS10) | _BV(CS11) | _BV(CS12));  // clears all clock selects bits
}


unsigned long AsynchLoop::read()    //returns the value of the timer in microseconds
{									//rember! phase and freq correct mode counts up to then down again
  	unsigned long tmp;				// AR amended to hold more than 65536 (could be nearly double this)
  	unsigned int tcnt1;				// AR added

	oldSREG= SREG;
  	cli();
  	tmp=TCNT1;
	SREG = oldSREG;

	char scale=0;
	switch (clockSelectBits)
	{
	case 1:// no prescalse
		scale=0;
		break;
	case 2:// x8 prescale
		scale=3;
		break;
	case 3:// x64
		scale=6;
		break;
	case 4:// x256
		scale=8;
		break;
	case 5:// x1024
		scale=10;
		break;
	}

	do {	// Nothing -- max delay here is ~1023 cycles.  AR modified
		oldSREG = SREG;
		cli();
		tcnt1 = TCNT1;
		SREG = oldSREG;
	} while (tcnt1==tmp); //if the timer has not ticked yet

	//if we are counting down add the top value to how far we have counted down
	tmp = (  (tcnt1>tmp) ? (tmp) : (long)(ICR1-tcnt1)+(long)ICR1  );		// AR amended to add casts and reuse previous TCNT1
	return ((tmp*1000L)/(F_CPU /1000L))<<scale;
}


// JCB CUSTOM

void AsynchLoop::callAsyncLoops() {

  for ( int id = 0 ; id < MAX_ASYNC_LOOPS ; id++ )
    if ( _loops[id].handlerFunction && _loops[id].counter ) {

      _loops[id].counter--;

      if ( _loops[id].counter == 0 ) {

        _loops[id].handlerFunction();

        if ( _loops[id].loopType == CYCLIC )
          _loops[id].counter = _loops[id].period;
        else
          _loops[id].handlerFunction = NULL; // disponibiliza el espacio

      }

    }

}

//

#endif
