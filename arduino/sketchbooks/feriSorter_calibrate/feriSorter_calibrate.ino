#include <Servo.h>
 
Servo servo_krog; // pripravimo spremenljivko za servo
 
String serialString = ""; // pripravimo spremenljivko za tekst

// pripravimo spremenljivke za logiko svetil
bool led_R = false;
bool led_G = false;
bool led_B = false;
bool led_off = true;

// !!! nastavi pine na katerih so priplopjene barve
int pin_R = 7;
int pin_G = 8;
int pin_B = 9;

// !!! nastavi pozicije za servo motor
const int servoPozicija_zagrabi = 10;
const int servoPozicija_senzor = 105;
const int servoPozicija_spusti = 180;

int servoPozicija = servoPozicija_spusti; // zacetna servo pozicija

int senzor = 0; // pripravi spremenljivko za vrednost senzorja
 
void setup() {
  serialString.reserve(3);
 
  Serial.begin(9600);
 
  servo_krog.attach(10); // !!! nastavi pin na katerega je priklopjen servo

  
  pinMode(pin_R, OUTPUT);
  pinMode(pin_G, OUTPUT);
  pinMode(pin_B, OUTPUT);

  // izpisi navodila
  Serial.println("Za kontrolo svetila posiljaj znake r, g, b, o.");
  Serial.println("Za kontrolo servo motorja posiljaj znak s.");
}
 
void loop() {
  // put your main code here, to run repeatedly:
 
  if (led_R) {
    digitalWrite(pin_G, LOW);
    digitalWrite(pin_B, LOW);
    
    digitalWrite(pin_R, HIGH);
    Serial.print("svetilo R - vrednost senzorja = ");
  } else {
    digitalWrite(pin_R, LOW);
  }
 
  if (led_G) {
    Serial.print("svetilo G - vrednost senzorja = ");
    digitalWrite(pin_G, HIGH);
  } else {
    digitalWrite(pin_G, LOW);
  }
 
  if (led_B) {
    Serial.print("svetilo B - vrednost senzorja = ");
    digitalWrite(pin_B, HIGH);
  } else {
    digitalWrite(pin_B, LOW);
  }

  if (led_off) {
    Serial.print("svetilo brez - vrednost senzorja = ");
    digitalWrite(pin_R, LOW);
    digitalWrite(pin_G, LOW);
    digitalWrite(pin_B, LOW);
  }
 
  // preberimo vrednost iz senzorja
  senzor = analogRead(A0);
  // izpisimo vrednost senzorja
  Serial.println(senzor);
 
  // premakni servo
  servo_krog.write(servoPozicija);
 
  // pocakajmo da se senzor umiri
  delay(250);
}
 
void serialEvent() {
  while (Serial.available()) {
   
    char inChar = (char)Serial.read();
 
    serialString += inChar;
   
    if (inChar == '\n') {
     
      if (serialString.indexOf("r") >= 0) {
        if (led_R) {
          led_R = false;
        } else {
          led_off = false;
          led_R = true;
          led_G = false;
          led_B = false;
        }
      }
      if (serialString.indexOf("g") >= 0) {
        if (led_G) {
          led_G = false;
        } else {
          led_off = false;
          led_G = true;
          led_R = false;
          led_B = false;
        }
      }
      if (serialString.indexOf("b") >= 0) {
        if (led_B) {
          led_B = false;
        } else {
          led_off = false;
          led_B = true;
          led_R = false;
          led_G = false;
        }
      }
      if (serialString.indexOf("o") >= 0) {
        if (led_off) {
          led_off = false;
        }
        led_off = true;
        
        led_R = false;
        led_G = false;
        led_B = false;
      }
 
      if (serialString.indexOf("s") >= 0) {
        switch (servoPozicija) {
          case servoPozicija_spusti:
            servoPozicija = servoPozicija_zagrabi;
            break;
          case servoPozicija_zagrabi:
            servoPozicija = servoPozicija_senzor;
            break;
          case servoPozicija_senzor:
            servoPozicija = servoPozicija_spusti;
            break;  
        }
      }
 
      serialString = "";
    }
  }
}
