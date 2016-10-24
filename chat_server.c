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

    struct       sockaddr_in addr, cli_addr, footAddr, baseAddr, master_addr; 
    int slack_socket, client_socket, bind_id, listen_id, listen_id_two, listen_id_three, val, listen_id_master, 
    send_id, newslack_socket, recieve_id, i, max_sd, activity, y, name_index, foot_socket, base_socket, master_socket, u, x;

    int client_socketArray[10], slack_socketArray[10], base_socket_Array[10], foot_socket_Array[10], temp_sock_array[10];
    unsigned int clilen;
    char message[BUFF_SIZE], temp_message[BUFF_SIZE], client_Message[BUFF_SIZE];
    int max_connection = 10;
    int opt = 1;
    int current_fd = 0;
    fd_set readfds, testfds;
    char name_split = '[';
    char * char_name;
    char *name_of_room;
    int cur_cli_port_number;
    char temp_int[10];
    //initialise all client_socket[] to 0 so not checked 
    for (i = 0; i < max_connection; i++)  
    {  
        client_socketArray[i] = -1;  
        slack_socketArray[i] = -1;
        base_socket_Array[i] = -1;
        foot_socket_Array[i] = -1;
        temp_sock_array[i] = -1;
    }  
     // create master socket
    slack_socket = socket(AF_INET, SOCK_STREAM, 0);
    foot_socket = socket(AF_INET, SOCK_STREAM, 0);
    base_socket = socket(AF_INET, SOCK_STREAM, 0);
    master_socket = socket(AF_INET, SOCK_STREAM, 0);

    if(master_socket < 0){
        perror("Server: can open master socket");
        exit(1);
    }
    printf("Master Socket Open %d\n", master_socket);
    if(slack_socket < 0){
		perror("server: can't open stream socket one");
        exit(1);
	}
    printf("Socket Number One %d\n", slack_socket);
    if(foot_socket < 0){
        perror("server: can't open stream socket three");
        exit(1);
    }
    printf("Socket Number Two %d\n", foot_socket);
    if(base_socket < 0){
        perror("server: can't open stream socket three");
        exit(1);
    }
    printf("Socket Number Three %d\n", base_socket);
    //set master socket to allow multiple connections , 
    //this is just a good habit, it will work without this 

    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0 )  
    {  
        perror("setsockopt master");  
        exit(1);  
    }  
    if( setsockopt(slack_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0 )  
    {  
        perror("setsockopt one");  
        exit(1);  
    }  
    
    if( setsockopt(foot_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0 )  
    {  
        perror("setsockopt two");  
        exit(1);  
    }  
    if( setsockopt(base_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0 )  
    {  
        perror("setsockopt three");  
        exit(1);  
    }  
    printf("All Three Sockets Created!\n");

    memset((char *)&addr, 0, sizeof(master_addr));
    master_addr.sin_family = AF_INET;
    master_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    master_addr.sin_port = htons(atoi(argv[1]));

    bind_id = bind(master_socket, (struct sockaddr *) &master_addr, sizeof(master_addr));
    if (bind_id < 0){
        perror("server: can't bind to master local address");
        exit(1);
    }
    // bind socket one
	memset((char *)&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(atoi(argv[2]));

    bind_id = bind(slack_socket, (struct sockaddr *) &addr, sizeof(addr));
    if (bind_id < 0){
    	perror("server: can't bind to first local address");
        exit(1);
    }

    // bind socket two
    memset((char *)&footAddr, 0, sizeof(footAddr));
    footAddr.sin_family = AF_INET;
    footAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    footAddr.sin_port = htons(atoi(argv[3]));

    bind_id = bind(foot_socket, (struct sockaddr *) &footAddr, sizeof(footAddr));
    if (bind_id < 0){
        perror("server: can't bind to second local address");
        exit(1);
    }

    // bind socket three
    memset((char *)&baseAddr, 0, sizeof(baseAddr));
    baseAddr.sin_family = AF_INET;
    baseAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    baseAddr.sin_port = htons(atoi(argv[4]));

    bind_id = bind(base_socket, (struct sockaddr *) &baseAddr, sizeof(baseAddr));
    if (bind_id < 0){
        perror("server: can't bind to third local address");
        exit(1);
    }

    listen_id_master = listen(master_socket, 10);
    listen_id = listen(slack_socket, 10);
    listen_id_two = listen(foot_socket, 10);
    listen_id_three = listen(base_socket, 10);

    if(listen_id_master < 0){
        perror("master socket couldn't connect");
        exit(1);
    }
    printf("Live and Listening at port %d!\n",   ntohs(master_addr.sin_port));
    if(listen_id < 0) {
      perror("You are already connected");
    }
    printf("Live and Listening at port %d!\n",   ntohs(addr.sin_port));
    if(listen_id_two < 0) {
      perror("You are already connected to second port");
    }
     printf("Live and Listening at port %d!\n",   ntohs(footAddr.sin_port));
    if(listen_id_three < 0) {
      perror("You are already connected to third port");
    }
    printf("Live and Listening at port %d!\n",   ntohs(baseAddr.sin_port));
    

    printf("Waiting for connections\n");

    // get largest number of the file descriptors
    val = 2;
    max_sd = master_socket;
    if(max_sd < slack_socket){
        max_sd = slack_socket;
    }
    if(max_sd < base_socket){
        max_sd = base_socket;
    }
    if(max_sd < foot_socket){
        max_sd = base_socket;
    }

    while (val > 0) {

        FD_ZERO(&readfds);
        FD_SET(master_socket, &readfds);
        FD_SET(slack_socket, &readfds);
        FD_SET(foot_socket, &readfds);
        FD_SET(base_socket, &readfds);

        for(i = 0; i < max_connection; i++){
            if(client_socketArray[i] != -1){
                FD_SET(client_socketArray[i], &readfds);
                 //printf("value is inside sets %d\n", client_socketArray[i]);
            }
            else{
                //printf("no value is inside sets %d %d\n", slack_socket, FD_SETSIZE);
            }
        }
        
        activity = select(FD_SETSIZE, &readfds, NULL, NULL, NULL);
        if(activity == 0){
            printf("timeout\n");
            continue;
        }
        if((activity < 0) && (errno !=EINTR)){
            printf("select error");
            continue;
        }
        
        
        for ( i = 0 ; i < max_sd + 1; i++)  {  
           printf("not readable %d %d\n", i, max_sd);
             
            //socket descriptor 
            current_fd = client_socketArray[i];  
              
            //if valid socket descriptor then add to read list 
            //highest file descriptor number, need it for the select function 
            
            //If something happened on the master socket , 
            //then its an incoming connection 
            /*
            */
            if(FD_ISSET(i, &readfds)){
               printf("got signal from %d \n", i);
                if(i == slack_socket || i == base_socket || i == foot_socket || i == master_socket) {
                    
                    memset(message, 0, BUFF_SIZE); // clear message 

                    if(i == master_socket){
                        clilen = sizeof(cli_addr);
                        client_socket = accept(master_socket, (struct sockaddr *) &cli_addr, &clilen);
                        if (client_socket < 0){
                            perror("accept");
                            continue;
                        } 
                        name_of_room = "Main Room\0";
                        sprintf(message, "Connected to %s, Type (1) for Slack, Type (2) Skype, Type (3) Facebook. Type new room to join new room", name_of_room);
                        send_id = send(client_socket, message, BUFF_SIZE, 0);
                        if(send_id < 0){
                            perror("send error");
                            exit(1);
                        }   

                        
                        memset(message, 0, BUFF_SIZE);
                        sprintf(message, "%d",  ntohs(addr.sin_port));
                        send_id = send(client_socket, message, BUFF_SIZE, 0);
                        if(send_id < 0){
                            perror("send error");
                            continue;
                        }     

                        memset(message, 0, BUFF_SIZE);
                        sprintf(message, "%d", ntohs(baseAddr.sin_port));
                        send_id = send(client_socket, message, BUFF_SIZE, 0);
                        if(send_id < 0){
                            perror("send error");
                            exit(1);
                        }   

                        memset(message, 0, BUFF_SIZE);
                        sprintf(message, "%d", ntohs(footAddr.sin_port));
                        send_id = send(client_socket, message, BUFF_SIZE, 0);
                        if(send_id < 0){
                            perror("send error");
                            continue;
                        }   
                        printf("hitting master\n");
                        continue;
                    }
                    else if(i == slack_socket){
                        clilen = sizeof(cli_addr);
                        client_socket = accept(slack_socket, (struct sockaddr *) &cli_addr, &clilen);
                        if (client_socket < 0){
                            perror("accept");
                            exit(1);
                        }
                        snprintf(message, BUFF_SIZE, "Connected to Slack Chat Room\0", name_of_room);
                        name_of_room = "Slack\0";
                        printf("Welcome message tried sent successfully from slack_socket!\n");
                        
                        if(client_socket > max_sd){
                            max_sd = client_socket;
                        }
                        for(y = 0; y < max_connection; y++){
                        // if position is empty
                            //printf("Not Adding to list of socket as %d\n", y);
                            if (slack_socketArray[y] == -1){
                                slack_socketArray[y] = client_socket;
                                printf("Adding to list of socket as %d\n", client_socket);
                                break;
                            }
                        }
                    }
                    else if(i == base_socket){
                        clilen = sizeof(cli_addr);
                        client_socket = accept(base_socket, (struct sockaddr *) &cli_addr, &clilen);
                        if (client_socket < 0){
                            perror("accept");
                            exit(1);
                        } 
                        name_of_room = "Facebook\0";
                        snprintf(message, BUFF_SIZE, "Connected to Facebook Chat Room\0", name_of_room);
                        printf("Welcome message tried sent successfully from base socket!\n");
                        if(client_socket > max_sd){
                            max_sd = client_socket;
                        }
                        for(y = 0; y < max_connection; y++){
                        // if position is empty
                            //printf("Not Adding to list of socket as %d\n", y);
                            if (base_socket_Array[y] == -1){
                                base_socket_Array[y] = client_socket;
                                //printf("Adding to list of socket as %d\n", y);
                                break;
                            }
                        }
                    }
                    else{
                        clilen = sizeof(cli_addr);
                        client_socket = accept(foot_socket, (struct sockaddr *) &cli_addr, &clilen);
                        if (client_socket < 0){
                            perror("accept");
                            exit(1);
                        }
                        name_of_room = "Skype\0";
                        snprintf(message, BUFF_SIZE, "Connected to Skype Chat Room\0", name_of_room);
                        printf("Welcome message try sending successfully from foot_socket !\n");
                        if(client_socket > max_sd){
                            max_sd = client_socket;
                        }
                        for(y = 0; y < max_connection; y++){
                        // if position is empty
                            printf("Not Adding to list of socket as %d\n", y);
                            if (foot_socket_Array[y] == -1){
                                foot_socket_Array[y] = client_socket;
                                printf("Adding to list of socket as %d\n", y);
                                break;
                            }
                        }
                    }
                    printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , 
                    client_socket , inet_ntoa(cli_addr.sin_addr) , 
                     ntohs(cli_addr.sin_port));  
                    // check if connection already exist
                                
                    send_id = send(client_socket, message, BUFF_SIZE, 0);
                    if(send_id < 0){
                        perror("send error");
                       // exit(1);
                        exit(1);
                    }   
                    
                    //printf("did stuff!\n");
                    printf("Welcome message sent successfully!\n");
                    for(y = 0; y < max_connection; y++){
                        // if position is empty
                            //printf("Not Adding to list of socket as %d\n", y);
                            if (client_socketArray[y] == -1){
                                client_socketArray[y] = client_socket;
                                //printf("Adding to list of socket as %d\n", y);
                                break;
                            }
                    }
                }
                else {
                   printf("Print Recieve Message from a port !\n");

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
                            printf("Size of char_name: %s SIze of client Message: %s size of char %d\n", char_name, client_Message, name_index);
                            memset(temp_message, 0, BUFF_SIZE);
                            snprintf(temp_message, name_index, "%s", client_Message);
                            if (char_name == NULL){
                                 char_name = "No Name\0";
                            }
                            if(recieve_id == 0 ){
                                getpeername(current_fd, (struct sockaddr *)&cli_addr, &clilen);
                                printf("Host disconnected , ip %s , port %d \n" , 
                                  inet_ntoa(cli_addr.sin_addr) , ntohs(cli_addr.sin_port)); 
                                client_socketArray[i] = -1;
                            }
                            else if(recieve_id < 0){
                                perror("something went wrong");
                            }
                            else{
                                printf("Trying to sending information to other client on user port!\n");
                                
                                if (getsockname(current_fd, (struct sockaddr *)&cli_addr, &clilen) == -1){
                                    perror("getting socket name error");
                                    continue;
                                }
                                else{
                                    cur_cli_port_number = ntohs(cli_addr.sin_port);
                                    printf("Got port number of user that is sending next message. port: %d\n", cur_cli_port_number);
                                     memset(temp_sock_array, -1, sizeof(temp_sock_array) + 1);
                                    if(cur_cli_port_number == ntohs(addr.sin_port)){
                                       memcpy(temp_sock_array, slack_socketArray, sizeof(slack_socketArray) + 1);
                                       for(u = 0; u < sizeof(slack_socket); u++){
                                        printf("%d %d\n", temp_sock_array[u], slack_socketArray[u]);
                                       }
                                    }
                                    else if(cur_cli_port_number == ntohs(baseAddr.sin_port)){
                                        memcpy(temp_sock_array, base_socket_Array, sizeof(base_socket_Array) + 1);
                                    }
                                    else{
                                        memcpy(temp_sock_array, foot_socket_Array, sizeof(foot_socket_Array) + 1);
                                    }
                                //client_Message[recieve_id] = '\0';
                                    if(strcmp(temp_message, "new room\n") == 0){
                                        memset(message, 0, BUFF_SIZE);
                                        sprintf(message, "Connected to %s, Type (1) for Slack, Type (2) Skype, Type (3) Facebook.", name_of_room);
                                        printf("new room requested message %s %d\n",message, current_fd);
                                        send_id = write(current_fd, message, strlen(message));
                                        if(send_id < 0){
                                            perror("error sending to a user");
                                            continue;
                                        }
                                        client_socketArray[i] = -1;
                                    }
                                    else{
                                    for(x = 0; x < max_connection; x++){
                                        current_fd = temp_sock_array[x];
                                        memset(message, 0, BUFF_SIZE);
                                        //printf("from socket %d\n", current_fd);
                                        if(current_fd != -1){
                                            
                                            sprintf(message, "<%s> %s", char_name, temp_message);
                                            printf("from client message %s %d\n",message, current_fd);
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

             }
         }
        /*
                
                
                printf("print else hanging here\n");
            }  
            
            printf("print for hanging here\n");
        }  
        */
         //printf("print while hanging here %d\n", activity);  
    }
    return 0;
}