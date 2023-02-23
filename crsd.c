#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interface.h"

int establishConnection(sockaddr_in &serv){
	int sockfd, rc;
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		printf("Error opening socket");
		return 0;
	}
	
	if((rc = bind(sockfd, (struct sockaddr*)&serv, sizeof(serv))) == -1){
		printf("SERVER: error binding socket \n");
	}
	
	if((rc = listen(sockfd, 32)) == -1){
		printf("SERVER: error listening \n");
	}
	
	printf("Server is awaiting connections \n");
	
	return sockfd;	
}	


int server(const char* port){
	int t1;
	int sockfd, rc;
	char buffer[1024];
	
	if(port == NULL){
		printf("Invalid port number");
		return -1;
	}
	
	struct sockaddr_in serv;
	struct sockaddr_storage their_addr;
	socklen_t sin_size;
	int s_sock;
	
	memset(&serv, 0, sizeof(serv));
	serv.sin_family = AF_INET;
	serv.sin_addr.s_addr = htonl(INADDR_ANY);
	serv.sin_port = htons((short)port);
	
	pthread_t thread1, thread2, thread3;
	
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("Error opening socket");
		return 0;
	}
	
	if((rc = bind(sockfd, (struct sockaddr*)&serv, sizeof(serv))) < 0){
		printf("SERVER: error binding socket \n");
	}
	
	if((rc = listen(sockfd, 32)) < 0){
		printf("SERVER: error listening \n");
	}
	
	printf("Server is awaiting connections \n");	
	
	while(1){
		sin_size = sizeof their_addr;
		s_sock = accept(sockfd,(struct sockaddr*)&their_addr, &sin_size);
		if(s_sock < 0){
			printf("server: accept failed \n");
			continue;
		}
		printf("server: got connection\n");
		recv(s_sock, buffer, sizeof(buffer), 0);
		printf("server received the command %s\n", buffer);
		
		close(s_sock);
	}
	
	return 0;
}
	

int main(int argc, char** argv){

	if(argc != 2){
		fprintf(stderr, 
			"usage: enter port number \n");
			exit(1);
	}
	
	server(argv[1]);
	
	return 0;
}
