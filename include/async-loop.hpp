#ifndef ASYNC_LOOP_H
#define ASYNC_LOOP_H

#include <avr/io.h>
#include <avr/interrupt.h>

#define RESOLUTION         65536    // Timer1 is 16 bit
#define MAX_ASYNC_LOOPS    34

#define setTimeout(callback, millis) AsyncLoop.attach(callback, millis, AsynchLoop::ONE_TIME)
#define setInterval(callback, millis) AsyncLoop.attach(callback, millis, AsynchLoop::CYCLIC)
#define clearInterval(loopId) AsyncLoop.detach(loopId)

class AsynchLoop
{
public:

    typedef uint8_t LoopId;
    typedef enum {CYCLIC, ONE_TIME} LoopType;

    /**
     * Agrega una funcion callback con un intervalo de ejecucion determinado
     */
    LoopId attach(void (*isr)(), long milliseconds, LoopType loopType = CYCLIC);

    /**
     * Elimina el callback correspondiente al id
     * 
     */
    void detach(LoopId);

    /**
     * Recorre el array de callbacks y ejecuta cada uno segun corresponda
     */
    void callAsyncLoops(void);

private:
    
    void _init(long microseconds=1000);
    uint8_t _initialized = 0;
    unsigned char clockSelectBits;
	char oldSREG;					// To hold Status Register while ints disabled
    //void (*_asyncLoops[MAX_ASYNC_LOOPS])(void);

    typedef struct {
        LoopType loopType;
        long long period;
        long long counter;
        void (*handlerFunction)(void);
    } Loop;

    Loop _loops[MAX_ASYNC_LOOPS];

    void start();
    void stop();
    void restart();
	void resume();
	unsigned long read();
    void setPeriod(long microseconds);

};

extern AsynchLoop AsyncLoop;

#endif
