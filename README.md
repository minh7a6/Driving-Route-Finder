# Driving Route Finder
## Summary
A route finder where the user can indicate a start and end point anywhere in the city of Edmonton. It calculates the shortest route possible using the [Dijkstra algorithm](https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm "Dijkstra algorithm") and displays it on the Arduino.
## Running Instructions
### Hardware Components:
- Arduino Mega 2560 Board
- Banana plug wires
- Joystick Component
- 2 button components
- 2 brown band resistors
- Adafruit TFT display
### Wiring Instructions
| Display Pins | Arduino Pins |
| ------------ | ------------ |
| 1 GND | BB GND bus|
| 2 Vin | BB Vcc (positive) bus |
| 4 CLK | Pin 52 |
| 5 MISO | Pin 50 |
| 6 MOSI | Pin 51 |
| 7 CS | Pin 10 |
| 8 D/C | Pin 9|
| 19 CCS | Pin 6 |

| Button Pins | Arduino Pins |
| ------------ | ------------ |
| Zoom in button | Pin 2 |
| Zoom out button | Pin 3 |

| Joystick Pins | Arduino Pins |
| ------------ | ------------ |
| 5V | Vcc |
| GND | GND |
| VRX | Analog 1 |
| VRY | Analog 0 |
| SW | Pin 8 |


In order to correctly run the program, you must first navigate to the source directory with the project files. Navigate to the **client** subdirectory, then use the command:
```bash
make upload
```
After running the command above, navigate to the **server** subdirectory and use the command:
```bash
make && ./server
```
The server will be brought online and the program will be running.
## How to Use
Once the program is uplaoded to the Arduino and the server is brought online, the following user actions are permitted:
- The user can use the *joystick* to move the cursor on the screen, and also press the zoom in/out buttons to scale the map.
- The user will be prompted with the text "**FROM?**" at the bottom of the screen, indicating that it is waiting for the starting point.
- To *select* a point, **click the joystick**.
- The user will then be prompted with the text "**TO?**", indicating that it is waiting for the end point.
- Once both points are received, the shortest route possible will be displayed on the screen.

## Miscellaneous
### Makefile (server) Targets
- make (**server**) : builds the project and generates the server executable
- make **serialport.o** : compiles serialport.cpp into its respective object (.o) file
- make **digraph.o** : compiles digraph.cpp into its respective object (.o) file
- make **dijkstra.o** : compiles dijsktra.cpp into its respective object (.o) file
- make **server.o** : compiles server.cpp into its respective object (.o) file
- make **clean** : removes the .o and executable files from the source directory
- make **help** : prints out further information about the makefile targets available to use
