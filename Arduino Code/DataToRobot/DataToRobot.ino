/******************************************************
  Title:    DataToRobot
  Authors:  Kelsey Benz, Trevor Sands
  Date:     May 9, 2013
  
  Company:  Rochester Institute of Technology
  Course:   EE585 - Robotic Systems
  Advisor:  Dr. F. Sahin
  
  DESCRIPTION: 
  Allows Xbee to send and receive data from
  mobile robot.
  
  KNOWN BUGS:
  
  
  NOTES:
  Based on an example by D. Thiebaut from:
  <http://cs.smith.edu/dftwiki/index.php/Tutorial:_Arduino_and_XBee_Communication>
  
*******************************************************/

/******************************************************
  INCLUDED LIBRARIES
*******************************************************/

#include <SoftwareSerial.h>

/******************************************************
  OBJECT INITIALIZATIONS
*******************************************************/

SoftwareSerial xbee(2, 3); // RX, TX

/******************************************************
  GLOBAL VARIABLE DECLARATIONS
*******************************************************/

char gotChar;

/******************************************************
  MAIN FUNCTIONS (SETUP AND LOOP)
*******************************************************/

void setup()
{
  //Set baud for local serial communication
  Serial.begin(57600);
  Serial.println("Connected to robot via Xbee module");
 
  //Set baud for SoftwareSerial port
  xbee.begin( 19200 );
}
 
void loop() 
{
  //Monitor data from XBee, if the data is available
  //then read it to "gotChar"
  if (Serial.available()) {
      gotChar = Serial.read();
      xbee.print(gotChar);
  }
  
  //Send it back to terminal
  if (xbee.available() ) {
      gotChar = xbee.read();
      Serial.write(gotChar);
  }
}

/*****************************************************/
