using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Text;
using System.Collections.Generic;
using System.Diagnostics;
using CS;
using CustomNetworking;

namespace BoggleServerTests
{
    [TestClass]
    public class UnitTest1
    {
        static String s1 = null;
        static String s2 = null;
        static object p1 = null;
        static object p2 = null;

        [TestMethod()]
        public void Test3()
        {

            // Create two clients.
            TcpClient client1 = null;
            
            TcpClient client2 = null;

            //Create a ChatServer object
            string[] args = { "10", "..\\..\\..\\..\\PS8\\ChatServer\\bin\\Debug\\dictionary", "belloworldshimom" };
            Thread game = new Thread(() => BoggleServer.Main(args));
            game.Start();

            //Start client1
            client1 = new TcpClient("localhost", 2000);
            Socket clientSocket1 = client1.Client;
            
            // Wrap the client's connection into a StringSocket
            StringSocket player1Socket = new StringSocket(clientSocket1, new UTF8Encoding());

            player1Socket.BeginSend("Play Player1\n", (e, o) => { }, null);
            Thread.Sleep(2000);
            player1Socket.BeginReceive(CompletedReceive1, 1);
            Thread.Sleep(1000);

            // Start client2
            client2 = new TcpClient("localhost", 2000);
            Socket clientSocket2 = client2.Client;
            StringSocket player2Socket = new StringSocket(clientSocket2, new UTF8Encoding());

            player2Socket.BeginSend("Play Player2\n", (e, o) => { }, null);
            Thread.Sleep(2000);
            player2Socket.BeginReceive(CompletedReceive2, 2);

            Thread.Sleep(1000);

            Assert.AreEqual("Player1, welcome to the Chat Server!", s1);
            Assert.AreEqual(1, p1);

            Assert.AreEqual("Player2, welcome to the Chat Server!", s2);
            Assert.AreEqual(2, p2);


            // Keep receiving, should receive
            player1Socket.BeginReceive(CompletedReceive1, 3);
            Thread.Sleep(1000);
            Assert.AreEqual(3, p1);
            Assert.AreEqual("START BELLOWORLDSHIMOM 10 Player1", s1);

            player2Socket.BeginReceive(CompletedReceive2, 4);
            Thread.Sleep(1000);
            Assert.AreEqual(4, p2);
            Assert.AreEqual("TIME 9", s2);

            // Thread.Sleep(1000);

            player1Socket.BeginSend("WORD row\n", (e, o) => { }, null);
            player1Socket.BeginSend("WORD bellows\n", (e, o) => { }, null);
        }

        // Callback Player 1
        private static void CompletedReceive1(String s, Exception o, object payload)
        {
            s1 = s;
            p1 = payload;
        }

        // Callback Player 2
        private static void CompletedReceive2(String s, Exception o, object payload)
        {
            s2 = s;
            p2 = payload;
        }

    }
}
