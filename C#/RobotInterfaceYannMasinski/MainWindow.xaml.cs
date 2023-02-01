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
            serialPort1 = new ReliableSerialPort("COM7", 115200, Parity.None, 8, StopBits.One);
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
                Reception.Text += "0x" + c.ToString("X2") + " ";
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
            SendMessage();
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
        }

        private void textBoxEmission_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                SendMessage();
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
    }
}
