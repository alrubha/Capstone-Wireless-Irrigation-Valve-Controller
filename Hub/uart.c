#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <string.h>
#include "uart.h"
#define  F_CPU 16000000UL

#define USART_BAUDRATE 9600
#define BAUDVALUE  ((F_CPU/(USART_BAUDRATE * 16UL)) - 1 )

//***********************************************************************/
//*                                uart_init
//* Initialize uart for receiving from PC
//* RXD is PORT D bit 0
//* TXD is PORT D bit 1
//***********************************************************************/
void uart_init(){

    //tx and rx enabled, receive interrupt disabled
    //UCSR0B |= (1<<RXEN0) | (1<<TXEN0);
    UCSR0B |= (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0); //INTERRUPTS ENABLED

    //8-N-1
    UCSR0C |= (1<<UCSZ01) | (1<<UCSZ00);
    UBRR0H = (BAUDVALUE >>8 );      //load upper byte of the baud rate into UBRR
    UBRR0L =  BAUDVALUE;            //load lower byte of the baud rate into UBRR

}

//***********************************************************************/
//*                              uart_putc
//* send a character using uart
//***********************************************************************/
void uart_putc(char data){

    while (!(UCSR0A&(1<<UDRE0)));    // Wait for previous transmissions
    UDR0 = data;                     // Send data byte
    while (!(UCSR0A&(1<<UDRE0)));    // Wait for previous transmissions

}

//***********************************************************************/
//*                              uart_puts
//* send a string using uart
//***********************************************************************/

void uart_puts(char *str){

    int i = 0;
    // Loop through string, sending each character
    while(str[i] != '\0') {
        uart_putc(str[i]);
        i++;
    }
}

//***********************************************************************/
//*                                uart_getc
//* get a character from uart
//***********************************************************************/
char uart_getc(){
	uint16_t timer = 0;
    while (!(UCSR0A & (1<<RXC0))){
		timer++;
		if(timer >= 16000){
			return 0;
		}
		} // Wait for byte to arrive
		return UDR0;
}
