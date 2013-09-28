z2/*!\file   main.ino
           The main file for the code which will run on the Arduino
           This code includes functions for controlling the motors, reading data from the sensors, and communicating with the Beablebone Black
   \author David Dubois
   \date   September 9, 2013 - 
   \pre    The left motor is hooked up to pin 2 of the Arduino, the right motor is hooked up to pin 3 of the Arduino
 */

//pin 0 = data in
//pin 1 = data out
//pin 2 = momentary elevator switches
//pin 3 = motor 2
//pin 4 = power elevator switches
//pin 5 = motor 1
//pin 6 = elevator motor

//designed for controlling a Sabertooth 2x12, could be adapted for other hardware

#define FORWARD  255
#define BACKWARD 1
#define STOP     126

#define MOMENTARY_SWITCHES       2
#define LEFT_MOTOR               5
#define ELEVATOR_POWER_SWITCHES  4
#define RIGHT_MOTOR              3
#define ELEVATOR_MOTOR           6

void getSerialInput();
void getButtonInput();
void evaluateElevatorSwitches();
void motorsForward();
void motorsBackward();
void motorsRight();
void motorsLeft();
void motorsStop();
void sendSerialData( char data );

char serialData;
/*!\brief Holds the current and previous state of the inputs from the elevator switches

bit 0 is the current state of the input from the momentary switches
bit 1 is the last state of the input from the momentary switches
bit 2 is high if the momentary switches have just gone high, low otherwise
bit 3 is the current state of the input from the motor start switches
bit 4 is the last state of the input from the motor start switches
bit 5 is high if the motor start switches have just gone high, low otherwise*/
char elevarotInputs;

/*!\brief Initializes the Arduino state.

 Initializes serial communication to a frequency of 9600 Hz and brakes both motors
 Sets pins 2, and 4 to be input pins while 3, 5, and 6 are set to be output pins
 Stops all motors
 Sets the input from the elevator switches to 0
 */
void setup() {
  Serial.begin( 9600 );
  pinMode( 5, OUTPUT );
  pinMode( 3, OUTPUT );
  pinMode( MOMENTARY_SWITCHES, INPUT );
  pinMode( ELEVATOR_POWER_SWITCHES, INPUT );
  pinMode( ELEVATOR_MOTOR, OUTPUT );
  motorsStop();
  digitaalWrite( ELEVATOR_MOTOR, LOW );
  elevatorInputs = 0;
}

/*!\brief Looks at the current serial input and determines a motor configuration, then clears serial input buffer

 Possible serial values and corresponding motor configuration:
   serial data      right motor    left motor    Heroine movement
     f                forward        forward      forward
     b                backward       backward     backward
     r                backward       forward      pivot right
     l                forward        backward     pivot left
     s                stop           stop         stop
 */
void loop() {
  getSerialInput();
  getButtonInput();
  evaluateElevatorSwitches();
  
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

/*!\brief Checks if serial data is available. If so, the data is written to the variable 'serialData'*/
void getSerialInput() {
  if( Serial.available() > 0 ) {
    // read the incoming byte:
    serialData = Serial.read();
  }
}

/*!\brief Currently allows for a killswitch, will be updated to grab input from any sensor*/
void getButtonInput() {
  if( digitalRead( 13 ) == HIGH ) {
    motorsStop();
  }
}

/*!\brief Performs necessary calculations to move the drink elevator. See full description

 First, all the data in elevatorData is shifted one bit higher. This preserves the state of the inputs from last frame
 Next, the pin assigned to MOMENTARY_SWITCHES is checked. If it is high, bit 0 of elevatorData is set high. Else, bit 0 is set low
 Then, the pin assigned to ELEVATOR_POWER_SWITCHES is checked. If it is high, bit 3 of elevatorData is set high. Else, bit 3 is set low
 Once that is done, the Arduino checks if either input has gone high in the last frame
   This is done for the momentary switches by checking if bit 4 is low and bit 3 is high
   If so, the pin assigned to ELEVATOR_MOTOR is set to low so that the motor may move.
 A similiar thing is done for bits 0 and 1, save that ELEVATOR_MOTOR is set to high
 */
 void evaluateElevatorButtons() {
  elevatorInputs = elevatorInputs << 1;  
  if( digitalRead( MOMENTARY_SWITCHES ) == HIGH ) {
    elevatorInputs |= 1;
  } else {
    elevatorInputs &= ~1;
  }
  if( digitalRead( ELEVATOR_POWER_SWITCHES ) == HIGH ) {
    elevatorInputs |= (1 << 3);
  } else {
    elevatorInputs = elevatorInputs & ~(1 << 3);
  }
  if( (elevatorInputs & 2) == 0 && (elevatorInput & 1) == 1 ) {  //if bit 1 is high and bit 0 is low
    digitalWrite( ELEVATOR_MOTOR, HIGH );
  }
  if( (elevatorInputs & 16) == 0 && (elevatorInput & 8) == 8 ) {  //if bit 4 is high and bit 3 is low
    digitalWrite( ELEVATOR_MOTOR, LOW );
  }
}

/*!\brief Sets both motors to full speed forward*/
void motorsForward() {
  analogWrite( LEFT_MOTOR, FORWARD );
  analogWrite( RIGHT_MOTOR, FORWARD );
}

/*!\brief Sets both motors to full speed backward*/
void motorsBackward() {
  analogWrite( LEFT_MOTOR, BACKWARD );
  analogWrite( RIGHT_MOTOR, BACKWARD );
}

/*!\brief Sets the right motor to full speed backward and the left motor to full speed forward so that Heroine pivots to the right*/
void motorsRight() {
  analogWrite( LEFT_MOTOR, FORWARD );
  analogWrite( RIGHT_MOTOR, BACKWARD );
}

/*!\brief Sets the right morot to full speed forward and the left motor to full speed backward so that Heroine pivots to the left*/
void motorsLeft() {
  analogWrite( LEFT_MOTOR, BACKWARD );
  analogWrite( RIGHT_MOTOR, FORWARD );
}

/*!\brief Sets both motors to no movement*/
void motorsStop() {
  analogWrite( LEFT_MOTOR, STOP );
  analogWrite( RIGHT_MOTOR, STOP );
}

/*!\brief Sends a single byte of data through the serial interface

\param data The data to send*/
void sendSerialData( char data ) {
  Serial.write( data );
}
