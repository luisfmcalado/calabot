#include <AFMotor.h>
#include <Servo.h>

#define TRIGPIN A0
#define ECHOPIN A5

#define HCSR04 9
#define INITPOS 75

#define MOTORSPEED 90
#define TURNTIME 800

Servo hcsr;
AF_DCMotor motor1(1, MOTOR12_64KHZ);
AF_DCMotor motor2(2, MOTOR12_64KHZ);
AF_DCMotor motor3(3, MOTOR34_1KHZ);
AF_DCMotor motor4(4, MOTOR34_1KHZ);
int backwardmove = 0;
int pos = INITPOS;

void setup() {
  Serial.begin (9600);
  pinMode(TRIGPIN, OUTPUT);
  pinMode(ECHOPIN, INPUT);
  hcsr.attach(HCSR04);
  motor1.setSpeed(MOTORSPEED);
  motor2.setSpeed(MOTORSPEED);
  motor3.setSpeed(MOTORSPEED);
  motor4.setSpeed(MOTORSPEED);
}

void turnleft(){
    motor1.run(FORWARD);
    motor2.run(RELEASE);
    motor3.run(FORWARD);
    motor4.run(RELEASE);
    delay(TURNTIME);
}

void turnright(){
    motor1.run(RELEASE);
    motor2.run(FORWARD);
    motor3.run(RELEASE);
    motor4.run(FORWARD);
    delay(TURNTIME);
}

void forward(){
    motor1.run(FORWARD);
    motor2.run(FORWARD);
    motor3.run(RELEASE);
    motor4.run(RELEASE);
}

void backward(){
    motor1.run(BACKWARD);
    motor2.run(BACKWARD);
    motor3.run(RELEASE);
    motor4.run(RELEASE);
    delay(TURNTIME);
}

void brake(){
    motor1.run(RELEASE);
    motor2.run(RELEASE);
    motor3.run(RELEASE);
    motor4.run(RELEASE);
}

long ping(){
  long duration, distance;
  digitalWrite(TRIGPIN, LOW); 
  delayMicroseconds(2); 

  digitalWrite(TRIGPIN, HIGH);
  delayMicroseconds(10); 
 
  digitalWrite(TRIGPIN, LOW);
  duration = pulseIn(ECHOPIN, HIGH);
  distance = ((duration/2) / 29.1);
  return distance;
}

long pingbest(){
  long val = 10000, temp;
  for(int i = 0; i < 5; i++){
    temp = ping();
    if(val > temp) val = temp;
  }
  return val;
}

void printdistance(long distance){
  Serial.print("distance: ");
  Serial.print(distance);
  Serial.print(" cm");
  Serial.println();
}

long lookleft(){
    hcsr.write(0);
    delay(1000); 
    return pingbest();
}

long lookright(){
    hcsr.write(180);
    delay(1000); 
    return pingbest();
}

long lookahead(){
    hcsr.write(INITPOS);
    delay(500); 
    return pingbest();
}

void loop() {  
  long distance = 0;
  if(!backwardmove)
    distance = lookahead();

  if ((distance > 40 || distance <= 0) && !backwardmove){
    Serial.print("front ");
    printdistance(distance);
    forward();
    delay(300);
  }
  else {

    brake();
    
    int goforit = 1;
    distance = lookleft();
    backwardmove = 0;

    if(distance > 40 || distance <= 0){
      turnleft();
      brake();
      goforit = 0;

      Serial.print("turn left with ");
      printdistance(distance);
    }

    if(goforit){
      
      distance = lookright();
      if(distance > 40 || distance <= 0){
        turnright();
        brake();
        goforit = 0;

        Serial.print("turn right with ");
        printdistance(distance);
      }  
    }
    
    if(goforit){
      backward();
      brake();
      backwardmove = 1;
      Serial.println("going backward");
    }  
  }
}

