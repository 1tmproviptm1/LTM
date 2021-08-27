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
#define PORT 8080 
#define SA struct sockaddr 
#define PERMS 0644

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

    // create message queue
    system("touch msgq.txt");
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

    buf.mtype = 1; 

    // infinite loop for chat 
    for (;;) 
    { 
        bzero(buff, MAX); 
        char temp[MAX];

        // read the message from client and copy it in buffer 
        read(sockfd, buff, sizeof(buff)); 

        // print buffer which contains the client contents 
        printf("Client: %s", buff);

        //temp=buff
        strcpy(temp,buff);
        bzero(buff, MAX); 
        n = 0; 

       //buf.mtext=temp
       strcpy(buf.mtext,temp); 
    {

    // write message to message queue
    len = strlen(buf.mtext);
    /* remove newline at end, if it exists */
    if (buf.mtext[len-1] == '\n') buf.mtext[len-1] = '\0';
    if (msgsnd(msqid, &buf, len+1, 0) == -1) /* +1 for '\0' */
      perror("msgsnd");
   }
   strcpy(buf.mtext, "end");
   len = strlen(buf.mtext);

    // convert lowercase to uppercase
    for (int i = 0; temp[i] != '\0'; i++)
    {
        if (temp[i] >= 'a' && temp[i] <= 'z')
        {
            temp[i] = temp[i] - 32;
        }
    }

    // copy server message in the buffer 
    buff[n++]=temp; 

    // and send that buffer to client 
    write(sockfd,temp, sizeof(temp)); 
    bzero(temp,sizeof(temp));
    
    // if msg contains "Exit" then server exit and chat ended. 
    if (strncmp("exit", buff, 4) == 0) 
       { 
            printf("Server Exit...\n"); 
            break; 
        } 
    }
    printf("> Message queue: done sending messages.\n");
} 
  

int main() 
{ 
    int sockfd, connfd, len; 
    struct sockaddr_in servaddr, cli; 
  
    /// Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("> Socket creation failed...\n"); 
        exit(0); 
    } 
    else
        printf("> Socket successfully created...\n"); 
    bzero(&servaddr, sizeof(servaddr)); 
  
    // assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(PORT); 
  
    // Binding newly created socket to given IP and verification 
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
        printf("> Socket bind failed...\n"); 
        exit(0); 
    } 
    else
        printf("> Socket successfully binded...\n"); 
  
    // Now server is ready to listen and verification 
    if ((listen(sockfd, 5)) != 0) { 
        printf("> Listen failed...\n"); 
        exit(0); 
    } 
    else
        printf("> Server listening...\n"); 
    len = sizeof(cli); 
  
    // Accept the data packet from client and verification 
    connfd = accept(sockfd, (SA*)&cli, &len); 
    if (connfd < 0) { 
        printf("> Server acccept failed...\n"); 
        exit(0); 
    } 
    else
        printf("> Server acccept the client...\n"); 
  
    // Function for chatting between client and server 
    chat(connfd); 
  
    // After chatting close the socket 
    close(sockfd); 
} 