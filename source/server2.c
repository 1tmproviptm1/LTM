#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define PERMS2 0666

struct my_msgbuf 
{
   long mtype;
   char mtext[200];
};

int main(void) 
{
   struct my_msgbuf buf;
   int msqid;
   int toend;
   key_t key;
   // Connect to message queue 2
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


   printf("> Message queue: ready to receive messages \n");
   
   // Read message in message queue 2
   for(;;) 
   { 
      bzero(buf.mtext,sizeof(buf.mtext));
      if (msgrcv(msqid, &buf, sizeof(buf.mtext), 0, 0) == -1) 
      {
         perror("msgrcv");
         exit(1);
      }

      //
      printf("> Message: %s\n", buf.mtext);
      toend = strcmp(buf.mtext,"end");
      if (toend == 0)
      break;
   }
   printf("> Message queue: done receiving messages \n");
   system("rm msgq2.txt");
   return 0;
}