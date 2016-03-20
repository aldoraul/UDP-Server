/* CS 423 Project 2
 UDP_SERVER
 Aldo Anaya
*/

#include<stdio.h>
#include<cstdlib>
#include<cstdio>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<sys/wait.h>
#include<signal.h>
#include<iostream>

// get sockaddr, IPv4 or IPv6

void *get_in_addr(struct sockaddr *sa){
	if(sa->sa_family == AF_INET){
	return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void doConcurrent(int sockfd);

#define MYPORT "23456"
#define MAXBUFFLEN 100
#define ADDR_LEN 50

int main(void){
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int numbytes;
	int rv;
	struct sockaddr_storage their_addr;
	char buf[MAXBUFFLEN];
	char strptr[ADDR_LEN];
	socklen_t addr_len;
	int yes = 1;
	
	memset(&hints, 0, sizeof hints);
	printf("memset cleared\n");

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;

	if((rv = getaddrinfo(NULL, MYPORT, &hints, &servinfo)) != 0){
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
	
	for(p = servinfo;p != NULL; p=p->ai_next){
		if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("UDP_Server: socket");
			doConcurrent(sockfd);
			continue;
		}
		if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
			perror("setsockopt");
			exit(1);
		}
		if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1){
			close(sockfd);
			perror("UDP_Server: bind");
			continue;
		}
		break;
	}
	printf("now listening\n");

	if(p == NULL){
		fprintf(stderr, "UDP_Server: failed to bind socket\n");
		return 2;
	}

	freeaddrinfo(servinfo);
	
	while(1){
		printf("\n");
		addr_len = sizeof their_addr;
		if((numbytes = recvfrom(sockfd, buf, MAXBUFFLEN-1, 0, (struct sockaddr *)&their_addr, &addr_len))== -1){
			perror("\trecvfrom");
			exit(1);
		}
		printf("\tUDP_Server: got packet from %s\n", inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr),strptr, sizeof strptr));
		printf("\tUDP_Server: packet is %d bytes long\n", numbytes);
		buf[numbytes]='\0';
		printf("\tUDP_Server:packet contains \"%s\"\n", buf);
		if((numbytes = sendto(sockfd, "Server", 6, 0, (struct sockaddr *)&their_addr, addr_len)) == -1) {
			perror("\tUDP_Server: sendto error");
			exit(1);
		}
	}
	close(sockfd);
	return 0;
}
		
