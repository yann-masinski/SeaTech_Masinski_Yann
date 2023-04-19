#include <xc.h>
#include "UART_Protocol.h"
#include "CB_TX1.h"
#include "CB_RX1.h"
#include "RobotState.h"

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
    SendMessage(bytelist, sizeof (bytelist));

}
int msgDecodedFunction = 0;
int msgDecodedPayloadLength = 0;
unsigned char msgDecodedPayload[128];
int msgDecodedPayloadIndex = 0;

enum StateReception {
    Waiting,
    FunctionMSB,
    FunctionLSB,
    PayloadLengthMSB,
    PayloadLengthLSB,
    Payload,
    CheckSum,
};

enum StateReception rcvState = Waiting;

void UartDecodeMessage(unsigned char c) {
    //Fonction prenant en entree un octet et servant a reconstituer les trames
    switch (rcvState) {
        case Waiting:
            if (c == 0xFE)
                rcvState = FunctionMSB;
            break;

        case FunctionMSB:
            msgDecodedFunction = c << 8;
            rcvState = FunctionLSB;
            break;

        case FunctionLSB:
            msgDecodedFunction += c << 0;
            rcvState = PayloadLengthMSB;
            break;

        case PayloadLengthMSB:
            msgDecodedPayloadLength = c << 8;
            rcvState = PayloadLengthLSB;
            break;

        case PayloadLengthLSB:
            msgDecodedPayloadLength += c << 0;
            if (msgDecodedPayloadLength == 0) {
                rcvState = CheckSum;
            } else if (msgDecodedPayloadLength >= 1000) {
                rcvState = Waiting;
            } else {
                rcvState = Payload;
                msgDecodedPayloadIndex = 0;
            }
            break;

        case Payload:
            msgDecodedPayload[msgDecodedPayloadIndex++] = c;
            if (msgDecodedPayloadIndex >= msgDecodedPayloadLength) {
                rcvState = CheckSum;
            }
            break;

        case CheckSum:
        {
            unsigned char receivedChecksum = c;
            if (UartCalculateChecksum(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload) == receivedChecksum) {
                /*
                 Reception.Text += "OxFE";
                 Reception.Text += msgDecodedFunction.ToString("X4");
                 Reception.Text += msgDecodedPayloadLength.ToString("X4");
                 Reception.Text += Encoding.ASCII.GetString(msgDecodedPayload);
                 Reception.Text +=receivedChecksum.ToString("X2");
                 */
                UartProcessDecodedMessage(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);
            } else
                //"Erreur Reception Cheksum incorrect";
                rcvState = Waiting;
        }
            break;
        default:
            rcvState = Waiting;
            break;
    }
}

enum MsgFunction {
    texte = 0x0080,
    led = 0x0020,
    capteurIr = 0x0030,
    moteur = 0x0040,
    etat = 0x0050,
    SET_ROBOT_STATE= 0x0051,
    SET_ROBOT_MANUAL_CONTROL=0x0052,
    
};

void UartProcessDecodedMessage(int function, int payloadLength, unsigned char* payload) {
    //Fonction appelee apres le decodage pour executer l?action
    //correspondant au message recu
    enum MsgFunction msgFunction = function;
    switch (msgFunction) {
        case SET_ROBOT_STATE:
            SetRobotState(payload[0]);
            break;
        case SET_ROBOT_MANUAL_CONTROL:
            SetRobotAutoControlState(payload[0]);
            
            break;
            
        default:
            break;
    }
}

//*************************************************************************/
//Fonctions correspondant aux messages
//*************************************************************************/
