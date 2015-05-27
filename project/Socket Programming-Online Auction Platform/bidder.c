/*
** client.c -- a stream socket client demo
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h> 
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#define PORT "1793" // the port client will be connecting to
#define MAXDATASIZE 100 // max number of bytes we can get at once
#define MAXBUFLEN 200
#define BACKLOG 10
//void sigchld_handler(int s)
//{
//    while(waitpid(-1, NULL, WNOHANG) > 0);
//}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
if (sa->sa_family == AF_INET) {
return &(((struct sockaddr_in*)sa)->sin_addr);
}
return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
//int main(int argc, char *argv[])
int main(void)
{
/////////////////////////////////////begin fork()/////////////////////////////////////////////////////

if( !fork()){


	int sockfd, numbytes;
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];
	/*
	if (argc != 2) {
	fprintf(stderr,"usage: client hostname\n");
	exit(1);
	}
	*/
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	//if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
	if ((rv = getaddrinfo("nunki.usc.edu", PORT, &hints, &servinfo)) != 0) {
	fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
	return 1;
	}
	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
	if ((sockfd = socket(p->ai_family, p->ai_socktype,
	p->ai_protocol)) == -1) {
	perror("client: socket");
	continue;
	}
	if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
	close(sockfd);
	perror("client: connect");
	continue;

	}
	break;
	}

	//inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
	//s, sizeof s);
	////////////////////////////print local port number and ip address//////////////////////////////////////
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	int port;
	//char *p_ptr;
	struct hostent *hptr;
	char client_ip[40];

	hptr = gethostbyname("nunki.usc.edu");
	inet_ntop(hptr->h_addrtype,hptr->h_addr,s,sizeof s);

	getsockname(sockfd,(struct sockaddr*)&client_addr,&client_len );
	port = ntohs(client_addr.sin_port);
	printf("Phase1: <Bidder> 2 has TCP port %d and IP address: %s\n",port,s);

	////////////////////////////end print local port number and ip address/////////////////////////////////////


	if (p == NULL) {
	fprintf(stderr, "client: failed to connect\n");
	return 2;
	}
	///////////read bidder1////////////////////

	char bidder_file[50];
	bidder_file[50] = NULL; 
	FILE *fp;
	fp = fopen("bidderPass2.txt","r");
	fread(bidder_file,sizeof(char),50,fp);
	fclose(fp);
//	printf("bidder_file is %s\n",bidder_file);

	/////////////end read bidderPass////////////////



	//send data
	//char *msg = "Beej was here!";


	//char * bidder_file2[50];
	//bidder_file2 = bidder_file;
	char *msg;
	msg = bidder_file;
	int len, bytes_sent;
	len = strlen(msg);
	bytes_sent = send(sockfd, msg, len, 0);
	//send(sockfd, "What\n", strlen("what\n"),0);



	////////////////////////////begin print log in request////////////////////////////////////////////
	//	char *bidder_file1[50] = NULL;
	//   char bidder_file1[50];
	//  bidder_file1[50] = bidder_file[50];
	//   char * bidder_file1;
	//  bidder_file1 = bidder_file;
	char *b1;
	char *b2;
	char *b3;
	char *b4;
	//   char *user_type;
	//   char *user_name;
	//   char *password;
	//   char *bank_account;
	//  char delims2[] = " ";
	char *result1 = NULL;
	//   result2 = strtok( result1, delims2 );
	result1 = strtok(bidder_file, " \n");
	//	printf("msg = bidder_file is %s\n",bidder_file);
   int j =1;
   while(result1 != NULL){
	//		result2 = strtok( result1, delims2 );
		switch(j){
		case 1: b1 = result1; break;
		case 2: b2 = result1; break; 
		case 3: b3 = result1; break;
		case 4: b4 = result1; break;
		default: printf("result2 is: \n",result1); break;
		}
		result1 = strtok( NULL, " \n" );
		j++;
   }
   printf("Phase 1: Login request. User: %s Password: %s Bank account: %s \n", b2,b3,b4);
   
	////////////////////////////end print log in request///////////////////////////////////////////

	//////////////////////////////////////////////
	char recv_autherize[1];

	recv(sockfd, recv_autherize, 1, 0);
	if ( (recv_autherize[0] == '1') ){
		printf("Phase1 Login request reply: %s access successful\n\n",b2 );
	}
	else printf("Phase 1 Login request: &s access fail\n\n",b2 );
	//int reply;
	/////////////////////////////////////////////////

	//reply = recv(sockfd, buf, MAXDATASIZE-1, 0);


	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
	s, sizeof s);
	printf("client: connecting to %s\n", s);

	/////////////////////////////////////////////////////////////////
	freeaddrinfo(servinfo); // all done with this structure
	/*
	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
	perror("recv");
	exit(1);
	}
	*/
	buf[numbytes] = '\0';
	close(sockfd);
//	break;
	exit(0);
	}

//////////////////////////////////////end fork()////////////////////////////////////////////////////////////////
else{
int sockfd, numbytes;
char buf[MAXDATASIZE];
struct addrinfo hints, *servinfo, *p;
int rv;
char s[INET6_ADDRSTRLEN];
/*
if (argc != 2) {
fprintf(stderr,"usage: client hostname\n");
exit(1);
}
*/
memset(&hints, 0, sizeof hints);
hints.ai_family = AF_UNSPEC;
hints.ai_socktype = SOCK_STREAM;
//if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
if ((rv = getaddrinfo("nunki.usc.edu", PORT, &hints, &servinfo)) != 0) {
fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
return 1;
}
// loop through all the results and connect to the first we can
for(p = servinfo; p != NULL; p = p->ai_next) {
if ((sockfd = socket(p->ai_family, p->ai_socktype,
p->ai_protocol)) == -1) {
perror("client: socket");
continue;
}
if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
close(sockfd);
perror("client: connect");
continue;

}
break;
}

//inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
//s, sizeof s);
////////////////////////////print local port number and ip address//////////////////////////////////////
struct sockaddr_in client_addr;
socklen_t client_len = sizeof(client_addr);
int port;
//char *p_ptr;
struct hostent *hptr;
char client_ip[40];

hptr = gethostbyname("nunki.usc.edu");
inet_ntop(hptr->h_addrtype,hptr->h_addr,s,sizeof s);

getsockname(sockfd,(struct sockaddr*)&client_addr,&client_len );
port = ntohs(client_addr.sin_port);
printf("Phase1: <Bidder> 1 has TCP port %d and IP address: %s\n",port,s);

////////////////////////////end print local port number and ip address/////////////////////////////////////


if (p == NULL) {
fprintf(stderr, "client: failed to connect\n");
return 2;
}
///////////read bidder1////////////////////

char bidder_file[50];
bidder_file[50] = NULL; 
FILE *fp;
fp = fopen("bidderPass1.txt","r");
fread(bidder_file,sizeof(char),50,fp);
fclose(fp);
printf("bidder_file is %s\n",bidder_file);

/////////////end read bidderPass////////////////



//send data
//char *msg = "Beej was here!";


//char * bidder_file2[50];
//bidder_file2 = bidder_file;
char *msg;
msg = bidder_file;
int len, bytes_sent;
len = strlen(msg);
bytes_sent = send(sockfd, msg, len, 0);
//send(sockfd, "What\n", strlen("what\n"),0);



////////////////////////////begin print log in request////////////////////////////////////////////
//	char *bidder_file1[50] = NULL;
//   char bidder_file1[50];
//  bidder_file1[50] = bidder_file[50];
//   char * bidder_file1;
//  bidder_file1 = bidder_file;
   char *b1;
   char *b2;
   char *b3;
   char *b4;
//   char *user_type;
//   char *user_name;
//   char *password;
//   char *bank_account;
//  char delims2[] = " ";
   char *result1 = NULL;
//   result2 = strtok( result1, delims2 );
	result1 = strtok(bidder_file, " \n");
//	printf("msg = bidder_file is %s\n",bidder_file);
   int j =1;
   while(result1 != NULL){
//		result2 = strtok( result1, delims2 );
		switch(j){
		case 1: b1 = result1; break;
		case 2: b2 = result1; break; 
		case 3: b3 = result1; break;
		case 4: b4 = result1; break;
		default: printf("result2 is: \n",result1); break;
		}
		result1 = strtok( NULL, " \n" );
		j++;
   }
   printf("Phase 1: Login request. User: %s Password: %s Bank account: %s \n", b2,b3,b4);
   
////////////////////////////end print log in request///////////////////////////////////////////

//////////////////////////////////////////////
char recv_autherize[1];

recv(sockfd, recv_autherize, 1, 0);
if ( (recv_autherize[0] == '1') ){
	printf("Phase1 Login request reply: %s access successful\n\n", b2 );
}
else printf("Phase 1 Login request: access fail\n\n");
//int reply;
/////////////////////////////////////////////////

//reply = recv(sockfd, buf, MAXDATASIZE-1, 0);


inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
s, sizeof s);
printf("client: connecting to %s\n", s);

/////////////////////////////////////////////////////////////////
freeaddrinfo(servinfo); // all done with this structure

buf[numbytes] = '\0';
close(sockfd);

//sleep(1);
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////begin phase 3 bidder////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
//	int sockfd;
//    struct addrinfo hints, *servinfo, *p;
//    int rv;
//    int numbytes;
////////////////////////begin fork() bidder2 //////////////////////////////////
	if(!fork()){
	struct sockaddr_storage their_addr;
    char buf3[MAXBUFLEN] ={'0'};
    socklen_t addr_len;
//    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo("nunki.usc.edu", "3893", &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("listener: socket");
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("listener: bind");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "bidder2 listener: failed to bind socket\n");
        return 2;
    }

    freeaddrinfo(servinfo);




    printf("bidder2 listener: waiting to recvfrom...\n");
///////////////////////////////receive item lists///////////////////////////
    addr_len = sizeof their_addr;

    if ((numbytes = recvfrom(sockfd, buf3, MAXBUFLEN-1 , 0,
        (struct sockaddr *)&their_addr, &addr_len)) == -1) {
        perror("recvfrom");
        exit(1);
    }
	sleep(2);

///////////////////////////////begin print Bidder2 Port and IP address////////////////////////////////////
	char s3[INET6_ADDRSTRLEN];
	hptr = gethostbyname("nunki.usc.edu");
	inet_ntop(hptr->h_addrtype,hptr->h_addr,s3,sizeof s3);
    printf("Phase 3: bidder 2 has UDP port 3893 and IP address: %s \n", s3 );
////////////////////////////////end print Bidder2 Port and IP address/////////////////////////////////////

    printf("Phase 3: receiving the item list \n%s", buf3);
	
////////////////////////////////////read bidding1 Item lists //////////////////////////////////////////
	char bidding1_file[100] ={'0'};
//	memcpy(itemList1_file,0,50);
//	itemList1_file[50] ={'0'}; 
//	FILE *fp;
	FILE *fp1;
	fp1 = fopen("bidding2.txt","r");
	fread(bidding1_file,sizeof(char),100,fp1);
	fclose(fp1);
    printf("Phase 3: <Bidder 2> sending bidding information:\n%s ",bidding1_file);
/////////////////////////////////////end bidding1 Item lists///////////////////////////////////////	

/////////////////////////////////////begin send reply (bidding item) //////////////////////////////////////////////////////

//	printf("hello world1\n");
	sendto(sockfd, bidding1_file, sizeof(bidding1_file)+1, 0,
             (struct sockaddr *)&their_addr, addr_len);
//	printf("hello world2\n");

//////////////////////////////////////end sending bidding item//////////////////////////////////////////////////////

    close(sockfd);
	exit(0);
	}
	
/////////////////////////////fork () end bidder2 //////////////////////////////////
	
	
	
//////////////////////////////////main function bidder1//////////////////////////////////////////////
 else{   struct sockaddr_storage their_addr;
    char buf3[MAXBUFLEN] ={'0'};
    socklen_t addr_len;
//    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo("nunki.usc.edu", "3793", &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("listener: socket");
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("listener: bind");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "bidder1 listener: failed to bind socket\n");
        return 2;
    }

    freeaddrinfo(servinfo);




    printf("bidder1 listener: waiting to recvfrom...\n");
///////////////////////////////receive item lists///////////////////////////
    addr_len = sizeof their_addr;

    if ((numbytes = recvfrom(sockfd, buf3, MAXBUFLEN-1 , 0,
        (struct sockaddr *)&their_addr, &addr_len)) == -1) {
        perror("recvfrom");
        exit(1);
    }
	sleep(1);
/*
	printf("hello world1\n");
	sendto(sockfd, "I love god\n", sizeof("hello world")+1, 0,
             (struct sockaddr *)&their_addr, addr_len);
	printf("hello world2\n");
*/	
/*   printf("listener: got packet from %s\n",
        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s));
*/
//    printf("listener: packet is %d bytes long\n", numbytes);
//    buf3[numbytes] = '\0';
///////////////////////////////begin print Bidder1 Port and IP address////////////////////////////////////
	char s3[INET6_ADDRSTRLEN];
	hptr = gethostbyname("nunki.usc.edu");
	inet_ntop(hptr->h_addrtype,hptr->h_addr,s3,sizeof s3);
    printf("Phase 3: bidder 1 has UDP port 3793 and IP address: %s \n", s3 );
////////////////////////////////end print Bidder1 Port and IP address/////////////////////////////////////

    printf("Phase 3: receiving the item list \n%s", buf3);
	
////////////////////////////////////read bidding1 Item lists //////////////////////////////////////////
	char bidding1_file[100] ={'0'};
//	memcpy(itemList1_file,0,50);
//	itemList1_file[50] ={'0'}; 
//	FILE *fp;
	FILE *fp1;
	fp1 = fopen("bidding1.txt","r");
	fread(bidding1_file,sizeof(char),100,fp1);
	fclose(fp1);
    printf("Phase 3: <Bidder 1> sending bidding information:\n%s ",bidding1_file);
/////////////////////////////////////end bidding1 Item lists///////////////////////////////////////	

/////////////////////////////////////begin send reply (bidding item) //////////////////////////////////////////////////////

//	printf("hello world1\n");
	sendto(sockfd, bidding1_file, sizeof(bidding1_file)+1, 0,
             (struct sockaddr *)&their_addr, addr_len);
//	printf("hello world2\n");

//////////////////////////////////////end sending bidding item//////////////////////////////////////////////////////

    close(sockfd);

//	printf("bidder1 socket is closed");
}

///////////////////////////////////end phase3 bidder//////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////phase3 fork for bidder2 receiving bidding result/////////////////////////////

if(!fork()){
     int new_fd;  // listen on sock_fd, new connection on new_fd
//    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
//    char s[INET6_ADDRSTRLEN];
//    int rv;

	
	memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo("nunki.usc.edu", "4893", &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    if (p == NULL)  {
        fprintf(stderr, "server: phase3 bidder2 receiving result failed to bind\n");
        return 2;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

 //   sa.sa_handler = sigchld_handler; // reap all dead processes
 //   sigemptyset(&sa.sa_mask);
 //   sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");
	char buf_b1[50] = {'0'};
	char buf_b2[50] = {'0'};
	char buf_b3[50] = {'0'};
	char buf_b4[50] = {'0'};
	char buf_b5[50] = {'0'};
	char buf_b6[50] = {'0'};
	char buf_b7[50] = {'0'};
	char buf_b8[50] = {'0'};
	char buf_b9[50] = {'0'};
	char buf_b10[50] = {'0'};
//    while(1) {  // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
 //           continue;
        }

        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s);
        printf("server: got connection from %s\n", s);
	    
		
		recv(new_fd,buf_b1,50,0); recv(new_fd,buf_b2,50,0);
		recv(new_fd,buf_b3,50,0); recv(new_fd,buf_b4,50,0);
		recv(new_fd,buf_b5,50,0); recv(new_fd,buf_b6,50,0);
//		recv(new_fd,buf_b7,50,0); recv(new_fd,buf_b8,50,0);
//		recv(new_fd,buf_b9,50,0); recv(new_fd,buf_b10,50,0);\
		
		printf("<Bidder 2>\nItem %s was sold at price %s\n",buf_b1,buf_b2);
		printf("Item %s was sold at price %s\n",buf_b3,buf_b4);
		printf("Item %s was sold at price %s\n",buf_b5,buf_b6);
//		printf("Item %s was sold at price %s\n",buf_b7,buf_b8);
//		printf("Item %s was sold at price %s\n",buf_b9,buf_b10);
//		printf("\n%s\n%s\n",buf_b1,buf_b2);
/*        if (!fork()) { // this is the child process
            close(sockfd); // child doesn't need the listener
			
//            if (send(new_fd, "Hello, world!", 13, 0) == -1)
//                perror("send");

            close(new_fd);
            exit(0);
        }
*/		printf("\n\nEnd of Phase 3 for bidder 2\n");
        close(new_fd);
		exit(0);
}

//////////////////////////////phase 3 end for bidder2 receiving bidding result//////////////////////////////////
     int new_fd;  // listen on sock_fd, new connection on new_fd
//    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
//    char s[INET6_ADDRSTRLEN];
//    int rv;

	
	memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo("nunki.usc.edu", "4793", &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    if (p == NULL)  {
        fprintf(stderr, "server: phase 3 bidder 1 receiving bidding result failed to bind\n");
        return 2;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

 //   sa.sa_handler = sigchld_handler; // reap all dead processes
 //   sigemptyset(&sa.sa_mask);
 //   sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");
	char buf_b1[50] = {'0'};
	char buf_b2[50] = {'0'};
	char buf_b3[50] = {'0'};
	char buf_b4[50] = {'0'};
	char buf_b5[50] = {'0'};
	char buf_b6[50] = {'0'};
	char buf_b7[50] = {'0'};
	char buf_b8[50] = {'0'};
	char buf_b9[50] = {'0'};
	char buf_b10[50] = {'0'};
//    while(1) {  // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
 //           continue;
        }

        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s);
        printf("server: got connection from %s\n", s);
	    
		
		recv(new_fd,buf_b1,50,0); recv(new_fd,buf_b2,50,0);
		recv(new_fd,buf_b3,50,0); recv(new_fd,buf_b4,50,0);
//		recv(new_fd,buf_b5,50,0); recv(new_fd,buf_b6,50,0);
//		recv(new_fd,buf_b7,50,0); recv(new_fd,buf_b8,50,0);
//		recv(new_fd,buf_b9,50,0); recv(new_fd,buf_b10,50,0);\
		
		printf("<Bidder 1>\nItem %s was sold at price %s\n",buf_b1,buf_b2);
		printf("Item %s was sold at price %s\n",buf_b3,buf_b4);
//		printf("Item %s was sold at price %s\n",buf_b5,buf_b6);
//		printf("Item %s was sold at price %s\n",buf_b7,buf_b8);
//		printf("Item %s was sold at price %s\n",buf_b9,buf_b10);
//		printf("\n%s\n%s\n",buf_b1,buf_b2);
/*        if (!fork()) { // this is the child process
            close(sockfd); // child doesn't need the listener
			
//            if (send(new_fd, "Hello, world!", 13, 0) == -1)
//                perror("send");

            close(new_fd);
            exit(0);
        }
*/		printf("\n\nEnd of Phase 3 for bidder1\n");
        close(new_fd);  // parent doesn't need this
//    }




}

return 0;
}
