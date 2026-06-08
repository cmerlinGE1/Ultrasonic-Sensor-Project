#include <Arduino.h>

// Lecture de distance avec 2 capteurs HC-SR04
// ===========================================

// Définition des broches
const int trigPin1 = 10;  // Broche TRIG du capteur 1
const int echoPin1 = 11; // Broche ECHO du capteur 1
const int trigPin2 = 8;  // Broche TRIG du capteur 2
const int echoPin2 = 9; // Broche ECHO du capteur 2

// Seuils de détection
const int seuil1 = 51;
const int seuil2 = 55;

// Variables
long duration;  // Durée de l'impulsion en microsecondes
int distance;   // Distance calculée en cm
bool enPause = false;

int lectureDistance(int trigPin, int echoPin) {
  // Génération d'une impulsion de 10µs sur TRIG
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // --- Lecture de la durée de l'impulsion sur ECHO ---
  // avec Timeout de 30000 us = 30 ms
  duration = pulseIn(echoPin, HIGH, 30000);

  // Si timeout ou pas d'écho
  if (duration == 0) {
    return 999; // valeur hors plage
  }
  
  // --- Calcul de la distance en cm ---
  // Vitesse du son = 340 m/s = 0.034 cm/µs
  // Distance = (durée * vitesse) / 2
  distance = duration * 0.034 / 2;
  
  return distance;
}

void attendreEntree() {
  if (Serial.available() > 0) {
    String ligne = Serial.readStringUntil('\n');
    ligne.trim(); // enlève \r et espaces éventuels

    enPause = false;
    Serial.println("Reprise de la détection");
  }
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
  Serial.println("Appuyer sur Entree dans le moniteur serie pour reprendre apres detection");
  Serial.println("=====================================");
}

void loop() {
  if (enPause) {
    attendreEntree();
    return;
  }

  // Capteur 1
  int distance1 = lectureDistance(trigPin1, echoPin1);
  Serial.print("d1 = ");
  Serial.print(distance1);
  Serial.println(" cm");

  if (distance1 < seuil1) {
    Serial.println("Obstacle detecte - capteur 1 !");
    Serial.println("Detection en pause - appuie sur Entree pour reprendre");
    enPause = true;
    return;
  }

  delay(12);

  // Capteur 2
  int distance2 = lectureDistance(trigPin2, echoPin2);
  Serial.print("d2 = ");
  Serial.print(distance2);
  Serial.println(" cm");

  if (distance2 < seuil2) {
    Serial.println("Obstacle detecte - capteur 2 !");
    Serial.println("Detection en pause - appuie sur Entree pour reprendre");
    enPause = true;
    return;
  }

  delay(12);
}