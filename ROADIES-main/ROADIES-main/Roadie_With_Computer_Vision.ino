#include <SPI.h>
#include <RF24.h>
#include <printf.h>
#include <stdio.h>
#include <stdlib.h>


#define CE_PIN    30
#define CSN_PIN   32

//Motor Pins
const int in1A = 6;
const int in1B = 5;
const int in2A = 4;
const int in2B = 3;
const int enA = 7;
const int enB = 2;

// IR Sensor Pins
const int irSensor1Pin = 25; // Sensor 1 pin
const int irSensor2Pin = 26; // Sensor 2 pin
const int irSensor3Pin = 27; // Sensor 3 pin

// Switch variable
int movement; 

//Variables
byte top_button, bottom_button, emergency_switch, auto_switch;
//creates the radio

RF24 radio(CE_PIN, CSN_PIN);

// 12 Byte struct for the controller (need to add one more Byte for one more switch on the controller)
typedef struct Controller{
  int32_t   x;
  int32_t   y;
  byte t_button;
  byte b_button;
  byte e_switch;   
  byte a_switch; 
} Controller_t;

//Address that will be the pipeline
const uint64_t address = 0xE8E8F0F0E1LL;

Controller_t read;

void setup() {
  // Radio
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_HIGH);
  radio.startListening();

  //Motor Pins
  pinMode(in1A, OUTPUT);
  pinMode(in1B, OUTPUT);
  pinMode(in2A, OUTPUT);
  pinMode(in2B, OUTPUT);
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(34, OUTPUT);
  pinMode(36, OUTPUT);

  //IR Sensors 
  pinMode(irSensor1Pin, INPUT);
  pinMode(irSensor2Pin, INPUT);
  pinMode(irSensor3Pin, INPUT);

  Serial.begin(9600);
}

// Calculate turning motor speeds based on joystick x-axis value
int motor_speed_A;
int motor_speed_B;
int x_motor_speed;
bool Statement = false;
void loop() {

  if(radio.available()){
    radio.read(&read, sizeof(struct Controller));

    int32_t x = read.x;
    int32_t y = read.y;
    top_button = read.t_button;
    bottom_button = read.b_button;
    emergency_switch = read.e_switch;
    auto_switch = read.a_switch;
    
    if(auto_switch == 0){
      // If statements to access the switch cases
      if((y < 1700 && (x < 2048 && x > 1700)) && emergency_switch == 0){
        // Enable Motors
        digitalWrite(enA, HIGH);
        digitalWrite(enB, HIGH);
        // Move Forwards
        //movement = 'FORWARD';
        movement = 2;
        Serial.print("forward");
        //CHECK: switch(movement)
        switch(2);
      } else if((y > 2048 && (x < 2048 && x > 1700)) && emergency_switch == 0){
        // Enable Motors
        digitalWrite(enA, HIGH);
        digitalWrite(enB, HIGH);
        // Move Backwards
        //movement = 'BACKWARD';
        movement = 1;
        // CHECK: switch(movement)
        switch(1);
      } else if((x > 2048 && (y < 2048 && y > 1700)) && emergency_switch == 0){
        // Enable Motors
        digitalWrite(enA, HIGH);
        digitalWrite(enB, HIGH);
        // Move Right
        movement = 4;
        // CHECK: switch(movement);
        switch(4);
      } else if((x < 1700 && (y < 2048 && y > 1700)) && emergency_switch == 0){
        // Enable Motors
        digitalWrite(enA, HIGH);
        digitalWrite(enB, HIGH);
        // Move Left
        movement = 3;
        // CHECK: switch(movement);
        switch(3);
      } else {
        // Stop Roadie
        movement = NULL;
        // CHECK: switch(movement)
        switch(NULL);
      } 
    
      if(movement == NULL && top_button == 1){
        // IN2
        digitalWrite(36, LOW);
        // IN1
        digitalWrite(34, HIGH);
        Serial.println("UP");
      } else if (movement == NULL && bottom_button == 1){
        // IN1
        digitalWrite(34, LOW);
        // IN2
        digitalWrite(36, HIGH);
        Serial.println("DOWN");
      } else {
        digitalWrite(34, LOW);
        digitalWrite(36, LOW);
        Serial.println("STOP");
      }

      // Switch statements for movement
      switch(movement){
        case 1:
          // Roadie will be moving BACKWARDS
          // Map Motor Speed (204)
          motor_speed_A = map(y, 0, 4095, 0, 204);
          motor_speed_B = map(y, 0, 4095, 0, 204);
          // Move BACKWARDS
          digitalWrite(in1A, LOW);
          analogWrite(in2B, motor_speed_B);
          digitalWrite(in1B, LOW);
          analogWrite(in2A, motor_speed_A);
          Serial.println("BACKWARDS");
          break;
        case 2:
          // Roadie will be moving backwards
          // Map Motor Speed
          motor_speed_A = map(y, 4095, 0, 0, 204);
          motor_speed_B = map(y, 4095, 0, 0, 204);
          // Move FORWARDS
          analogWrite(in1A, motor_speed_A);
          digitalWrite(in2B, LOW);
          analogWrite(in1B, motor_speed_B);
          digitalWrite(in2A, LOW);
          Serial.println("FORWARDS");
          break;
        case 3:
          // Roadie will be pivoting left
          // Map Motor Speed
          x_motor_speed = map(x, 4095, 0, 0, 204);
          motor_speed_A = x_motor_speed;
          // Left Pivot
          digitalWrite(in1A, LOW);
          digitalWrite(in2B, LOW);
          analogWrite(in1B, motor_speed_A);
          analogWrite(in2A, motor_speed_A);
          Serial.println("LEFT");
          break;
        case 4:
          // Roadie will be pivoting right
          // Map Motor Speed
          x_motor_speed = map(x, 0, 4095, 0, 204);
          motor_speed_B = x_motor_speed;
          // Right Pivot
          analogWrite(in1A, motor_speed_B);
          analogWrite(in2B, motor_speed_B);
          digitalWrite(in1B, LOW);
          digitalWrite(in2A, LOW);
          Serial.println("RIGHT");
          break;
        default:
          // Roadie will not move
          // Disable everything
          digitalWrite(enA, LOW);
          digitalWrite(enB, LOW);
          digitalWrite(in2B, LOW);
          digitalWrite(in1A, LOW);
          digitalWrite(in2A, LOW);
          digitalWrite(in2B, LOW);
          motor_speed_A = 0;
          motor_speed_B = 0;
          x_motor_speed = 0;
          Serial.println("DEFAULT");
          break;
      } 
    /*


    ROADIE AUTO MODE

    */
    } else if(auto_switch == 1){
        String data = "";
        if(Serial.available() > 0){
          data = Serial.readStringUntil('\n');
        }

        //FRONT SENSOR
        if(digitalRead(irSensor2Pin) == LOW){
          Statement = true;
          // Stop Roadie
          movement = NULL;
          // CHECK: switch(movement)
          switch(NULL);
          Serial.println("FRONT STOP");
        }
          //LEFT SENSOR
        else if(digitalRead(irSensor1Pin) == LOW){
          Statement = true;
          //FRONT SENSOR
          if(digitalRead(irSensor2Pin) == LOW){
            // Enable Motors
            digitalWrite(enA, HIGH);
            digitalWrite(enB, HIGH);
            // Move Left
            movement = 3;
            // CHECK: switch(movement);
            switch(3);
          }
          //RIGHT SENSOR
          else if(digitalRead(irSensor3Pin) == LOW){
            // Stop Roadie
            movement = NULL;
            // CHECK: switch(movement)
            switch(NULL);
          }
          //ONLY LEFT SENSOR
          else{
            // Enable Motors
            digitalWrite(enA, HIGH);
            digitalWrite(enB, HIGH);
            // Move Left
            movement = 4;
            // CHECK: switch(movement);
            switch(4);
          }
        }
        //RIGHT SENSOR
        else if(digitalRead(irSensor3Pin) == LOW){
          Statement = true;
          //FRONT SENSOR
          if(digitalRead(irSensor2Pin) == LOW){
            // Enable Motors
            digitalWrite(enA, HIGH);
            digitalWrite(enB, HIGH);
            // Move Right
            movement = 4;
            // CHECK: switch(movement);
            switch(4);
          }
          //LEFT SENSOR
          else if(digitalRead(irSensor1Pin) == LOW){
            // Stop Roadie
            movement = NULL;
            // CHECK: switch(movement)
            switch(NULL);
            Serial.println("LEFT STOP");
          }
          //RIGHT SENSOR
          else{
            // Enable Motors
            digitalWrite(enA, HIGH);
            digitalWrite(enB, HIGH);
            // Move Right
            movement = 3;
            // CHECK: switch(movement);
            switch(3);
          }
        }
        else if(data.substring(4,15) == "Stop motors"){
          Serial.print("Received from rpi: ");
          Serial.println(data);

          // Stop Roadie
          movement = NULL;
          // CHECK: switch(movement)
          switch(NULL);
          Serial.println("STOP");
        }
        else if(data.substring(4,16) == "Move Forward") {
          Serial.print("Received from rpi: ");
          Serial.println(data);

          // Enable Motors
          digitalWrite(enA, HIGH);
          digitalWrite(enB, HIGH);
          // Move Forwards
          //movement = 'FORWARD';
          movement = 2;
          Serial.print("forward");
          //CHECK: 
          switch(2);
        }  
        else if(data.substring(4,13) == "Move Left") {
          Serial.print("Received from rpi: ");
          Serial.println(data);

          // Enable Motors
          digitalWrite(enA, HIGH);
          digitalWrite(enB, HIGH);
          // Move Left
          movement = 3;
          // CHECK: switch(movement);
          switch(3);
        }
        else if(data.substring(4,14) == "Move Right"){
          Serial.print("Received from rpi: ");
          Serial.println(data);

          // Enable Motors
          digitalWrite(enA, HIGH);
          digitalWrite(enB, HIGH);
          // Move Right
          movement = 4;
          // CHECK: switch(movement);
          switch(4);
        }
        else if(data.substring(4,13) == "Move Back"){
          Serial.print("Received from rpi: ");
          Serial.println(data);

          // Enable Motors
          digitalWrite(enA, HIGH);
          digitalWrite(enB, HIGH);
          // Move Backwards
          //movement = 'BACKWARD';
          movement = 1;
          // CHECK: switch(movement)
          switch(1);
        }else if(data.substring(4,13) == "Pivot Right"){
          Serial.print("Received from rpi: ");
          Serial.println(data);

          // Enable Motors
          digitalWrite(enA, HIGH);
          digitalWrite(enB, HIGH);
          // Move Right
          movement = 4;
          // CHECK: switch(movement);
          switch(4);
        }
        else if (Statement == true){
          // Stop Roadie
          movement = NULL;
          // CHECK: switch(movement)
          switch(NULL);
          Serial.println("STOP 2");
          Statement = false;
        }
        // Switch statements for movement
        switch(movement){
          case 1:
          // Roadie Backward
          motor_speed_A = 127;
          motor_speed_B = 127;
          digitalWrite(in1A, LOW);
          analogWrite(in2B, motor_speed_B);
          digitalWrite(in1B, LOW);
          analogWrite(in2A, motor_speed_A);
          break;
        case 2:
          // Roadie Forward
          motor_speed_A = 127;
          motor_speed_B = 127;
          analogWrite(in1A, motor_speed_A);
          digitalWrite(in2B, LOW);
          analogWrite(in1B, motor_speed_B);
          digitalWrite(in2A, LOW);
          break;
        case 3:
          // Roadie left pivot
          motor_speed_A = 75;
          digitalWrite(in1A, LOW);
          digitalWrite(in2B, LOW);
          analogWrite(in1B, motor_speed_A);
          analogWrite(in2A, motor_speed_A);
          break;
        case 4:
          // Roadie right pivot
          motor_speed_B = 75;
          analogWrite(in1A, motor_speed_B);
          analogWrite(in2B, motor_speed_B);
          digitalWrite(in1B, LOW);
          digitalWrite(in2A, LOW);
          break;
        default:
          // Stop Roadie
          digitalWrite(enA, LOW);
          digitalWrite(enB, LOW);
          digitalWrite(in2B, LOW);
          digitalWrite(in1A, LOW);
          digitalWrite(in2A, LOW);
          digitalWrite(in2B, LOW);
          motor_speed_A = 0;
          motor_speed_B = 0;
          x_motor_speed = 0;
        break;
      }
    }
  } 
}
