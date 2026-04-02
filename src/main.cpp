#include <Arduino.h>

// Lecture de distance avec 2 capteurs HC-SR04
// ===========================================

// Configuration du test : 1 = capteur 1, 2 = capteur 2
#define TEST 2

// Définition des broches
const int trigPin1 = 10;  // Broche TRIG du capteur 1
const int echoPin1 = 11; // Broche ECHO du capteur 1
const int trigPin2 = 8;  // Broche TRIG du capteur 2
const int echoPin2 = 9; // Broche ECHO du capteur 2

// Variables
long duration;  // Durée de l'impulsion en microsecondes
int distance;   // Distance calculée en cm

int lectureDistance(int trigPin, int echoPin) {
  // Génération d'une impulsion de 10µs sur TRIG
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Lecture de la durée de l'impulsion sur ECHO
  duration = pulseIn(echoPin, HIGH);
  
  // Calcul de la distance en cm
  // Vitesse du son = 340 m/s = 0.034 cm/µs
  // Distance = (durée * vitesse) / 2
  distance = duration * 0.034 / 2;
  
  return distance;
}

void setup() {
  // Initialisation de la communication série
  Serial.begin(9600);
  
  // Configuration des broches
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  
  // Mise à 0 de la broche TRIG
  digitalWrite(trigPin1, LOW);
  digitalWrite(trigPin2, LOW);
  
  Serial.println("Capteur HC-SR04 - Lecture de distance");
  Serial.println("=====================================");
}

void loop() {
  //#if TEST == 1
  // Lecture de la distance du capteur 1
  unsigned long before_mes = millis();
  distance = lectureDistance(trigPin1, echoPin1);
  unsigned long after_mes = millis();

  // Affichage de la distance sur le terminal
  Serial.print("d1 = ");
  Serial.print(distance);
  Serial.println(" cm");

  if(distance < 51) {
    Serial.println("Obstacle détecté à moins de 52 cm !");
    delay(2); // tempo pour éviter la détection multiple d'un colis
  }

  // détection d'un temps de mesure anormalement long (bug du capteur)
  if((after_mes - before_mes) > 5){
    Serial.print("Temps moyen de mesure : ");
    Serial.print(after_mes - before_mes);
    Serial.println(" ms");
  }

  delay(12); // délai idéal pour éviter de recevoir les echos d'une mesure précédente et suffisant pour ne pas manquer un colis

  //#elif TEST == 2
  // Lecture de la distance du capteur 2
  distance = lectureDistance(trigPin2, echoPin2);
  
  // Affichage de la distance sur le terminal
  Serial.print("d2 = ");  
  Serial.print(distance);
  Serial.println(" cm");

  if(distance < 55) {
    Serial.println("Obstacle détecté à moins de 56 cm !");
    delay(2); // tempo pour éviter la détection multiple d'un colis
  }
  
  delay(12); // délai idéal pour éviter de recevoir les echos d'une mesure précédente et suffisant pour ne pas manquer un colis
  //#endif
}