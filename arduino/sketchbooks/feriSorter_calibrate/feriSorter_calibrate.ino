#include <Servo.h>
 
Servo servo_krog; // pripravimo spremenljivko za servo
 
String serialString = ""; // pripravimo spremenljivko za tekst

// pripravimo spremenljivke za logiko svetil
bool led_R = false;
bool led_G = false;
bool led_B = false;
bool led_off = false;

// !!! nastavi pine na katerih so priplopjene barve
int pin_R = 4;
int pin_G = 2;
int pin_B = 3;
 
int servoPozicija = 90; // zacetna servo pozicija

// !!! nastavi pozicije za servo motor
int servoPozicija_zagrabi = 10;
int servoPozicija_senzor = 75;
int servoPozicija_spusti = 170;
 
 
int senzor = 0; // pripravi spremenljivko za vrednost senzorja
 
void setup() {
  serialString.reserve(3);
 
  Serial.begin(9600);
 
  servo_krog.attach(5); // !!! nastavi pin na katerega je priklopjen servo

  
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
  delay(50);
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
          led_R = true;
        }
      }
      if (serialString.indexOf("g") >= 0) {
        if (led_G) {
          led_G = false;
        } else {
          led_G = true;
        }
      }
      if (serialString.indexOf("b") >= 0) {
        if (led_B) {
          led_B = false;
        } else {
          led_B = true;
        }
      }
      if (serialString.indexOf("o") >= 0) {
        led_R = false;
        led_G = false;
        led_B = false;
      }
 
      if (serialString.indexOf("s") >= 0) {
        switch (servoPozicija) {
          case 10:
            servoPozicija = servoPozicija_zagrabi;
            break;
          case 75:
            servoPozicija = servoPozicija_senzor;
            break;
          case 170:
            servoPozicija = servoPozicija_spusti;
            break;  
        }
      }
 
      serialString = "";
    }
  }
}
