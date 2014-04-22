using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading;
using System.Windows.Forms;
using System.Text.RegularExpressions;

namespace SS
{
    public partial class FormLogin : Form
    {
        private SpreadsheetClient model;
        private string host = "localhost";
        private int port = 2000;
        
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
                model.CloseLoginEvent += CloseForm;
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

        private void CloseForm(string s)
        {
            Close();
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
        /// Enter a name of a new spreadsheet and send it to the model.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void EnterNew_Click(object sender, EventArgs e)
        {
           
                // Try connecting
         //       try
         //       {
         //           model.Connect(host, port, NameTextBox.Text);
         //       }
         //       catch
         //       {
                    
         //           IPAddrBox.Invoke(new Action(() => { IPAddrBox.Text = "Invalid hostname\r\n"; }));
          //      }

            string ssname = OpenNew.Text;
            if (!ValidName(ssname))
            {
                SSNewName.Invoke(new Action(() => { SSNewName.Visible = true; }));
                SSNewName.Invoke(new Action(() => { SSNewName.Text = "Entered name is invalid. A name should start with a letter and have only letters and numbers. Please try again." + "\r\n"; }));
                return;
            }

            else
            {
                // Send the word
                model.CreateMessage(OpenNew.Text);

                //Make all the boxes invisible except for ConnectAgain button.
                BigTextBox.Invoke(new Action(() => { BigTextBox.Visible = false; }));
                SSList.Invoke(new Action(() => { SSList.Visible = false; }));
                label2.Invoke(new Action(() => { label2.Visible = false; }));
                EnterNew.Invoke(new Action(() => { EnterNew.Visible = false; }));
                OpenNew.Invoke(new Action(() => { OpenNew.Visible = false; }));
                label6.Invoke(new Action(() => { label6.Visible = false; }));
                OpenExisting.Invoke(new Action(() => { OpenExisting.Visible = false; }));
                EnterExisting.Invoke(new Action(() => { EnterExisting.Visible = false; }));
                connectAgain.Invoke(new Action(() => { connectAgain.Visible = true; }));
                NameTextBox.Invoke(new Action(() => { NameTextBox.Visible = false; }));
                IPAddrBox.Invoke(new Action(() => { IPAddrBox.Visible = false; }));
                ConnectButton.Invoke(new Action(() => { ConnectButton.Visible = false; }));
                label1.Invoke(new Action(() => { label1.Visible = false; }));
                label3.Invoke(new Action(() => { label3.Visible = false; }));
                SSNewName.Invoke(new Action(() => { SSNewName.Visible = false; }));

                
            }
            
               
        }

         /// <summary>
        /// Click to open a new login form.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void connectAgain_Click(object sender, EventArgs e)
        {
           
           //Create a new thread which will open a new login form.
            LoadForm lform = new LoadForm();

            Thread oThread = new Thread(new ThreadStart(lform.Load));

            // Start the thread
            oThread.Start();

            //Close the old login form.
            Close();
        }

        /// <summary>
        /// Forwards received message to appropriate buttons and textboxes on the Form view.
        /// </summary>
        /// <param name="line"></param>
        private void MessageReceived(String line)
        {
            if ((!(line == null)))
            {
			    string[] tokens = line.Split((Char)27);
			    switch (tokens[0])
			    {
				    case "INVALID":
					    BigTextBox.Invoke(new Action(() => { BigTextBox.Visible=true; }));
					    BigTextBox.Invoke(new Action(() => { BigTextBox.Text = "Entered password is invalid. Please try again." + "\r\n"; }));
					    break;
				    case "FILELIST":
					    string files = "";
					    for (int i = 1; i < tokens.Length; i++)
					    {
						    files += tokens[i] + "\r\n";
					    }
                        SSList.Text = files;

                        BigTextBox.Invoke(new Action(() => { BigTextBox.Visible = false; }));
                        SSList.Invoke(new Action(() => { SSList.Visible = true; }));
                        label2.Invoke(new Action(() => { label2.Visible = true; }));
                        EnterNew.Invoke(new Action(() => { EnterNew.Visible = true; }));
                        OpenNew.Invoke(new Action(() => { OpenNew.Visible = true; }));
                        label6.Invoke(new Action(() => { label6.Visible = true; }));
                        OpenExisting.Invoke(new Action(() => { OpenExisting.Visible = true; }));
                        EnterExisting.Invoke(new Action(() => { EnterExisting.Visible = true; }));
                        break;
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
                string[] tokens = line.Split((Char)27);

                int vn=0;

                try
                {
                    vn = Convert.ToInt32(tokens[1]);
                }
                catch 
                {
                    Console.WriteLine("Input string for a version number is not a sequence of digits.");
                }

                //run the new form on a new thread
                Thread oThread = new Thread(new ThreadStart( () => {  Application.EnableVisualStyles();
                    // Application.SetCompatibleTextRenderingDefault(false);
                    // Start an application context and run one form inside it
                    DemoApplicationContext appContext = DemoApplicationContext.getAppContext();
                    appContext.RunForm(new Form1(host, port, model, vn));
                    Application.Run(appContext);

                }));

                // Start the thread
                oThread.Start();
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

        private void EnterExisting_Click(object sender, EventArgs e)
        {
            // Send the word
            model.OpenMessage(OpenExisting.Text);

            //Make all the boxes invisible except for ConnectAgain button.
            BigTextBox.Invoke(new Action(() => { BigTextBox.Visible = false; }));
            SSList.Invoke(new Action(() => { SSList.Visible = false; }));
            label2.Invoke(new Action(() => { label2.Visible = false; }));
            EnterNew.Invoke(new Action(() => { EnterNew.Visible = false; }));
            OpenNew.Invoke(new Action(() => { OpenNew.Visible = false; }));
            label6.Invoke(new Action(() => { label6.Visible = false; }));
            OpenExisting.Invoke(new Action(() => { OpenExisting.Visible = false; }));
            EnterExisting.Invoke(new Action(() => { EnterExisting.Visible = false; }));
            connectAgain.Invoke(new Action(() => { connectAgain.Visible = true; }));
            NameTextBox.Invoke(new Action(() => { NameTextBox.Visible = false; }));
            IPAddrBox.Invoke(new Action(() => { IPAddrBox.Visible = false; }));
            ConnectButton.Invoke(new Action(() => { ConnectButton.Visible = false; }));
            label1.Invoke(new Action(() => { label1.Visible = false; }));
            label3.Invoke(new Action(() => { label3.Visible = false; }));
            SSNewName.Invoke(new Action(() => { SSNewName.Visible = false; }));
        }

        /// <summary>
        /// Checks if the name is not null and if it starts with 1 or more letters, followed by one or more numbers.
        /// </summary>
        /// <param name="name"></param>
        /// <returns></returns>
        private bool ValidName(String name)
        {

            if (name == null)
                return false;
            Regex r;
            Match m;
            r = new Regex(@"^([a-zA-Z]+)([a-zA-Z1-9]\d*)$");
            m = r.Match(name);
            if (m.Success)
                return true;
            else return false;
        }

        
    }

    public class LoadForm
    {

        // This method that will be called when the thread is started
        public void Load()
        {
              Application.EnableVisualStyles();
              Application.SetCompatibleTextRenderingDefault(false);
            // Start an application context and run one form inside it
            DemoApplicationContext appContext1 = DemoApplicationContext.getAppContext();
            appContext1.RunForm(new FormLogin());
            Application.Run(appContext1);
        }
    }

}
