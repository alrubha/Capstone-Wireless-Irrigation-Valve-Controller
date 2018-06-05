//Author: Ali Alrubh
//Date: 12/3/2017
// ADD INPUT VALIDATION
#define  F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "nrf24l01.h"
#include "nrf24l01registers.h"
#include "uart.h"

volatile uint8_t rcv_rdy;
uint8_t addrtx0[NRF24L01_ADDRSIZE] ={0,0,0,0,0};			// address of the chosen valve is saved here for transmission	
char rx_char;
char user_in[8];							// v#/S,(V/D/cycle);  EX: S;  ,  1,V;   ,  1,D;   ,   1,25;
uint8_t status;								//transmission status
uint8_t pipe = 0;							//receiving pipe
int x = 0;									//holds cycle
int i = 0;                                  //dummy for loop variable
// for voltage
//***********************************************************
float vout = 0;
float vin = 0;
uint8_t h = 0;
uint8_t l = 0;
char     array_h[16];  //holds string to send to user
char     array_l[16];  //holds string to send to user
//***********************************************************

//*******************************************************************
//                    Clear User input
//*******************************************************************
void clear_user_in(){
	user_in[0] = '0';
	user_in[1] = '0';
	user_in[2] = '0';
	user_in[3] = '0';
}

//*******************************************************************
//            Convert char to int for cycle and save on x
//*******************************************************************
void cycle(){

	if(user_in[3] == ';'){
		x = user_in[2] - '0';
	}//if
	
	  else if (user_in[3] == '0'){
		x = (user_in[2] -'0')*10;
	}//else if
	
	else {
		user_in[2] -= '0';
		user_in[3] -= '0';
		int p = 1;
		while (p <= user_in[3]){
		p *= 10;
		}//while
		x = user_in[2]*p + user_in[3];
	}//else
}

//*******************************************************************
//                    Convert Voltage
//*******************************************************************
void volt(uint8_t adc_result){
    
    vout = adc_result * 0.004882;                 //convert read result to real voltage, the voltage on PC0, (5/1024) = 0.004882
    vin = vout * (66)/(10);						  //by using voltage division, measure the input resistors are 5600ohm and 1000ohm
    h = vin;                                      //whole
    l = fmod(vin, 1) * 10;                        //fraction
    itoa(h,array_h,10);
    itoa(l,array_l,10);
    
}

//*******************************************************************
//                    Set valve address
//*******************************************************************
void set_addr(char valve){
	
	switch (valve){
		
		case '1':
		//addrtx0[NRF24L01_ADDRSIZE] = {0xE8, 0xE8, 0xF0, 0xF0, 0xE3};
			addrtx0[0] = 0xE8;
			addrtx0[1] = 0xE8;
			addrtx0[2] = 0xF0;
			addrtx0[3] = 0xF0;
			addrtx0[4] = 0xE3;
		break;
		
		case '2':
		//addrtx0[NRF24L01_ADDRSIZE] = {0xE8, 0xE8, 0xF0, 0xF0, 0xE4};
			addrtx0[0] = 0xE8;
			addrtx0[1] = 0xE8;
			addrtx0[2] = 0xF0;
			addrtx0[3] = 0xF0;
			addrtx0[4] = 0xE4;
		break;
		
		default:
		uart_puts("Invalid Input \n");
		
		
	}//end switch
}


int main(){

	//nrf24l01 variables
	uint8_t tx_b[NRF24L01_PAYLOAD];                         //transmit buffer
	uint8_t rx_b[NRF24L01_PAYLOAD];                         //receive buffer

	uart_init();

	//init nrf24l01
	nrf24l01_init();

	sei();
	
	while (1){
				
		if(user_in[0] == 'S'){
			
			for(i=1;i<=2;i++){									//increase the loop condition when add more valves
				set_addr(i+'0');
			tx_b[0] = 'S';
			uart_puts("Asking for valve states");
			uart_puts(" valve ");
			uart_putc(i+'0');
			uart_puts(">>>\n");

			//set transmission address for pipe 0, here you put the address you want to transmit to
			nrf24l01_settxaddr(addrtx0);

			//write buffer and transmit and report the status on status
			status = nrf24l01_write(tx_b);

			if(status == 1)
			uart_puts("successes\n");

			else
			uart_puts("failed\n");

			_delay_ms(1000);

			//change to receiver to get states
			if(nrf24l01_readready(&pipe)) { //if data is ready

				uart_puts("getting state<<<\n");

				//read buffer
				nrf24l01_read(rx_b);
				uart_puts("Valve: ");
				uart_putc(rx_b[0]);
				uart_puts(" is open\n");
			}//inner if
			_delay_ms(10);
			}//for
			clear_user_in();
		}//end if

	  else if(user_in[0] != '0' && user_in[0] != 'S') {     
	
	_delay_ms(10);	
	
	set_addr(user_in[0]);
	
    if(user_in[2] == 'D'){
		tx_b[0] = 'D';					// load transmit buffer with 'S' to be transmitted to RX
		uart_puts("Disabling valve>>>\n");

		//set transmission address for pipe 0, here you put the address you want to transmit to.
		nrf24l01_settxaddr(addrtx0);

		//write buffer and transmit and report the status on status
		status = nrf24l01_write(tx_b);

		if(status == 1)
		uart_puts("successes\n");

		else
		uart_puts("failed\n");

		_delay_ms(1000);
		clear_user_in();
        }//if
    
    else if(user_in[2] == 'V'){
		tx_b[0] = user_in[2];
		uart_puts("Asking for battery voltage>>>\n");
		//set transmission address for pipe 0, here you put the address you want to transmit to
		nrf24l01_settxaddr(addrtx0);
		//write buffer and transmit and report the status on status
		status = nrf24l01_write(tx_b);
		if(status == 1)
		uart_puts("successes\n");

		else
		uart_puts("failed\n");

		_delay_ms(1000);

		// change to receiver to receive voltage
		if(nrf24l01_readready(&pipe)) { //if data is ready
			uart_puts("getting voltage<<<\n");
			//read buffer
			nrf24l01_read(rx_b);
            volt(rx_b[0]);
			uart_puts("Voltage= ");
            uart_puts(array_h);
            uart_putc('.');
            uart_puts(array_l);
			uart_puts(" Volts\n");
		}//if
		_delay_ms(10);
		clear_user_in();
        }//else if
    
    else if(user_in[2] != 'V' && user_in[2] != 'D'){
		
		cycle();
		tx_b[0] = x;
					
        uart_puts("sending cycle time>>>\n");

		//set transmission address for pipe 0, here you put the address you want to transmit to.
		nrf24l01_settxaddr(addrtx0);

		//write buffer and transmit and report the status on status
		status = nrf24l01_write(tx_b);

		if(status == 1)
		uart_puts("successes\n");

		else
		uart_puts("failed\n");

		_delay_ms(1000);
		
		// change to receiver to get status
		if(nrf24l01_readready(&pipe)) { //if data is ready
			uart_puts("getting status<<<\n");
			//read buffer
			nrf24l01_read(rx_b);
			switch (rx_b[0]){
				
				case '1':
				uart_puts("Valve has been opened\n");
				break;
				
				case '2':
				uart_puts("Battery Voltage is low, Valve was not opened\n");
				break;
				
			}//switch
			
		}//if
		_delay_ms(10);
		
		clear_user_in();
        }//else if
		}//end else
	}//WHILE
}//MAIN

ISR(USART_RX_vect){
	static  uint8_t  i;
	rx_char = UDR0;              //get character
	user_in[i++]=rx_char;  //store in array
	//if entire string has arrived, set flag, reset index
	if(rx_char == ';'){
		rcv_rdy=1;
		//user_in[--i]  = (' ');     //clear the count field
		user_in[i+1]  = (' ');
		user_in[i+2]  = (' ');
		i=0;
	}
}
