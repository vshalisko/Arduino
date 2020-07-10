#include <AFMotor.h>      //add Adafruit Motor Shield library
#include <Servo.h>        //add Servo Motor library
#include <NewPing.h>        //add Ultrasonic Ping library

#define MAX_SPEED 200 // sets speed of DC traction motors to 200/250 or about 70% of full speed - to get power drain down.
#define MAX_SPEED_OFFSET 40 // this sets offset to allow for differences between the two DC traction motors

#define TRIG_PIN A4 // Pin A0 on the Motor Drive Shield soldered to the ultrasonic sensor
#define ECHO_PIN A5 // Pin A1 on the Motor Drive Shield soldered to the ultrasonic sensor
#define MAX_DISTANCE 300 // sets maximum useable sensor measuring distance to 300cm
#define COLL_DIST 40 // sets distance at which robot stops and reverses to 30cm
#define COLL_DIST_MIN 15 // sets distance at which robot stops and reverses to 15cm
#define TURN_DIST COLL_DIST + 20 // sets distance at which robot veers away from object
NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE); // sets up sensor library to use the correct pins to measure distance.

AF_DCMotor leftMotor1(1, MOTOR12_1KHZ); // create motor #1 using M1 output on Motor Drive Shield, set to 1kHz PWM frequency
AF_DCMotor leftMotor2(2, MOTOR12_1KHZ); // create motor #2, using M2 output, set to 1kHz PWM frequency
AF_DCMotor rightMotor1(3, MOTOR34_1KHZ); // create motor #3, using M3 output, set to 1kHz PWM frequency
AF_DCMotor rightMotor2(4, MOTOR34_1KHZ); // create motor #4, using M4 output, set to 1kHz PWM frequency
Servo headservo;  // create servo object to control a head servo 

int leftDistance, rightDistance; //distances on either side
int curDist = 0;
String motorSet = "";
int speedSet = 0;
int ledPin = 2;     // usamos un pin de salida al LED
int serialState = '1';    // Variable lectrura serial
  
void setup() {
     // put your setup code here, to run once:
     headservo.attach(10);  // attaches the servo on pin 10 (SERVO_1 on the Motor Drive Shield to the servo object)
     headservo.write(125); // tells the servo to start position ie. facing forward.
     delay(1000); // delay for one seconds

     pinMode(ledPin, OUTPUT);   // Declara pin de Salida
     digitalWrite(ledPin, LOW); // Normalmente Apagado
     Serial.begin(9600);  
}

void loop() {
    // si el modulo Bluetooth manda dato, guardarlo en estado.
    if(Serial.available() > 0){
         serialState = Serial.read();
    } // read only one character from serial input

    if (serialState == '1') {
      // The default "automatic" state signal received
       Serial.println("Automatic mode");
       digitalWrite(ledPin, HIGH);
       curDist = readPing();   // read distance
       moveHead();
       if ((curDist < COLL_DIST_MIN) || (leftDistance < COLL_DIST_MIN) || (rightDistance < COLL_DIST_MIN)) {
           moveBackward();  // move backward
           delay(500);
       } else {
          if ((curDist < COLL_DIST) || (leftDistance < COLL_DIST) || (rightDistance < COLL_DIST)) {
              // if forward is blocked change direction
              changePath();
          } else { 
              moveForward();  // move forward
              delay(2000);
          }
       }
    } else if (serialState == '0') {
       // "Stop" signal received
       digitalWrite(ledPin, LOW);
       Serial.println("Stop mode");
       moveStop();     // stop
       moveHead();   // move head
       delay(1000);              
    } else if (serialState == 'F') {
         Serial.println("Manual mode: forward");
         moveForward();
         delay(3000);
         // moveStop();     // stop                  
         serialState == '1';   // change to automatic mode
    } else if (serialState == 'B') {
         Serial.println("Manual mode: backward");
         moveBackward();
         delay(3000);
         moveStop();     // stop         
         changePath(); // move head
         serialState == '1';   // change to automatic mode
    } else if (serialState == 'L') {
         Serial.println("Manual mode: turn right");
         turnLeft();
         delay(1500);    
         moveStop();     // stop                  
         serialState == '1';   // change to automatic mode
    } else if (serialState == 'R') {
         Serial.println("Manual mode: turn left");
         turnRight();
         delay(1500);    
         moveStop();     // stop                   
         serialState == '1';   // change to automatic mode
    }
 }


void changePath() {
    moveStop();     // stop
    headservo.write(170);  // check distance to the right
    delay(500);
    rightDistance = readPing(); //set right distance
    delay(500);
    headservo.write(80);  // check distace to the left
    delay(700);
    leftDistance = readPing(); //set left distance
    delay(500);
    headservo.write(125); //return to center
    delay(100);
    compareDistance();
}

void compareDistance()   // find the longest distance
{
  if (leftDistance > rightDistance) //if left is less obstructed 
  {
    turnRight();
    delay(500);
    moveStop();
  }
  else if (rightDistance > leftDistance) //if right is less obstructed
  {
    turnLeft();
    delay(500);
    moveStop();
  }
   else //if they are equally obstructed
  {
    moveBackward();
    delay(500);
    turnAround();
    moveStop();
  }
}

void moveHead() {
    headservo.write(150);  // check distance to the right
    delay(300);
    rightDistance = readPing(); //set right distance
    delay(200);
    headservo.write(100);  // check distace to the left
    delay(500);
    leftDistance = readPing(); //set left distance
    delay(200);
    headservo.write(125); //return to center
    delay(100);
}

int readPing() { // read the ultrasonic sensor distance
  delay(70);   
  unsigned int uS = sonar.ping();
  int cm = uS / US_ROUNDTRIP_CM;
  Serial.println("Distance:");
  Serial.println(cm);
  return cm;
}

void moveStop() {
  leftMotor1.run(RELEASE); 
  leftMotor2.run(RELEASE); 
  rightMotor1.run(RELEASE); 
  rightMotor2.run(RELEASE);
}

void moveForward() {
    motorSet = "FORWARD";
    leftMotor1.run(FORWARD);      // turn it on going forward
    leftMotor2.run(FORWARD);      // turn it on going forward
    rightMotor1.run(FORWARD);     // turn it on going forward
    rightMotor2.run(FORWARD);     // turn it on going forward
  for (speedSet = 0; speedSet < MAX_SPEED; speedSet +=2) // slowly bring the speed up to avoid loading down the batteries too quickly
  {
    leftMotor1.setSpeed(speedSet);
    leftMotor2.setSpeed(speedSet);
    rightMotor1.setSpeed(speedSet); 
    rightMotor2.setSpeed(speedSet);
    delay(5);
  }
}

void moveBackward() {
    motorSet = "BACKWARD";
    leftMotor1.run(BACKWARD);     // turn it on going backward
    leftMotor2.run(BACKWARD);     // turn it on going backward
    rightMotor1.run(BACKWARD);    // turn it on going backward
    rightMotor2.run(BACKWARD);    // turn it on going backward
  for (speedSet = 0; speedSet < MAX_SPEED; speedSet +=2) // slowly bring the speed up to avoid loading down the batteries too quickly
  {
    leftMotor1.setSpeed(speedSet);
    leftMotor2.setSpeed(speedSet);
    rightMotor1.setSpeed(speedSet); 
    rightMotor2.setSpeed(speedSet); 
    delay(5);
  }
} 
void turnRight() {
  motorSet = "RIGHT";
  leftMotor1.run(FORWARD);      // turn motor 1 forward
  leftMotor2.run(FORWARD);      // turn motor 2 forward
  rightMotor1.run(BACKWARD);    // turn motor 3 backward
  rightMotor2.run(BACKWARD);    // turn motor 4 backward
  rightMotor1.setSpeed(speedSet + MAX_SPEED_OFFSET);      
  rightMotor2.setSpeed(speedSet + MAX_SPEED_OFFSET);     
  delay(1500); // run motors this way for 1500        
  motorSet = "FORWARD";
  leftMotor1.run(FORWARD);      // set both motors back to forward
  leftMotor2.run(FORWARD);
  rightMotor1.run(FORWARD);
  rightMotor2.run(FORWARD);      
}  

void turnLeft() {
  motorSet = "LEFT";
  leftMotor1.run(BACKWARD);      // turn motor 1 backward
  leftMotor2.run(BACKWARD);      // turn motor 2 backward
  leftMotor1.setSpeed(speedSet + MAX_SPEED_OFFSET);     
  leftMotor2.setSpeed(speedSet + MAX_SPEED_OFFSET);    
  rightMotor1.run(FORWARD);     // turn motor 3 forward
  rightMotor2.run(FORWARD);     // turn motor 4 forward
  delay(1500); // run motors this way for 1500  
  motorSet = "FORWARD";
  leftMotor1.run(FORWARD);      // turn it on going forward
  leftMotor2.run(FORWARD);      // turn it on going forward
  rightMotor1.run(FORWARD);     // turn it on going forward
  rightMotor2.run(FORWARD);     // turn it on going forward
}  

void turnAround() {
  motorSet = "RIGHT";
  leftMotor1.run(FORWARD);      // turn motor 1 forward
  leftMotor2.run(FORWARD);      // turn motor 2 forward
  rightMotor1.run(BACKWARD);    // turn motor 3 backward
  rightMotor2.run(BACKWARD);    // turn motor 4 backward
  rightMotor1.setSpeed(speedSet + MAX_SPEED_OFFSET);      
  rightMotor2.setSpeed(speedSet + MAX_SPEED_OFFSET);
  delay(5000); // run motors this way for long interval
  motorSet = "FORWARD";
  leftMotor1.run(FORWARD);      // set both motors back to forward
  leftMotor2.run(FORWARD);
  rightMotor1.run(FORWARD);
  rightMotor2.run(FORWARD);      
}  
