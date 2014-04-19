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
                socket.BeginSend("PASSWORD\\e" + name + "\n", (e, p) => { }, null);
                Thread.Sleep(200);
                socket.BeginReceive(LineReceived, null);
            }
            else
            {
                socket.BeginSend("PASSWORD\\e" + name + "\n", (e, p) => { }, null);
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
        public void SendMessage(String line)
        {
            if (socket != null)
            {
                socket.BeginSend("CREATE\\e" +line + "\n", (e, p) => { }, null);
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
                socket.BeginSend("ENTER " + line + "\n", (e, p) => { }, null);
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
            // If it is null, or starts with IGNORE, simply return
            if (s == null || s.StartsWith("IGNORING")) //&&(IncomingLineEvent != null))
            {
                Thread.Sleep(200);
                return;
            }

            if (s.StartsWith("FILELIST"))
            {
                //StartLineEvent(board, opponentName);
                IncomingLineEvent(s);
            }
            else if (s.StartsWith("OPENNEW"))
            {
                OpenNewLineEvent(s);
            }
            else if (s.StartsWith("OK"))
            {
                EditLineEvent(s);
            }

            else
            {
                IncomingLineEvent(s);
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
