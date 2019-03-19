/*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Assignment 2 Part 2: Driving Route Finder
Names: Rutvik Patel, Kaden Dreger
ID: 1530012, 1528632
CCID: rutvik, kaden
CMPUT 275 Winter 2018

This program demonstrates a client-side program on the Arduino. It handles all
screen and joystick movement. It also handles the GUI for this assignment. It
also communicates with the server side and draws the route between the points.
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*/
#include <Arduino.h>
#include <Adafruit_ILI9341.h>
#include <SD.h>
#include "consts_and_types.h"
#include "map_drawing.h"

shared_vars shared;

Adafruit_ILI9341 tft = Adafruit_ILI9341(clientpins::tft_cs, clientpins::tft_dc);

bool draw = false;  // bool to see if we have to draw route or not


void setup() {
  // initialize Arduino
  init();

  // initialize zoom pins
  pinMode(clientpins::zoom_in_pin, INPUT_PULLUP);
  pinMode(clientpins::zoom_out_pin, INPUT_PULLUP);

  // initialize joystick pins and calibrate centre reading
  pinMode(clientpins::joy_button_pin, INPUT_PULLUP);
  // x and y are reverse because of how our joystick is oriented
  shared.joy_centre = xy_pos(analogRead(clientpins::joy_y_pin),
                             analogRead(clientpins::joy_x_pin));

  // initialize serial port
  Serial.begin(9600);
  Serial.flush();  // get rid of any leftover bits

  // initially no path is stored
  shared.num_waypoints = 0;

  // initialize display
  shared.tft = &tft;
  shared.tft->begin();
  shared.tft->setRotation(3);
  shared.tft->fillScreen(ILI9341_BLUE);  // so we know the map redraws properly

  // initialize SD card
  if (!SD.begin(clientpins::sd_cs)) {
      Serial.println("Initialization has failed. Things to check:");
      Serial.println("* Is a card inserted properly?");
      Serial.println("* Is your wiring correct?");
      Serial.println("* Is the chipSelect pin the one for your shield or module?");
      while (1) {}  // nothing to do here, fix the card issue and retry
  }

  // initialize the shared variables, from map_drawing.h
  // doesn't actually draw anything, just initializes values
  initialize_display_values();

  // initial draw of the map, from map_drawing.h
  draw_map();
  draw_cursor();

  Serial.flush();  // flushing the Serial to get rid of leftover bits
  // initial status message
  status_message("FROM?");
}

void process_input() {
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
The process_input function takes no parameters.

It returns no parameters:

The point of this function is to process the input from the joystick and the
buttons.
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
  // read the zoom in and out buttons
  shared.zoom_in_pushed = (digitalRead(clientpins::zoom_in_pin) == HIGH);
  shared.zoom_out_pushed = (digitalRead(clientpins::zoom_out_pin) == HIGH);

  // read the joystick button
  shared.joy_button_pushed = (digitalRead(clientpins::joy_button_pin) == LOW);

  // joystick speed, higher is faster
  const int16_t step = 64;

  // get the joystick movement, dividing by step discretizes it
  // currently a far joystick push will move the cursor about 5 pixels
  xy_pos delta(
    // the funny x/y swap is because of our joystick orientation
    (analogRead(clientpins::joy_y_pin)-shared.joy_centre.x)/step,
    (analogRead(clientpins::joy_x_pin)-shared.joy_centre.y)/step);
  delta.x = -delta.x;  // horizontal axis is reversed in our orientation

  // check if there was enough movement to move the cursor
  if (delta.x != 0 || delta.y != 0) {
    // if we are here, there was noticeable movement

    // the next three functions are in map_drawing.h
    erase_cursor();       // erase the current cursor
    move_cursor(delta);   // move the cursor, and the map view if the edge was nudged
    if (shared.redraw_map == 0) {
      // it looks funny if we redraw the cursor before the map scrolls
      draw_cursor();      // draw the new cursor position
    }
  }
}


void drawWaypoints() {
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
The drawWaypoints function takes no parameters.

It returns no parameters.

The point of this function is to draw the given waypoints on the arduino
according to their coordinates.
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    // for the number of waypoints...
    for (int i = 0; i < shared.num_waypoints - 1; i++) {
        int8_t num = shared.map_number;  // current zoom elevel

        /*Get two points*/
        lon_lat_32 p1 = shared.waypoints[i];
        lon_lat_32 p2 = shared.waypoints[i + 1];

        /*Configure the points*/
        xy_pos p1_loc = xy_pos(longitude_to_x(num, p1.lon),
          latitude_to_y(num, p1.lat));
        p1_loc.x -= shared.map_coords.x;
        p1_loc.y -= shared.map_coords.y;

        xy_pos p2_loc = xy_pos(longitude_to_x(num, p2.lon),
          latitude_to_y(num, p2.lat));
        p2_loc.x -= shared.map_coords.x;
        p2_loc.y -= shared.map_coords.y;


        // draw the line from one point to the other
        shared.tft->drawLine(p1_loc.x, p1_loc.y, p2_loc.x,
                             p2_loc.y, ILI9341_BLUE);
    }
}


/** Reads an uint32_t from Serial3, starting from the least-significant
 * and finishing with the most significant byte. 
 */
String read_num() {
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
The read_num function takes no parameters.

It returns the parameters:
    num: the num read in from the serial monitor

The point of this function is to read a number from the serial monitor and
return it once a newline character is reached.
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
  String num = "";
  char byte;

  while (true) {
    if (Serial.available()) {
        byte = Serial.read();  // take in a byte
        if (byte != '\n') {  // if newline is taken in, break
            num += byte;
        } else {
            return num;  // return the buffer
        }
    }
  }
}


String readWaypoint(uint32_t timeout) {
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
The readWaypoint function takes the parameters:
    timeout: the time required for a timeout

It returns the parameters:
    word: the waypoint read in from serial mon

The point of this function is to read in the waypoint and determine if a timeout
occured.
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    String word = "";
    char byte;
    uint32_t start = millis();  // start timer

    while (true) {
        if (Serial.available()) {
            byte = Serial.read();  // read in a byte
            if (byte != '\n' && byte != ' ') {
                word += byte;
            } else {
                return word;
            }
        } else if (millis() - start > timeout) {  // check timeout
            return "";
        }
    }
}
void sendRequest(lon_lat_32 start, lon_lat_32 end) {
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
The sendRequest function takes the parameters:
    start: the starting point
    end: the ending point

It returns no parameters.

The point of this function is to send the starting and ending points from the
Arduino to the server.
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    Serial.flush();
    Serial.print("R ");
    Serial.print(start.lat);
    Serial.print(" ");
    Serial.print(start.lon);
    Serial.print(" ");
    Serial.print(end.lat);
    Serial.print(" ");
    Serial.print(end.lon);
    Serial.println();
    Serial.flush();
}

void sendAck() {
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
The sendAck function takes no parameters.

It returns no parameters.

The point of this function is to send an acknowledgement to the server.
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    // send the A character followed by a newline
    Serial.println("A");
}

bool checkTimeout(bool timeout, uint32_t time, uint32_t startTime) {
    uint32_t endTime;
    while (!Serial.available()) {
        endTime = millis();
        if ((endTime-startTime) >= time) {  // check the timeout
            timeout = true;
            break;
        }
    }
    return timeout;
}

void clientCom(lon_lat_32 start, lon_lat_32 end) {
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
The clientCom function takes the parameters.
    start: the starting point
    end: the end point

It returns no parameters.

The point of this function is to process the communication between the Arduino
and the server. It reads in the waypoints and stores them in shared.waypoints.
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    status_message("Receiving path...");  // indicate to user that we are
                                          // calculating path
    while (true) {
        draw = false;
        bool timeout = false;
        uint32_t startTime = millis();
        while (!timeout && !Serial.available()) {
            sendRequest(start, end);  // send the request
            timeout = checkTimeout(timeout, 10000, startTime);
        }
        // store path length in shared.num_waypoints
        if (Serial.available() && !timeout) {
            // string splitting method found from:
            // geeksforgeeks.org/boostsplit-c-library/
            char letter;
            letter = Serial.read();  // read in the letter
            Serial.read();  // read in space
            if (letter == 'N') {
                // reading in the number of waypoints
                String num = read_num();
                // default num of points
                shared.num_waypoints = 0;
                if (num.toInt() > 0) {
                    shared.num_waypoints = num.toInt();
                    sendAck();  // send ack
                } else {
                    status_message("NO PATH");
                    // delay of 2-3 seconds...
                    delay(3000);
                    status_message("FROM?");
                    break;  // need to wait for new points
                }
            } else {
                // send request again with the same point
                timeout = true;
            }

            for (int i = 0; (i < shared.num_waypoints) && !timeout; i++) {
                startTime = millis();
                String letter = readWaypoint(1000);
                if (letter == "") {
                  timeout = true;  // there was a timeout
                }
                if (letter == "W") {
                    lon_lat_32 point;
                    point.lat = (int32_t)readWaypoint(1000).toInt();
                    point.lon = (int32_t)readWaypoint(1000).toInt();
                    shared.waypoints[i] = point;  // save point to shared
                    timeout = false;
                    sendAck();  // send ack
                } else {
                    timeout = true;  // there was a timeout
                }
            }
            startTime = millis();
            timeout = checkTimeout(timeout, 1000, startTime);

            if (Serial.available() && !timeout) {
                char endChar = Serial.read();
                if (endChar != 'E') {
                    // send request again with the same point
                    timeout = true;
                }
            }
        }
        if (timeout) {
            Serial.flush();
            continue;  // retry request
        } else {
            draw = true;
            break;
        }
    }
    status_message("FROM?");
}


int main() {
  // This is the main function of the program.
  setup();

  // very simple finite state machine:
  // which endpoint are we waiting for?
  enum {WAIT_FOR_START, WAIT_FOR_STOP} curr_mode = WAIT_FOR_START;

  // the two points that are clicked
  lon_lat_32 start, end;

  while (true) {
    // clear entries for new state
    shared.zoom_in_pushed = 0;
    shared.zoom_out_pushed = 0;
    shared.joy_button_pushed = 0;
    shared.redraw_map = 0;

    // reads the three buttons and joystick movement
    // updates the cursor view, map display, and sets the
    // shared.redraw_map flag to 1 if we have to redraw the whole map
    // NOTE: this only updates the internal values representing
    // the cursor and map view, the redrawing occurs at the end of this loop
    process_input();

    // if a zoom button was pushed, update the map and cursor view values
    // for that button push (still need to redraw at the end of this loop)
    // function zoom_map() is from map_drawing.h
    if (shared.zoom_in_pushed) {
      zoom_map(1);
      shared.redraw_map = 1;
    } else if (shared.zoom_out_pushed) {
      zoom_map(-1);
      shared.redraw_map = 1;
    }

    // if the joystick button was clicked
    if (shared.joy_button_pushed) {
      if (curr_mode == WAIT_FOR_START) {
        // if we were waiting for the start point, record it
        // and indicate we are waiting for the end point
        start = get_cursor_lonlat();
        curr_mode = WAIT_FOR_STOP;
        status_message("TO?");

        delay(500);  // handling button bounce
        // wait until the joystick button is no longer pushed
        while (digitalRead(clientpins::joy_button_pin) == LOW) {}
      } else {
        // if we were waiting for the end point, record it
        // and then communicate with the server to get the path
        end = get_cursor_lonlat();

        clientCom(start, end);  // running client communication

        if (draw) {
          status_message("drawing...");
          drawWaypoints();
        }
        status_message("FROM?");
        // now we have stored the path length in
        // shared.num_waypoints and the waypoints themselves in
        // the shared.waypoints[] array, switch back to asking for the
        // start point of a new request
        curr_mode = WAIT_FOR_START;

        delay(500);  // handling button bounce
        // wait until the joystick button is no longer pushed
        while (digitalRead(clientpins::joy_button_pin) == LOW) {}
      }
    }

    if (shared.redraw_map) {
      // redraw the correct status message
      if (curr_mode == WAIT_FOR_START) {
        status_message("FROM?");
      } else {
        status_message("TO?");
      }

      // redraw the map and cursor
      draw_map();
      draw_cursor();
      if (draw) {
        drawWaypoints();
        if (curr_mode == WAIT_FOR_START) {
            status_message("FROM?");
        } else if (curr_mode == WAIT_FOR_STOP) {
            status_message("TO?");
        }
      }
    }
  }

  Serial.flush();
  return 0;
}
