#include <AFMotor.h>      //add Adafruit Motor Shield library
#include <Servo.h>        //add Servo Motor library

#define MAX_SPEED 200 // sets speed of DC traction motors to 150/250 or about 70% of full speed - to get power drain down.
#define MAX_SPEED_OFFSET 40 // this sets offset to allow for differences between the two DC traction motors

AF_DCMotor leftMotor1(1, MOTOR12_1KHZ); // create motor #1 using M1 output on Motor Drive Shield, set to 1kHz PWM frequency
AF_DCMotor leftMotor2(2, MOTOR12_1KHZ); // create motor #2, using M2 output, set to 1kHz PWM frequency
AF_DCMotor rightMotor1(3, MOTOR34_1KHZ); // create motor #3, using M3 output, set to 1kHz PWM frequency
AF_DCMotor rightMotor2(4, MOTOR34_1KHZ); // create motor #4, using M4 output, set to 1kHz PWM frequency
Servo headservo;  // create servo object to control a head servo 

String motorSet = "";
int speedSet = 0;
int ledPin = 2;     // usamos un pin de salida al LED
int state = '1';    // Variable lectrura serial
  
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
    //si el modulo Bluetooth manda dato, guardarlo en estado.
    if(Serial.available() > 0){
         state = Serial.read();
    } // esta parte del código es para solo 1 Carácter o Unidad. 

    if (state == '0') {
       // "OFF" signal received
       digitalWrite(ledPin, LOW);
       Serial.println("LED: off");
       moveStop();     // stop
       changePath();   // move head
       delay(500);              
    }
    // "ON" signal received
    else if (state == '1') {
       digitalWrite(ledPin, HIGH);
       Serial.println("LED: on");
       //  curDist = readPing();   // read distance
       //  if (curDist < COLL_DIST) {changePath();}  // if forward is blocked change direction
       moveForward();  // move forward
       delay(1500);
       moveStop();     // stop
       delay(500);
       //  moveBackward();
       //  delay(500);
       changePath(); // move head
    }
    else if (state == 'F') {
         moveForward();
         delay(2500);
         moveStop();     // stop                  
         state == '1';
    }
    else if (state == 'B') {
         moveBackward();
         delay(2500);
         moveStop();     // stop         
         changePath(); // move head
         state == '1';
    }
    else if (state == 'L') {
         turnLeft();
         delay(1500);    
         moveStop();     // stop                  
         state == '1';
    }
    else if (state == 'R') {
         turnRight();
         delay(1500);    
         moveStop();     // stop                  
         state == '1';
    }
 }

void changePath() {
  moveStop();   // stop forward movement
  headservo.write(170);  // check distance to the right
    delay(500);
//    rightDistance = readPing(); //set right distance
    delay(500);
    headservo.write(80);  // check distace to the left
    delay(700);
//    leftDistance = readPing(); //set left distance
    delay(500);
    headservo.write(125); //return to center
    delay(100);
//    compareDistance();
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
  rightMotor1.setSpeed(speedSet+MAX_SPEED_OFFSET);      
  rightMotor2.setSpeed(speedSet+MAX_SPEED_OFFSET);     
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
  leftMotor1.setSpeed(speedSet+MAX_SPEED_OFFSET);     
  leftMotor2.setSpeed(speedSet+MAX_SPEED_OFFSET);    
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
  rightMotor1.setSpeed(speedSet+MAX_SPEED_OFFSET);      
  rightMotor2.setSpeed(speedSet+MAX_SPEED_OFFSET);
  delay(1000); // run motors this way for long interval
  motorSet = "FORWARD";
  leftMotor1.run(FORWARD);      // set both motors back to forward
  leftMotor2.run(FORWARD);
  rightMotor1.run(FORWARD);
  rightMotor2.run(FORWARD);      
}  
