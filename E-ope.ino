const int led1 = 2; // võtab digitalPin ja muudab selle stringiks
const int led2 = 3;
const int led3 = 4;
const int led4 = 5;
int patternCount = 0; // Tsükli kogus

void setup() {
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  Serial.begin(9600); // Käivitab seerial monitori
}

void loop() {
  for (int i = 0; i < 5; i++) { // Tsükkel loeb 0-5, et tööle ja kinni minna
    patternCount++; // Lisab numbri, et lugeda mitu tsüklit teinud on.
    Serial.print("Tsükkel käivitatud: "); // Tekst siis mitu tsükklit on olnud
    Serial.println(patternCount);
    
    digitalWrite(led1, HIGH); // led sisse
    delay(500);               // töötab 500 ms
    digitalWrite(led1, LOW);  // led kinni

    digitalWrite(led2, HIGH); 
    delay(500);              
    digitalWrite(led2, LOW);  

    digitalWrite(led3, HIGH); 
    delay(500);             
    digitalWrite(led3, LOW);
    
    digitalWrite(led4, HIGH); 
    delay(500);             
    digitalWrite(led3, LOW);  
  }
}
// Leidsin projekti mis Rainiga tegime ja tsükli siit: https://www.instructables.com/Running-LEDs-Arduino-Uno/