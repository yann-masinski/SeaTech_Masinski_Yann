using ExtendedSerialPort;
using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Threading;

namespace RobotInterfaceYannMasinski
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    /// 
    public partial class MainWindow : Window
    {
        ReliableSerialPort serialPort1;
        DispatcherTimer timerAffichage;
        Robot robot;

        public MainWindow()
        {
            InitializeComponent();
            serialPort1 = new ReliableSerialPort("COM8", 115200, Parity.None, 8, StopBits.One);
            serialPort1.DataReceived += serialPort1_DataReceived;
            serialPort1.Open();

            robot = new Robot();

            timerAffichage = new DispatcherTimer(priority: DispatcherPriority.Input);
            timerAffichage.Interval = new TimeSpan(0, 0, 0, 0, 100);
            timerAffichage.Tick += TimerAffichage_Tick;
            timerAffichage.Start();


        }

        private void TimerAffichage_Tick(object sender, EventArgs e)
        {
            //if (robot.receivedText != "")
            //{
            //    Reception.Text += "Reçu Robot : " + robot.receivedText + "\n";
            //    robot.receivedText = "";

            //}

            while (robot.byteListReceived.Count != 0)
            {
                var c = robot.byteListReceived.Dequeue();
                DecodeMessage(c);
                //Reception.Text += "0x" + c.ToString("X2") + " ";
            }
        }

        public void serialPort1_DataReceived(object sender, DataReceivedArgs e)
        {
            //robot.receivedText += Encoding.UTF8.GetString(e.Data, 0, e.Data.Length);

            for (int i = 0; i < e.Data.Length; i++)
            {
                robot.byteListReceived.Enqueue(e.Data[i]);
            }
        }

        private void textBox_TextChanged(object sender, TextChangedEventArgs e)
        {

        }

        private void buttonEnvoyer_Click(object sender, RoutedEventArgs e)
        {
            if (buttonEnvoyer.Background == Brushes.RoyalBlue)
            {
                buttonEnvoyer.Background = Brushes.Beige;
            }
            else
            {
                buttonEnvoyer.Background = Brushes.RoyalBlue;
            }

            string s = textBoxEmission.Text;
            byte[] array = Encoding.ASCII.GetBytes(s);
            UartEncodeAndSendMessage(0x0080, array.Length, array);
            textBoxEmission.Text = "";
        }

        private void buttonClear_Click(object sender, RoutedEventArgs e)
        {
            Reception.Text = "";
        }

        private void buttonTest_Click(object sender, RoutedEventArgs e)
        {
            string s = "Bonjour";
            byte[] array = Encoding.ASCII.GetBytes(s);
            UartEncodeAndSendMessage(0x0080, array.Length, array);

            array = new byte[2];
            array[0] = 1;
            array[1] = 1;
            UartEncodeAndSendMessage(0x0020, array.Length, array);

            array = new byte[3];
            array[0] = 1;
            array[1] = 2;
            array[2] = 3;
            UartEncodeAndSendMessage(0x0030, array.Length, array);

            array = new byte[2];
            array[0] = 1;
            array[1] = 1;
            UartEncodeAndSendMessage(0x0040, array.Length, array);

        }

        private void textBoxEmission_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                //SendMessage();
                string s = textBoxEmission.Text;
                byte[] array = Encoding.ASCII.GetBytes(s);
                UartEncodeAndSendMessage(0x0080, array.Length, array);
                textBoxEmission.Text = "";
            }
        }

        private void SendMessage()
        {
            //Reception.Text += "Reçu : " + textBoxEmission.Text + "\n";

            serialPort1.WriteLine(textBoxEmission.Text);
            textBoxEmission.Text = "";
        }

        byte CalculateChecksum(int msgFunction, int msgPayloadLength, byte[] msgPayload)
        {
            byte chks = 0;
            chks ^= (byte)0xFE;
            chks ^= (byte)(msgFunction >> 8);
            chks ^= (byte)(msgFunction >> 0);
            chks ^= (byte)(msgPayloadLength >> 8);
            chks ^= (byte)(msgPayloadLength >> 0);

            for (int i = 0; i < msgPayloadLength; i++)
            {
                chks ^= msgPayload[i];
            }
            return chks;
        }

        void UartEncodeAndSendMessage(int msgFunction, int msgPayloadLength, byte[] msgPayload)
        {
            byte[] bytelist = new byte[6 + msgPayloadLength];

            int position = 0;
            bytelist[position++] = 0xFE;
            bytelist[position++] = (byte)(msgFunction >> 8);
            bytelist[position++] = (byte)(msgFunction >> 0);
            bytelist[position++] = (byte)(msgPayloadLength >> 8);
            bytelist[position++] = (byte)(msgPayloadLength >> 0);
            for (int i = 0; i < msgPayloadLength; i++)
            {
                bytelist[position++] = msgPayload[i];
            }
            bytelist[position] = CalculateChecksum(msgFunction, msgPayloadLength, msgPayload);
            serialPort1.Write(bytelist, 0, bytelist.Length);
        }

        public enum StateReception
        {
            Waiting,
            FunctionMSB,
            FunctionLSB,
            PayloadLengthMSB,
            PayloadLengthLSB,
            Payload,
            CheckSum
        }

        StateReception rcvState = StateReception.Waiting;
        int msgDecodedFunction = 0;
        int msgDecodedPayloadLength = 0;
        byte[] msgDecodedPayload;
        int msgDecodedPayloadIndex = 0;
        int receivedChecksum = 0;


        private void DecodeMessage(byte c)
        {
            switch (rcvState)
            {

                case StateReception.Waiting:

                    if (c == 0xFE)
                        rcvState = StateReception.FunctionMSB;
                    break;

                case StateReception.FunctionMSB:
                    msgDecodedFunction = c << 8;
                    rcvState = StateReception.FunctionLSB;
                    break;

                case StateReception.FunctionLSB:
                    msgDecodedFunction += c << 0;
                    rcvState = StateReception.PayloadLengthMSB;
                    break;

                case StateReception.PayloadLengthMSB:
                    msgDecodedPayloadLength = c << 8;
                    rcvState = StateReception.PayloadLengthLSB;
                    break;

                case StateReception.PayloadLengthLSB:
                    msgDecodedPayloadLength += c << 0;
                    if (msgDecodedPayloadLength == 0)
                    {
                        rcvState = StateReception.CheckSum;
                    }
                    else
                    {
                        rcvState = StateReception.Payload;
                        msgDecodedPayloadIndex = 0;
                        msgDecodedPayload = new byte[msgDecodedPayloadLength];
                    }
                    break;

                case StateReception.Payload:
                    msgDecodedPayload[msgDecodedPayloadIndex++] = c;
                    if (msgDecodedPayloadIndex >= msgDecodedPayloadLength)
                    {
                        rcvState = StateReception.CheckSum;
                    }
                    break;

                case StateReception.CheckSum:
                    receivedChecksum = c;
                    if (CalculateChecksum(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload) == receivedChecksum)
                    {
                        /*
                         Reception.Text += "OxFE";
                         Reception.Text += msgDecodedFunction.ToString("X4");
                         Reception.Text += msgDecodedPayloadLength.ToString("X4");
                         Reception.Text += Encoding.ASCII.GetString(msgDecodedPayload);
                         Reception.Text +=receivedChecksum.ToString("X2");
                        */
                        ProcessDecodedMessage(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);
                    }
                    else
                        Reception.Text += "Erreur Reception Cheksum incorrect";
                    rcvState = StateReception.Waiting;
                    break;
                default:
                    rcvState = StateReception.Waiting;
                    break;
            }
        }

        public enum MsgFunction
        {
            texte = 0x0080,
            led = 0x0020,
            capteurIr=0x0030,
            moteur=0x0040,
            etat=0x0050
        }

        public enum StateRobot
        {
            STATE_ATTENTE = 0,
            STATE_ATTENTE_EN_COURS = 1,
            STATE_AVANCE = 2,
            STATE_AVANCE_EN_COURS = 3,
            STATE_TOURNE_GAUCHE = 4,
            STATE_TOURNE_GAUCHE_EN_COURS = 5,
            STATE_TOURNE_DROITE = 6,
            STATE_TOURNE_DROITE_EN_COURS = 7,
            STATE_TOURNE_SUR_PLACE_GAUCHE = 8,
            STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS = 9,
            STATE_TOURNE_SUR_PLACE_DROITE = 10,
            STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS = 11,
            STATE_ARRET = 12,
            STATE_ARRET_EN_COURS = 13,
            STATE_RECULE = 14,
            STATE_RECULE_EN_COURS = 15
        }

        void ProcessDecodedMessage(int msgFunction, int msgPayloadLength, byte[] msgPayload)
        {
            switch ((MsgFunction)msgFunction)
            {


                case MsgFunction.texte:
                    // Reception.Text += Encoding.ASCII.GetString(msgDecodedPayload);
                    // Reception.Text += "\n";
                    break;

                case MsgFunction.led:

                    if (msgPayload[1] == 1)
                    {
                        if (msgPayload[1] == 0) Led1.IsChecked = false;
                        else Led1.IsChecked = true;
                    }
                    if (msgPayload[1] == 2)
                    {
                        if (msgPayload[2] == 0) Led2.IsChecked = false;
                        else Led2.IsChecked = true;
                    }
                    if (msgPayload[1] == 3)
                    {
                        if (msgPayload[3] == 0) Led3.IsChecked = false;
                        else Led3.IsChecked = true;
                    }
                    break;

                case MsgFunction.capteurIr:

                    labelIrGauche.Content = "IR Gauche : " + msgPayload[0].ToString() + " cm";
                    labelIrCentre.Content = "IR Centre : " + msgPayload[1].ToString() + " cm";
                    labelIrDroit.Content = "IR Droit : " + msgPayload[2].ToString() + " cm";
                    break;
                case MsgFunction.moteur:

                    MoteurGauche.Content = "Moteur Gauche : " + msgDecodedPayload[0].ToString("X2") + " %";
                    MoteurDroit.Content = "Moteur Droit : " + msgDecodedPayload[1].ToString("X2") + " %";
                    break;

                case MsgFunction.etat:
                    int instant = (((int)msgPayload[1]) << 24) + (((int)msgPayload[2]) << 16) + (((int)msgPayload[3]) << 8) + ((int)msgPayload[4]);
                    Reception.Text += "\nRobot␣State␣:␣" + ((StateRobot)(msgPayload[0])).ToString() + "␣-␣" + instant.ToString() + "␣ms";
                    break;


            }

        }

    }
}
