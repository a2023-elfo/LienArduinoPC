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
int joy_X_Value = 0;
int joy_Y_Value = 0;
int joy_X_etat = 0;
int joy_Y_etat = 0;

int pinLED = 24;
int pinPOT = A0;
int pinjoy_X= A15;
int pinjoy_Y= A14;

int SW1 = 30 ;
//int SW2 = PIN ;
//int SW3 = PIN ;
//int SW4 = PIN ;
//int SW5 = PIN ;
//int SW6 = PIN ;

int bouton = 0 ;
 

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
  lcd.print("Lien Arduino-PC");

}

/* Boucle principale (infinie) */
void loop() {
 
   if (shouldRead_) {
      readMsg();
      sendMsg();
    }

  joy_X_Value = analogRead(pinjoy_X);
  joy_Y_Value = analogRead(pinjoy_Y);

  if(joy_X_Value>800 && joy_Y_Value<580 && joy_Y_Value>480){

    joy_X_etat = 1; 
  }
   else if (joy_Y_Value>800 && joy_X_Value<580 && joy_X_Value>480){

    joy_Y_etat = 1;
  }
  else if (joy_X_Value<200 && joy_Y_Value<580 && joy_Y_Value>480){

    joy_X_etat = -1;
  }
  else if (joy_Y_Value<200 && joy_X_Value<580 && joy_X_Value>480){

    joy_Y_etat = -1;
  }
  //DIAGONALE
  else if (joy_Y_Value<200 && joy_X_Value>800 ){

    joy_Y_etat = -1;
    joy_X_etat = 1;
  }
  else if (joy_Y_Value<200 && joy_X_Value<200 ){

    joy_Y_etat = -1;
    joy_X_etat = -1;
  }
  else if (joy_Y_Value>800 && joy_X_Value<200 ){

    joy_Y_etat = 1;
    joy_X_etat = -1;
  }
  else if (joy_Y_Value>800 && joy_X_Value>800 ){

    joy_Y_etat = 1;
    joy_X_etat = 1;
  }

  else{
    joy_X_etat = 0;
    joy_Y_etat =0;
  }

  // Bouton 
  if (digitalRead(SW1)== LOW)
  {
    bouton = 1;
    delay(10);
  }
  /*else if (digitalRead(SW2)== LOW)
  {
    bouton = 2;
    delay(100);
  }
  else if (digitalRead(SW3)== LOW)
  {
    bouton = 3;
    delay(100);
  }
  else if (digitalRead(SW4)== LOW)
  {
    bouton = 4;
    delay(100);
  }
  else if (digitalRead(SW5)== LOW)
  {
    bouton = 5;
    delay(100);
  }
  else if (digitalRead(SW6)== LOW)
  {
    bouton = 6;
    delay(100);
  }*/
  else
  {
    bouton = 0;
  }

    
   
  
  delay(10);  // delais de 10 ms
}

// 1023 - 800 // 0-200 // X et Y 

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
  doc["X"] = joy_X_etat;
  doc["Y"] = joy_Y_etat;
  doc["Bouton"] = bouton;

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
  parse_msg = doc["Affichage"];
  
  if (!parse_msg.isNull()) {
    lcd.clear();
    lcd.setCursor(0,1);  
    lcd.print(parse_msg.as<String>());
  }
}

