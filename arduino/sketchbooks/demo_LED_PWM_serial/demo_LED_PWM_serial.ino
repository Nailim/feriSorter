String serialString = "";
bool stringComplete = false;

bool printString_R = true;
bool printString_G = false;
bool printString_B = false;

bool saveString_R = false;
bool saveString_G = false;
bool saveString_B = false;

int led_R = 0;
int led_G = 0;
int led_B = 0;


void setup() {
  serialString.reserve(3);
  
  Serial.begin(9600);

  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
}

void loop() {
  // preberimo barvo luci
  if (printString_R) {
    Serial.println("Vpisi rdeco barvo (0-255)");
    printString_R = false;

    saveString_R = true;
  }
  if (printString_G) {
    Serial.println("Vpisi zeleno barvo (0-255)");
    printString_G = false;
    
    saveString_G = true;
  }
  if (printString_B) {
    Serial.println("Vpisi modro barvo (0-255)");
    printString_B = false;

    saveString_B = true;
  }
  
  if (stringComplete) {
    Serial.println(serialString);

    if (saveString_R) {
      led_R = serialString.toInt();
      saveString_R = false;
      printString_G = true;
    }

    if (saveString_G) {
      led_G = serialString.toInt();
      saveString_G = false;
      printString_B = true;
    }

    if (saveString_B) {
      led_B = serialString.toInt();
      saveString_B = false;
      printString_R = true;
    }
    
    
    serialString = "";
    stringComplete = false;
  }

  // nastavimo vrednost led svetilom
  analogWrite(9, led_B);
  analogWrite(10, led_G);
  analogWrite(11, led_R);

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
