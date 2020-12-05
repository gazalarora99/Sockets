#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#define MAX_CUE 1 

/*
 struct sockaddr_in {
               sa_family_t    sin_family;  address family: AF_INET 
               in_port_t      sin_port;    port in network byte order 
               struct in_addr sin_addr;    internet address 
           };
*/
int main(int argc, char * argv[]){ 


struct sockaddr_in * server_addr =  (struct sockaddr_in * )malloc(sizeof(struct sockaddr_in)); 
struct in_addr * server_inad = (struct in_addr *)malloc(sizeof(struct in_addr));


struct sockaddr_in * client_addr =  (struct sockaddr_in * )malloc(sizeof(struct sockaddr_in));
struct in_addr * client_inad = (struct in_addr *)malloc(sizeof(struct in_addr)); 

server_addr->sin_family  = AF_INET; 
server_addr->sin_port = htons(55000); // this port value will be raead in as an arg but I'm assigning one for test purposes 






int server_soc = socket(AF_INET, SOCK_STREAM, 0); 

if(sock_fd <0){ 
return 0;
}

struct sockaddr * ptr2 = (struct sockaddr *)server_addr;


int error = bind(server_soc, &(server_addr->sin_port) , addr_size);


if(error < 0){ 
	return 0;
}



int err = listen(sock_fd,MAX_CUE); 

if(err <0){ 
	return 0;
}


int accept_soc = accept(sock_fd, (struct sockaddr * )client_addr , ptr ); 

if(accept_soc<0){ 
return 0;
}

char arr[100]; 

read(accept_soc, (void *)arr , sizeof(char)*100);



close(sock_fd); 
close(accept_soc);
free(addr);
free(inad);



}
 
