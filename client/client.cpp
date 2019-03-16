#include <Arduino.h>
#include <Adafruit_ILI9341.h>
#include <SD.h>
#include "consts_and_types.h"
#include "map_drawing.h"
//#include <vector>
//#include "serialport.h"
//#include <bits/stdc++.h>
//#include <boost/algorithm/string.hpp>
// the variables to be shared across the project, they are declared here!
shared_vars shared;

Adafruit_ILI9341 tft = Adafruit_ILI9341(clientpins::tft_cs, clientpins::tft_dc);

//SerialPort port("/dev/ttyACM0");

void setup() {
  // initialize Arduino
  init();

  // initialize zoom pins
  pinMode(clientpins::zoom_in_pin, INPUT_PULLUP);
  pinMode(clientpins::zoom_out_pin, INPUT_PULLUP);

  // initialize joystick pins and calibrate centre reading
  pinMode(clientpins::joy_button_pin, INPUT_PULLUP);
  // x and y are reverse because of how our joystick is oriented
  shared.joy_centre = xy_pos(analogRead(clientpins::joy_y_pin), analogRead(clientpins::joy_x_pin));

  // initialize serial port
  Serial.begin(9600);
  Serial.flush(); // get rid of any leftover bits

  // initially no path is stored
  shared.num_waypoints = 0;

  // initialize display
  shared.tft = &tft;
  shared.tft->begin();
  shared.tft->setRotation(3);
  shared.tft->fillScreen(ILI9341_BLUE); // so we know the map redraws properly

  // initialize SD card
  if (!SD.begin(clientpins::sd_cs)) {
      Serial.println("Initialization has failed. Things to check:");
      Serial.println("* Is a card inserted properly?");
      Serial.println("* Is your wiring correct?");
      Serial.println("* Is the chipSelect pin the one for your shield or module?");

      while (1) {} // nothing to do here, fix the card issue and retry
  }

  // initialize the shared variables, from map_drawing.h
  // doesn't actually draw anything, just initializes values
  initialize_display_values();

  // initial draw of the map, from map_drawing.h
  draw_map();
  draw_cursor();

  Serial.flush();
  // initial status message
  status_message("FROM?");
}

void process_input() {
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
    (analogRead(clientpins::joy_x_pin)-shared.joy_centre.y)/step
  );
  delta.x = -delta.x; // horizontal axis is reversed in our orientation

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

/* The following two functions are identical to the ones provided in the
assignment description. These functions take in the longitude and latitude
(respectively) and return the mapped location onto the screen*/
int16_t  lon_to_x(int32_t  lon, map_box_t mapbox, int16_t width) {
    return  map(lon , mapbox.W , mapbox.E , 0, width);
}


int16_t  lat_to_y(int32_t  lat, map_box_t mapbox, int16_t height) {
    return  map(lat , mapbox.N , mapbox.S , 0, height);
}


void drawWaypoints() {
    for (int i = 0; i < shared.num_waypoints - 1; i++) {
        int x1, x2, y1, y2;
        int8_t num = shared.map_number;
        x1 = lon_to_x(shared.waypoints[i].lat, mapdata::map_box[num], mapdata::map_x_limit[num]);
        x2 = lon_to_x(shared.waypoints[i + 1].lat, mapdata::map_box[num], mapdata::map_x_limit[num]);
        y1 = lat_to_y(shared.waypoints[i].lon, mapdata::map_box[num], mapdata::map_y_limit[num]);
        y2 = lat_to_y(shared.waypoints[i + 1].lon, mapdata::map_box[num], mapdata::map_y_limit[num]);
        shared.tft->drawLine(x1, y1, x2, y2,ILI9341_GREEN);
        draw_map();
        draw_cursor();
    }
}

/** Writes an uint32_t to Serial3, starting from the least-significant
 * and finishing with the most significant byte. 
 */
void ll_to_serial(ll num) {
  Serial.write((char) (num >> 0));
  Serial.write((char) (num >> 8));
  Serial.write((char) (num >> 16));
  Serial.write((char) (num >> 24));
  Serial.write((char) (num >> 32));
  Serial.write((char) (num >> 40));
  Serial.write((char) (num >> 48));
  Serial.write((char) (num >> 56));
}


/** Reads an uint32_t from Serial3, starting from the least-significant
 * and finishing with the most significant byte. 
 */
ll ll_from_serial() {
  ll num = 0;
  num = num | ((ll) Serial.read()) << 0;
  num = num | ((ll) Serial.read()) << 8;
  num = num | ((ll) Serial.read()) << 16;
  num = num | ((ll) Serial.read()) << 24;
  num = num | ((ll) Serial.read()) << 32;
  num = num | ((ll) Serial.read()) << 40;
  num = num | ((ll) Serial.read()) << 48;
  num = num | ((ll) Serial.read()) << 56;
  return num;
}

void sendRequest(lon_lat_32 start, lon_lat_32 end) {
  /*
    port.writeline("R");
    port.writeline(" ");
    port.writeline(start.lat);
    port.writeline(" ");
    port.writeline(start.lon);
    port.writeline(" ");
    port.writeline(end.lat);
    port.writeline(" ");
    port.writeline(end.lon);
    port.writeline("\n");
*/  
    //Serial.flush();
    Serial.print("R ");
    Serial.print(start.lat);
    Serial.print(" ");
    Serial.print(start.lon);
    Serial.print(" ");
    Serial.print(end.lat);
    Serial.print(" ");
    Serial.println(end.lon);
    Serial.flush();
}

void sendAck() {
    // send the A character followed by a newline
    //port.writeline("A\n");
    Serial.print("A\n");
}

bool checkTimeout(bool timeout, int time, int startTime) {
    int endTime;
    while (!Serial.available()) {
        endTime = millis();
        if (endTime-startTime >= time) {
            timeout = true;
            break;
        }
    }
    return timeout;
}

void clientCom(lon_lat_32 start, lon_lat_32 end) {
    status_message("Receiving path...");
    while (true) {
    bool timeout = false;
    // TODO: communicate with the server to get the waypoints

        // send request
        sendRequest(start, end);
        //Serial.println("sent...");
        int startTime = millis();
        // store path length in shared.num_waypoints
        timeout = checkTimeout(timeout, 10, startTime);
        delay(3000);
        if (Serial.available() && !timeout) {
            // string splitting method found from: 
            // geeksforgeeks.org/boostsplit-c-library/
            //char *line = new char[100];
            char letter;
            letter = Serial.read();
            Serial.read();  // read in space
            if (letter == 'N') {
                ll num = ll_from_serial();
                if (num != 0) {
                    shared.num_waypoints = static_cast<int16_t>(num);
                    sendAck();
                } else {
                    status_message("NO PATH");
                    // add a delay of 2-3 seconds...
                    delay(3000);
                    break;  // need to wait for new points
                }
            } else {
                // send request again with the same point
                timeout = true;
            }
            // store waypoints in shared.waypoints[]
            for (int i = 0; i < shared.num_waypoints && !timeout; i++) {
                startTime = millis();
                timeout = checkTimeout(timeout, 1, startTime);
                char letter = Serial.read();
                Serial.read();
                if (letter == 'W') {
                    ll lat = ll_from_serial();
                    Serial.read();
                    ll lon = ll_from_serial();
                    lon_lat_32 Point;
                    Point.lat = static_cast<int32_t>(lat);
                    Point.lon = static_cast<int32_t>(lon);
                    shared.waypoints[i] = Point;
                    sendAck();
                } else {
                    // send request again with the same point
                    timeout = true;
                }
            }
            // Serial.flush();
            startTime = millis();
            timeout = checkTimeout(timeout, 1, startTime);
            if (Serial.available() && !timeout){
                char endChar = Serial.read();
                if (endChar != 'E' && !timeout) {
                    // send request again with the same point
                    timeout = true;
                }
            }
        }
        if (timeout) {
            continue;
        } else {
            drawWaypoints();
            break;
        }
    }
}


int main() {
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
    }
    else if (shared.zoom_out_pushed) {
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

        // wait until the joystick button is no longer pushed
        while (digitalRead(clientpins::joy_button_pin) == LOW) {}
      }
      else {
        // if we were waiting for the end point, record it
        // and then communicate with the server to get the path
        end = get_cursor_lonlat();

        clientCom(start, end);
        // now we have stored the path length in
        // shared.num_waypoints and the waypoints themselves in
        // the shared.waypoints[] array, switch back to asking for the
        // start point of a new request
        curr_mode = WAIT_FOR_START;

        // wait until the joystick button is no longer pushed
        while (digitalRead(clientpins::joy_button_pin) == LOW) {}
      }
    }

    if (shared.redraw_map) {
      // redraw the status message
      if (curr_mode == WAIT_FOR_START) {
        status_message("FROM?");
      }
      else {
        status_message("TO?");
      }

      // redraw the map and cursor
      draw_map();
      draw_cursor();

      // TODO: draw the route if there is one
    }
  }

  Serial.flush();
  return 0;
}