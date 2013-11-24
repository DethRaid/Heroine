/*!\file   main.ino
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

#define FORWARD  255
#define BACKWARD 1
#define STOP     126

#define RIGHT_MOTOR              3
#define LEFT_MOTOR               5

#define MOMENTARY_SWITCHES       2
#define ELEVATOR_POWER_SWITCHES  4
#define ELEVATOR_SWITCH_TOP      6
#define ELEVATOR_SWITCH_BOTTOM   7
#define ELEVATOR_MOTOR_UP_OUT    9
#define ELEVATOR_MOTOR_DOWN_OUT  10

#define RIGHT_WHEEL_SWITCH       11
#define LEFT_WHEEL_SWITCH        12

void getSerialInput();
void getButtonInput();
void evaluateElevatorSwitches();
void motorsForward();
void motorsBackward();
void motorsRight();
void motorsLeft();
void motorsStop();
void checkMotorInputs();
void sendSerialData( char data );

char serialData;

boolean rightMotorForward, leftMotorForward;
boolean momentaryCurState, momentaryLastState;
boolean motorPowerCurState, motorPowerLastState;
boolean motorShouldUp;

/*!\brief Initializes the Arduino state.

 Initializes serial communication to a frequency of 9600 Hz and brakes both motors
 Sets pins 2, and 4 to be input pins while 3, 5, and 6 are set to be output pins
 Stops all motors
 Sets the input from the elevator switches to 0
 */
void setup() {
  Serial.begin( 9600 );
  pinMode( RIGHT_MOTOR, OUTPUT );
  pinMode( LEFT_MOTOR, OUTPUT );
  pinMode( MOMENTARY_SWITCHES, INPUT );
  pinMode( ELEVATOR_POWER_SWITCHES, INPUT );
  pinMode( ELEVATOR_SWITCH_TOP, INPUT );
  pinMode( ELEVATOR_SWITCH_BOTTOM, INPUT );
  pinMode( ELEVATOR_MOTOR_UP_OUT, OUTPUT );
  pinMode( ELEVATOR_MOTOR_DOWN_OUT, OUTPUT );
  pinMode( RIGHT_MOTOR_SWITCH, INPUT );
  pinMode( LEFT_MOTOR_SWITCH, INPUT );
  motorsStop();
  digitalWrite( ELEVATOR_MOTOR_UP_OUT, LOW );
  digitalWrite( ELEVATOR_MOTOR_DOWN_OUT, LOW );
  momentaryCurState = false;
  momentaryLastState = false;
  motorPowerCurState = false;
  motorPowerLastState = false;
  motorShouldUp = false;
  rightMotorForward = false;
  leftMotorForward = false;
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
  checkMotorSwitches();
  getSerialInput();
  getButtonInput();
  evaluateElevatorSwitches();
  
  switch( serialData ) {
    case 'f':
      motorsForward();
      break;
    case 'b':
      motorsBackward();
      break;
    case 'r':
      motorsRight();
      break;
    case 'l':
      motorsLeft();
      break;
    case 's':
      motorsStop();
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

/*!\brief Checks if the switch on the left or right motor is high. If so, it checks the direction of that motor and tells the Beagle Bone about it

Bit 0 is if the motor is forward
Bit 1 is if the motor is right
Bit 7 means something is being transmitted
*/
void checkMotorSwitches() {
  if( digitalRead( RIGHT_MOTOR_SWITCH ) == HIGH ) {
    sendSerialData( 66 + (rightMotorForward ? 1 : 0) );
  }
  if( digitalRead( LEFT_MOTOR_SWITCH ) == HIGH ) {
    sendSerialData( 64 + (leftMotorForwad ? 1 : 0) );
  }
}

/*!\brief Performs necessary calculations to move the drink elevator*/
void evaluateElevatorSwitches() {
  motorPowerLastState = motorPowerCurState;
  momentaryLastState = momentaryCurState;
  momentaryCurState = digitalRead( MOMENTARY_SWITCHES ) == HIGH;
  motorPowerCurState = digitalRead( ELEVATOR_POWER_SWITCHES ) == HIGH;
  if( digitalRead( ELEVATOR_SWITCH_BOTTOM ) == HIGH ) {
    motorShouldUp = true;
    Serial.println( "The elevator should go up" );
  } else if( digitalRead( ELEVATOR_SWITCH_TOP ) == HIGH ) {
    motorShouldUp = false;
    Serial.println( "The elevator should go down" );
  }
  if( motorPowerCurState && !motorPowerLastState ) {  //if the motor should turn on
    if( motorShouldUp ) {
      digitalWrite( ELEVATOR_MOTOR_UP_OUT, HIGH );
      digitalWrite( ELEVATOR_MOTOR_DOWN_OUT, LOW );
      Serial.println( "Starting elevator" );
    } else {
      digitalWrite( ELEVATOR_MOTOR_UP_OUT, LOW );
      digitalWrite( ELEVATOR_MOTOR_DOWN_OUT, HIGH );
      Serial.println( "Starting elevator" );
    }
  }
  if( momentaryCurState && !momentaryLastState ) {  //if the motor should turn off
    digitalWrite( ELEVATOR_MOTOR_UP_OUT, LOW );
    digitalWrite( ELEVATOR_MOTOR_DOWN_OUT, LOW );
    Serial.println( "Stopping elevator" );
  }
}

/*!\brief Sets both motors to full speed forward*/
void motorsForward() {
  analogWrite( LEFT_MOTOR, FORWARD );
  analogWrite( RIGHT_MOTOR, FORWARD );
  rightMotorForward = true;
  leftMotorForward = true;
}

/*!\brief Sets both motors to full speed backward*/
void motorsBackward() {
  isForward = false;
  analogWrite( LEFT_MOTOR, BACKWARD );
  analogWrite( RIGHT_MOTOR, BACKWARD );
  rightMotorForward = false;
  leftMotorForward = false;
}

/*!\brief Sets the right motor to full speed backward and the left motor to full speed forward so that Heroine pivots to the right*/
void motorsRight() {
  analogWrite( LEFT_MOTOR, FORWARD );
  analogWrite( RIGHT_MOTOR, BACKWARD );
  rightMotorForward = false;
  leftMotorForward = true;
}

/*!\brief Sets the right morot to full speed forward and the left motor to full speed backward so that Heroine pivots to the left*/
void motorsLeft() {
  analogWrite( LEFT_MOTOR, BACKWARD );
  analogWrite( RIGHT_MOTOR, FORWARD );
  rightMotorForward = true;
  leftMotorForward = false;
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
