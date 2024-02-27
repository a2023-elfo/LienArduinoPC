/*
 * Auteurs: Jean-Samuel Lauzon
 * Date: Fevrier 2022
 * Modif : Janvier 2023, Compatible VisualStudio, JpGouin
*/

/*------------------------------ Librairies ---------------------------------*/
#include <iostream>
#include <string>
using namespace std;

/*-------------------------- Librairies externes ----------------------------*/
#include "include/serial/SerialPort.hpp"
#include "include/json.hpp"
using json = nlohmann::json;


/*------------------------------ Constantes ---------------------------------*/
#define BAUD 9600           // Frequence de transmission serielle
#define MSG_MAX_SIZE 1024   // Longueur maximale d'un message


/*------------------------- Prototypes de fonctions -------------------------*/
bool SendToSerial(SerialPort* arduino, json j_msg);
bool RcvFromSerial(SerialPort* arduino, string& msg);


/*---------------------------- Variables globales ---------------------------*/
SerialPort* arduino; //doit etre un objet global!

/*----------------------------- Fonction "Main" -----------------------------*/
int main() {
    string raw_msg;

    // Initialisation du port de communication
    string com;
    cout << "Entrer le port de communication du Arduino: ";
    cin >> com;
    arduino = new SerialPort(com.c_str(), BAUD);

    //Struct. Données JSON 
    int recuX = 0;
    int recuY = 0;
    int bouton = 0;
    
    //const char com = "\\\\.\\COM3";
    //SerialPort arduino = SerialPort("\\\\.\\COM3");
    if (!arduino->isConnected()) {
        cerr << "Impossible de se connecter au port " << string(com) << ". Fermeture du programme!" << endl;
        exit(1);
    }
    else {
        cout << "Connexion OK "<<endl;
    }

    // Structure de donnees JSON pour envoie et reception
    int led_state = 1;
    json j_msg_send, j_msg_rcv;

    // Boucle pour tester la communication bidirectionnelle Arduino-PC
    for (int i = 0; i < 30; i++) {
        // Envoie message Arduino
        j_msg_send["Affichage"] = "X=" + to_string(recuX) + " Y=" + to_string(recuY) + " B=" + to_string(bouton);


        // Reception message Arduino
        
        if(!SendToSerial(arduino, j_msg_send)){
        cerr << "Erreur lors de l'envoie du message. " << endl;
        }
       
        
        j_msg_rcv.clear();
        if (!RcvFromSerial(arduino, raw_msg)) {
            cerr << "Erreur lors de la reception du message. " << endl;
        }
       
      
        
        // Impression du message de l'Arduino si valide
        if (raw_msg.size() > 0) {
            // cout << "raw_msg: " << raw_msg << endl;  // debug
            // Transfert du message en json
            j_msg_rcv = json::parse(raw_msg);
            cout << "Message de l'Arduino: " << j_msg_rcv << endl;

            recuX = j_msg_rcv.value("X", 0);
            recuY = j_msg_rcv.value("Y", 0);
            bouton = j_msg_rcv.value("Bouton", 0);


        }
        Sleep(1000); // 1000ms
    }
    return 0;
}



/*---------------------------Definition de fonctions ------------------------*/
bool SendToSerial(SerialPort* arduino, json j_msg) {
    // Return 0 if error
    string msg = j_msg.dump();
    bool ret = arduino->writeSerialPort(msg.c_str(), msg.length());
    return ret;
}


bool RcvFromSerial(SerialPort* arduino, string& msg) {
    // Return 0 if error
    // Message output in msg
    string str_buffer;
    char char_buffer[MSG_MAX_SIZE];
    int buffer_size;

    msg.clear(); // clear string
    // Read serialport until '\n' character (Blocking)

    // Version fonctionnel dans VScode, mais non fonctionnel avec Visual Studio
/*
    while(msg.back()!='\n'){
        if(msg.size()>MSG_MAX_SIZE){
            return false;
        }

        buffer_size = arduino->readSerialPort(char_buffer, MSG_MAX_SIZE);
        str_buffer.assign(char_buffer, buffer_size);
        msg.append(str_buffer);
    }
*/

// Version fonctionnelle dans VScode et Visual Studio
    buffer_size = arduino->readSerialPort(char_buffer, MSG_MAX_SIZE);
    str_buffer.assign(char_buffer, buffer_size);
    msg.append(str_buffer);

    //msg.pop_back(); //remove '/n' from string

    return true;
}

