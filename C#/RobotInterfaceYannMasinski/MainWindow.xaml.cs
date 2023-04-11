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
            array[0] =1;
            array[1] = 1;
            UartEncodeAndSendMessage(0x0020, array.Length, array);

            array = new byte[3];
            array[0] =1;
            array[1] =2;
            array[2] =3;
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
                    
                    if (c==0xFE)
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
                       
                        Reception.Text += "OxFE";
                        Reception.Text += msgDecodedFunction.ToString("X4");
                        Reception.Text += msgDecodedPayloadLength.ToString("X4");
                        Reception.Text += Encoding.ASCII.GetString(msgDecodedPayload);
                        Reception.Text +=receivedChecksum.ToString("X2");

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


        void ProcessDecodedMessage(int msgFunction, int msgPayloadLength, byte[] msgPayload)
        {
            if (msgFunction== 0x0080) { 

            }
            if (msgFunction == 0x0020) { 
                             
                if (msgPayload[1] == 1)
                {
                    if (msgPayload[1] == 0) Led1.IsChecked = false;
                    else Led1.IsChecked= true;
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
            }
            if (msgFunction == 0x0030) {
                labelIrGauche.Content = "IR Gauche : " + msgPayload[0].ToString("X2");
                labelIrGauche.Content = "IR Centre : " + msgPayload[1].ToString("X2");
                labelIrGauche.Content = "IR Droit : " + msgPayload[2].ToString("X2");
            }
            if (msgFunction == 0x0040) {
                MoteurGauche.Content = "Moteur Gauche : " + msgDecodedPayload[0].ToString("X2") + " %";
                MoteurDroit.Content = "Moteur Droit : " + msgDecodedPayload[1].ToString("X2") + " %";

            }
        }
    }
}
