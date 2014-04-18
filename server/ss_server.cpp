#include <iostream>
#include <cstring>      // Needed for memset
#include <sys/socket.h> // Needed for the socket functions
#include <netdb.h>      // Needed for the socket functions
#include <arpa/inet.h> 	// for inet_ntop function
#include <pthread.h>
//#include <list>
//#include <iterator>
#include <sstream>
#include <stdio.h>
#include <errno.h>
#include <mysql/mysql.h> 

// DB definitions
#define SERVER "atr.eng.utah.edu"
#define USER "cs4540_tpayne"
#define PASSWORD "502365727"
#define DATABASE "cs4540_tpayne"
// Compiling with: mysql - g++ ss_server.cpp -pthread `mysql_config --cflags --libs`

int server_start_listen() ;
int server_establish_connection(int server_fd);
int server_send(int fd, std::string data);
void *tcp_server_read(void *arg) ;
void mainloop(int server_fd) ;
void pass_received(int new_sd);

//server constants
const  char * PORT = "2500" ; 
const int MAXLEN = 1024 ;   // Max lenhgt of a message.
const int MAXFD = 15 ;       // Maximum file descriptors to use. Equals maximum clients.
const int BACKLOG = 15 ;     // Number of connections that can wait in que before they be accept()ted

// This needs to be declared volatile because it can be altered by an other thread. Meaning the compiler cannot
// optimise the code, because it's declared that not only the program can change this variable, but also external
// programs. In this case, a thread.
volatile fd_set the_state;

pthread_mutex_t mutex_state = PTHREAD_MUTEX_INITIALIZER;


int main()
{
/*	int dbres = db_example();
    if (dbres == 1)
      {
	return 1;
      }
      */

int server_fd = server_start_listen() ;
    if (server_fd == -1)
    {
      std::cout << "An error occured. Closing program." ;
        return 1 ;
    }

    mainloop(server_fd);

    return 0;
}

int server_start_listen()
{

    int status;
    struct addrinfo host_info;       // The struct that getaddrinfo() fills up with data.
    struct addrinfo *host_info_list; // Pointer to the linked list of host_info's.

    int sock_fd;

    int server_fd; // the fd the server listens on
    int ret;
    int yes = 1;

    // Use the memset function to make sure all fields are NULL.
    memset(&host_info, 0, sizeof host_info);

    // std::cout << "Setting up the structs..."  << std::endl;

    host_info.ai_family = AF_UNSPEC;     // IP version not specified. Can be both.
    host_info.ai_socktype = SOCK_STREAM; // Use SOCK_STREAM for TCP or SOCK_DGRAM for UDP.
    host_info.ai_flags = AI_PASSIVE;     // IP Wildcard

    // Now fill up the linked list of host_info structs with the server's address information.
    status = getaddrinfo(NULL, PORT, &host_info, &host_info_list);
   
     if (status != 0)  std::cout << "getaddrinfo error" << gai_strerror(status) ;

    std::cout << "Creating a socket..."  << std::endl;
    
    server_fd = socket(host_info_list->ai_family, host_info_list->ai_socktype,
                      host_info_list->ai_protocol);
    if (server_fd == -1)  std::cout << "socket error " ;

    std::cout << "Binding socket..."  << std::endl;
    // we use to make the setsockopt() function to make sure the port is not in use
    // by a previous execution of our code.
   
    status = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    status = bind(server_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    // if (status == -1)  std::cout << "bind error" << std::endl ;

    // std::cout << "Listen()ing for connections..."  << std::endl;
    status =  listen(server_fd, BACKLOG);
    if (status == -1)  std::cout << "listen error" << std::endl ;

    return server_fd;
}

int server_establish_connection(int server_fd)
// This function will establish a connection between the server and the
// client. It will be executed for every new client that connects to the server.
// This functions returns the socket filedescriptor for reading the clients data
// or an error if it failed.
{


    char ipstr[INET6_ADDRSTRLEN];
    int port;

    int new_sd;
    struct sockaddr_storage their_addr;
    socklen_t addr_size = sizeof(their_addr);



    new_sd = accept(server_fd, (struct sockaddr *)&their_addr, &addr_size);
    if (new_sd == -1)
    {
        std::cout << "listen error" << std::endl ;
    }

    pass_received(new_sd);

       
    getpeername(new_sd, (struct sockaddr*)&their_addr, &addr_size);

   // deal with both IPv4 and IPv6:
if (their_addr.ss_family == AF_INET) {
    struct sockaddr_in *s = (struct sockaddr_in *)&their_addr;
    port = ntohs(s->sin_port);
    inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
} else { // AF_INET6
    struct sockaddr_in6 *s = (struct sockaddr_in6 *)&their_addr;
    port = ntohs(s->sin6_port);
    inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
}

 std::cout << "Connection accepted from "  << ipstr <<  " using port " << port << std::endl;



    return new_sd;
}

void pass_received(int new_sd)
{
     ssize_t bytes_received;
      char incomming_data_buffer[1024];
      bytes_received = recv(new_sd, incomming_data_buffer,1024, 0);
    // If no data arrives, the program will just wait here until some data arrives.
      if (bytes_received == 0) std::cout << "host shut down." << std::endl ;
      if (bytes_received == -1)std::cout << "recieve error!" << std::endl ;
      std::cout << bytes_received << " bytes recieved :" << std::endl ;
      incomming_data_buffer[bytes_received] = '\0';
      std::cout << incomming_data_buffer << std::endl;

       if((incomming_data_buffer[9]=='P')&&(incomming_data_buffer[10]=='A')&&(incomming_data_buffer[11]=='S')&&(incomming_data_buffer[12]=='S'))
       {
	  server_send(new_sd, "CONNECTED\r\n");
       }
       else 
	 {
                 server_send(new_sd, "INVALID\r\n");
		 pass_received(new_sd);
	 }
}

int server_send(int fd, std::string data)
// This function will send data to the clients fd.
// data contains the message to be send

{
  int ret;

    ret = send(fd, data.c_str(), strlen(data.c_str()),0);
    //if(ret != strlen(data.c_str()) throw some error;
    return 0;
}

void *tcp_server_read(void *arg)
/// This function runs in a thread for every client, and reads incomming data.
/// It also writes the incomming data to all other clients.

{
    int rfd;

    char buf[MAXLEN];
    int buflen;
    int wfd;

    rfd = *((int*)(&arg));
    for(;;)
    {

       //read incomming message.
        buflen = read(rfd, buf, sizeof(buf));

        if (buflen <= 0)
        {
	  std::cout << "client disconnected. Clearing fd. " << rfd << std::endl ;
            pthread_mutex_lock(&mutex_state);
            FD_CLR(rfd, &the_state);      // free fd's from  clients
            pthread_mutex_unlock(&mutex_state);
            close(rfd);
            pthread_exit(NULL);
        }

	std::string line="";
	int i=0;
	while ((buf[i]!='\r')&&(buf[i]!='\n'))
	  {
            line+=buf[i];
	    i++;
	  }

	       std::cout<<buf<<std::endl;
	
	if ((buf[0]=='C')&&(buf[1]=='R')&&(buf[2]=='E')&&(buf[3]=='A')&&(buf[4]=='T')&&(buf[5]=='E'))
	  server_send(rfd, "OPENNEW\r\n");
	else if ((buf[0]=='E')&&(buf[1]=='N')&&(buf[2]=='T')&&(buf[3]=='E')&&(buf[4]=='R'))
	  {
	  server_send(rfd, "OK"+line+"\r\n");

 // send the data to the other connected clients
        pthread_mutex_lock(&mutex_state);

        for (wfd = 3; wfd < MAXFD; ++wfd)
        {
            if (FD_ISSET(wfd, &the_state) && (rfd != wfd))
            {
                  // add the users FD to the message to give it an unique ID.
	      //     string userfd;
              //      stringstream out;
              //      out << wfd;
              //      userfd = out.str();
              //      userfd = userfd + ": ";

	      //   server_send(wfd, userfd);
                server_send(wfd, "OK"+line+"\r\n");
            }

        }
	  }

        pthread_mutex_unlock(&mutex_state);

 

    }
    return NULL;
}

void mainloop(int server_fd)

// This loop will wait for a client to connect. When the client connects, it creates a
// new thread for the client and starts waiting again for a new client.
{
pthread_t threads[MAXFD]; //create 15  handles for threads.

    FD_ZERO(&the_state); // FD_ZERO clears all the filedescriptors in the file descriptor set fds.


    while(1) // start looping here
    {
        int rfd;
        void *arg; 

        // if a client is trying to connect, establish the connection and create a fd for the client.
        rfd = server_establish_connection(server_fd);

        if (rfd >= 0)
        {
	  std::cout << "Client connected. Using file desciptor " << rfd << std::endl;
            if (rfd > MAXFD)
            {
	      std::cout << "To many clients trying to connect." << std::endl;
                close(rfd);
                continue;
            }

            pthread_mutex_lock(&mutex_state);  // Make sure no 2 threads can create a fd simultanious.

            FD_SET(rfd, &the_state);  // Add a file descriptor to the FD-set.

            pthread_mutex_unlock(&mutex_state); // End the mutex lock

            arg = (void *) rfd;

           
            // now create a thread for this client to intercept all incomming data from it.
            pthread_create(&threads[rfd], NULL, tcp_server_read, arg);
        }
    }
}

/**
 * Example code on how to connect with and use the database
 */

/*
int db_example()
{
    MYSQL *con;
    con = mysql_init(NULL);

    if (!con)
    {
        std::cout << "Mysql Initialization Failed";
        return 1;
    }
    
    // See definitions at beggining of file for values of SERVER, USER, PASSWORD, DATABASE
    con = mysql_real_connect(con, SERVER, USER, PASSWORD, DATABASE, 0,NULL,0);

    if (con)
    {
        std::cout << "DB Connection Succeeded\n";
    }
    else
    {
        std::cout << "DB Connection Failed\n";
    }

    MYSQL_RES *res_set;
    MYSQL_ROW row;

    // Creating an INSERT statement
    std::string ss_name = "ss5"; // ***If this spreadsheet name already exists, the insert will fail***
    std::string sql_insert  = "INSERT INTO Spreadsheet VALUES ('" + ss_name + "')";

    // Returns 0 on success
    if ( mysql_query (con, sql_insert.c_str()) )
      {
	std::cout << mysql_error(con) << std::endl;
	mysql_close (con);
	std::cout << "Database connection closed." << std::endl;
	return 1 ;
      }

    // The number of rows affected
    int rows_inserted = mysql_affected_rows(con);
    std::cout << "Inserted " << rows_inserted << " rows" << std::endl;


    // Now we will do a SELECT statement
    std::string sql_query  = "SELECT * FROM Spreadsheet";
    
    if ( mysql_query (con, sql_query.c_str()) )
      {
	std::cout << mysql_error(con) << std::endl;
	mysql_close (con);
	std::cout << "Database connection closed." << std::endl;
	return 1 ;
      }

    // Get the result of the query
    res_set = mysql_store_result(con);
    int numrows = mysql_num_rows(res_set);

    int i = 0;
    std::cout << "Spreadsheets in the database:" << std::endl;
     while (((row = mysql_fetch_row(res_set)) != NULL))
    {
        std::cout << row[i] << std::endl;
    }

    // Creating an UPDATE statement
    ss_name = "ss1";
    std::string cell_content = "Hello";
    std::string sql_update  = "UPDATE Cell SET contents = '" + cell_content + "' WHERE ssname = '" + ss_name + "' AND cell = 'A2'";

    // Returns 0 on success
    if ( mysql_query (con, sql_update.c_str()) )
      {
	std::cout << mysql_error(con) << std::endl;
	mysql_close (con);
	std::cout << "Database connection closed." << std::endl;
	return 1 ;
      }

    // The number of rows affected
    int rows_updated = mysql_affected_rows(con);
    std::cout << "Inserted " << rows_updated << " rows" << std::endl;

    // Close the connection!
    mysql_close (con);
    std::cout << "Database connection closed." << std::endl;

    return 0;
}
   */ 


     
