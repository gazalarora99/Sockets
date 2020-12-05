#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#define MAX_CUE 3


int main(int argc, char * argv[]){ 



struct sockaddr_in * addr =  (struct sockaddr_in * )malloc(sizeof(struct sockaddr_in)); 
struct in_addr * inad = (struct in_addr *)malloc(sizeof(struct in_addr));

addr->sin_family  = AF_INET; 
addr->sin_port = htons(55000); // this port value will be raead in as an arg but I'm assigning one for test purposes 

socklen_t addr_size = (socklen_t)sizeof(*(addr));  

//socklen_t * ptr  = &(addr_size) ; 

//*ptr = addr_size;



int sock_fd = socket(AF_INET, SOCK_STREAM, 0); 

if(sock_fd <0){ 
return 0;
}

struct sockaddr * ptr2 = (struct sockaddr *)addr;

/*
int error = bind(sock_fd, ptr2 , addr_size);


if(error < 0){ 
	return 0;
}



int err = listen(sock_fd,MAX_CUE); 

if(err <0){ 
	return 0;
}

*/ 

int accept_soc = connect(sock_fd, (struct sockaddr * )addr , addr_size ); 

if(accept_soc<0){ 
return 0;
}

char arr[100]; 

write(accept_soc, (void *)arr , sizeof(char)*100);


close(accept_soc);


}
 



