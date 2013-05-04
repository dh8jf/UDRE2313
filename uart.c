/*
Title:			UART
Autor:			Frank Jonischkies
Umgebung:		WinAVR-20070122
*/ 

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "uart.h"

// Globale Variablen
static char TxBuff[TX_BUFFER_SIZE];

static volatile uint8_t TxRead;		// Zeigt auf Zeichen, das gesendet werden soll
static uint8_t TxWrite;				// Zeigt auf nächste Schreibposition im Puffer

// Funktionen Öffentlich
void InitUART(void) {
	UBRRL = (F_CPU/(16L*UART_BAUD_RATE))-1;
	UCSRB |= (1<<TXEN);									// UART TX einschalten
}

void UartPutC(char data) {
	while(TxWrite != TxRead);								// Warte bis Platz im Puffer
	TxBuff[TxWrite++] = data;
	if(TxWrite >= TX_BUFFER_SIZE) TxWrite = 0;
	UCSRB |= (1<<UDRIE);									// UDRE Interrupt ein
}

void UartPutS(const char *s) {
	while(*s) UartPutC(*s++);
}

void UartPutP(const char *progmem_s) {
	register char c;
	while((c = pgm_read_byte(progmem_s++))) UartPutC(c);
}

// Interrupt Service Routine
ISR(USART_UDRE_vect) {
	if(TxRead == TxWrite) UCSRB &= ~(1<<UDRIE);			// UDRE Interrupt aus
	else {
		UDR = TxBuff[TxRead++];							// nächtes Zeichen aus Puffer senden
		if(TxRead >= TX_BUFFER_SIZE) TxRead = 0;
	} 							
}
