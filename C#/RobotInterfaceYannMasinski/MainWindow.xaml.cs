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
        string receivedText;
        DispatcherTimer timerAffichage;

        public MainWindow()
        {
            InitializeComponent();
            serialPort1= new ReliableSerialPort("COM4", 115200, Parity.None, 8, StopBits.One );
            serialPort1.DataReceived += serialPort1_DataReceived;
            serialPort1.Open();
            timerAffichage= new DispatcherTimer();
            timerAffichage.Interval = new TimeSpan(0, 0, 0, 0, 100);
            timerAffichage.Tick += TimerAffichage_Tick; ;
            timerAffichage.Start();
        }

        private void TimerAffichage_Tick(object sender, EventArgs e)
        {
            if (receivedText != "")
            {
                Reception.Text += "Reçu : " + receivedText + "\n";
                receivedText = "";
            }
        }

        public void serialPort1_DataReceived(object sender, DataReceivedArgs e)
        {
            receivedText+= Encoding.UTF8.GetString(e.Data, 0, e.Data.Length);
        }

        private void textBox_TextChanged(object sender, TextChangedEventArgs e)
        {

        }

        private void buttonEnvoyer_Click(object sender, RoutedEventArgs e)
        {
            if (buttonEnvoyer.Background == Brushes.RoyalBlue){
                buttonEnvoyer.Background = Brushes.Beige;
            } else { buttonEnvoyer.Background = Brushes.RoyalBlue; 
            }
            SendMessage();
        }

        private void buttonClear_Click(object sender, RoutedEventArgs e)
        {
            Reception.Text = "";
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
            Reception.Text += "Reçu : " + textBoxEmission.Text + "\n";
            serialPort1.WriteLine(textBoxEmission.Text);
            textBoxEmission.Text = "";
        }

    }
}
