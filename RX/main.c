//*****5/4/2018*****
//Author: Ali Alrubh
#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "uart_functions_m48.h"
#include "nrf24l01.h"
#include "nrf24l01registers.h"
#define ON PORTD = 0x24
#define OFF PORTD = 0x28
#define  DOWN PORTD = 0x00
uint8_t status;                 //transmission status
uint8_t pipe = 0;               //receiving pipe
uint8_t sec = 0;                //seconds
uint8_t timee = 0;              //cycle time held here
uint8_t i = 0;                  //loop variable for adc read
uint8_t vs = 0;                 //determine the valve status. 0 -> closed, 1 -> open
uint8_t vth = 0;                //determine if in threshold voltage zone
uint16_t adc_result = 0;        //holds adc result
char cycle[2];                  //cycle time is saved here

//*******************************************************//
uint8_t debug = 1; // 1 enable usart for debugging, 0 disable usart
//*******************************************************//

//********************************************************//
//                TCNT2 to count time
//********************************************************//
void tcnt2_init(){
	TCCR2B |= (1 << CS22) |(1 << CS21);					    // normal mode, 256 pre-scaling
	TIMSK2 |= (1 << TOIE2);									// interrupt on overflow
}//tcnt2_init

//********************************************************//
//                Timer to shut down the valve
//********************************************************//
void timer(){
    if(sec == timee*60){
        disable();
    }//if
}//timer

//********************************************************//
//                ISR for TCNT2
//********************************************************//
// frequency = (62500) / 2^8 * 256  ~= 0.95
ISR(TIMER2_OVF_vect){
	static uint8_t count = 0;
	count++;
	if((count%256) == 0){
		sec++;
    }//if
    timer();
}//ISR


//********************************************************//
//                ADC Init
//********************************************************//
void adc_init(){
    ADMUX |= (1 << REFS0);
    ADCSRA |= (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0); //ADC enabled, don't start yet, single shot mode
}

//********************************************************//
//                ADC Read
//********************************************************//
void adc_read(){
    for(i = 0; i < 11; i++){                                    //Take 10 ADC readings
    ADCSRA |= (1 << ADSC);                                      //poke ADSC and start conversion
    while(bit_is_clear(ADCSRA,ADIF)){}                          //spin while interrupt flag not set
    ADCSRA |= (1 << ADIF);                                      //its done, clear flag by writing a one
    adc_result += ADC;                                          //read the ADC output as 16 bits
        _delay_us(10);
    }//loop
    adc_result = adc_result/10;                                 //Average readings
}


//********************************************************//
//                Debounce Switch
//********************************************************//
uint8_t debounce_switch(){
    static uint16_t state = 0;
    state = (state << 1) | (!bit_is_clear(PIND,4)) | 0xE000;
    if(state == 0xF000){
        return 1;
    }//if
    return 0;
}

//********************************************************//
//                Shut off Valve
//********************************************************//
void disable(){
    OFF;
    _delay_ms(2000);
    DOWN;
    timee = 0;
    vs = 0;
}

//********************************************************//
//                Main function
//********************************************************//
int main(){
    
    DDRC |= 0x00;                //PORTC pin 0 as input
    PORTC |= 0x00;               //PORTC pull down
    
    DDRD |= 0x2C;                //PORTD pin 2,3 as output.
    PORTD |= 0x00;               //PORTD pull down
    
	tcnt2_init();
    adc_init();
    nrf24l01_init();
	sei();
    

    //nrf24l01 variables
    uint8_t tx_b[NRF24L01_PAYLOAD];     //transmit buffer, data transmitted here
    uint8_t rx_b[NRF24L01_PAYLOAD];     //receive buffer , data received here
    
    //the address that will be transmitted to. (Address of the HUB)
    uint8_t addrtx0[NRF24L01_ADDRSIZE] = {0xE8, 0xE8, 0xF0, 0xF0, 0xE2};

    while (1) {
        
        //read adc every time the loop is repeated
        adc_read();
        
        //manual valve shut off
        if(debounce_switch()){
            disable();
        }//if
        
        //voltage closure (Change to 3.3V, lower bound of Vth)
        if(adc_result < 128){
            if(vs == 1){
                disable();
            }//inner if
            vth = 1;
        }//outer if
        
        //get out of threshold voltage (Change to upper bound of Vth)
        if(adc_result >= 138){                //voltage closure
            vth = 0;
        }//if

        //Receive Routine:
        if(nrf24l01_readready(&pipe)){ //if data is ready
        //read buffer
        nrf24l01_read(rx_b);
            
        //Valve State sub_routine
            if(rx_b[0] == 'S'){
                if(vs == 1){
                    tx_b[0] = '2';  //valve send its number
                    nrf24l01_settxaddr(addrtx0);
                    status = nrf24l01_write(tx_b);
                    _delay_ms(1000);
                }//inner if
            }//if
        
        //Voltage sub_routine
            else if(rx_b[0] == 'V'){
                tx_b[0] = adc_result;
                nrf24l01_settxaddr(addrtx0);
                status = nrf24l01_write(tx_b);
                _delay_ms(1000);
            }//else if
            
        //Disable Valve sub_routine
            else if(rx_b[0] == 'D'){
                disable();
            }//else if
            
        //Cycle sub_routine
            else if(rx_b[0] != 'S' && rx_b[0] != 'V' && rx_b[0] != 'D'){
                
                //out of threshold
                if(vth == 0){
                    if(adc_result > 128){   //(change to new lower bound)
                        ON;
                        //send the status to hub, valve was opened
                        tx_b[0] = '1';
                        nrf24l01_settxaddr(addrtx0);
                        status = nrf24l01_write(tx_b);
                        _delay_ms(2000);
                        vs = 1;
                    }//if
                    else{
                        //send the status to hub, valve was not opened
                        tx_b[0] = '2';
                        nrf24l01_settxaddr(addrtx0);
                        status = nrf24l01_write(tx_b);
                        _delay_ms(1000);
                    }//else if
                }//outter if
                
                //in threshold
                else if(vth == 1){
                    if(adc_result > 138){   //(change to upper bound)
                        ON;
                        //send the status to hub, valve was opened
                        tx_b[0] = '1';
                        nrf24l01_settxaddr(addrtx0);
                        status = nrf24l01_write(tx_b);
                        _delay_ms(2000);
                        vs = 1;
                    }//if
                    else{
                        //send the status to hub, valve was not opened
                        tx_b[0] = '2';
                        nrf24l01_settxaddr(addrtx0);
                        status = nrf24l01_write(tx_b);
                        _delay_ms(1000);
                    }//else if
                }//outer if
                
                sec = 0;
                DOWN;
                
                //stop the adc reading for 2s to stop closing the valve
                ADCSRA |= (0 << ADSC);
                _delay_ms(2000);
                timee = rx_b[0];
            }//else if
            
        }//if
        
        _delay_ms(10);
        
    }//while
}//main

