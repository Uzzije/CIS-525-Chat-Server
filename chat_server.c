#include "stdlib.h"
#include <stdio.h>
#include <sys/types.h>
#include "sys/socket.h"
#include "string.h"
#include "netinet/in.h"
#include "inet.h"
#include <unistd.h>
#include <sys/time.h>

#define BUFF_SIZE 1025

struct User {
    int port;
    char name[BUFF_SIZE];
    char roomName[BUFF_SIZE];
    int userID;
} user;


int main(int argc, char * argv[]){

    struct       sockaddr_in addr, cli_addr; 
    int master_socket, client_socket;
    int bind_id;
    int listen_id;
    int val;
    unsigned int clilen;
    char message[BUFF_SIZE], temp_message[BUFF_SIZE];
    int send_id;
    int newmaster_socket;
    int recieve_id;
    int client_socketArray[10]; 
    char client_Message[BUFF_SIZE];
    int max_connection = 10;
    int opt = 1;
    int i;
    int max_sd;
    int current_fd = 0;
    fd_set readfds, testfds;
    int activity;
    int y;
    char name_split = '[';
    char * char_name;
    int name_index;
    //initialise all client_socket[] to 0 so not checked 
    for (i = 0; i < max_connection; i++)  
    {  
        client_socketArray[i] = -1;  
    }  
     // create master socket
    master_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(master_socket < 0){
		perror("server: can't open stream socket");
        exit(1);
	}
    //set master socket to allow multiple connections , 
    //this is just a good habit, it will work without this 
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0 )  
    {  
        perror("setsockopt");  
        exit(1);  
    }  
    printf("Socket Created!\n");

    // bind socket
	memset((char *)&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(atoi(argv[1]));

    bind_id = bind(master_socket, (struct sockaddr *) &addr, sizeof(addr));
    if (bind_id < 0){
    	perror("server: can't bind local address");
        exit(1);
    }

    listen_id = listen(master_socket, 10);
    if(listen_id < 0) {
      perror("You are already connected");
    }
    printf("Live and Listening!\n");

    printf("Waiting for connections\n");

    val = 2;
    //fflush(stdout);
    
    max_sd = master_socket;
    while (val > 0) {

        FD_ZERO(&readfds);
        FD_SET(master_socket, &readfds);
        for(i = 0; i < max_connection; i++){
            if(client_socketArray[i] != -1){
                FD_SET(client_socketArray[i], &readfds);
                 //printf("value is inside sets %d\n", client_socketArray[i]);

            }
            else{
                //printf("no value is inside sets %d %d\n", master_socket, FD_SETSIZE);
            }
        }
        
        activity = select(FD_SETSIZE, &readfds, NULL, NULL, NULL);
        if(activity == 0){
            printf("timeout\n");
            continue;
        }
        if((activity < 0) && (errno !=EINTR)){
            printf("select error");
        }
        
        
        for ( i = 0 ; i < max_sd + 1; i++)  {  
           //printf("not readable %d %d\n", i, max_sd);
             
            //socket descriptor 
            current_fd = client_socketArray[i];  
              
            //if valid socket descriptor then add to read list 
            //highest file descriptor number, need it for the select function 
            
            //If something happened on the master socket , 
            //then its an incoming connection 
            /*
            */
            if(FD_ISSET(i, &readfds)){
               printf("value is inside %d %d\n", i, master_socket);
                if(i == master_socket){
                    printf("I am master");
                    clilen = sizeof(cli_addr);
                    client_socket = accept(master_socket, (struct sockaddr *) &cli_addr, &clilen);
                    if (client_socket < 0){
                        perror("accept");
                        exit(1);
                    }
                    printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , 
                    client_socket , inet_ntoa(cli_addr.sin_addr) , 
                     ntohs(cli_addr.sin_port));  
                    snprintf(message, BUFF_SIZE, "Connected\0");
                    // check if connection already exist
                    send_id = send(client_socket, message, BUFF_SIZE, 0);
                    if(send_id < 0){
                        perror("send error");
                        exit(1);
                    }
                    printf("Welcome message sent successfully!\n");
                    max_sd = max_sd + 1;
                    for(y = 0; y < max_connection; y++){
                    // if position is empty
                        printf("Not Adding to list of socket as %d\n", y);
                        if (client_socketArray[y] == -1){
                            client_socketArray[y] = client_socket;
                            printf("Adding to list of socket as %d\n", y);
                            break;
                        }
                    }
                    //printf("did stuff!\n");
                }
                else {
                   // printf("Print Else !\n");

                // else its some IO operation on some other socket
                    for( i = 0; i < max_connection; i++){
                        current_fd = client_socketArray[i];

                        if(FD_ISSET(current_fd, &readfds)){
                            //Check if it was for closing , and also read the 
                            //incoming message 
                            memset(client_Message, 0, BUFF_SIZE);
                            recieve_id = recv(current_fd, client_Message, BUFF_SIZE, 0);
                            char_name = strchr(client_Message, name_split);
                            name_index = char_name - client_Message;
                            char_name = char_name + 1;
                            printf("size of char %d\n", name_index);
                            memset(temp_message, 0, BUFF_SIZE);
                            snprintf(temp_message, name_index, "%s", client_Message);
                            if (char_name == NULL){
                                 char_name = "No Name\0";
                            }
                            if(recieve_id == 0 ){
                                getpeername(current_fd, (struct sockaddr *)&cli_addr, &clilen);
                                printf("Host disconnected , ip %s , port %d \n" , 
                                  inet_ntoa(cli_addr.sin_addr) , ntohs(cli_addr.sin_port)); 
                                close(current_fd);
                                client_socketArray[i] = 0;
                            }
                            else if(recieve_id < 0){
                                perror("something went wrong");
                            }
                            else{
                                //client_Message[recieve_id] = '\0';
                                for(i = 0; i < max_connection; i++){
                                    current_fd = client_socketArray[i];
                                    memset(message, 0, BUFF_SIZE);
                                    //printf("from socket %d\n", current_fd);
                                    if(current_fd != -1){
                                        
                                        sprintf(message, "<%s> %s", char_name, temp_message);
                                        printf("from client message %s %d\n",message, strlen(message));
                                        send_id = write(current_fd, message, strlen(message));
                                        if(send_id < 0){
                                            perror("error sending to a user");
                                        }
                                    } 
                                }
                            }
                        }
                    }
                    
                }

             }
         }
        /*
                
                
                printf("print else hanging here\n");
            }  
            
            printf("print for hanging here\n");
        }  
        */
         printf("print while hanging here %d\n", activity);  
    }
    return 0;
}