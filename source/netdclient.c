#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#define MAX 80 
#define PORT 8000
#define SA struct sockaddr 
#define PERMS 0644

struct my_msgbuf 
{
   long mtype;
   char mtext[200];
};

void chat(int sockfd) 
{ 
   struct my_msgbuf buf;
   int msqid;
   int toend;
   key_t key;
   
   // Connect to the message queue
   if ((key = ftok("msgq.txt", 'B')) == -1) 
   {
      perror("ftok");
      exit(1);
   }
  
   
   if ((msqid = msgget(key, PERMS | IPC_CREAT)) == -1) 
   { 
      perror("msgget");
      exit(1);
   }

    // Read message in message queue
    char buff[MAX]; 
    for(;;)
    {
        bzero(buf.mtext,MAX); 
        if (msgrcv(msqid, &buf, sizeof(buf.mtext), 0, 0) == -1) 
        {
         perror("msgrcv");
         exit(1);
        }

        // Send message to netdsrv
        write(sockfd, buf.mtext, MAX); 
        if ((strncmp(buf.mtext, "exit", 4)) == 0) 
        { 
            printf("- Client Exit...\n"); 
            break; 
        } 
    }
} 
  
int main() 
{ 
    int sockfd, connfd; 
    struct sockaddr_in servaddr, cli; 
  
    // socket create and varification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("> Connection creation failed...\n"); 
        exit(0); 
    } 
    else
        printf("> Connection successfully created..\n"); 
    bzero(&servaddr, sizeof(servaddr)); 
  
    // assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    servaddr.sin_port = htons(PORT); 
  
    // connect the client socket to server socket 
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
        printf("> Connection with netdsrv failed...\n"); 
        exit(0); 
    } 
    else
    {
        printf("> Connected to netdsrv..\n"); 
        printf("***READY TO SEND MESSAGE***\n\n");
        
    }

    // function for chat 
    chat(sockfd); 
  
    // close the socket 
    close(sockfd); 
} 