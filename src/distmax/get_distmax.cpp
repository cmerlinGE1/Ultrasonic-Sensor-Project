#include <Arduino.h>

// 
// Obtenir la distance maximum dans la dropbox
// ===========================================

// Définition des broches
const int trigPin = 9;  // Broche TRIG du capteur
const int echoPin = 10; // Broche ECHO du capteur

// Variables
long duration;  // Durée de l'impulsion en microsecondes
int distanceInitiale;
int distance;   // Distance calculée en cm
int cpt=0;

int lectureDistance() {
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
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  // Mise à 0 de la broche TRIG
  digitalWrite(trigPin, LOW);
}

void loop() {
  
  // Lecture de la distance
  distanceInitiale = lectureDistance();
  Serial.print(cpt);
  Serial.print(" - Distance initiale : ");
  Serial.print(distanceInitiale);
  Serial.println(" cm");
  cpt++;

  // Attente de 500 ms avant la prochaine mesure
  delay(500);
}