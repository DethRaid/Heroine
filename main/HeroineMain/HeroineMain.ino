/*!\file   main.ino
           The main file for the code which will run on the Arduino
           This code includes functions for controlling the motors, reading data from the sensors, and communicating with the Beablebone Black
   \author David Dubois
   \date   September 9, 2013 - 
   \pre    The left motor is hooked up to pin 2 of the Arduino, the right motor is hooked up to pin 3 of the Arduino
 */

//pin 0 = data in
//pin 1 = data out
//pin 5 = motor 1
//pin 3 = motor 2

//designed for controlling a Sabertooth 2x12, could be adapted for other hardware

#define FORWARD  255
#define BACKWARD 1
#define STOP     126

void getSerialInput();
void getButtonInput();
void motorsForward();
void motorsBackward();
void motorsRight();
void motorsLeft();
void motorsStop();
void sendSerialData( char data );

char serialData;

/*!\brief Initializes the serial communication to a frequency of 9600 Hz and brakes both motors*/
void setup() {
  Serial.begin( 9600 );
  pinMode( 5, OUTPUT );
  pinMode( 3, OUTPUT );
  motorsStop();
}

/*!\brief Looks at the current serial input and determines a motor configuration*/
void loop() {
  getSerialInput();
  getButtonInput();
  
  switch( serialData ) {
    case 'f':
      motorsForward();
      Serial.println( "Moving motors forward" );
      break;
    case 'b':
      motorsBackward();
      Serial.println( "Moving motors backward" );
      break;
    case 'r':
      motorsRight();
      Serial.println( "Turning right" );
      break;
    case 'l':
      motorsLeft();
      Serial.println( "Turning left" );
      break;
    case 's':
      motorsStop();
      Serial.println( "Stopping motors" );
      break;
  }
  serialData = 0;
}

void getSerialInput() {

  if( Serial.available() > 0 ) {
    // read the incoming byte:
    serialData = Serial.read();

    // say what you got:
    Serial.print( "I received: " );
    Serial.println( serialData, DEC );
  }
}

void getButtonInput() {
  if( digitalRead( 13 ) == HIGH ) {
    motorsStop();
  }
}

/*!\brief Sets both motors to full speed forward*/
void motorsForward() {
  analogWrite( 5, FORWARD );
  analogWrite( 3, FORWARD );
}

/*!\brief Sets both motors to full speed backward*/
void motorsBackward() {
  analogWrite( 5, BACKWARD );
  analogWrite( 3, BACKWARD );
}

/*!\brief Sets the right motor to full speed backward and the left motor to full speed forward so that Heroine pivots to the right*/
void motorsRight() {
  analogWrite( 5, FORWARD );
  analogWrite( 3, BACKWARD );
}

/*!\brief Sets the right morot to full speed forward and the left motor to full speed backward so that Heroine pivots to the left*/
void motorsLeft() {
  analogWrite( 5, BACKWARD );
  analogWrite( 3, FORWARD );
}

/*!\brief Sets both motors to no movement*/
void motorsStop() {
  analogWrite( 5, STOP );
  analogWrite( 3, STOP );
}

/*!\brief Sends a single byte of data through the serial interface

\param data The data to send*/
void sendSerialData( char data ) {
  Serial.write( data );
}
