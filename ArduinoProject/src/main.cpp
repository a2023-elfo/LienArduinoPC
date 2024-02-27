/* 
 * Auteurs: Jean-Samuel Lauzon    
 * Date: Fevrier 2022
*/

/*------------------------------ Librairies ---------------------------------*/
#include <Arduino.h>
#include <ArduinoJson.h>
#include <LiquidCrystal.h>


/*------------------------------ Constantes ---------------------------------*/

#define BAUD 9600        // Frequence de transmission serielle
const int rs = 42, en = 40, d4 = 38, d5 = 36, d6 = 34, d7 = 32;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

/*---------------------------- Variables globales ---------------------------*/

volatile bool shouldSend_ = false;  // Drapeau prêt à envoyer un message
volatile bool shouldRead_ = false;  // Drapeau prêt à lire un message

int ledState = 0;
int potValue = 0;
int xValueAnalogique = 0;
int yValueAnalogique = 0;
int xValueSent = 0;
int yValueSent = 0;


int pinLED = 24;
int pinPOT = A0;
int pinJoyX = A15;
int pinJoyY = A14;
int pinSW1 = 30;


/*------------------------- Prototypes de fonctions -------------------------*/
void sendMsg(); 
void readMsg();
void serialEvent();
/*---------------------------- Fonctions "Main" -----------------------------*/

void setup() {
  Serial.begin(BAUD);               // Initialisation de la communication serielle
  pinMode(pinLED, OUTPUT);
  digitalWrite(pinLED, ledState);

  lcd.begin(16, 2);
  lcd.print("Hello, ELFO!");
}

/* Boucle principale (infinie) */
void loop() {

  if(shouldRead_){
    readMsg();
    sendMsg();
  }

  potValue = analogRead(pinPOT);
  //Serial.println(potValue);          // debug
  delay(10);  // delais de 10 ms
}

/*---------------------------Definition de fonctions ------------------------*/

void serialEvent() { shouldRead_ = true; }


/*---------------------------Definition de fonctions ------------------------
Fonction d'envoi
Entrée : Aucun
Sortie : Aucun
Traitement : Envoi du message
-----------------------------------------------------------------------------*/
void sendMsg() {
  StaticJsonDocument<500> doc;
  // Elements du message
  doc["time"] = millis();
  doc["analog"] = potValue;

  // On parse l'entrée du joystick
  xValueAnalogique = analogRead(pinJoyX);
  yValueAnalogique = analogRead(pinJoyY);

  if(xValueAnalogique>800 && yValueAnalogique<580 && yValueAnalogique>480){

    xValueSent = 1; 
  }
   else if (yValueAnalogique>800 && xValueAnalogique<580 && xValueAnalogique>480){

    yValueSent = 1;
  }
  else if (xValueAnalogique<200 && yValueAnalogique<580 && yValueAnalogique>480){

    xValueSent = -1;
  }
  else if (yValueAnalogique<200 && xValueAnalogique<580 && xValueAnalogique>480){

    yValueSent = -1;
  }
  //DIAGONALE
  else if (yValueAnalogique<200 && xValueAnalogique>800 ){

    yValueSent = -1;
    xValueSent = 1;
  }
  else if (yValueAnalogique<200 && xValueAnalogique<200 ){

    yValueSent = -1;
    xValueSent = -1;
  }
  else if (yValueAnalogique>800 && xValueAnalogique<200 ){

    yValueSent = 1;
    xValueSent = -1;
  }
  else if (yValueAnalogique>800 && xValueAnalogique>800 ){

    yValueSent = 1;
    xValueSent = 1;
  }
  else{
    xValueSent = 0;
    yValueSent =0;
  }

  doc["X"] = xValueSent;
  doc["Y"] = yValueSent;
  doc["Bouton"] = digitalRead(pinSW1);

  // Serialisation
  serializeJson(doc, Serial);

  // Envoie
  Serial.println();
  shouldSend_ = false;
}

/*---------------------------Definition de fonctions ------------------------
Fonction de reception
Entrée : Aucun
Sortie : Aucun
Traitement : Réception du message
-----------------------------------------------------------------------------*/
void readMsg(){
  // Lecture du message Json
  StaticJsonDocument<500> doc;
  JsonVariant parse_msg;

  // Lecture sur le port Seriel
  DeserializationError error = deserializeJson(doc, Serial);
  shouldRead_ = false;

  // Si erreur dans le message
  if (error) {
    Serial.print("deserialize() failed: ");
    Serial.println(error.c_str());
    return;
  }
  
  // Analyse des éléments du message message
  parse_msg = doc["led"];
  if (!parse_msg.isNull()) {
    // mettre la led a la valeur doc["led"]
    digitalWrite(pinLED,doc["led"].as<bool>());
  }

  parse_msg = doc["affichage_format"];
  if (!parse_msg.isNull()) {
    // mettre la led a la valeur doc["led"]
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(doc["affichage_format"].as<String>());
  }
}