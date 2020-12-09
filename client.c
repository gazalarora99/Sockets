#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#define MAX_CUE 3


int main(int argc, char * argv[]){ 

/*
copied this code from lecture to use as a test client for the server we are going to make.

*/

int sockfd = -1; 
int portno = -1;
int n = -1;
char buffer[256]; 
struct sockaddr_in serverAddressInfo; 
struct hostent * serverIPAddress;


if(argc< 3){ // user didn't enter enough arguments 
return 0;
}
portno = atoi(argv[2]); 


serverIPAddress = gethostbyname(argv[1]); 


if(serverIPAddress == NULL){ 
	printf("no such host");
	return 0;
}


sockfd = socket(AF_INET,SOCK_STREAM,0); 

if(sockfd<0){ 
	return 0;
}


bzero((char *)&serverAddressInfo, sizeof(serverAddressInfo)); 

serverAddressInfo.sin_family = AF_INET;

serverAddressInfo.sin_port = htons(portno);

bcopy( (char *)serverIPAddress->h_addr,(char *)&serverAddressInfo.sin_addr.s_addr, serverIPAddress->h_length );


if(connect(sockfd, (struct sockaddr *)&serverAddressInfo, sizeof(serverAddressInfo)) < 0 ){ 
	return 0;
}
// fcntl(sockfd,F_SETFL,O_NONBLOCK);
 int count = 0;
//printf("enter the message:"); 
 while(1){
bzero(buffer,256); 
n = read(sockfd,buffer,255);

printf("%s\n",buffer);
bzero(buffer, 256);
 fgets(buffer,255,stdin); 

 n = write(sockfd,buffer,strlen(buffer)); 

//printf("client: %s", buffer);
 count++;
 }

close(sockfd);
return 0;

}




