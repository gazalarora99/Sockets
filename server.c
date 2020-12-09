#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
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

void check(int newsock, char* buffer, int expec_len, int msg_num){
  buffer = (char*) malloc((expec_len+1)*sizeof(char));
  buffer[expec_len] = '\0';
  char * buf = (char*) malloc(2*sizeof(char));
  buf[1] = '\0';
  int n;
  int mode = 1;
  //  n =  ioctl(newsock,FIONBIO,(char*) &mode);
  //  ioctl(newsock,FIONREAD,(char*) &n);
  //fcntl(newsock,F_SETFL,O_NONBLOCK);
  //printf("n: %d\n", n);
  n = read(newsock, buf, 1);
  strcat(buffer,buf);
  fcntl(newsock,F_SETFL,O_NONBLOCK);    
  int i=1;
  int end;
  // if(n != expec_len) end = n;
  // else end = expec_len;
  while(i<expec_len){
    //    strcat(buffer,buf);
    // printf("buffer %s\n", buffer);
    //i++;
     n = read(newsock, buf, 1);
     if(n < 0) { break; }
     strcat(buffer,buf);
     printf("n: %d, buffer %s\n", n, buffer);
     i++;
  }
  if(errno==EAGAIN) puts("rec error");
  printf("n: %d\n", n);
  printf("buffer %s\n", buffer);
  free(buffer);
  free(buf);
  /*
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
  */
}

int main(int argc, char * argv[]){ 

int sockfd = -1; 
int newsockfd = -1; 
int portno = -1; 
int clilen = -1; 
int n = -1; 
char* buffer; 

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

//listen(sockfd,MAX_CUE);

//clilen = sizeof(clientAddressInfo);
/* should pass blank sock addre struct becuase we will need a place to store client                                                                      
info */
//newsockfd = accept(sockfd,(struct sockaddr *)&clientAddressInfo, &clilen);
 int mode = 1;
/*infinite loop to for server to keep listening and accepting requests from clients */
while(1){
  
				listen(sockfd,MAX_CUE); 

				clilen = sizeof(clientAddressInfo); 

				/* should pass blank sock addre struct becuase we will need a place to store client
				info 
				*/
				
  				newsockfd = accept(sockfd,(struct sockaddr *)&clientAddressInfo, &clilen);
				//ioctl(newsockfd,FIONBIO,(char*) &mode);
				//char * welcome = "REG|13|Knock, Knock.|\0";
				buffer = (char*) malloc(22*sizeof(char));
				strcpy(buffer, "REG|13|Knock, Knock.|\0");
			          /*for(i=0;i<22;i++){
                                        buffer[i] = *(welcome +i);
                                }*/
				n = write(newsockfd,buffer,22);
				printf("%s\n", buffer);
				free(buffer);

				check(newsockfd, buffer, 20, 1); //1 for the msg who's there
				
				/*if(compare(buffer, 1) == -1){
				  //check type of error
				}
				else{
				  //print the buffer msg after separation
				  
				  printf("Who's there?\n");
				}
				*/
				bzero(buffer, 256);

				char* stud = "REG|8|Student.|\0";
				
				for(int i=0; i<16; i++){
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
				for(int i = 0 ; i < 24 ; i++){
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
				 close(newsockfd);
				//				bzero(buffer, 256);
 }			        
// close(newsockfd);
 close(sockfd);
}
 
