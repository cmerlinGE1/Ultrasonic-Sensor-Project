#include <Arduino.h>

// Lecture de distance avec 2 capteurs HC-SR04
// ===========================================

// Définition des broches
const int trigPin1 = 10;  // Broche TRIG du capteur 1
const int echoPin1 = 11; // Broche ECHO du capteur 1
const int trigPin2 = 8;  // Broche TRIG du capteur 2
const int echoPin2 = 9; // Broche ECHO du capteur 2

// Paramètres de détection
const int TAILLE_MOYENNE = 10;        // Nombre de mesures pour la moyenne glissante
const int SEUIL_DETECTION = 3;        // Écart en cm pour détecter un objet (> variation naturelle de 1cm)
const int DETECTIONS_CONSECUTIVES = 3; // Nombre de détections consécutives requises (filtre anti-rebond)
const unsigned long TEMPS_DETECTION = 2000; // Durée minimale de détection en ms

// Variables de mesure
int distance1;   // Distance actuelle du capteur 1 en cm
int distance2;   // Distance actuelle du capteur 2 en cm

// Moyennes glissantes
float historique1[TAILLE_MOYENNE];    // Historique des mesures du capteur 1
float historique2[TAILLE_MOYENNE];    // Historique des mesures du capteur 2
int indexHistorique = 0;              // Index circulaire dans l'historique
float reference1 = 0;                 // Distance de référence capteur 1 (moyenne)
float reference2 = 0;                 // Distance de référence capteur 2 (moyenne)

// État de détection
bool colisDetecte = false;
unsigned long tempsDetection = 0;
bool calibrationComplete = false;
int compteurDetections = 0;  // Compteur de détections consécutives (filtre anti-rebond)

int lectureDistance(int trigPin, int echoPin) {

  // Variables locales
  long duration;  // Durée de l'impulsion en microsecondes
  int distance;   // Distance calculée en cm

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
  
  Serial.println("Capteur HC-SR04 - Détection de colis");
  Serial.println("====================================");
  Serial.println("Calibration en cours...");
  
  // Calibration initiale : remplir l'historique
  for(int i = 0; i < TAILLE_MOYENNE; i++) {
    historique1[i] = lectureDistance(trigPin1, echoPin1);
    historique2[i] = lectureDistance(trigPin2, echoPin2);
    delay(100);
  }
  
  // Calcul des références initiales
  reference1 = 0;
  reference2 = 0;
  for(int i = 0; i < TAILLE_MOYENNE; i++) {
    reference1 += historique1[i];
    reference2 += historique2[i];
  }
  reference1 /= TAILLE_MOYENNE;
  reference2 /= TAILLE_MOYENNE;
  
  calibrationComplete = true;
  Serial.print("Calibration terminée - Ref1: ");
  Serial.print(reference1);
  Serial.print(" cm, Ref2: ");
  Serial.print(reference2);
  Serial.println(" cm");
  Serial.println("Système prêt !");
  Serial.println();
}

void loop() {
  // Lecture de la distance des capteurs 1 et 2
  distance1 = lectureDistance(trigPin1, echoPin1);
  distance2 = lectureDistance(trigPin2, echoPin2);
  
  // Calcul des écarts par rapport aux références
  float ecart1 = reference1 - distance1;
  float ecart2 = reference2 - distance2;
  
  // Détection brute : au moins un capteur détecte un objet
  bool detectionBrute = (ecart1 > SEUIL_DETECTION) || (ecart2 > SEUIL_DETECTION);
  
  // Filtre anti-rebond : compteur de détections consécutives
  if(detectionBrute) {
    compteurDetections++;
    if(compteurDetections > 10) compteurDetections = 10; // Limiter le compteur
  } else {
    compteurDetections--;
    if(compteurDetections < 0) compteurDetections = 0;
  }
  
  // Confirmation de détection uniquement si seuil atteint
  bool detectionConfirmee = (compteurDetections >= DETECTIONS_CONSECUTIVES);
  
  if(detectionConfirmee) {
    if(!colisDetecte) {
      // Nouvelle détection confirmée
      colisDetecte = true;
      tempsDetection = millis();
      Serial.println();
      Serial.println("╔════════════════════════════════╗");
      Serial.println("║    COLIS DÉTECTÉ !             ║");
      Serial.println("╚════════════════════════════════╝");
    }
    
    // Affichage pendant la détection
    Serial.print("DÉTECTION - d1: ");
    Serial.print(distance1);
    Serial.print(" cm (écart: ");
    Serial.print(ecart1, 1);
    Serial.print(" cm) | d2: ");
    Serial.print(distance2);
    Serial.print(" cm (écart: ");
    Serial.print(ecart2, 1);
    Serial.println(" cm)");
    
  } else {
    // Pas de détection confirmée
    if(colisDetecte && (millis() - tempsDetection > TEMPS_DETECTION)) {
      // Fin de la détection
      colisDetecte = false;
      Serial.println("→ Fin de détection");
      Serial.println();
    }
    
    // Mise à jour de la référence (moyenne glissante) uniquement si pas de détection
    if(!colisDetecte) {
      historique1[indexHistorique] = distance1;
      historique2[indexHistorique] = distance2;
      indexHistorique = (indexHistorique + 1) % TAILLE_MOYENNE;
      
      // Recalcul des références
      reference1 = 0;
      reference2 = 0;
      for(int i = 0; i < TAILLE_MOYENNE; i++) {
        reference1 += historique1[i];
        reference2 += historique2[i];
      }
      reference1 /= TAILLE_MOYENNE;
      reference2 /= TAILLE_MOYENNE;
      
      // Affichage normal
      Serial.print("d1: ");
      Serial.print(distance1);
      Serial.print(" cm (ref: ");
      Serial.print(reference1, 1);
      Serial.print(" cm) | d2: ");
      Serial.print(distance2);
      Serial.print(" cm (ref: ");
      Serial.print(reference2, 1);
      Serial.println(" cm)");
    }
  }
  
  // Attente avant la prochaine mesure
  delay(10);
}