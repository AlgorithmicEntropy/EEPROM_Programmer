/*
 * Shift.h
 *
 * Created: 16.04.2020 14:49:26
 *  Author: Sebastian
 */ 


#ifndef SHIFT_H_
#define SHIFT_H_

//pin definitions
#define lPin 0b00001000 //Pin D3 latch
#define cPin 0b00010000 //Pin D4 clock
#define dPin 0b00100000 //Pin D5 data out

//pin macros
#define Latch_HIGH() PORTD |= lPin
#define Latch_LOW() PORTD &= ~lPin
#define Clock_HIGH() PORTD |= cPin
#define Clock_LOW() PORTD &= ~cPin
#define Data_HIGH() PORTD |= dPin
#define Data_LOW() PORTD &= ~dPin

#define Latch_Pulse() Latch_HIGH(); Latch_LOW();

//bitorder enum
enum BITORDER {MSBFIRST, LSBFIRST};
typedef enum BITORDER bitorder;

//function prototypes
extern void shiftOutByte(uint8_t, uint8_t, bitorder);
extern void shiftOutShort(uint16_t, bitorder);
extern void setupShiftIO(void);

#endif /* SHIFT_H_ */