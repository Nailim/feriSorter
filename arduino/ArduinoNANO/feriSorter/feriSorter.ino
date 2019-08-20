#include <Servo.h>



// globalne spremenljivke ...

Servo servoMotor_dovod;         // premika bonbon iz hranilnika do senzorja ter od senzorja do tobogana
Servo servoMotor_tobogan;       // usmerja bonbon v pravi loncek

String serialString = "";
bool printString = false;       // prvic kar zacnemo sortirati brez uporabnika
bool stringComplete = true;     // prvic kar zacnemo sortirati brez uporabnika

int senzorVrednosti_kalibracija[] = {0, 0, 0, 0};     // zajete vrednosti brez bonbona uporabljene za kalibracijo (brez, rdeca, zelena, modra)
float senzorVrednost_faktor[] = {0.0, 0.0, 0.0, 0.0}; // faktor izracunan med kalibracijo za upostevanje ambientne svetlobe (brez, rdeca, zelena, modra)
int senzorVrednosti_meritev[] = {0, 0, 0, 0};         // zajete vrednosti uporabljene pri izracunu barve (brez, rdeca, zelena, modra)
int senzorVrednosti_izracun[] = {0, 0, 0, 0};         // izracunane vrednosti glede na ambientno svetlobo (brez, rdeca, zelena, modra)



// nastavitve za pravilno delovanje (se vedno globalne spremenljivke) ...

// nastavitve pinov - servo
int pinServoMotor_dovod = 10;
int pinServoMotor_tobogan = 11;

// nastavitve pozicij - servo
int servoPozicije_dovod[] = {10, 80, 165};                      // zgrabi bonbon, senzor, spusti bonbon 
int servoPozicije_tobogan[] = {90, 15, 45, 77, 105, 143, 175};  // brez bonbona, moder, zelen, rumen, rjavi, oranzen, rdec

// hranjenje lokacija pozicij (za reguliranje hitrosti premikanja)
int servoStaraPozicija_dovod = 2;
int servoStaraPozicija_tobogan = 0;

// nastavi cas med posameznim korakom servo motorja (za reguliranje hitrosti premikanja)
int servoCasCakanja = 10;       // merimo v milisekundah (funkcija "delay")

// nastavitve pinov - indikator svetilo LED
int pinIndikatorLED_rdeca = 3;  // svetilo indikator - RDECA - pin 3
int pinIndikatorLED_zelena = 5; // svetilo indikator - ZELENA - pin 5
int pinIndikatorLED_modra = 6;  // svetilo indikator - MODRA - pin 6

// nastavitve pinov - senzor svetilo LED
int pinSenzorLED_rdeca = 7;     // svetilo senzor - RDECA - pin 7
int pinSenzorLED_zelena = 8;    // svetilo senzor - ZELENA - pin 8
int pinSenzorLED_modra = 9;     // svetilo senzor - MODRA - pin 9

// nastavi vrednosti (8 bitni vmesnik: 0 - 255): {rdeca, zelena, modra} za mesanje barv pri svetilu za indikator
int barvaIndikatorLED_brez[] = {0, 0, 0};
int barvaIndikatorLED_modra[] = {0, 0, 200};
int barvaIndikatorLED_zelena[] = {0, 200, 0};
int barvaIndikatorLED_rumena[] = {200, 200, 0};
int barvaIndikatorLED_rjava[] = {200, 100, 0};
int barvaIndikatorLED_oranzna[] = {200, 150, 0};
int barvaIndikatorLED_rdeca[] = {200, 0, 0};

// nastavi VREDNOSTI SENZORJA (10 bitni pretvornik: 0 - 1023) za posamezno barvo bonbona
int barvaBonbonVrednost_brez[] = {626, 143, 195, 295};
int barvaBonbonVrednost_modra[] = {665, 455, 375, 351};
int barvaBonbonVrednost_zelena[] = {660, 443, 441, 518};
int barvaBonbonVrednost_rumena[] = {644, 163, 247, 410};
int barvaBonbonVrednost_rjava[] = {665, 363, 440, 505};
int barvaBonbonVrednost_oranzna[] = {653, 176, 329, 450};
int barvaBonbonVrednost_rdeca[] = {659, 206, 435, 501};

// nastavimo zaporedje imena barv katera uporabljamo
String barvaBonbon_ime[] = {"BREZ", "MODRA", "ZELENA", "RUMENA", "RJAVA", "ORANZNA", "RDECA"};


void setup() {
  // nastavimo in inicializiramo potrebne komponente za delovanje robota
  
  Serial.begin(9600);                               // vzpostavi povezavo na racunalnik za izpis informacij

  Serial.println("Zagon robota ...");               // obvestimo uporabnika da se robot zaganja

  servoMotor_dovod.attach(pinServoMotor_dovod);     // priklopi kontrolni signal PWM na pin 10
  delay(500);
  servoMotor_tobogan.attach(pinServoMotor_tobogan); // priklopi kontrolni signal PWM na pin 11
  delay(500);
  
  // nastavi pine za LED svetilo za indikator
  pinMode(pinIndikatorLED_rdeca, OUTPUT);
  pinMode(pinIndikatorLED_zelena, OUTPUT);
  pinMode(pinIndikatorLED_modra, OUTPUT);

  // nastavi pine za LED svetilo za senzor
  pinMode(pinSenzorLED_rdeca, OUTPUT);
  pinMode(pinSenzorLED_zelena, OUTPUT);
  pinMode(pinSenzorLED_modra, OUTPUT);



  kalibracijaSenzorja();  // poklicimo funkcijo za kalibriranje senzorja
}



void loop() {
  // sortirajmo dokler zaznavamo bonbone

  int kateriBonbon = 0;     // spremenljivka v kateri shranimo zaznan bonbon

  bool smoKoncali = false;  // spremenljivka v kateri hranimo stanje ali se zaznavamo bonbone

  // ce ne zaznamo vec bonbonov obvestimo uporabnika da lahko zacnemo znova
  if (printString){
    Serial.println("Pritisni ENTER za ponovno sortiranje");
    Serial.println("");
    printString = false;
    smoKoncali = false;
  }

  // ce smo prejeli ukaz za sortiranje in se imamo bonbone, potem sortirajmo
  if (stringComplete) {
    while (!smoKoncali) {
      // ugasni indikator
      nastaviSvetilo_indikator(0);

      // obvesti uporabnika da obdelujemo nov bonbon
      Serial.println("Izvajanje nove meritve ...");

      // zagrabi bonbon
      nastaviServo_dovod(0, false);
      delay(500);

      // nesi bonbon do senzorja
      nastaviServo_dovod(1, false);
      delay(500);

      // izmeri brez luci
      nastaviSvetilo_senzor(0);
      delay(100);
      senzorVrednosti_meritev[0] = zajamiVrednostSenzorja();

      // izmeri rdeco luc
      nastaviSvetilo_senzor(1);
      delay(100);
      senzorVrednosti_meritev[1] = zajamiVrednostSenzorja();

      // izmeri zeleno luc
      nastaviSvetilo_senzor(2);
      delay(100);
      senzorVrednosti_meritev[2] = zajamiVrednostSenzorja();

      // izmeri modro luc
      nastaviSvetilo_senzor(3);
      delay(100);
      senzorVrednosti_meritev[3] = zajamiVrednostSenzorja();

      // ugasni
      nastaviSvetilo_senzor(0);

      // izracunaj vrednosti glede na ambientno svetlobo
      senzorVrednosti_izracun[0] = senzorVrednost_faktor[0] * (float)senzorVrednosti_meritev[0];
      senzorVrednosti_izracun[1] = senzorVrednost_faktor[1] * (float)senzorVrednosti_meritev[1];
      senzorVrednosti_izracun[2] = senzorVrednost_faktor[2] * (float)senzorVrednosti_meritev[2];
      senzorVrednosti_izracun[3] = senzorVrednost_faktor[3] * (float)senzorVrednosti_meritev[3];

      // izpisi meritve uporabniku
      Serial.print("Vrednosti senzorja: ");
      Serial.print(String(senzorVrednosti_meritev[0]) + ", ");
      Serial.print(String(senzorVrednosti_meritev[1]) + ", ");
      Serial.print(String(senzorVrednosti_meritev[2]) + ", ");
      Serial.print(String(senzorVrednosti_meritev[3]));
      Serial.println("");
      Serial.print("Izracunane vrednosti: ");
      Serial.print(String(senzorVrednosti_izracun[0]) + ", ");
      Serial.print(String(senzorVrednosti_izracun[1]) + ", ");
      Serial.print(String(senzorVrednosti_izracun[2]) + ", ");
      Serial.print(String(senzorVrednosti_izracun[3]));
      Serial.println("");

      // izracunajmo barvo bonbona
      kateriBonbon = uganiBarvoBonbona();

      // prizgemo indikator z zaznano barvo
      nastaviSvetilo_indikator(kateriBonbon);

      // izpisemo rezultat
      Serial.println("Izbrani indeks (barva): " + String(kateriBonbon) + " (" + String(barvaBonbon_ime[kateriBonbon]) + ")");
      
      // spustimo bonbon na izracunano mesto
      nastaviServo_tobogan(kateriBonbon, false);
      delay(500);
      nastaviServo_dovod(2, false);
      delay(500);

      // ce nismo zaznali bonbona koncamo
      if (kateriBonbon == 0){
        smoKoncali = true;
      }
      
      Serial.println("");
    }

    printString = true;
    stringComplete = false;
  }
}



int uganiBarvoBonbona() {
  // izracunaj razliko (razdaljo) vseh shranjenih barv proti zaznanim vrednostim iz senzorja
  // najmanjsa razlika (razdaljo) pomeni najvecja verjetnost da je bonbon tiste barve

  // barvna lestvica vrednosti
  // vrednost 0 - brez bonbona
  // vrednost 1 - modri bonbon
  // vrednost 2 - zeleni bonbon
  // vrednost 3 - rumeni bonbon
  // vrednost 4 - rjavi bonbon
  // vrednost 5 - oranzni bonbon
  // vrednost 6 - rdeci bonbon 

  int izracunaRazlika[7] = {0, 0, 0, 0, 0, 0, 0,};  // brez, modra, zelena, rumena, rjava, oranzna, rdeca
  int izbranaRazlika = 0;

  int zacasnaVrednost = 0;

  // izracunaj razliko (razdaljo) od vrednosti brez bonbona
  zacasnaVrednost = barvaBonbonVrednost_brez[1] - senzorVrednosti_izracun[1]; // razlika rdece barve
  izracunaRazlika[0] = izracunaRazlika[0] + abs(zacasnaVrednost);
  zacasnaVrednost = barvaBonbonVrednost_brez[2] - senzorVrednosti_izracun[2]; // razlika zelene barve
  izracunaRazlika[0] = izracunaRazlika[0] + abs(zacasnaVrednost);
  zacasnaVrednost = barvaBonbonVrednost_brez[3] - senzorVrednosti_izracun[3]; // razlika modre barve
  izracunaRazlika[0] = izracunaRazlika[0] + abs(zacasnaVrednost);

  // izracunaj razliko (razdaljo) od vrednosti modrega bonbona
  zacasnaVrednost = barvaBonbonVrednost_modra[1] - senzorVrednosti_izracun[1]; // razlika rdece barve
  izracunaRazlika[1] = izracunaRazlika[1] + abs(zacasnaVrednost);
  zacasnaVrednost = barvaBonbonVrednost_modra[2] - senzorVrednosti_izracun[2]; // razlika zelene barve
  izracunaRazlika[1] = izracunaRazlika[1] + abs(zacasnaVrednost);
  zacasnaVrednost = barvaBonbonVrednost_modra[3] - senzorVrednosti_izracun[3]; // razlika modre barve
  izracunaRazlika[1] = izracunaRazlika[1] + abs(zacasnaVrednost);

  // izracunaj razliko (razdaljo) od vrednosti zelenega bonbona
  zacasnaVrednost = barvaBonbonVrednost_zelena[1] - senzorVrednosti_izracun[1]; // razlika rdece barve
  izracunaRazlika[2] = izracunaRazlika[2] + abs(zacasnaVrednost);
  zacasnaVrednost = barvaBonbonVrednost_zelena[2] - senzorVrednosti_izracun[2]; // razlika zelene barve
  izracunaRazlika[2] = izracunaRazlika[2] + abs(zacasnaVrednost);
  zacasnaVrednost = barvaBonbonVrednost_zelena[3] - senzorVrednosti_izracun[3]; // razlika modre barve
  izracunaRazlika[2] = izracunaRazlika[2] + abs(zacasnaVrednost);

  // izracunaj razliko (razdaljo) od vrednosti rumenega bonbona
  zacasnaVrednost = barvaBonbonVrednost_rumena[1] - senzorVrednosti_izracun[1]; // razlika rdece barve
  izracunaRazlika[3] = izracunaRazlika[3] + abs(zacasnaVrednost);
  zacasnaVrednost = barvaBonbonVrednost_rumena[2] - senzorVrednosti_izracun[2]; // razlika zelene barve
  izracunaRazlika[3] = izracunaRazlika[3] + abs(zacasnaVrednost);
  zacasnaVrednost = barvaBonbonVrednost_rumena[3] - senzorVrednosti_izracun[3]; // razlika modre barve
  izracunaRazlika[3] = izracunaRazlika[3] + abs(zacasnaVrednost);

  // izracunaj razliko (razdaljo) od vrednosti rjavega bonbona
  zacasnaVrednost = barvaBonbonVrednost_rjava[1] - senzorVrednosti_izracun[1]; // razlika rdece barve
  izracunaRazlika[4] = izracunaRazlika[4] + abs(zacasnaVrednost);
  zacasnaVrednost = barvaBonbonVrednost_rjava[2] - senzorVrednosti_izracun[2]; // razlika zelene barve
  izracunaRazlika[4] = izracunaRazlika[4] + abs(zacasnaVrednost);
  zacasnaVrednost = barvaBonbonVrednost_rjava[3] - senzorVrednosti_izracun[3]; // razlika modre barve
  izracunaRazlika[4] = izracunaRazlika[4] + abs(zacasnaVrednost);

  // izracunaj razliko (razdaljo) od vrednosti oranznega bonbona
  zacasnaVrednost = barvaBonbonVrednost_oranzna[1] - senzorVrednosti_izracun[1]; // razlika rdece barve
  izracunaRazlika[5] = izracunaRazlika[5] + abs(zacasnaVrednost);
  zacasnaVrednost = barvaBonbonVrednost_oranzna[2] - senzorVrednosti_izracun[2]; // razlika zelene barve
  izracunaRazlika[5] = izracunaRazlika[5] + abs(zacasnaVrednost);
  zacasnaVrednost = barvaBonbonVrednost_oranzna[3] - senzorVrednosti_izracun[3]; // razlika modre barve
  izracunaRazlika[5] = izracunaRazlika[5] + abs(zacasnaVrednost);

  // izracunaj razliko (razdaljo) od vrednosti rdecega bonbona
  zacasnaVrednost = barvaBonbonVrednost_rdeca[1] - senzorVrednosti_izracun[1]; // razlika rdece barve
  izracunaRazlika[6] = izracunaRazlika[6] + abs(zacasnaVrednost);
  zacasnaVrednost = barvaBonbonVrednost_rdeca[2] - senzorVrednosti_izracun[2]; // razlika zelene barve
  izracunaRazlika[6] = izracunaRazlika[6] + abs(zacasnaVrednost);
  zacasnaVrednost = barvaBonbonVrednost_rdeca[3] - senzorVrednosti_izracun[3]; // razlika modre barve
  izracunaRazlika[6] = izracunaRazlika[6] + abs(zacasnaVrednost);

  // poisci najmanjso razliko (razdaljo)
  for (int indeks = 0; indeks < 7; indeks++){
    if (izracunaRazlika[indeks] < izracunaRazlika[izbranaRazlika]){
      izbranaRazlika = indeks;
    }
  }

  // obvestimo uporabnika o vrednostih
  Serial.print("Izracunane razdalje: ");
  Serial.print(String(izracunaRazlika[0]) + ", ");
  Serial.print(String(izracunaRazlika[1]) + ", ");
  Serial.print(String(izracunaRazlika[2]) + ", ");
  Serial.print(String(izracunaRazlika[3]) + ", ");
  Serial.print(String(izracunaRazlika[4]) + ", ");
  Serial.print(String(izracunaRazlika[5]) + ", ");
  Serial.print(String(izracunaRazlika[6]));
  Serial.println("");
  
  return izbranaRazlika;
}


void kalibracijaSenzorja() {
  // preberimo vrednosti iz senzorja brez bonbona za kalibracijo ambientne svetlobe
  // te vrednosti uporabimo kasneje za NORMALIZACIJO prebranih vrednosti z bonbonom
  
  // obvestimo uporabnika da izvajamo kalibracijo senzorja robota
  Serial.println("Kalibracija senzorja:");

  // nastavimo pozicijo dovoda na senzor
  nastaviServo_dovod(1, true);
  delay(500);

  // izvedi meritev brez svetlobe
  nastaviSvetilo_senzor(0);
  delay(250);
  senzorVrednosti_kalibracija[0] = zajamiVrednostSenzorja();

  // izvedi meritev za rdeco svetlobo
  nastaviSvetilo_senzor(1);
  delay(250);
  senzorVrednosti_kalibracija[1] = zajamiVrednostSenzorja();

  // izvedi meritev za zeleno svetlobo
  nastaviSvetilo_senzor(2);
  delay(250);
  senzorVrednosti_kalibracija[2] = zajamiVrednostSenzorja();

  // izvedi meritev za modro svetlobo
  nastaviSvetilo_senzor(3);
  delay(250);
  senzorVrednosti_kalibracija[3] = zajamiVrednostSenzorja();

  // ugasni luc
  nastaviSvetilo_senzor(0);

  // izracunaj korekcijski faktor za upostevanje ambientne svetlobe
  senzorVrednost_faktor[0] = (float)barvaBonbonVrednost_brez[0] / (float)senzorVrednosti_kalibracija[0];
  senzorVrednost_faktor[1] = (float)barvaBonbonVrednost_brez[1] / (float)senzorVrednosti_kalibracija[1];
  senzorVrednost_faktor[2] = (float)barvaBonbonVrednost_brez[2] / (float)senzorVrednosti_kalibracija[2];
  senzorVrednost_faktor[3] = (float)barvaBonbonVrednost_brez[3] / (float)senzorVrednosti_kalibracija[3];

  // izpisimo rezultat
  Serial.print("Kalibracijske vrednosti: ");
  Serial.print(String(senzorVrednosti_kalibracija[0]) + ", ");
  Serial.print(String(senzorVrednosti_kalibracija[1]) + ", ");
  Serial.print(String(senzorVrednosti_kalibracija[2]) + ", ");
  Serial.print(String(senzorVrednosti_kalibracija[3]));
  Serial.println("");

  // izpisimo rezultat
  Serial.print("Korekcijski fakstor: ");
  Serial.print(String(senzorVrednost_faktor[0], 3) + ", ");
  Serial.print(String(senzorVrednost_faktor[1], 3) + ", ");
  Serial.print(String(senzorVrednost_faktor[2], 3) + ", ");
  Serial.print(String(senzorVrednost_faktor[3], 3));
  Serial.println("");
  
  Serial.println("");
}



int zajamiVrednostSenzorja() {
  // izvedemo meritev senzorja svetlobe s AD pretvornikom
  
  int sestevekVrednosti = 0;

  // zaradi suma pri izvajanju meritev s senzorjem
  // koncno vrednost izboljsamo z vecimi meritvami katere na koncu povprecimo
  for (int i = 0; i < 32; i ++){
    sestevekVrednosti = sestevekVrednosti + analogRead(A0); // izmerimo vrednost na senzorju in jo pristejemo k vsem meritvam
    delay(10);                                              // pocakamo trenutek da se mikrokrmilnik lahko pripravi na naslednjo meritev
  }
  
  return sestevekVrednosti / 32;  // sestete meritve povprecimo s stevilom meritev in vrnemo celo stevilo (funkcija tipa int)
}



void nastaviServo_dovod(int pozicija, bool hitro) {
  // nastavi pozicijo servo motorja, 3 pozicije (0-2), ter hitrost obracanja (true-hitro, false-pocasi)
  
  if (hitro) {
    // hitro premikanje, direktno premakni servo na zeleno pozicijo
    servoMotor_dovod.write(servoPozicije_dovod[pozicija]);
    
  }
  else {
    // pocasno premikanje, korakaj servo na zeleno pozicijo s pavzo na vsakem koraku
    if (servoPozicije_dovod[pozicija] > servoPozicije_dovod[servoStaraPozicija_dovod]) {
      // ce je nova pozicija vecja od stare
      for (int i = servoPozicije_dovod[servoStaraPozicija_dovod]; i <= servoPozicije_dovod[pozicija]; i++) {
        servoMotor_dovod.write(i);
        delay(servoCasCakanja);
      } 
    } else {
      // ce je nova pozicija manjsa od stare
      for (int i = servoPozicije_dovod[servoStaraPozicija_dovod]; i >= servoPozicije_dovod[pozicija]; i--) {
        servoMotor_dovod.write(i);
        delay(servoCasCakanja);
      } 
    }
  }

  servoStaraPozicija_dovod = pozicija;  // shranimo si trenutno pozicijo za kasneje
}



void nastaviServo_tobogan(int pozicija, bool hitro) {
  // nastavi pozicijo servo motorja, 6 pozicij (0-5), ter hitrost obracanja (true-hitro, false-pocasi)
  
  if (hitro) {
    // hitro premikanje, direktno premakni servo na zeleno pozicijo
    servoMotor_tobogan.write(servoPozicije_tobogan[pozicija]);
    
  }
  else {
    // pocasno premikanje, korakaj servo na zeleno pozicijo s pavzo na vsakem koraku
    if (servoPozicije_tobogan[pozicija] > servoPozicije_tobogan[servoStaraPozicija_tobogan]) {
      // ce je nova pozicija vecja od stare
      for (int i = servoPozicije_dovod[servoStaraPozicija_tobogan]; i <= servoPozicije_tobogan[pozicija]; i++) {
        servoMotor_tobogan.write(i);
        delay(servoCasCakanja);
      } 
    } else {
      // ce je nova pozicija manjsa od stare
      for (int i = servoPozicije_tobogan[servoStaraPozicija_tobogan]; i >= servoPozicije_tobogan[pozicija]; i--) {
        servoMotor_tobogan.write(i);
        delay(servoCasCakanja);
      } 
    }
  }

  servoStaraPozicija_tobogan = pozicija;  // shranimo si trenutno pozicijo za kasneje
}



void nastaviSvetilo_senzor(int barva) {
  // barve gredo od 0 do 4
  // 0 - ugasnjeno svetilo
  // 1 - prizgana samo RDECA barva
  // 2 - prizgana samo ZELENA barva
  // 3 - prizgana samo MODRA barva

  if (barva == 0) {
    digitalWrite(pinSenzorLED_rdeca, LOW);
    digitalWrite(pinSenzorLED_zelena, LOW);
    digitalWrite(pinSenzorLED_modra, LOW);
  }
  else if (barva == 1) {
    digitalWrite(pinSenzorLED_rdeca, HIGH);
    digitalWrite(pinSenzorLED_zelena, LOW);
    digitalWrite(pinSenzorLED_modra, LOW);
  }
  else if (barva == 2) {
    digitalWrite(pinSenzorLED_rdeca, LOW);
    digitalWrite(pinSenzorLED_zelena, HIGH);
    digitalWrite(pinSenzorLED_modra, LOW);
  }
  else if (barva == 3) {
    digitalWrite(pinSenzorLED_rdeca, LOW);
    digitalWrite(pinSenzorLED_zelena, LOW);
    digitalWrite(pinSenzorLED_modra, HIGH);
  }
}



void nastaviSvetilo_indikator(int barva) {
  // barve gredo od 0 do 6
  // ugasnjen, modra, zelena, rumena, rjava, oranzna, rdeca

  if (barva == 0) {
    analogWrite(pinIndikatorLED_rdeca, barvaIndikatorLED_brez[0]);
    analogWrite(pinIndikatorLED_zelena, barvaIndikatorLED_brez[1]);
    analogWrite(pinIndikatorLED_modra, barvaIndikatorLED_brez[2]);

  }
  else if (barva == 1) {
    analogWrite(pinIndikatorLED_rdeca, barvaIndikatorLED_modra[0]);
    analogWrite(pinIndikatorLED_zelena, barvaIndikatorLED_modra[1]);
    analogWrite(pinIndikatorLED_modra, barvaIndikatorLED_modra[2]);

  }
  else if (barva == 2) {
    analogWrite(pinIndikatorLED_rdeca, barvaIndikatorLED_zelena[0]);
    analogWrite(pinIndikatorLED_zelena, barvaIndikatorLED_zelena[1]);
    analogWrite(pinIndikatorLED_modra, barvaIndikatorLED_zelena[2]);
  }
  else if (barva == 3) {
    analogWrite(pinIndikatorLED_rdeca, barvaIndikatorLED_rumena[0]);
    analogWrite(pinIndikatorLED_zelena, barvaIndikatorLED_rumena[1]);
    analogWrite(pinIndikatorLED_modra, barvaIndikatorLED_rumena[2]);
  }
  else if (barva == 4) {
    analogWrite(pinIndikatorLED_rdeca, barvaIndikatorLED_rjava[0]);
    analogWrite(pinIndikatorLED_zelena, barvaIndikatorLED_rjava[1]);
    analogWrite(pinIndikatorLED_modra, barvaIndikatorLED_rjava[2]);
  }
  else if (barva == 5) {
    analogWrite(pinIndikatorLED_rdeca, barvaIndikatorLED_oranzna[0]);
    analogWrite(pinIndikatorLED_zelena, barvaIndikatorLED_oranzna[1]);
    analogWrite(pinIndikatorLED_modra, barvaIndikatorLED_oranzna[2]);
  }
  else if (barva == 6) {
    analogWrite(pinIndikatorLED_rdeca, barvaIndikatorLED_rdeca[0]);
    analogWrite(pinIndikatorLED_zelena, barvaIndikatorLED_rdeca[1]);
    analogWrite(pinIndikatorLED_modra, barvaIndikatorLED_rdeca[2]);
  }
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
