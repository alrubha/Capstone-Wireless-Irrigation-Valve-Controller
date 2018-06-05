# Capstone-Wireless-Irrigation-Valve-Controller
2018 ECE senior design project at Oregon State University for Team 16. 
The code is for avr atmega328 microcontroller and it can be used for other avr microcontroller with few modifications. It uses SPI to communicate with NRF24L01+ to establish wireless communications and control electric irrigation valves with DC solonoid. This code is a part of a Capstone project. The project link: https://sites.google.com/a/oregonstate.edu/ece44x201716/ece-senior-design-example-project/welcome
# Microcontroller connections
The hub interfaces with nrf only while RX interfaces with nrf and the other components. 
MCU PB5 -> NRF SCK \n
MCU PB4 -> NRF MISO
MCU PB3 -> NRF MOSI
MCU PB1 -> NRF CSN
MCU PB0 -> NRF CE
MCU PD4 -> Push button 
MCU PD3 -> Valve control
MCU PD2 -> Valve control
MCU PC0 -> 50kOHM and 10kOhm voltage dividor connected to battery


