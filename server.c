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

/*infinite loop to for server to keep listening and accepting requests from clients */
while(1){
				
				listen(sockfd,MAX_CUE); 

				clilen = sizeof(clientAddressInfo); 

				/* should pass blank sock addre struct becuase we will need a place to store client
				info */ 
				
				newsockfd = accept(sockfd,(struct sockaddr *)&clientAddressInfo, &clilen);
				
				n = read(newsockfd,buffer,255); 

				printf("buffer content %s \n", buffer);

				bzero(buffer, 256); 

				char * welcome = "welcome";

				int i =0; 

				for(i;i<7;i++){ 
					buffer[i] = *(welcome +i);
				}

				n= write(newsockfd,buffer,255);
		
	}

}
 
