#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "inet.h"
#include <netdb.h>
#include <sys/select.h>
#include <stdio.h>
#include <unistd.h>

#define MAX 1025

int sockfd;
int main(int argc, char **argv){

	struct sockaddr_in client_ddr, serv_addr;
	
	int connect_id;
	struct hostent *hp;
	int is_connected = 1;
	char server_message[MAX];
	int rec_id;
	char output_message[MAX];
	int send_id;
	pthread_t rThread;
	int thread_rec;
	fd_set revfds, testfds, clientfds;
	int i;
	char *user_name;
	char message[MAX + 1];
	hp = gethostbyname(argv[1]);
    if(hp == 0){
      perror("Unknown Host");
    }
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family  = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));
    memcpy((void *)&serv_addr.sin_addr, hp->h_addr_list[0], hp->h_length);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
    	perror("client: can't open stream socket");
        exit(1);
    }
    connect_id = connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    if(connect_id < 0){
    	perror("client: can't connect to server");
        exit(1);
    }
    user_name = argv[3];
    printf(" Yey %s you are connected to server\n", user_name);

    
    rec_id = recv(sockfd, server_message, MAX, 0);

    if (rec_id < 0){
            //printf("Recieved Information %s: from server at port number %d\n", output, serv_addr.sin_port);
         perror("Could not recieve message from server");
         exit(1); 
      }
      
      printf("Server message %s\n", server_message );
      
      /*
      int check_Input = 1;
      thread_rec = pthread_create(&rThread, NULL, recieveMessage, &sockfd);
      if (thread_rec){
      	printf("ERROR: Returned Code From pthread_create()\n");
      	exit(1);
      }
	*/
     
     FD_ZERO(&clientfds);
     FD_SET(sockfd, &clientfds);
     FD_SET(0, &clientfds);//stdin
 
     

     while(is_connected == 1){

     	testfds = clientfds;
     	select(FD_SETSIZE, &testfds, NULL, NULL, NULL);
     	for(i=0; i < FD_SETSIZE; i++){
     		if(FD_ISSET(i, &testfds)){
     			if(i==sockfd){
     				memset(server_message, 0, MAX);
     				rec_id = recv(sockfd, server_message, MAX, 0);
    				if(rec_id < 0){
     					perror("didn't recieve message");
    				}
    				else if(rec_id > 0){
    					printf("%s\n", server_message);
    					//printf("%s: from server at socket number %d %d\n", server_message, sockfd, i);  
    				}
     			}
     			else if(i==0){

     				memset(output_message, 0, MAX);
     				memset(message, 0, MAX);
     				fgets(output_message, MAX, stdin);
     				printf(" message going out should be null %s\n", output_message);
     				sprintf(message, "%s [%s", output_message, user_name);
     				
     				send_id = send(sockfd, message, strlen(message), 0);
     				if (send_id < 0){
     					perror("Message Was not sent, try again");
     					continue;
     				}	
     				//printf("Sent this! %s %d\n", message, strlen(message));
     				if(strcmp(message, "quit") == 0){
     					printf("Sweet Exit, Bye!\n");
     					exit(0);
     				}
     			}
     		}
     	}	
     	
     }
      return 0;
}
