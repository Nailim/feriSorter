#include <Servo.h>

Servo mojServo

int mojServo_pozicija = 90; // nastavi pozicijo servo motorja

String serialString = "";
bool printString = true;
bool stringComplete = false;

void setup() {
  serialString.reserve(3);
  
  Serial.begin(9600);

  mojServo.attach(5);  // nastavi pravi pin na akterega je priklopjen servo motor
  
}

void loop() {

  if (printString) {
    Serial.println("Vpisi servo pozicijo");
    printString = false;
  }
  if (stringComplete) {
    Serial.println(serialString);
    mojServo_pozicija = serialString.toInt();
    serialString = "";
    stringComplete = false;
    printString = true;
  }

  
  
  // premakni servo na pozicijo med 0 in 180 stopinj (10 in 170 ce cudno ropota)  
  mojServo.write(mojServo_pozicija);
  delay(1000);  
}



void serialEvent() {
  while (Serial.available()) {
    
    char inChar = (char)Serial.read();
    
    serialString += inChar;
    
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}

