void setup() {
  // put your setup code here, to run once:
  pinMode(13, OUTPUT);   // ENG: set to correct pin
                         // SLO: nastavi na pravi pin
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(13, HIGH);   // ENG: set pin to high state - logic 1 (connect electricity)
                            // SLO: nastavimo pin na visoko stanje - logina 1 (priklopimo elektriko)
  
  delay(1000);              // ENG: wait 1 second (1000 miliseconds)
                            // SLO: pocakamo 1 sekundo (1000 milisekund) 
  
  digitalWrite(13, LOW);   // ENG: set pin to low state - logic 0 (disconnect electricity)
                            // SLO: nastavimo pin na nizko stanje - logina 0 (odklopimo elektriko)
  
  delay(1000);              // ENG: wait 1 second (1000 miliseconds)
                            // SLO: pocakamo 1 sekundo (1000 milisekund) 
}
