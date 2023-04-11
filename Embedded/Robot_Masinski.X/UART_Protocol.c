#include <xc.h>
#include "UART_Protocol.h"
#include "CB_TX1.h"
#include "CB_RX1.h"

unsigned char UartCalculateChecksum(int msgFunction, int msgPayloadLength, unsigned char* msgPayload) {
    //Fonction prenant en entree la trame et sa longueur pour calculer le checksum
    unsigned char chks = 0;
    chks ^= (char) 0xFE;
    chks ^= (char) (msgFunction >> 8);
    chks ^= (char) (msgFunction >> 0);
    chks ^= (char) (msgPayloadLength >> 8);
    chks ^= (char) (msgPayloadLength >> 0);

    for (int i = 0; i < msgPayloadLength; i++) {
        chks ^= (char) (msgPayload)[i];
    }
    return chks;

}

void UartEncodeAndSendMessage(int msgFunction, int msgPayloadLength, unsigned char* msgPayload) {
    //Fonction d?encodage et d?envoi d?un message
    unsigned char bytelist[6 + msgPayloadLength];
    int position = 0;
    bytelist[position++] = (char) 0xFE;
    bytelist[position++] = (char) (msgFunction >> 8);
    bytelist[position++] = (char) (msgFunction >> 0);
    bytelist[position++] = (char) (msgPayloadLength >> 8);
    bytelist[position++] = (char) (msgPayloadLength >> 0);
    
    for (int i = 0; i < msgPayloadLength; i++) {
        bytelist[position++] = msgPayload[i];
    }
    bytelist[position] = UartCalculateChecksum(msgFunction, msgPayloadLength, msgPayload);
        SendMessage(bytelist,sizeof(bytelist));

}
int msgDecodedFunction = 0;
int msgDecodedPayloadLength = 0;
unsigned char msgDecodedPayload[128];
int msgDecodedPayloadIndex = 0;

/*
void UartDecodeMessage(unsigned char c) {
    //Fonction prenant en entree un octet et servant a reconstituer les trames
    
}

void UartProcessDecodedMessage(int function, int payloadLength, unsigned char* payload) {
    //Fonction appelee apres le decodage pour executer l?action
    //correspondant au message recu
    
}
 */ 
//*************************************************************************/
//Fonctions correspondant aux messages
//*************************************************************************/
