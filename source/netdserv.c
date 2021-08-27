#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#define MAX 80 
#define PORT 8000
#define SA struct sockaddr 
#define PERMS2 0666

struct my_msgbuf 
{
   long mtype;
   char mtext[200];
};

char buff[MAX]; 
    int n; 
   int msqid;
   int len;
   key_t key;


// Function designed for chat between client and server. 
void chat(int sockfd) 
{ 
    struct my_msgbuf buf;

    // Create message queue 2
   system("touch msgq2.txt");
   if ((key = ftok("msgq2.txt", 'B')) == -1) 
   {
      perror("ftok");
      exit(1);
   }
   
   if ((msqid = msgget(key, PERMS2 | IPC_CREAT)) == -1) 
   {
      perror("msgget");
      exit(1);
   }

    buf.mtype = 1; 

    // Receive message from client  & Write in message queue 2
     for(;;)
	{
		bzero(buff,MAX); 
		bzero(buf.mtext, MAX);

		// read the message from client and copy it in buffer 
		read(sockfd, buff, sizeof(buff)); 

        // Write message in message queue 2
        strcpy(buf.mtext,buff);
		len = strlen(buf.mtext);
        /* remove newline at end, if it exists */
        if (buf.mtext[len-1] == '\n') buf.mtext[len-1] = '\0';
        if (msgsnd(msqid, &buf, len+1, 0) == -1) /* +1 for '\0' */
        perror("msgsnd");
		if (strncmp("exit", buff, 4) == 0) 
        { 
			printf("Netdsrv exit...\n"); 
			break; 
		}
	}
       
}    
  

int main() 
{ 
    int sockfd, connfd, len; 
    struct sockaddr_in servaddr, cli; 
  
    /// Create socket
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
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(PORT); 
  
    // Binding newly created socket to given IP and verification 
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
        printf("> Connection bind failed...\n"); 
        exit(0); 
    } 
    else
        printf("> Connection successfully binded..\n"); 
  
    // Now server is ready to listen and verification 
    if ((listen(sockfd, 5)) != 0) { 
        printf("> Listen failed...\n"); 
        exit(0); 
    } 
    else
        printf("> Netdsrv listening..\n"); 
    len = sizeof(cli); 
  
    // Accept the data packet from client and verification 
    connfd = accept(sockfd, (SA*)&cli, &len); 
    if (connfd < 0) { 
        printf("> Netdsrv acccept failed...\n"); 
        exit(0); 
    } 
    else
    {
        printf("> Netdsrv acccepted Netdclient...\n"); 
        printf("***READY TO RECEIVE MESSAGE***\n");
    
    }

    // Function for chatting between netdclient and netdsrv 
    chat(connfd); 
  
    // After chatting close the socket 
    close(sockfd); 
} 