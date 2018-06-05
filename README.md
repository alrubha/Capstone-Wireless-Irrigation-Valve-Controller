# Capstone-Wireless-Irrigation-Valve-Controller
2018 ECE senior design project at Oregon State University for Team 16. 
The code is for avr atmega328 microcontroller and it can be used for other avr microcontroller with few modifications. It uses SPI to communicate with NRF24L01+ to establish wireless communications and control electric irrigation valves with DC solonoid. This code is a part of a Capstone project. The project link: https://sites.google.com/a/oregonstate.edu/ece44x201716/ece-senior-design-example-project/welcome
# Microcontroller connections
The hub interfaces with nrf only while RX interfaces with nrf and the other components.<br />
MCU PB5 -> NRF SCK<br />
MCU PB4 -> NRF MISO<br />
MCU PB3 -> NRF MOSI<br />
MCU PB1 -> NRF CSN<br />
MCU PB0 -> NRF CE<br />
MCU PD4 -> Push button <br />
MCU PD3 -> Valve control<br />
MCU PD2 -> Valve control<br />
MCU PC0 -> 50kOHM and 10kOhm voltage dividor connected to battery


