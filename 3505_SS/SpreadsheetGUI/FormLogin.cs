﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading;
using System.Windows.Forms;

namespace SS
{
    public partial class FormLogin : Form
    {
        private SpreadsheetClient model;
        private string host = "localhost";
        private int port = 2500;
        /// <summary>
        /// For synchronizing receives
        /// </summary>
        //private readonly object receiveSync = new object();

        public FormLogin()
        {
            InitializeComponent();
            model = new SpreadsheetClient();
            
                model.IncomingLineEvent += MessageReceived;
                model.ServerCrashedLineEvent += CrashedReceived;
                model.OpenNewLineEvent += OpenNewSS;
        }

       
        /// <summary>
        /// Prints an error message when server or a client disconnects.
        /// </summary>
        /// <param name="obj"></param>
        private void CrashedReceived(string error)
        {
            BigTextBox.Invoke(new Action(() => { BigTextBox.Text += error + "\r\n"; }));
            //Thread.Sleep(500); 
        }

   
        /// <summary>
        /// Connect a client to a server.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ConnectButton_Click(object sender, EventArgs e)
        {
            
            // Change default if they provided a hostname
            if (IPAddrBox.Text != "")
                host = IPAddrBox.Text;

            
            // Try connecting
            try
            {
                model.Connect(host, port, NameTextBox.Text);
            }
            catch
            {
                // TODO
                IPAddrBox.Invoke(new Action(() => { IPAddrBox.Text = "Invalid hostname\r\n"; }));
            }

        }

        /// <summary>
        /// Enter a word and send it to the model.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void EnterNew_Click(object sender, EventArgs e)
        {

            // Try connecting
            try
            {
                model.Connect(host, port, NameTextBox.Text);
            }
            catch
            {
                // TODO
                IPAddrBox.Invoke(new Action(() => { IPAddrBox.Text = "Invalid hostname\r\n"; }));
            }
            // Send the word
            model.SendMessage(OpenNew.Text);

            // Clear the textbox
            OpenNew.Text = "";
        }

        /// <summary>
        /// Forwards received message to appropriate buttons and textboxes on the Form view.
        /// </summary>
        /// <param name="line"></param>
        private void MessageReceived(String line)
        {
            if ((!(line == null)))
            {
                if (line.StartsWith("INVALID"))
                {
                  BigTextBox.Invoke(new Action(() => { BigTextBox.Visible=true; }));
                  BigTextBox.Invoke(new Action(() => { BigTextBox.Text = "Entered password is invalid. Please try again." + "\r\n"; }));
                }
                else if (line.StartsWith("FILELIST"))
                {
                    BigTextBox.Invoke(new Action(() => { BigTextBox.Visible = false; }));
                    SSList.Invoke(new Action(() => { SSList.Visible = true; }));
                    label2.Invoke(new Action(() => { label2.Visible = true; }));
                    EnterNew.Invoke(new Action(() => { EnterNew.Visible = true; }));
                    OpenNew.Invoke(new Action(() => { OpenNew.Visible = true; }));
                    label6.Invoke(new Action(() => { label6.Visible = true; }));
                    OpenExisting.Invoke(new Action(() => { OpenExisting.Visible = true; }));
                    EnterExisting.Invoke(new Action(() => { EnterExisting.Visible = true; }));
                    
                    String files = "";
                    for (int i = 10; i < line.Length; i++)
                    {
                        if (line[i].Equals('\\'))
                        {
                            files += "\r\n";
                            i++;
                        }
                        else files += line[i];
                    }

                    SSList.Text = files;
                }
                   
               // Thread.Sleep(500); 
            }

           
        }


        /// <summary>
        /// Opens a new spreadsheet.
        /// </summary>
        /// <param name="line"></param>
        private void OpenNewSS(String line)
        {
            if (!(line == null))
            {
                model.Quit();               
                Application.EnableVisualStyles();
                Application.SetCompatibleTextRenderingDefault(false);
                // Start an application context and run one form inside it
                DemoApplicationContext appContext = DemoApplicationContext.getAppContext();
                
                appContext.RunForm(new Form1(host, port));
                Application.Run(appContext);

                
                
            }
        }

        /// <summary>
        /// Disconnect the client when "disconnect button is pressed".
        /// </summary>
        private void DisconnectButton_Click(object sender, EventArgs e)
        {
            // Disconnect
            model.Quit();
            MessageBox.Show("Disconnected from\r\nBoggle Server.");
        }
    }
}
