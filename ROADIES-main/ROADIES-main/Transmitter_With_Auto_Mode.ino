#include <SPI.h>
#include <RF24.h>
#include <printf.h>
#include <stdio.h>
#include <stdlib.h>

// Pins for the NRF
#define MISO_PIN  12
#define MOSI_PIN  11
#define SCK_PIN  13
#define CE_PIN  8
#define CSN_PIN 9
  
// Variables 
int32_t  joy_x, joy_y;
byte top_button, bottom_button, emergency_button, auto_button;

//Creates the radio
RF24 radio(CE_PIN, CSN_PIN);

// Address that will be the pipeline
const uint64_t address = 0xE8E8F0F0E1LL;

// 12 Byte struct for the controller (need to add one more Byte for one more switch on the controller)
typedef struct Controller{
  int32_t   x;
  int32_t   y;
  byte t_button;
  byte b_button;
  byte e_switch;
  byte a_switch;
} Controller_t;

void grab (Controller_t* data);

void setup() {
  // put your setup code here, to run once:
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_LOW);
  radio.stopListening();

  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(D7, INPUT);
  pinMode(D6, INPUT);
  pinMode(D5, INPUT);
  pinMode(D4, INPUT);

  Serial.begin(9600);
}

// Grabs the controller data
void grab(Controller_t* data){
  data->x = joy_x;
  data->y = joy_y;
  data->t_button = top_button;
  data->b_button = bottom_button;
  data->e_switch = emergency_button;
  data->a_switch = auto_button;
}

void loop() {
  // Joystick Values
  joy_x = analogRead(A0);
  joy_y = analogRead(A1);
  // Button Values
  top_button = digitalRead(D6);
  bottom_button = digitalRead(D7);
  emergency_button = digitalRead(D5);
  auto_button = digitalRead(D4);

  // Grab data from the joysticks and button values
  Controller_t reading = {joy_x, joy_y, top_button, bottom_button, emergency_button, auto_button};

  //Sets the new values
  grab(&reading);

  // Sends data to the receiver
  radio.write(&reading, sizeof(struct Controller));
  delay(20);
  Serial.print("x: ");
  Serial.print(reading.x); //print joystick x
  Serial.print("\t"); //Tab Space  
  Serial.print("y: ");
  Serial.println(reading.y); //print joystick y  
  Serial.print("top_button: ");
  Serial.print(reading.t_button); //print top button
  Serial.print("\t"); //Tab Space 
  Serial.print("b_button: "); 
  Serial.print(reading.b_button); //print bottom button
  Serial.print("\t"); //Tab Space 
  Serial.print("e_switch: "); 
  Serial.println(reading.e_switch); //print emergency button
  Serial.print("\t"); //Tab Space 
  Serial.print("a_switch: "); 
  Serial.println(reading.a_switch); //print emergency button
}

 // TESTING SERIAL PRINTING

  // Serial.print("x: ");
  // Serial.print(reading.x); //print joystick x
  // Serial.print("\t"); //Tab Space  
  // Serial.print("y: ");
  // Serial.print(reading.y); //print joystick y
  // Serial.print("\n"); //Newline   
  // Serial.print("top_button: ");
  // Serial.print(reading.t_button); //print top button
  // Serial.print("\t"); //Tab Space 
  // Serial.print("b_button: "); 
  // Serial.println(reading.b_button); //print bottom button
  // Serial.print("e_button: "); 
  // Serial.println(reading.e_button); //print emergency button
