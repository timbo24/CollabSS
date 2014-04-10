using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using CustomNetworking;
//using BB;
using System.IO;
using System.Text.RegularExpressions;
using System.Collections;
using System.Timers;



namespace CS
{   
    public class BoggleServer
    {
        
        // Listens for incoming connections
        public TcpListener server;

        // Keeps track of string sockets and their clients
        private Dictionary<StringSocket, Client> map;

      

       
        /// <summary>
        /// Launches a Boggle server that listens on port 2000
        /// </summary>
        /// <param name="args"></param>
        public static void Main(string[] args)
        {
       

                new BoggleServer();

                Console.ReadLine();  //TODO - what is this for?
            }
        


        /// <summary>
        /// Creates a BoggleServer that listens for connections on port 2000
        /// </summary>
        /// <param name="time"></param>
        /// <param name="dictionary"></param>
      //  public BoggleServer(int time, string dictionary) :this(time, dictionary, "")
      //  {
            // Uses 3 param constructor passing in an empty string for the board
      //  }

        /// <summary>
        /// Creates a BoggleServer that listens for connections on port 2000
        /// </summary>
        /// <param name="time">How long the game should be in seconds</param>
        /// <param name="dictionary">The filepath to a dictionary of valid words</param>
        /// <param name="board">String of letters for a BoggleBoard</param>
        public BoggleServer ()
{
            // Create a server that listens on port 2000
            server = new TcpListener(IPAddress.Any, 2000);

            // Start the server and begin accepting sockets
            server.Start();
            server.BeginAcceptSocket(ConnectionReceived, null);


            new Thread(()=>Console.ReadLine()); 
            

        }

        /// <summary>
        /// server property
        /// </summary>
        public TcpListener Server
        {
            get { return server; }
            
        }

  

        /// <summary>
        /// Deals with connection requests
        /// The callback for BeginAcceptSocket
        /// </summary>
        /// <param name="ar">Result of BeginAcceptSocket</param>
        private void ConnectionReceived (IAsyncResult ar) 
        {
            try
            {
                // Save the socket
                Socket socket = server.EndAcceptSocket(ar);

                // Make a StringSocket our of the given socket
                StringSocket ss = new StringSocket(socket, UTF8Encoding.Default);

                // Start receiving with the StringSocket
                ss.BeginReceive(NameReceived, ss);

                // Keep accepting sockets on the server
                server.BeginAcceptSocket(ConnectionReceived, null);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
        }

        /// <summary>
        /// Receives the first line of text from the client, which contains the name of the remote
        /// user.  Uses it to compose and send back a welcome message.
        /// </summary>
        /// <param name="name">Contains the name of the client</param>
        /// <param name="e"></param>
        /// <param name="p"></param>
        private void NameReceived (String password, Exception e, object p) {
            try
            {
                // Get the string socket
                StringSocket ss = (StringSocket)p;

                // Lock the map
               // lock (map)
               // {
                    // Make sure the sting passed in is all uppercase
                    String passwordUpper = password.ToUpper();

                    // Make sure it matches protocol "PLAY @", where @ is the name of the player
                    // Also, this should be a new string socket
                    if (passwordUpper.StartsWith("PASSWORD"))// && !(map.ContainsKey(ss)))
                    {
                        // If it matches the protocol, save the name of the player
                        password = password.Substring(9).ToUpper();

                    

                        // Send a welcome message to the client
                        if (password.Equals("PASSWORD"))
                        {
                            ss.BeginSend("CONNECTED" + "\r\n", (ee, pp) => { }, null);
                            ss.BeginReceive(IncomingCallback, ss);
                        }
                        else
                        {
                            ss.BeginSend("INVALID" + "\r\n", (ee, pp) => { }, null);
                           
                            ss.BeginReceive(NameReceived, ss);
                        }
                       
              
                            }
                   
                    // If the message did not match protocol
                    else
                    {
                        // Ignore invalid command, and let client know that
                        ss.BeginSend("IGNORING " + password + "\r\n", (ee, pp) => { }, null);

                        // Keep receiving
                        ss.BeginReceive(NameReceived, ss);
                    }
               // }
            }
            catch (Exception ex)
            {
                
                Console.WriteLine(ex.Message);
            }
        }

       

   

        /// <summary>
        /// Check if a player is still alive. If he is disconnected, send "Terminated" message
        /// to his opponent and close the opponent's socket.
        /// </summary>
        /// <param name="ex">Exception</param>
        /// <param name="py">payload = player's socket</param>
        private void stillAlive(Exception ex, object py)
        {
            if (ex != null)
            {
                // Save the StringSocket
                StringSocket clientss = (StringSocket)py;

                // Save the StringSocket of Clients opponent to make coding simpler
                StringSocket opponentss = map[clientss].Opponent;
                opponentss.BeginSend("TERMINATED" + "\r\n", (ex2, py2) => { }, null);
                opponentss.close();

            }

        }

        /// <summary>
        /// Check if a client is still alive. If he is disconnected, remove him from pairs queue and from map. 
        /// </summary>
        /// <param name="ex">Exception</param>
        /// <param name="py">payload = player's socket</param>
        private void stillAliveClient(Exception ex, object py)
        {
            if (ex != null)
            {
                // Save the StringSocket
                StringSocket clientss = (StringSocket)py;

             //   pairs.Dequeue();
                map.Remove(clientss);

            }

        }

  

        /// <summary>
        /// Deals with lines of text as they arrive at the server.
        /// </summary>
        /// <param name="line"></param>
        /// <param name="e"></param>
        /// <param name="ss"></param>
        private void IncomingCallback(String line, Exception e, object ss)
        {
            if ((line == null) || (e != null))
            {
                
                return;
            }
            try
            {
                // Save the StringSocket
                StringSocket clientss = (StringSocket)ss;


           
                // Convert incoming line to uppercase
                String nameOfSS = line.ToUpper();

                // Check incoming word protocol "CREATE $", where $ is the spreadsheet's name
                if (nameOfSS.StartsWith("CREATE"))
                {
                   
        
                            
                            // Send the client permission to open a new spreadsheet
                            clientss.BeginSend("OPENNEW" + "\r\n", (ex, py) => { }, null);
           
           //                 Thread.Sleep(200);
           
                    }
                else if(nameOfSS.StartsWith("ENTER"))
                {
                     clientss.BeginSend("OK" + line +"\r\n", (ex, py) => { }, null);
                }

                else
                {
                    // If the received line did not follow protocol
                    clientss.BeginSend("IGNORING " + line + "\r\n", (ee, pp) => { }, null);
                    Thread.Sleep(200);
                }

                // Keep receiving
                clientss.BeginReceive(IncomingCallback, clientss);
            }
            catch (Exception ex)
            {
                //StringSocket ssf = (StringSocket)ss;
                //map[ssf].Opponent.BeginSend("TERMINATED\r\n", (ee, pp) => { }, null);
                Console.WriteLine(ex.Message);
            }
        }

        /// <summary>
        /// Client class for making a client object.
        /// </summary>
        public class Client
        {
            /// <summary>
            /// Name of the client.
            /// </summary>
            private string name;

            /// <summary>
            /// Game status.
            /// </summary>
            private bool playing;

            /// <summary>
            /// The client's score.
            /// </summary>
            private int score;

           /// <summary>
           /// List of the client's illegal words.
           /// </summary>
            private HashSet<string> illegalWords;

            /// <summary>
            /// List of the clien't legal words.
            /// </summary>
            private HashSet<string> legalWords;

            /// <summary>
            /// List of the client's common words.
            /// </summary>
            private HashSet<string> commonWords;

            /// <summary>
            /// The opponent's socket.
            /// </summary>
            private StringSocket opponent;

            /// <summary>
            /// Timer for a game, which watches when the time for the whole game expires.
            /// </summary>
            private System.Timers.Timer aTimer;

            /// <summary>
            /// Timer for sending remaining time of the game to clients every 1 second.
            /// </summary>
            private System.Timers.Timer secondsTimer;
           
            /// <summary>
            /// Constructor for client objects
            /// </summary>
            /// <param name="_name"></param>
            public Client(string _name)
            {
                this.name = _name;
                this.playing = false;
                this.score = 0;
                this.opponent = null;
                illegalWords = new HashSet<string>();
                legalWords = new HashSet<string>();
                commonWords = new HashSet<string>();
                //Set the timer for the whole game.
                aTimer = new System.Timers.Timer();

                //Set the timer for every second of the game.
                secondsTimer = new System.Timers.Timer(1000);
            }

            /// <summary>
            /// atimer property
            /// </summary>
            public System.Timers.Timer ATimer
            {
                get { return aTimer; }
               // set { aTimer = value; }
            }

            /// <summary>
            /// secondsTimer property
            /// </summary>
            public System.Timers.Timer SecondsTimer
            {
                get { return secondsTimer; }
               // set { secondsTimer = value; }
            }

            /// <summary>
            /// name's property
            /// </summary>
            public string Name
            {
                get { return name; }
               // set { name = value; }
            }

            /// <summary>
            /// connected property
            /// </summary>
            public bool Playing
            {
                get { return playing; }
                set { playing = value; }
            }

            /// <summary>
            /// score's property
            /// </summary>
            public int Score
            {
                get { return score; }
                set { score = value; }
            }

            /// <summary>
            /// illegalWords property
            /// </summary>
            public HashSet<String> IllegalWords
            {
                get { return illegalWords; }
                //set { illegalWords = value; }
            }

            /// <summary>
            /// legalWords property
            /// </summary>
            public HashSet<String> LegalWords
            {
                get { return legalWords; }
                //set { legalWords = value; }
            }

            /// <summary>
            /// commonWords property
            /// </summary>
            public HashSet<String> CommonWords
            {
                get { return commonWords ;}
                //set { commonWords = value; }
            }

            /// <summary>
            /// opponent property
            /// </summary>
            public StringSocket Opponent
            {
                get { return opponent; }
                set { opponent = value; }
            }
        }

    }
}
