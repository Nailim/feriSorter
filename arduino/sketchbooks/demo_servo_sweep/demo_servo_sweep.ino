#include <Servo.h>

Servo myservo;

int pos = 0;

void setup() {
  myservo.attach(10);
}

void loop() {

  myservo.write(90);

  delay(500);

  for (pos = 90; pos <= 175; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(5);                       // waits 15ms for the servo to reach the position
  }
  
  delay(500);
  
  for (pos = 175; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(5);                       // waits 15ms for the servo to reach the position
  }
  
  delay(500);
  
  for (pos = 0; pos <=90; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(5);                       // waits 15ms for the servo to reach the position
  }

}
