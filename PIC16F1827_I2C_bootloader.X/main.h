// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef MAIN_H
#define	MAIN_H

#include <xc.h> // include processor files - each processor file is guarded.  

#define _XTAL_FREQ 4000000

void SYSTEM_Initialize(void);
void OSCILLATOR_Initialize(void);
void WDT_Initialize(void);
void PIN_MANAGER_Initialize (void);

#endif	/* MAIN_H */

