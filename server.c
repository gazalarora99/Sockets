#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#define MAX_CUE 2 

/* 
note to self, if i run the program and it continues to run. Don't worry just yet sockets will wait until 
they recieve an incomming request 
*/

int compare(char buffer[256], int c){

  char* who = "REG|12|Who's there?|\0";
  char* stu = "REG|13|Student, who?|\0";
  if(c==1){
  for(int i = 0; i < strlen(who); i++){
    if(who[i]==buffer[i]) continue;
    else return -1;
  }
  }
  else if (c==2){
    for(int i = 0; i < strlen(stu); i++){
    if(stu[i]==buffer[i]) continue;
    else return	-1;
  }
  }
  return 0;
}

void error(char buffer[256], int expec_len, int msg_num){
  if(buffer[0]=='E'){
    //received an error msg from client
  }
  else if(buffer[0]=='R'){
    //send error msg to client as server received incorrect msg
    if(buffer[1]!='E' || buffer[2]!='G' || buffer[3]!='|'){
      //format error
      return;
    }
    else{
      int s=0;
      for(int i = 4 ; i < 255; i++){
	if(buffer[i]=='|'){
	  s++;
	}
      }
      if(s!=2){
	//missing | format error
	return;
      }
      int k = 0;
      int length = 0;
      for(int i = 4 ; i < 255; i++){
	if(buffer[i]=='|'){
	  break;
	}
	//if not a number then return error, ex reg|23yuh|hu|
	k++;
      }
      if(k==0) {
	//missing length LN error
	return;
      }
      for(int i = 4; i<4+k; i++){
	//	length = ((10*length) + atoi(buffer[i]));	
      }
      if(length!=expec_len){
	//LN error
	return;
      }
      //compare strings now
    }

  }
  else{
    //missing msg type
  }
}

int main(int argc, char * argv[]){ 

int sockfd = -1; 
int newsockfd = -1; 
int portno = -1; 
int clilen = -1; 
int n = -1; 
char buffer[256]; 

struct sockaddr_in serverAddressInfo; 
struct sockaddr_in clientAddressInfo; 

if(argc<2){ 
printf("Error"); 
exit(1);
}

//  read in IP address to connect to and port number from command line
portno = atoi(argv[1]);


sockfd = socket(AF_INET, SOCK_STREAM, 0); 

bzero( (char *)&serverAddressInfo, sizeof(serverAddressInfo)); 

serverAddressInfo.sin_port = htons(portno); 

serverAddressInfo.sin_family = AF_INET; 


/*This below statement means I am willing to acccept connections 
from anyone*/
serverAddressInfo.sin_addr.s_addr = INADDR_ANY; 


if(bind(sockfd,(struct sockaddr *)&serverAddressInfo, sizeof(serverAddressInfo))<0){
	
		printf("error");
	} 

listen(sockfd,MAX_CUE);

clilen = sizeof(clientAddressInfo);
/* should pass blank sock addre struct becuase we will need a place to store client                                                                      
info */
newsockfd = accept(sockfd,(struct sockaddr *)&clientAddressInfo, &clilen);
 int count = 0;
/*infinite loop to for server to keep listening and accepting requests from clients */
while(1){
  /*
				listen(sockfd,MAX_CUE); 

				clilen = sizeof(clientAddressInfo); 

				/* should pass blank sock addre struct becuase we will need a place to store client
				info 
  
				
  				newsockfd = accept(sockfd,(struct sockaddr *)&clientAddressInfo, &clilen);
  */
  bzero(buffer, 256);
  
char * welcome = "REG|13|Knock, Knock.|\0";

                                int i;

                                for(i=0;i<22;i++){
                                        buffer[i] = *(welcome +i);
                                }
				n = write(newsockfd,buffer,255);
				printf("Knock, Knock.\n");
				bzero(buffer, 256);
				n = read(newsockfd,buffer,255);
				//	printf("%s \n", buffer); 
				
				if(compare(buffer, 1) == -1){
				  //check type of error
				}
				else{
				  //print the buffer msg after separation
				  
				  printf("Who's there?\n");
				}
				
				bzero(buffer, 256);

				char* stud = "REG|8|Student.|\0";
				
				for(i=0; i<16; i++){
				  buffer[i] = *(stud+i);
				}
				n = write(newsockfd,buffer,255);
                                printf("Student.\n");
                                bzero(buffer, 256);
				n = read(newsockfd,buffer,255);
				//printf("%s \n", buffer); 
   				if(compare(buffer,2) == -1){
				  //check type of error
				}
				else{
				  //print the buffer msg after separation
				  
				  printf("Student, who?\n");
				}
				
				bzero(buffer, 256);
				char* punch = "REG|14|Systems Master.|\0";
				for(i = 0 ; i < 24 ; i++){
				  buffer[i] = *(punch+i);
				}
				n = write(newsockfd,buffer,255);
				printf("Systems Master.\n");
				bzero(buffer, 256);

				n = read(newsockfd,buffer,255);
				if(buffer[0]=='R'){
				  //getMsg(buffer, 3);
				  printf("%s \n", buffer);
				}
				//				bzero(buffer, 256);
 }			        
 close(newsockfd);
 close(sockfd);
}
 
