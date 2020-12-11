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
#define CR_RANGE(X) (X =='4' || X == '2' || X == '0')
#define IN_RANGE(X) (X =='5' || X == '3' || X == '1')  
/* in range macro is used to check number for error message */

char* LNerror(char* buffer, char* result, int msg_num){
result = malloc(10*sizeof(char));
          if(msg_num == 1){
            strcpy(result, "ERR|M1LN|\0");
            free(buffer);
            return result;
          }
          else if(msg_num == 3){
            strcpy(result, "ERR|M3LN|\0");
            free(buffer);
            return result;
          }
          else if(msg_num == 5){
            strcpy(result, "ERR|M5LN|\0");
            free(buffer);
            return result;
          }

}

char* FTerror(char* buffer, char* result, int msg_num){
result = malloc(10*sizeof(char));
    if(msg_num == 1){
      strcpy(result, "ERR|M1FT|\0");
      free(buffer);
      return result;
    }
    else if(msg_num == 3){
      strcpy(result, "ERR|M3FT|\0");
      free(buffer);
      return result;
    }
    else if(msg_num == 5){
      strcpy(result, "ERR|M5FT|\0");
      free(buffer);
      return result;
    }
}

/* Return res
res == "close" means we received an error from client                                                               
res == "correct" means we received what we expected so                  
we can send a regular response.                                                                         
res == "ERR|....|" means                                                
we received an incorrectly formatted msg so we need to           
send/report an error to our client which is stored in res, so                                  
simply compare first three chars of res with ERR in while loop                                                       
*/
char* check(int newsock, char* buffer, int expec_len, int msg_num){
  char* result; //we return this to our while loop
  buffer = (char*) malloc((expec_len+1)*sizeof(char));
  bzero(buffer,expec_len+1);
  buffer[expec_len] = '\0';
  
  char * buf = (char*) malloc(2*sizeof(char));
  bzero(buf,2);// my edit
  buf[1] = '\0';
  int n;
  int len=0;
  int t=0;
  n = read(newsock, buf, 1); //wait for client's reply & read 1 byte
  if(buf[0]=='R'){
    strcat(buffer,buf);
    n = read(newsock, buf, 1);
    if(buf[0]=='E'){
      strcat(buffer,buf);
      n = read(newsock, buf, 1);
      if(buf[0]=='G'){
	strcat(buffer,buf);
	n = read(newsock, buf, 1);
	if(buf[0]=='|'){
	  strcat(buffer,buf);

	  do{
	  n = read(newsock, buf, 1);
	  if(buf[0]==124) { strcat(buffer,buf); break; }
	  else if((buf[0]<48 || buf[0] > 57) && buf[0]!=124){
	    return FTerror(buffer, result, msg_num);
	  }
	  if(n==0){
              printf("Connection closed by client as read was blocked due to incorrect msg length provided by client in their message %s\n", buffer);
              result = malloc(6*sizeof(char));
              strcpy(result, "close\0");
              free(buffer);
              return result;
	  }
	  strcat(buffer,buf);
	  len = (10*len)+(buf[0]-'0');
	  t++;
	  } while(buf[0]!='|');
	  
	  if(len==0){
	    return LNerror(buffer, result, msg_num);
	  }
	  if(expec_len < (len+t+6)){ return LNerror(buffer, result, msg_num);}

	  if(msg_num == 1 && len!=12){
	    return LNerror(buffer, result, msg_num);
	  }
	  if(msg_num == 3 && len!=10){
	    return LNerror(buffer, result, msg_num);
	  }
	  //	  printf("len %d\n",len);
	  int i=0;
	  while(i<len+1){
	    n = read(newsock, buf, 1);
	    if(n==0){
	      printf("Connection closed by client as read was blocked due to incorrect msg length provided by client in their message %s\n", buffer);
	      result = malloc(6*sizeof(char));
	      strcpy(result, "close\0");
	      free(buffer);
	      return result;
	    }
	    strcat(buffer,buf);
	    if(buf[0]==124) { break; }
	    i++;
	  }
	  if(i<len) { return LNerror(buffer, result, msg_num); }
	  if(buf[0]!=124){ return FTerror(buffer, result, msg_num); }
	  
	  //have complete msg here
	  //now compare
	  char* str = malloc((len+1)*sizeof(char));
	  str[len] = '\0';
	  int m, l;
	  for(m=0, l=t+5; m<len, l<t+5+len; m++, l++){
	    str[m] = buffer[l];
	  }
	  //	  printf("%s\n",str);
	  if(msg_num==1 && strcmp(str,"Who's there?\0")==0){
	    result = malloc(8*sizeof(char));
	    strcpy(result, "correct\0");
	    printf("%s\n",str);
	    free(str);
	    free(buffer);
	    return result;
	  }
	  else if(msg_num==3 && strcmp(str,"Tank, who?\0")==0){
	    result = malloc(8*sizeof(char));
	    strcpy(result, "correct\0");
            printf("%s\n",str);
	    free(str);
	    free(buffer);
            return result;
	  }

	  else if(msg_num==5){
	    if(str[len-1]!=33 && str[len-1]!=46 && str[len-1]!=63){
	      result = malloc(10*sizeof(char));
	      strcpy(result, "ERR|M5CT|\0");
	      printf("Missing or incorrect punctuation at end of msg5, accepted punctuations: '?' '.' '!'\n");
	      free(buffer);
	      free(str);
	      return result;
	    }
	    //printf("correct %s\n",str);                                                                                                                                                    
	    result = malloc(8*sizeof(char));
	    strcpy(result, "correct\0");
	    printf("%s\n",str);
	    free(str);
	    free(buffer);
	    return result;
	    
	  }
	  else{
	    //content error for msg 1 or msg3                                                                                                                                                
	    result = malloc(10*sizeof(char));
	    if(msg_num == 1){
	      strcpy(result, "ERR|M1CT|\0");
	      free(str);
	      free(buffer);
	      return result;
	    }
	    else if(msg_num == 3){
	      strcpy(result, "ERR|M3CT|\0");
	      free(str);
	      free(buffer);
	      return result;
	    }


	  }
	}
	else { return FTerror(buffer, result, msg_num); }
      }
      else { return FTerror(buffer, result, msg_num); }
    }
    else {return FTerror(buffer, result, msg_num);}
  }
    


  else if (buf[0]=='E'){
    result = malloc(6*sizeof(char));
    strcpy(result, "close\0");
    
    strcat(buffer,buf);
    n = read(newsock, buf, 1);
    if(buf[0]=='R'){
      strcat(buffer,buf);
      n = read(newsock, buf, 1);
      if(buf[0]=='R'){
	strcat(buffer,buf);
	n = read(newsock, buf, 1);
	if(buf[0]=='|'){
	  strcat(buffer,buf);
	  n = read(newsock, buf, 1);
	  if(buf[0]=='M'){
	    strcat(buffer,buf);
	    n = read(newsock, buf, 1);
	    if(CR_RANGE(buf[0])){
	      strcat(buffer,buf);
	      n = read(newsock, buf, 1);
	      if(buf[0]=='F' || buf[0]=='C'){
		strcat(buffer,buf);
		n = read(newsock, buf, 1);
		if(buf[0]=='T'){
		  strcat(buffer,buf);
		  n = read(newsock, buf, 1);
		  if(buf[0]=='|'){
		    strcat(buffer,buf);
		    printf("Received error %s from client, closing connection\n",buffer);
		    free(buffer);
		    return result;
		  }
		  else{
		    printf("Client sent error %s in incorrect format, closing connection\n", buffer); 
		    free(buffer);
		    return result;
		  }
		}
		else{
		  printf("Client sent error %s in incorrect format, closing connection\n", buffer);
                  free(buffer);
		  return result;  
		}
	      }
	      else if(buf[0]=='L'){
		strcat(buffer,buf);
                n = read(newsock, buf, 1);
		if(buf[0]=='N'){
		  strcat(buffer,buf);
                  n = read(newsock, buf, 1);
                  if(buf[0]=='|'){
                    strcat(buffer,buf);
                    printf("Received error %s from client, closing connection\n",buffer);
                    free(buffer);
                    return result;
                  }
                  else{
                    printf("Client sent error %s in incorrect format, closing connection\n", buffer);
                    free(buffer);
                    return result;
                  }
		}
                else{
                  printf("Client sent error %s in incorrect format, closing connection\n", buffer);
                  free(buffer);
                  return result;
		}
	      }
	      else {
		printf("Client sent error %s in incorrect format, closing connection\n", buffer);
		free(buffer);
		return result;
	      }
	    }
	    else{
	      printf("Client sent error %s in incorrect format, closing connection\n", buffer);
	      free(buffer);
	      return result;
	    }
	  }
	  else {
	    printf("Client sent error %s in incorrect format, closing connection\n", buffer);
	    free(buffer);
	    return result;
	  }
	}
	else{
	  printf("Client sent error %s in incorrect format, closing connection\n", buffer);
	  free(buffer);
	  return result;
	}
      }
      else{
	printf("Client sent error %s in incorrect format, closing connection\n", buffer);
	free(buffer);
	return result;
      }
    }
    else{
      printf("Client sent error %s in incorrect format, closing connection\n", buffer);
      free(buffer);
      return result;
    }
  }
  else{
    return  FTerror(buffer, result, msg_num);
  }

  /*
  fcntl(newsock,F_SETFL,O_NONBLOCK); //unblock read    
  int i=1;
  while(i<expec_len){
     n = read(newsock, buf, 1);
     if(n < 0) { break; }
     buffer = strcat(buffer,buf);
     // printf("n: %d, buffer %s\n", n, buffer);
     i++;
  }
  //  if(errno==EAGAIN) puts("rec error");
  //printf("n: %d\n", n);
  //  printf("buffer %s\n", buffer);
  int flags = fcntl(newsock, F_GETFL);
  flags &= ~O_NONBLOCK;
  fcntl(newsock,F_SETFL,flags); //block read
  //free(buffer);
  //  free(buf);
  
  if(buffer[0]=='E' && buffer[1] == 'R' && buffer[2]=='R' && buffer[3]=='|' && buffer[4] == 'M' && IN_RANGE(buffer[5]) && buffer[8]=='|'  ){ 
    //received an error msg from client
    //res will be set to "close"
   	
    if( (buffer[6] == 'C' || buffer[6] == 'F') && buffer[7] == 'T' && CR_RANGE(buffer[5])){ 
      printf("Received error %s from client, closing connection",buffer);
      result = malloc(6*sizeof(char));
      strcpy(result, "close\0");
      return result;
    }
    else if ( buffer[6] == 'L' && buffer[7] == 'N' && CR_RANGE(buffer[5])){ 
      printf("Received error %s from client, closing connection",buffer);
      result = malloc(6*sizeof(char));
      strcpy(result, "close\0");
      return result;
    }  
    else{ 
      printf("client sent error %s in incorrect format, closing connection", buffer);
      strcpy(result, "close\0");
      return result;
    }
    //printf("Received error from client, closing connection: %s",buffer);
    //result = malloc(6*sizeof(char));
    //strcpy(result, "close\0");
    //return result;
  
  }
  else if(buffer[0]=='R' && buffer[1]=='E' && buffer[2]=='G' && buffer[3]=='|'){
    
    int s=0; //keeps count of |
    for(int i = 4 ; i < strlen(buffer); i++){
	if(buffer[i]=='|'){
	  s++;
	}
    }
    if(s!=2){
	//extra or missing | character so format error
	result = malloc(10*sizeof(char));
	if(msg_num == 1){
	  strcpy(result, "ERR|M1FT|\0");
	  free(buffer);
	  return result;
	}
	else if(msg_num == 3){
	  strcpy(result, "ERR|M3FT|\0");
	  free(buffer);
	  return result;
	}
	else if(msg_num == 5){
	  strcpy(result, "ERR|M5FT|\0");
	  free(buffer);
	  return result;
	}
    }

      int k = 0; //num of characters that represent length
      int length = 0;
      int pipe = -1;
      for(int i = 4 ; i < strlen(buffer); i++){
	//break once you find very next | character, ex reg|23|bla|
	if(buffer[i]=='|'){
	  pipe=i;
	  break;
	}
	//if not a number then return length error, ex reg|23yuh|hu|
	if(buffer[i] < 48 && buffer[i] > 57){
	  result = malloc(10*sizeof(char));
	  if(msg_num == 1){
	    strcpy(result, "ERR|M1LN|\0");
	    free(buffer);
	    return result;
	  }
	  else if(msg_num == 3){
	    strcpy(result, "ERR|M3LN|\0");
	    free(buffer);
	    return result;
	  }
	  else if(msg_num == 5){
	    strcpy(result, "ERR|M5LN|\0");
	    free(buffer);
	    return result;
	  }
	}
	k++;
      }
      if(k==0) {
	//missing length LN error
	result = malloc(10*sizeof(char));
          if(msg_num == 1){
            strcpy(result, "ERR|M1LN|\0");
            free(buffer);
            return result;
          }
          else if(msg_num == 3){
            strcpy(result, "ERR|M3LN|\0");
            free(buffer);
            return result;
          }
          else if(msg_num == 5){
            strcpy(result, "ERR|M5LN|\0");
            free(buffer);
            return result;
          }
      }

      //  if(pipe == (4+k)) puts("yes, pipe same as 4+k");
      for(int i = 4; i<4+k; i++){
	length = ((10*length) + (buffer[i]-'0'));	
      }
      //printf("len, %d\n", length);
      if(msg_num == 1 && length!=12){
	result = malloc(10*sizeof(char));
	strcpy(result, "ERR|M1LN|\0");
	free(buffer);
	return result;
      }
      if(msg_num == 3 && length!=18){
	result = malloc(10*sizeof(char));
	strcpy(result, "ERR|M3LN|\0");
	free(buffer);
	return result;
      }

      if(buffer[pipe+length+1] != '|'){
	//pipe | not present at expected position ex reg|3|ughh| or reg|3|ug| or reg|3|ug|h
	//LN error
	result = malloc(10*sizeof(char));
          if(msg_num == 1){
            strcpy(result, "ERR|M1LN|\0");
            free(buffer);
            return result;
          }
          else if(msg_num == 3){
            strcpy(result, "ERR|M3LN|\0");
            free(buffer);
            return result;
          }
          else if(msg_num == 5){
            strcpy(result, "ERR|M5LN|\0");
            free(buffer);
            return result;
          }
      }

      //here if REG|numbers|blabla|
      char* str = malloc((length+1)*sizeof(char));
      //compare strings now
      str[length] = '\0';
      int i, j;
      for(i = pipe+1, j=0; i < pipe+length+1, j<length; i++, j++){
	str[j] = buffer[i];
      }
      //printf("str %s\n",str);
      if(msg_num==1 && strcmp(str,"Who's there?\0")==0){
	//printf("correct %s\n",str);
	result = malloc(8*sizeof(char));
	strcpy(result, "correct\0");
	printf("%s\n",str);
	free(str);
	free(buffer);
	return result;
      }
      else if(msg_num==3 && strcmp(str,"JA Francisco, who?\0")==0){
	//printf("correct %s\n",str);
	result = malloc(8*sizeof(char));
        strcpy(result, "correct\0");
	printf("%s\n",str);
	free(str);
	free(buffer);
        return result;
      }
      else if(msg_num==5){
	//extra char after last pipe ex reg|3|ugh|hhhh
	if(buffer[pipe+length+2]!=0){
	  result = malloc(10*sizeof(char));
	  if(msg_num == 5){
	    strcpy(result, "ERR|M5FT|\0");
	    free(buffer);
	    free(str);
	    return result;
	  }
	}
	if(str[length-1]!=33 && str[length-1]!=46 && str[length-1]!=63){
	  result = malloc(10*sizeof(char));
	  strcpy(result, "ERR|M5CT|\0");
	  //	  printf("%c\n", str[length-2]);
	  free(buffer);
	  free(str);
	  return result;
	}
	//printf("correct %s\n",str);
	result = malloc(8*sizeof(char));
        strcpy(result, "correct\0");
	printf("%s\n",str);
	free(str);
        free(buffer);
        return result;
      }
      else{
	//content error for msg 1 or msg3
	result = malloc(10*sizeof(char));
	if(msg_num == 1){
	  strcpy(result, "ERR|M1CT|\0");
	  free(str);
	  free(buffer);
	  return result;
	}
	else if(msg_num == 3){
	  strcpy(result, "ERR|M3CT|\0");
	  free(str);
	  free(buffer);
	  return result;
	}
      }
  }

  //}
  else{
    //missing msg type REG| or ERR| then send format error for msg_num
    result = malloc(10*sizeof(char));
    if(msg_num == 1){
      strcpy(result, "ERR|M1FT|\0");
      free(buffer);
      return result;
    }
    else if(msg_num == 3){
      strcpy(result, "ERR|M3FT|\0");
      free(buffer);
      return result;
    }
    else if(msg_num == 5){
      strcpy(result, "ERR|M5FT|\0");
      free(buffer);
      return result;
    }
  }
  */
  free(buf);
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
if(portno < 5000 || portno > 64000){ 
	printf("The port number does not fall within range \n"); 
	exit(1);
}


sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if(sockfd<0){ 
		printf("error with scoket\n");
		exit(1);
	}

bzero( (char *)&serverAddressInfo, sizeof(serverAddressInfo)); 

serverAddressInfo.sin_port = htons(portno); 

serverAddressInfo.sin_family = AF_INET; 


/*This below statement means I am willing to acccept connections 
sfrom anyone*/
serverAddressInfo.sin_addr.s_addr = INADDR_ANY; 


if(bind(sockfd,(struct sockaddr *)&serverAddressInfo, sizeof(serverAddressInfo))<0){
  printf("error");
}

 char* res;
/*infinite loop to for server to keep listening and accepting requests from clients */
while(1){
  printf("Waiting for new client connection to start KKJ\n");
  listen(sockfd,MAX_CUE); 

				clilen = sizeof(clientAddressInfo); 

				/* should pass blank sock addre struct becuase we will need a place to store client
				info 
				*/
				
  				newsockfd = accept(sockfd,(struct sockaddr *)&clientAddressInfo, &clilen);
			   	if(newsockfd <0){ 
						printf("error on accept \n");
						exit(1);
					}

					buffer = (char*) malloc(22*sizeof(char));
				strcpy(buffer, "REG|13|Knock, Knock.|\0");
				n = write(newsockfd,buffer,21); //sending msg 0
				printf("Knock, Knock.\n");
				free(buffer); 
				res = check(newsockfd, buffer, 20, 1); //receiving msg 1 who's there
				/* res == "close" means we received an error from client
				   res == "correct" means we received what we expected so 
				   we can send a regular response. 
				   res == "ERR|....|" means
				   we received an incorrectly formatted msg so we need to
				   send/report an error to our client which is stored in res, so 
				   simply compare first three chars of res with ERR
				*/
				if(strcmp(res, "close") == 0){
				  //close(newsockfd);  //received an error from client, close connection & move-on
				  free(res);
				  close(newsockfd);
				  continue;
				}
				else if(res[0] == 'E' && res[1] == 'R' && res[2] == 'R'){
				  write(newsockfd, res, 9); //need to send an error to client
				  printf("Incorrect message from client, sending error %s, closing connection\n", res);
				  free(res);
				  close(newsockfd);
                                  continue;
				}
				
				buffer = (char*) malloc(13*sizeof(char));
				strcpy(buffer, "REG|5|Tank.|\0");
				n = write(newsockfd, buffer, 12); //sending msg 2
				printf("Tank.\n");
				free(buffer);
					       
				res = check(newsockfd, buffer, 18, 3); //receiving msg 3 JA Francisco, who?
			        if(strcmp(res, "close") == 0){
                                  //close(newsockfd);  //received an error from client, close connection & move-on
				  free(res);
				  close(newsockfd);
				  continue;
                                }
                                else if(res[0] == 'E' && res[1] == 'R' && res[2] == 'R'){
                                  write(newsockfd, res, 9); //need to send an error to client
				  printf("Incorrect message from client, sending error %s, closing connection\n", res);
                                  free(res);
				  close(newsockfd);
                                  continue;
				}
				
				buffer = (char*) malloc(24*sizeof(char));
				strcpy(buffer,"REG|15|You're welcome!|\0");
				n = write(newsockfd,buffer,23); //sending msg 4
				printf("You're welcome!\n");
				free(buffer);
				res = check(newsockfd, buffer, 1024, 5); //receiving msg 5 expression of A/D/S
				if(strcmp(res, "close") == 0){
                                  //close(newsockfd);  //received an error from client, close connection & move-on
				  free(res);
				  close(newsockfd);
				  continue;
                                }
                                else if(res[0] == 'E' && res[1] == 'R' && res[2] == 'R'){
                                  write(newsockfd, res, 9); //need to send an error to client
				  printf("Incorrect message from client, sending error %s, closing connection\n", res);
                                  free(res);
				  close(newsockfd);
                                  continue;
                                }
				close(newsockfd);			       
 }			        
 close(sockfd);
}
 
