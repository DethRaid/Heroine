 //This code will be attached to robot and all information will be from the robot
#include <SoftwareSerial.h>
 
SoftwareSerial xbee(2, 3); // RX, TX
char GotChar;
 
void setup()  {
   Serial.begin(9600);
   Serial.println( "Arduino started sending bytes via XBee" );
 
   // set the data rate for the SoftwareSerial port
   xbee.begin( 19200 );
}
 
void loop()  
  //Send data out to listening xbees
  if ( Serial.available() ) {
      GotChar = Serial.read();
      xbee.print(GotChar);
  }
// Monitor data from XBee , if the data is available then 
// read it to "GotChar". Then send it to terminal.
  if (xbee.available() ) {
      GotChar = xbee.read();
      Serial.write(GotChar);
  }
}
