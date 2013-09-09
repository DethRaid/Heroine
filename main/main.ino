/*!\file   main.ino
           The main file for the code which will run on the Arduino
           This code includes functions for controlling the motors, reading data from the sensors, and communicating with the Beablebone Black
   \author David Dubois
   \date   September 9, 2013 - 
   \pre    The left motor is hooked up to pin 2 of the Arduino, the right motor is hooked up to pin 3 of the Arduino
 */

//pin 0 = data in
//pin 1 = data out
//pin 2 = motor 1
//pin 3 = motor 2

//designed for controlling a Sabertooth 2x12, could be adapted for other hardware

char serialData;

/*!\brief Initializes the serial communication to a frequency of 9600 Hz and brakes both motors*/
void setup() {
  Serial.begin( 9600 );
  motorsStop();
}

/*!\brief Looks at the current serial input and determines a motor configuration*/
void loop() {
  switch( serialData ) {
    case 'f':
      motorsForward();
    case 'b':
      motorsBackward();
    case 'r':
      motorsRight()
    case 'l':
      motorsLeft();
    case 's':
      motorsStop();
    default:
      serialData = 0;
  }
}

/*!\brief Sets both motors to full speed forward*/
void motorsForward() {
  analogWrite( 1, 255 );
  analogWrite( 2, 255 );
}

/*!\brief Sets both motors to full speed backward*/
void motorsBackward() {
  analogWrite( 1, 127 );
  analogWrite( 2, 127 );
}

/*!\brief Sets the right motor to full speed backward and the left motor to full speed forward so that Heroine pivots to the right*/
void motorsRight() {
  analogWrite( 1, 255 );
  analogWrite( 2, 127 );
}

/*!\brief Sets the right morot to full speed forward and the left motor to full speed backward so that Heroine pivots to the left*/
void motorsLeft() {
  analogWrite( 1, 127 );
  analogWrite( 2, 255 );
}

/*!\brief Sets both motors to no movement*/
void motorsStop() {
  analogWrite( 1, 128 );
  analogWrite( 2, 128 );
}

/*!\brief Called whenever a serial message is recieved by the Arduino*/
void serialEvent() {
  serialData = Serial.read();
}

/*!\brief Sends a single byte of data through the serial interface

\param data The data to send*/
void sendSerialData( char data ) {
  Serial.write( data );
}
