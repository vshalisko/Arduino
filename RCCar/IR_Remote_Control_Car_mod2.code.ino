#include <IRremote.h>
#include <IRremoteInt.h>

int RECV_PIN = 12;
IRrecv irrecv(RECV_PIN);
decode_results results;

// define IR codes
#define IR_Go           0x00ff629d
#define IR_Back         0x00ffa857
#define IR_Left         0x00ff22dd
#define IR_Right        0x00ffc23d
#define IR_Stop         0x00ff02fd
#define IR_ESC          0x00ff52ad
#define IR_Program1     0x00ff6897

// define IO Pins as constants
const int Lpwm_pin = 5;        // Letf pin for adjusting speed 
const int Rpwm_pin = 10;       // Right for pin adjusting speed 
const int pinLB = 2;           // defining pin2 left rear
const int pinLF = 4;           // defining pin4 left front
const int pinRB = 7;           // defining pin7 right rear
const int pinRF = 8;           // defining pin8 right front
const int SensorLeft = 6;      //input pin of left sensor
const int SensorMiddle = 9;    //input pin of middle sensor
const int SensorRight = 11;    //input pin of right sensor

const long interval = 50;           // interval at which to drive back (milliseconds)
unsigned long currentMillis = millis();
unsigned long previousMillis = 0;        // will store last time state was changed

int ECHOinputPin = A0;  // ultrasonic module   ECHO to A0
int ECHOoutputPin = A1;  // ultrasonic module  TRIG to A1

// define vars
int H = 20;  // Obstacle distance
int Car_state = 5;  // Car state memory
unsigned char Lpwm_val = 200;   // Left speed
unsigned char Rpwm_val = 200;   // Right speed
unsigned char SL;        //state of left sensor
unsigned char SM;        //state of middle sensor
unsigned char SR;        //state of right sensor

// define initialization routines
void M_Control_IO_config(void)
{
  pinMode(pinLB,OUTPUT); 
  pinMode(pinLF,OUTPUT); 
  pinMode(pinRB,OUTPUT);  
  pinMode(pinRF,OUTPUT);
  pinMode(Lpwm_pin,OUTPUT);  
  pinMode(Rpwm_pin,OUTPUT);  
  pinMode(SensorLeft,INPUT);
  pinMode(SensorMiddle,INPUT);
  pinMode(SensorRight,INPUT);
  pinMode(ECHOinputPin, INPUT);    
  pinMode(ECHOoutputPin, OUTPUT);  
}
void Set_Speed(unsigned char Left,unsigned char Right)
{
  analogWrite(Lpwm_pin,Left);
  analogWrite(Rpwm_pin,Right);
}

// define main routines
void Sensor_Scan(void)
{
  SL = digitalRead(SensorLeft);
  SM = digitalRead(SensorMiddle);
  SR = digitalRead(SensorRight);
}

int Ultrasonic_Ranging(unsigned char Mode) //function of ultrasonic distance detecting ，MODE=1，displaying，no displaying under other situation
{ 
  //int old_distance;
  digitalWrite(ECHOoutputPin, LOW);  
  delayMicroseconds(2); 
  digitalWrite(ECHOoutputPin, HIGH); 
  delayMicroseconds(10); 
  digitalWrite(ECHOoutputPin, LOW);    
  int distance = pulseIn(ECHOinputPin, HIGH);  // reading the duration of high level
  distance= distance / 58;   // Transform pulse time to distance   
  if(Mode == 1){
         Serial.print("\n H = ");
         Serial.print(distance,DEC); 
         return distance;
  }
   else
   {
        return distance;
   }
} 

void stopp()         //stop
    {
     digitalWrite(pinRB,HIGH);
     digitalWrite(pinRF,HIGH);
     digitalWrite(pinLB,HIGH);
     digitalWrite(pinLF,HIGH);
    }
void advance()     // going forward
    {
     digitalWrite(pinRB,HIGH);  
     digitalWrite(pinRF,LOW);
     digitalWrite(pinLB,HIGH);  
     digitalWrite(pinLF,LOW); 
    }
void turnR()        //turning right(dual wheel)
    {
     digitalWrite(pinRB,LOW);  
     digitalWrite(pinRF,HIGH);
     digitalWrite(pinLB,HIGH);
     digitalWrite(pinLF,LOW);  
    }
void turnL()        //turning left(dual wheel)
    {
     digitalWrite(pinRB,HIGH);
     digitalWrite(pinRF,LOW );   
     digitalWrite(pinLB,LOW);    
     digitalWrite(pinLF,HIGH);
    }    
void back()          //back up
    {
     digitalWrite(pinRB,LOW);  
     digitalWrite(pinRF,HIGH);
     digitalWrite(pinLB,LOW);  
     digitalWrite(pinLF,HIGH);
    }
void program1()          
    {
     // programa es interrumpida cada vez que tomamos delay
     // adelante
     digitalWrite(pinRB,HIGH);  
     digitalWrite(pinRF,LOW);
     digitalWrite(pinLB,HIGH);  
     digitalWrite(pinLF,LOW);
     delay(1000);
     // girar
     digitalWrite(pinRB,HIGH);
     digitalWrite(pinRF,LOW);   
     digitalWrite(pinLB,LOW);   
     digitalWrite(pinLF,HIGH);
     delay(random(200, 2000));
     // atras     
     digitalWrite(pinRB,LOW); 
     digitalWrite(pinRF,HIGH);
     digitalWrite(pinLB,LOW);
     digitalWrite(pinLF,HIGH);
     delay(1000);
     // stop
     digitalWrite(pinRB,HIGH);
     digitalWrite(pinRF,HIGH);
     digitalWrite(pinLB,HIGH);
     digitalWrite(pinLF,HIGH);
    }

         
void IR_Control(void)
{ 
  unsigned long Key;
  while(Key != IR_ESC)
  {
    currentMillis = millis(); 
    if(irrecv.decode(&results)) // judging if IR receives data   
    {
      Key = results.value; // store the IR key value
      switch(Key)
      {
         case IR_Go:
            Car_state = 1;   //forward
         break;
         case IR_Back:
            Car_state = 2;    //back
         break;
         case IR_Left:
            Car_state = 3;    //Left    
         break;
         case IR_Right:
            Car_state = 4;    //Righ
         break;
         case IR_Stop: 
            Car_state = 5;   //stop
         break;
         case IR_Program1:
            Car_state = 6;  //program 1
         break;       
         default: 
         break;      
      } 

     irrecv.resume(); // Receive the next value 
     } 
        if ((unsigned long)(currentMillis - previousMillis) >= interval) {
         
          // change the state of the car as the interval was exceded (checking each 500 ms)
          
          //Serial.print("\n M = ");
           //Serial.print(currentMillis,DEC);  
          // return the car state to stop mode
          if (Car_state == 9) {
            Car_state = 5;
          }
          
          // ultrasonic obstacule detection
          H = Ultrasonic_Ranging(0);
          delay(10);
          if(H < 20)         
          {
              // set the car state backward movement
              // until the obstacle is avoided
              Car_state = 9;
          }   
          // save the last time you changed the car state
          previousMillis = millis();
         
      }   
      switch(Car_state)
      {
         case 1:
            advance();   //forward
         break;
         case 2: 
            back();    //back
         break;
         case 3:
            turnL();   //Left    
         break;
         case 4: 
            turnR();   //Righ
         break;
         case 5:
            stopp();   //stop
         break;
         case 6: 
            program1();  //program 1
         break; 
         case 9: 
            back();
         break;               
         default: 
            stopp();
         break;      
      }       
  }
}

// main setup
void setup() 
{ 
   M_Control_IO_config();
   Set_Speed(Lpwm_val,Rpwm_val);
   irrecv.enableIRIn();  // Start the IR receiver
   Serial.begin(9600);   //initializing serial port, Bluetooth used as serial port, setting baud ratio at 9600 
   stopp();  
}

// main loop
void loop() 
{  
          
        if (irrecv.decode(&results)) {
          if(results.value == IR_Stop ) IR_Control();
          //IR_Control();
          irrecv.resume(); // Receive the next value   
        } 

        

     

    


      
     // optical black detection
     // Sensor_Scan();
     // if (SM == HIGH) // middle sensor in black area
     //{
     //   stopp();              
     //   delay(50);    
     //   back();
     //   delay(100);
     //   stopp();        
     // } 
     // else
     // {

     // }   
}
