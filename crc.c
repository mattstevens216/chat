#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interface.h"
#include <pthread.h>
#include <cstring>
#include <iostream>


/*
 * TODO: IMPLEMENT BELOW THREE FUNCTIONS
 */
int connect_to(const char *host, const char *port);
struct Reply process_command(const int sockfd, char* command);
void process_chatmode(const char* host, const char* port);
void *send_message(void *fd);
void *receive_message(void *fd);

int main(int argc, char** argv) 
{
	if (argc != 3) {
		fprintf(stderr,
				"usage: enter host address and port number\n");
		exit(1);
	}

    display_title();
    
	while (1) {
	
		int sockfd = connect_to(argv[1], argv[2]);
    
		char command[MAX_DATA]; //256
        get_command(command, MAX_DATA);

		struct Reply reply = process_command(sockfd, command);
		display_reply(command, reply);
		
		touppercase(command, strlen(command) - 1);
		if (strncmp(command, "JOIN", 4) == 0) {
			char port[MAX_DATA];
			sprintf(port,"%d",reply.port);
			printf("Now you are in the chatmode\n");
			process_chatmode(argv[1], port);
		}
	
		close(sockfd);
    }

    return 0;
}

/*
 * Connect to the server using given host and port information
 *
 * @parameter host    host address given by command line argument
 * @parameter port    port given by command line argument
 * 
 * @return socket fildescriptor
 */
int connect_to(const char *host, const char *port)
{
	int sockfd, status;
	struct addrinfo server, *response;
	memset(&server, 0, sizeof server);
	server.ai_socktype = SOCK_STREAM;
	server.ai_family = AF_INET;
	
	if((status = getaddrinfo(host, port, &server, &response)) != 0){
		std::cout<<"LOL"<<status<<"\n"; //DEBUGGING
		std::cout<<"Problem with getting address info.\n";
		return -1;
	}
	
	sockfd = socket(response->ai_family, response->ai_socktype, response->ai_protocol);
	if(sockfd < 0){
		std::cout<<"Problem creating socket.\n";
		return -1;
	}
	
	if(connect(sockfd, response->ai_addr, response->ai_addrlen) < 0){
		std::cout<<"Could not connect to server.\n";
		return -1;
	}
	
	std::cout<<"Connected to server: " << host <<".\n";
	
	return sockfd;
	
	
	
	// ------------------------------------------------------------
	// GUIDE :
	// In this function, you are suppose to connect to the server.
	// After connection is established, you are ready to send or
	// receive the message to/from the server.
	// 
	// Finally, you should return the socket fildescriptor
	// so that other functions such as "process_command" can use it
	// ------------------------------------------------------------

    // below is just dummy code for compilation, remove it.
	//int sockfd = -1;
}

/* 
 * Send an input command to the server and return the result
 *
 * @parameter sockfd   socket file descriptor to commnunicate
 *                     with the server
 * @parameter command  command will be sent to the server
 *
 * @return    Reply    
 */
struct Reply process_command(const int sockfd, char* command)
{
	// ------------------------------------------------------------
	// GUIDE 1:
	// In this function, you are supposed to parse a given command
	// and create your own message in order to communicate with
	// the server. Surely, you can use the input command without
	// any changes if your server understand it. The given command
    // will be one of the followings:
	//
	// CREATE <name>
	// DELETE <name>
	// JOIN <name>
    // LIST
	//
	// -  "<name>" is a chatroom name that you want to create, delete,
	// or join.
	// 
	// - CREATE/DELETE/JOIN and "<name>" are separated by one space.
	// ------------------------------------------------------------


	// ------------------------------------------------------------
	// GUIDE 2:
	// After you create the message, you need to send it to the
	// server and receive a result from the server.
	// ------------------------------------------------------------


	// ------------------------------------------------------------
	// GUIDE 3:
	// Then, you should create a variable of Reply structure
	// provided by the interface and initialize it according to
	// the result.
	//
	// For example, if a given command is "JOIN room1"
	// and the server successfully created the chatroom,
	// the server will reply a message including information about
	// success/failure, the number of members and port number.
	// By using this information, you should set the Reply variable.
	// the variable will be set as following:
	//
	// Reply reply;
	// reply.status = SUCCESS;
	// reply.num_member = number;
	// reply.port = port;
	// 
	// "number" and "port" variables are just an integer variable
	// and can be initialized using the message fomr the server.
	//
	// For another example, if a given command is "CREATE room1"
	// and the server failed to create the chatroom becuase it
	// already exists, the Reply varible will be set as following:
	//
	// Reply reply;
	// reply.status = FAILURE_ALREADY_EXISTS;
    // 
    // For the "LIST" command,
    // You are suppose to copy the list of chatroom to the list_room
    // variable. Each room name should be seperated by comma ','.
    // For example, if given command is "LIST", the Reply variable
    // will be set as following.
    //
    // Reply reply;
    // reply.status = SUCCESS;
    // strcpy(reply.list_room, list);
    // 
    // "list" is a string that contains a list of chat rooms such 
    // as "r1,r2,r3,"
	// ------------------------------------------------------------

	// REMOVE below code and write your own Reply.
	struct Reply reply;
	int reply_data[3];
	char list_room[MAX_DATA];
	
	reply.status = FAILURE_NOT_EXISTS;
	reply.num_member = 5;
	reply.port = 3000;
	memset(reply.list_room,'\0', MAX_DATA);
	
	memset(list_room, '\0', MAX_DATA);
	touppercase(command, strlen(command)-1);
	
	send(sockfd, command, MAX_DATA, 0);
	recv(sockfd, reply_data, sizeof(int) * 3, 0);
	recv(sockfd, list_room, MAX_DATA, 0);
	
	if(strncmp(command, "LIST", 4) == 0){
		for(int i = 0; i < MAX_DATA; i++){
			reply.list_room[i] = list_room[i];
		}
		reply.status = (Status)reply_data[0];
	}
	else{
		reply.status = (Status)reply_data[0];
		reply.num_member = reply_data[1];
		reply.port = reply_data[2];
	}
	
	
	return reply;
	
}

/* 
 * Get into the chat mode
 * 
 * @parameter host     host address
 * @parameter port     port
 */
 
void *send_message(void *fd){
	char message[MAX_DATA];
	memset(message, '\0', MAX_DATA);
	long sockfd = (long) fd;
	
	while(true){
		get_message(message, sizeof(message));
		int result = send(sockfd, message, sizeof(message), MSG_NOSIGNAL);
		if(result < 0){
			memset(message, '\0', MAX_DATA);
			close(sockfd);
		}
	}
	
}
void *receive_message(void *fd){
	char message[MAX_DATA];
	memset(message, '\0', MAX_DATA);
	long sockfd = (long) fd;
	
	while(true){
		recv(sockfd, message, sizeof(message), 0);
		display_message(message);
		std::cout<<std::endl;
	}
}
 
 
void process_chatmode(const char* host, const char* port)
{
	// ------------------------------------------------------------
	// GUIDE 1:
	// In order to join the chatroom, you are supposed to connect
	// to the server using host and port.
	// You may re-use the function "connect_to".
	// ------------------------------------------------------------

	// ------------------------------------------------------------
	// GUIDE 2:
	// Once the client have been connected to the server, we need
	// to get a message from the user and send it to server.
	// At the same time, the client should wait for a message from
	// the server.
	// ------------------------------------------------------------
	
    // ------------------------------------------------------------
    // IMPORTANT NOTICE:
    // 1. To get a message from a user, you should use a function
    // "void get_message(char*, int);" in the interface.h file
    // 
    // 2. To print the messages from other members, you should use
    // the function "void display_message(char*)" in the interface.h
    //
    // 3. Once a user entered to one of chatrooms, there is no way
    //    to command mode where the user  enter other commands
    //    such as CREATE,DELETE,LIST.
    //    Don't have to worry about this situation, and you can 
    //    terminate the client program by pressing CTRL-C (SIGINT)
	// ------------------------------------------------------------
	
	int sockfd;
	pthread_t send_t, receive_t;
	
	sockfd = connect_to(host, port);
	pthread_create(&send_t, NULL, send_message, (void*)sockfd);
	pthread_create(&receive_t, NULL, receive_message, (void*)sockfd);
	pthread_exit(NULL);
}


