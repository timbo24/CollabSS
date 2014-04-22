using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;
using CustomNetworking;
using System.Threading;

namespace SS
{
    public class SpreadsheetClient
    {
        // The socket used to communicate with the server.  If no connection has been
        // made yet, this is null.
        private StringSocket socket;

        private bool load = true;

        // Register for this event to be motified when a line of text arrives.
        public event Action<String> IncomingLineEvent;
        public event Action<String> OpenNewLineEvent;
        public event Action<String> ServerCrashedLineEvent;
        public event Action<String> EditLineEvent;
        /// <summary>
        /// For synchronizing receives
        /// </summary>
        private readonly object receiveSync = new object();

        /// <summary>
        /// Indicates if the socket is still connected to the server.
        /// </summary>
        private bool isConnected = false;

        /// <summary>
        /// Creates a not yet connected client model.
        /// </summary>
        public SpreadsheetClient()
        {
            socket = null;
        }

        /// <summary>
        /// Connect to the server at the given hostname and port and with the give name.
        /// </summary>
        public void Connect(string hostname, int port, String name)
        {
            if (socket != null)
            {
                Thread t1 = new Thread(() =>
                {
                    socket.BeginSend(" " + "\r\n", (ex1, py1) => { stillAlive(ex1, py1); }, socket);
                });

                t1.Start();
                t1.Join();
            }

            if ((socket == null) || (isConnected == false))
            {
                TcpClient client = new TcpClient(hostname, port);
                socket = new StringSocket(client.Client, UTF8Encoding.Default);
                isConnected = true;
                socket.BeginSend("PASSWORD" + (Char)27 + name + "\n", (e, p) => { }, null);
                Thread.Sleep(200);
                socket.BeginReceive(LineReceived, null);
            }
            else
            {
                socket.BeginSend("PASSWORD"+ (Char)27 + name + "\n", (e, p) => { }, null);
                Thread.Sleep(200);
                socket.BeginReceive(LineReceived, null);
            }
        }

        /// <summary>
        /// Check if a client is still alive. If he is disconnected, set the boolean flag. 
        /// </summary>
        /// <param name="ex">Exception</param>
        /// <param name="py">payload = player's socket</param>
        private void stillAlive(Exception ex, object py)
        {
            if (ex != null)
            {
                isConnected = false;
            }
        }

        /// <summary>
        /// Send a line of text to the server.
        /// </summary>
        /// <param name="line"></param>
        public void CreateMessage(String line)
        {
            if (socket != null)
            {
                socket.BeginSend("CREATE" + (Char)27 + line + "\n", (e, p) => { }, null);
            }
        }

        /// <summary>
        /// Send a line of text to the server.
        /// </summary>
        /// <param name="line"></param>
        public void OpenMessage(String line)
        {
            if (socket != null)
            {
                socket.BeginSend("OPEN" + (Char)27 + line + "\n", (e, p) => { }, null);
            }
        }

        /// <summary>
        /// Send a line of text to the server.
        /// </summary>
        /// <param name="line"></param>
        public void SendMessageEdit(String line)
        {
            if (socket != null)
            {
                socket.BeginSend(line, (e, p) => { }, null);
            }
        }

        /// <summary>
        /// Send a line of text to the server.
        /// </summary>
        /// <param name="line"></param>
        public void SendMessageSync(String line)
        {
            if (socket != null)
            {
                socket.BeginSend(line + "\n", (e, p) => { }, null);
            }
        }

        /// <summary>
        /// Send an UNDO command to the server.
        /// </summary>
        /// <param name="line"></param>
        public void SendUndo(String line)
        {
            if (socket != null)
            {
                socket.BeginSend(line, (e, p) => { }, null);
            }
        }

        /// <summary>
        /// Send a Save command to the server.
        /// </summary>
        /// <param name="line"></param>
        public void SaveRequest(String line)
        {
            if (socket != null)
            {
                socket.BeginSend(line, (e, p) => { }, null);
            }
        }

        /// <summary>
        /// Send a Disconnect command to the server.
        /// </summary>
        /// <param name="line"></param>
        public void DisconnectRequest(String line)
        {
            if (socket != null)
            {
                socket.BeginSend(line, (e, p) => { }, null);
            }
        }

        /// <summary>
        /// Deal with an arriving line of text.
        /// </summary>
        private void LineReceived(String s, Exception e, object p)
        {
            if (!(e == null))
            {
                ServerCrashedLineEvent("Connection to Boggle Server has been lost"); //e.Message
                return;
            }

            string[] tokens = s.Split((Char)27);

               // If it is null, or starts with IGNORE, simply return
            if (s == null || tokens[0] == "IGNORING") //&&(IncomingLineEvent != null))
            {
                Thread.Sleep(200);
                return;
            }

            switch (tokens[0])
            {
                case "FILELIST":
                    IncomingLineEvent(s);
                    break;
                case "UPDATE":
                    if (load)
                    {
                        OpenNewLineEvent(s);
                        load = false;
                    }
                    if (tokens.Length > 2)
                    {
                        Thread.Sleep(2000);
                        EditLineEvent(s);
                    }
                    EditLineEvent(s);
                    break;
                case "ERROR":
                    Console.WriteLine("WE DONE ...");
                    break;
                case "SYNC":
                    EditLineEvent(s);
                    break;
                default:
                    IncomingLineEvent(s);
                    break;
            }
            socket.BeginReceive(LineReceived, null);
        }

       
        /// <summary>
        /// Close the socket
        /// </summary>
        public void Quit()
        {
            // To disconnect, we simply close the StringSocket
            socket.close();
        }
    }
}
