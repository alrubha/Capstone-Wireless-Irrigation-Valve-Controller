# Capstone-Wireless-Irrigation-Valve-Controller
2018 ECE senior design project at Oregon State University for Team 16. 
The code is for avr atmega328 microcontroller and it can be used for other avr microcontroller with few modifications. It uses SPI to communicate with NRF24L01+ to establish wireless communications and control electric irrigation valves with DC solonoid. This code is a part of a Capstone project. The project link: https://sites.google.com/a/oregonstate.edu/ece44x201716/ece-senior-design-example-project/welcome
# Microcontroller Connections
The hub interfaces with nrf only while RX interfaces with nrf and the other components.<br />
MCU PB5 -> NRF SCK<br />
MCU PB4 -> NRF MISO<br />
MCU PB3 -> NRF MOSI<br />
MCU PB1 -> NRF CSN<br />
MCU PB0 -> NRF CE<br />
MCU PD4 -> Push button <br />
MCU PD3 -> Valve control<br />
MCU PD2 -> Valve control<br />
MCU PC0 -> 50kOHM and 10kOhm voltage divider connected to battery terminal
# To Add a New Valve
In Hub main.c -> add a new case on the function "void set_addr(char valve)" on line 84. The case must be the valve controller number. Under the case set the address of the new controller it must be 5 bytes address.<br /> 
In line 131 on main.c for Hub, increase the for loop condition to agree with the number of valves controllers on the system.<br /> 
In RX main.c, on line 162,"tx_b[0] = '#';  //valve send its number", change # to the valve controller number you set on the Hub main.c.<br /> 
In RX nrf24l01.h, change the address on line 77 and line  83 to agree with the address you set on the case statement on Hub main.c.<br /> 
# Availabe Commands
S; -> capital S followed by semicolon, check the valves state if they are connected or open.<br />
#,#; -> valve number followed by a comma followed by another number followed by semicolon, opens the valve number # for # minutes.<br />
#,D; -> valve number followed by a comma followed by a capital D followed by semicolon, disable the valve (close it).<br />
#,V; -> valve number followed by a comma followed by a capital V followed by semicolon, report the battery voltage.<br />
**Using a serial terminal to communicate with the hub is faster than using the gui**
