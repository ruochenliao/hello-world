/*
** server.c -- a stream socket server demo
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#define PORT "1793" // the port users will be connecting to
#define BACKLOG 10 // how many pending connections queue will hold
#define MAXDATASIZE 100
/*
struct users{
	char * bidder1;
	char * bidder2;
	char * seller1;
	char * seller2;
};
*/
int exist_function(char *c1,char *c2, char *c3, char *c4, char *a1,char *a2,char *a3 );
void print_ip(char* a);
char *cmp_function(char *b1,char *b2, char *b3,char* a1,char*a2,char*a3,char*a4,char*a5,char*a6,char*a7,char*a8,char*a9);
char * add0(char *p1,char*p2);
void sigchld_handler(int s)
{
while(waitpid(-1, NULL, WNOHANG) > 0);
}
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{

//if sa_family matches IPv4, then return the sin_addr which is ip address 
if (sa->sa_family == AF_INET) {  
//Sa_family:
//struct sockaddr{unsigned short sa_family;}
//AF_INET is IPv4 
return &(((struct sockaddr_in*)sa)->sin_addr); 
//struct sockaddr_in {
//struct inaddr sin_addr(Internet address)
//}
//sockaddr has the same struct as sockaddr_in.
//sockaddr_in {sin_family//; sin_port;sin_addr;sin_zero[8]}
}
return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
int main(void)
{
int account_seller = 0;
int account_end =0;
int sockfd, new_fd; // listen on sock_fd, new connection on new_fd
struct addrinfo hints, *servinfo, *p;//
struct sockaddr_storage their_addr; // connector's address information
struct sockaddr_storage their_addr1;
socklen_t sin_size;
//addrlen is a local integer variable
//that should be set to sizeof(struct sockaddr_storage) before its address is passed to accept().
struct sigaction sa;
//(Also, this whole sigaction() thing might be new to you—that's ok. The code that's there is
//responsible for reaping zombie processes that appear as the fork()ed child processes exit.
int yes=1;
char s[INET6_ADDRSTRLEN];
int rv;
///////////////////////read auctionfile and divide data/////////////////////////////////////////

//char acc[30];
//char bcc[30];
//char ccc[30];
char * a1;
char * a2;
char * a3;
char * a4;
char * a5;
char * a6;
//char dcc[30];
//char ecc[30];
//char fcc[30];
//////////////////read file///////////////////
char n[160]; 
FILE *fp;
fp = fopen("Registration.txt","r");
fread(n,sizeof(char),160,fp);
fclose(fp);
///////////////////end read file ///////////////////////
//	char str[] = "now # is the time for all # good men to come to the # aid of their country";
   char delims[] = "\n";
   char *result = NULL;
   int i =1;
   result = strtok( n, delims );
//   for( i; result = NULL ; i++) {
while(result != NULL){	
	switch(i){
		case 1:a1 = result; break;
		case 2:a2 = result; break;
		case 3:a3 = result; break;
		case 4:a4 = result; break;
		case 5:a5 = result; break;
		case 6:a6 = result; break;
		default: break;
 	} 
	i++;
//	printf("%s\n", acc);
    result = strtok( NULL, delims );
   }
//printf("%s\n%s\n%s\n%s\n%s\n%s\n", a1,a2,a3,a4,a5,a6);
printf("this is server\n");   

/////////////////////////////end read auctionfile and divide data/////////////////////////////////////
/*
struct users pass;
struct users * p_pass;
p_pass = &pass;
p_pass-> bidder1 = "1 Mary 123456 451912345";
p_pass-> bidder2 =  "1 James pass123 451965432";
p_pass-> seller1 = "2 Taylor abc654 451934578";
p_pass-> seller2 = "2 Julia 654321 451900001";
//printf("%s");
*/
memset(&hints, 0, sizeof hints);
hints.ai_family = AF_UNSPEC;
hints.ai_socktype = SOCK_STREAM;
hints.ai_flags = AI_PASSIVE; // use my IP
if ((rv = getaddrinfo("nunki.usc.edu", PORT, &hints, &servinfo)) != 0) {
//getaddrinfo is a function. to do DNS lookup, fill out the structs you need. 
//then you'd load that information by hand into a struct sockaddr_in
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
//printf("\n\n sockfd is %d \n\n",sockfd);
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
if (p == NULL) {
fprintf(stderr, "server: failed to bind\n");
return 2;

}

freeaddrinfo(servinfo); // all done with this structure
if (listen(sockfd, BACKLOG) == -1) {
perror("listen");
exit(1);
}
/*sa.sa_handler = sigchld_handler; // reap all dead processes
sigemptyset(&sa.sa_mask);
sa.sa_flags = SA_RESTART;
if (sigaction(SIGCHLD, &sa, NULL) == -1) {
perror("sigaction");
exit(1);
}*/
print_ip(PORT);

printf("server: waiting for connections...\n");
while(1) { // main accept() loop
sin_size = sizeof their_addr;
new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
//printf("new_fd is %d\n",new_fd);
if (new_fd == -1) {
perror("accept");
continue;
}
inet_ntop(their_addr.ss_family,
get_in_addr((struct sockaddr *)&their_addr),
s, sizeof s);
//printf("server: got connection from %s\n", s);
	if (!fork()) { // this is the child process

		int reply;
		int flag_1;
		char buf[MAXDATASIZE];
//		char * no = "0";
	 
		close(sockfd); // child doesn't need the listener
//	if (send(new_fd, "Hello, world!", 13, 0) == -1)
//		perror("send");
		reply = recv(new_fd, buf, MAXDATASIZE-1, 0);
//		printf("buf is %s\n",buf);
///////////////////////extract pass_word from buf//////////////////////////////////

   char delims1[] = "\n";
   char *result1 = NULL;
   result1 = strtok( buf, delims1 );
//   printf("result1 is:%s\n",result1);

/////////////////////end extrac pass_word from buf/////////////////////////////



/////////////////////////////reply ////////////////////////////////
		int authorize; 
		char *yes_no;
		if ( (authorize = (strcmp((result1 + 2),a1) )) ==0 ){
				printf("a1 access successful\n");
				flag_1 = send(new_fd, "1", 1, 0);
//				printf("%d\n  !!!",flag_1);
		}
		else if( (authorize = (strcmp((result1 + 2),a2) ))==0 ){
				printf("a2 access successful\n");
				flag_1 = send(new_fd, "1", 1, 0);
		}
				else if( (authorize = (strcmp((result1 + 2),a3) ))==0 ){
				printf("a3 access successful\n");
				flag_1 = send(new_fd, "1", 1, 0);
		}
				else if( (authorize = (strcmp((result1 + 2),a4) ))==0 ){
				printf("a4 access successful\n");
				flag_1 = send(new_fd, "1", 1, 0);
		}
				else if( (authorize = (strcmp((result1 + 2),a5) ))==0 ){
				printf("a5 access successful\n");
				flag_1 = send(new_fd, "1", 1, 0);
		}
				else if( (authorize = (strcmp((result1 + 2),a6) ))==0 ){
				printf("a6 access successful\n");
				flag_1 = send(new_fd, "1", 1, 0);
		}		
		else printf("don't have such a user\n\n");
		
		
		if(authorize == 0){
			yes_no = "yes";
		} 
		else yes_no ="no";
//////////////////////////////end reply/////////////////////////////////


////////////////////////print receiving authentication request//////////////////////////////////////
   char *b1;
   char *b2;
   char *b3;
   char *b4;
//   char *user_type;
//   char *user_name;
//   char *password;
//   char *bank_account;
//  char delims2[] = " ";
   char *result2 = NULL;
//   result2 = strtok( result1, delims2 );
	result2 = strtok(buf, " \n");
   i=1;
   while(result2 != NULL){
//		result2 = strtok( result1, delims2 );
		switch(i){
		case 1: b1 = result2; break;
		case 2: b2 = result2; break; 
		case 3: b3 = result2; break;
		case 4: b4 = result2; break;
		default: printf("result2 is: \n",result2); break;
		}
		result2 = strtok( NULL, " \n" );
		i++;
   }
  
/*	if(b1 = 1){
		b1 = "bidder";
	}
	else if (b1 =2 ) b1 = "seller";  */
   
/*	char   *ptr, **pptr;
    struct hostent *hptr;
//    char   str[32];
    pptr=hptr->h_addr_list;
	char* str = "nunki";
   	hptr = gethostbyname("str");
*/
    printf("phase1: Auction Server request. User %s : Usernames %s ,Password: %s, Bank Accout: %s User IP Addr:%s. Authorized:%s\n",b1,b2,b3,b4,s,yes_no);
	print_ip(b2);
///////////////////////////////print server IP addr_and_port number/////////////////////////////	
	char * server_ip_port = "1";
	print_ip( server_ip_port);
//	printf("Phase1 Auction Server IP Address: %s , PreAuction Port Number is %s\n",PORT);
	
///////////////////////////////end print server IP addr and port number /////////////////////////////////
//			inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str)));
  
//////////////////////// end print print receiving authentication request ///////////////////////////////	
	
	
		close(new_fd);
		exit(0);
	}
close(new_fd); // parent doesn't need this

account_end ++;
//printf("account_ end is : %d\n\n",account_end);
	if(account_end >= 4){
	    sleep(2);
		printf("End of Phase 1 for Auction Server\n\n\n");
		close(new_fd);
		//printf("");
		break;
	}

}
/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////begin main function phase 2////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
//	close (sockfd);
	sockfd = 0;
	printf("\n\n sockfd is %d \n\n",sockfd);
	memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo("nunki.usc.edu", "1893", &hints, &servinfo)) != 0) {
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
        fprintf(stderr, "server: failed to bind\n");
        return 2;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
	print_ip("1893");
    printf("server: waiting for connections...\n");
	printf("sockfd is %d\n ",sockfd );
//	printf("hello world  1\n");
    while(1) {  // main accept() loop
        sin_size = sizeof their_addr1;
		int new_fd1;
//		printf("hello world  2\n");
        new_fd1 = accept(sockfd, (struct sockaddr *)&their_addr1, &sin_size);
//        printf("hello world 3 \n");
		if (new_fd1 == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr1.ss_family,
            get_in_addr((struct sockaddr *)&their_addr1),
            s, sizeof s);
        printf("server: got connection from %s\n", s);
		
        if (!fork()) { // this is the child process
            close(sockfd); // child doesn't need the listener
    //        if (send(new_fd, "Hello, world!", 13, 0) == -1)
    //            perror("send");
		char buf[MAXDATASIZE] ={'0'};
		
		recv(new_fd1, buf, MAXDATASIZE-1, 0);
		int seller_num;
		if((buf[0]=='T')&(buf[1]=='a')){
		      seller_num = 1;
		}
		else if((buf[0] =='J')&(buf[1])=='u') seller_num =2;
		
		
		printf("phase 2: <Seller %d> send item lists.\n Phase2: %s \n",seller_num,buf);
//		printf("End of Phase 2 for Auction Server\n");
		
        close(new_fd1);
            exit(0);
        }
        close(new_fd1);  // parent doesn't need this
		
//		int account_seller = 0;
		account_seller++;
		if(account_seller >=2 ){ 
//			printf("end of phase 2 \n\n");		
			break;
//			printf("end of phase 2 \n\n");
		}
    }
	sleep(3);
	printf("end of phase 2 \n\n");

//////////////////////////////end main function phase 2////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////begin phase 3//////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////begin phase 3 UDP bidder1 ///////////////////////////////////////////////
//	   int sockfd;
//    struct addrinfo hints, *servinfo, *p;
//    int rv;
    int numbytes;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;


    if ((rv = getaddrinfo("nunki.usc.edu", "3793", &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and make a socket
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("talker: socket");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "talker: failed to bind socket\n");
        return 2;
    }



////////////////////////////////////read broadcast Item lists //////////////////////////////////////////
	char broadcastList_file[200] ={'0'};
//	memcpy(itemList1_file,0,50);
//	itemList1_file[50] ={'0'}; 
//	FILE *fp;
	FILE *fp3;
	fp3 = fopen("broadcastList.txt","r");
	fread(broadcastList_file,sizeof(char),200,fp3);
	fclose(fp3);
    printf("Phase3 broadcast Item list:\n %s \n",broadcastList_file);
/////////////////////////////////////end broadcast Item lists///////////////////////////////////////	


///////////////////////////////////begin send to bidder1/////////////////////////////////////////////////

	  if ((numbytes = sendto(sockfd, broadcastList_file, strlen(broadcastList_file), 0,
             p->ai_addr, p->ai_addrlen)) == -1) {
        perror("talker: sendto");
        exit(1);
    }
	
/////////////////////////////////////end send to bidder1///////////////////////////////////////////////

	
////////////////////////////////////////begin receive reply from bidder1////////////////////////////////////////////
		struct sockaddr_storage their_addr3;
		char buf_b1[100] ={'0'};
		socklen_t addr_len;
		addr_len = sizeof their_addr3;

//		printf("hello world1\n");
		recvfrom(sockfd, buf_b1, 100 , 0,
        p->ai_addr, &p->ai_addrlen);
		printf("Phase3 : Auction server received a bidding from <Bidder 1>\n%s ", buf_b1);
		printf("hello world1\n");
		//////////////////////////////////////////end receive from bidder1//////////////////////////////////////////
		
/////////////////////////////////////////////begin divide bidding 1///////////////////////////////////////////////////
	
//   char delims3[] = " \n";
	char *a_1;
	char *a_2;
	char *a_3;
	char *a_4;
	char *a_5;
	char *a_6;
	char *a_7;
	char *a_8;
	char *a_9;
   char *result_b1 = NULL;
   int i_b1 =1;
   result_b1 = strtok( buf_b1, " \n" );
//   for( i; result = NULL ; i++) {
while(result_b1 != NULL){	
	switch(i_b1){
		case 1:a_1 = result_b1; break;
		case 2:a_2 = result_b1; break;
		case 3:a_3 = result_b1; break;
		case 4:a_4 = result_b1; break;
		case 5:a_5 = result_b1; break;
		case 6:a_6 = result_b1; break;
		case 7:a_7 = result_b1; break;
		case 8:a_8 = result_b1; break;
		case 9:a_9 = result_b1; break;
		default: break;
 	} 
	i_b1++;
//	printf("%s\n", acc);
    result_b1 = strtok( NULL, " \n" );
   }
   printf("\na_1,... a_9  is %s %s\n\n\n", a_1,a_9);
////////////////////////////////////////////end divide biding 2/////////////////////////////////////////////////		
		

		
//////////////////////////////////begin phase 3 port number and IP address////////////////////////////////////////
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	int port_phase3;
	//char *p_ptr;
	struct hostent *hptr;
	char client_ip[40];
	char s3[INET6_ADDRSTRLEN];
	hptr = gethostbyname("nunki.usc.edu");
	inet_ntop(hptr->h_addrtype,hptr->h_addr,s3,sizeof s3);

	getsockname(sockfd,(struct sockaddr*)&client_addr,&client_len );
	port_phase3 = ntohs(client_addr.sin_port);
	printf("Phase3: Auction Server IP Address: %s Auction UDP Port number: %d \n",s3,port_phase3);	
//	print_ip("phase3_ip_port");
	
	
///////////////////////////////////end port number and IP address//////////////////////////////////////////////		
	

    freeaddrinfo(servinfo);
 //   printf("talker: sent %d bytes to %s\n", numbytes, argv[1]);
 
    close(sockfd);
//////////////////////////////////////end phase 3 UDP bidder1////////////////////////////////////////////////////








////////////////////////////////////////////begin phase3 UDP Bidder2///////////////////////////////////////////////////////////////////////	
//    int numbytes;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;


    if ((rv = getaddrinfo("nunki.usc.edu", "3893", &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and make a socket
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("talker: socket");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "talker: failed to bind socket\n");
        return 2;
    }



////////////////////////////////////  read broadcast Item lists to bidder2 //////////////////////////////////////////
//	broadcastList_file[200] ={'0'};
//	memcpy(itemList1_file,0,50);
//	itemList1_file[50] ={'0'}; 
//	FILE *fp;
	FILE *fpb2;
	fpb2 = fopen("broadcastList.txt","r");
	fread(broadcastList_file,sizeof(char),200,fpb2);
	fclose(fpb2);
//    printf("Phase3 broadcast Item list:\n %s \n",broadcastList_file);
/////////////////////////////////////end broadcast Item listsc to bidder2///////////////////////////////////////	


///////////////////////////////////begin send to bidder2/////////////////////////////////////////////////

	  if ((numbytes = sendto(sockfd, broadcastList_file, strlen(broadcastList_file), 0,
             p->ai_addr, p->ai_addrlen)) == -1) {
        perror("talker: sendto");
        exit(1);
    }
	
/////////////////////////////////////end send to bidder2///////////////////////////////////////////////

	
////////////////////////////////////////begin receive reply from bidder2////////////////////////////////////////////
		struct sockaddr_storage their_addrb2;
		char buf_b2[100] ={'0'};
//		socklen_t addr_len;
		addr_len = sizeof their_addrb2;

//		printf("hello world1\n");
		recvfrom(sockfd, buf_b2, 100 , 0,
        p->ai_addr, &p->ai_addrlen);
		printf("Phase3 : Auction server received a bidding from <Bidder 2>\n%s ", buf_b2);
//		printf("hello world1\n");
		
//////////////////////////////////////////end receive from bidder2//////////////////////////////////////////
		
/////////////////////////////////////////////begin divide bidding 1///////////////////////////////////////////////////
	
//   char delims3[] = " \n";
	char *b_1;
	char *b_2;
	char *b_3;
	char *b_4;
	char *b_5;
	char *b_6;
	char *b_7;
	char *b_8;
	char *b_9;
	char *b_10;
	char *b_11;
	char *b_12;
   char *result_b2 = NULL;
   int i_b2 =1;
   result_b2 = strtok( buf_b2, " \n" );
//   for( i; result = NULL ; i++) {
while(result_b2 != NULL){	
	switch(i_b2){
		case 1:b_1 = result_b2; break;
		case 2:b_2 = result_b2; break;
		case 3:b_3 = result_b2; break;
		case 4:b_4 = result_b2; break;
		case 5:b_5 = result_b2; break;
		case 6:b_6 = result_b2; break;
		case 7:b_7 = result_b2; break;
		case 8:b_8 = result_b2; break;
		case 9:b_9 = result_b2; break;
		case 10:b_10 = result_b2; break;
		case 11:b_11 = result_b2; break;
		case 12:b_12 = result_b2; break;
		default: break;
 	} 
	i_b2++;
//	printf("%s\n", acc);
    result_b2 = strtok( NULL, " \n" );
   }
   printf("\nb_1,... b_12  is %s %s\n\n\n", b_1,b_12);
////////////////////////////////////////////end divide biding 2/////////////////////////////////////////////////				
		
    freeaddrinfo(servinfo);
 //   printf("talker: sent %d bytes to %s\n", numbytes, argv[1]);
 
    close(sockfd);
////////////////////////////////////////////end phase3 UDP Bidder2///////////////////////////////////////////////////////////////////////	
char zero[20] = "0";
//char *src = "80";
//char *result;
a_6 = add0(zero,a_6); a_3 = add0(zero,a_3);a_9 = add0(zero,a_9);b_3 = add0(zero,b_3);b_6 = add0(zero,b_6);
b_9 = add0(zero,b_9);b_12 = add0(zero,b_12);
////////////////////////////////////////////////////


////////////////////////////////////////////	
char *cmp1;
char *cmp2;
char *cmp3;
char *cmp4;
char *cmp5;
//printf("a6 is :%s\n",b_6);
//printf("b1,b2,b3 is :%s %s %s\n",b_1,b_2,b_3);
cmp1 = cmp_function(b_1,b_2,b_3,a_1,a_2,a_3,a_4,a_5,a_6,a_7,a_8,a_9);
cmp2 = cmp_function(b_4,b_5,b_6,a_1,a_2,a_3,a_4,a_5,a_6,a_7,a_8,a_9);
cmp3 = cmp_function(b_7,b_8,b_9,a_1,a_2,a_3,a_4,a_5,a_6,a_7,a_8,a_9);
cmp4 = cmp_function(b_10,b_11,b_12,a_1,a_2,a_3,a_4,a_5,a_6,a_7,a_8,a_9);


//strlen exist_function(cmp1,cmp2,cmp3,cmp4,);
//printf("b_1,b_2,b_3 are \n%s%s%s\n",a_7,a_8,a_9);
if (exist_function(cmp1,cmp2,cmp3,cmp4,a_1,a_2,a_3) ==1 ) cmp5 = a_1;
if ( exist_function(cmp1,cmp2,cmp3,cmp4,a_4,a_5,a_6) ==1) cmp5 = a_4;
if ( exist_function(cmp1,cmp2,cmp3,cmp4,a_7,a_8,a_9) ==1 ) cmp5 =a_7;
//printf("\n\ncmp1,2,3,4 are: %s\n%s\n%s\n%s\n%s\n",cmp1,cmp2,cmp3,cmp4,cmp5); 
char* d1;char *d2;char* d3;char *d4;char* d5;char *d6;char* d7;char *d8;char* d9;char *d10;

d1 = strtok(cmp1,",");d2 = strtok(NULL,",");
d3 = strtok(cmp2,",");d4 = strtok(NULL,",");
d5 = strtok(cmp3,",");d6 = strtok(NULL,",");
d7 = strtok(cmp4,",");d8 = strtok(NULL,",");
d9 = strtok(cmp5,",");d10= strtok(NULL,",");

//printf("\nd1,d2.. are %s%s%s%s%s%s%s%s%s%s\n",d1,d2,d3,d4,d5,d6,d7,d8,d9,d10);
printf("Item %s was sold at price %s\n",d1,d2);
printf("Item %s was sold at price %s\n",d3,d4);
printf("Item %s was sold at price %s\n",d5,d6);
printf("Item %s was sold at price %s\n",d7,d8);
printf("Item %s was sold at price %s\n",d9,d10);

/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////end phase 3/////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
int k =0;
while(1){
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
if ( k==0 ){
    if ((rv = getaddrinfo("nunki.usc.edu", "4793", &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
}
else if ( k==1){
	if ((rv = getaddrinfo("nunki.usc.edu", "4893", &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
	}	
}
else if( k==2){
	if ((rv = getaddrinfo("nunki.usc.edu", "2793", &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
	}	
}
else if( k==3){
	if ((rv = getaddrinfo("nunki.usc.edu", "2893", &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
	}	
}

else break;
k++; 
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

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure
//	printf("\nd1 is %s\n",d1);
if(k==0){
	send(sockfd, d1,50,0);send(sockfd, d2,50,0);
	send(sockfd, d9,50,0);send(sockfd, d10,50,0);
}
else if (k==1){
	send(sockfd, d3,50,0);send(sockfd, d4,50,0);
	send(sockfd, d5,50,0);send(sockfd, d6,50,0);
	send(sockfd, d7,50,0);send(sockfd, d8,50,0);
}

else {
	send(sockfd, d1,50,0);send(sockfd, d2,50,0);
	send(sockfd, d3,50,0);send(sockfd, d4,50,0);
	send(sockfd, d5,50,0);send(sockfd, d6,50,0);
	send(sockfd, d7,50,0);send(sockfd, d8,50,0);
	send(sockfd, d9,50,0);send(sockfd, d10,50,0);
}

/*    if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
        perror("recv");
        exit(1);
    }
  
    buf[numbytes] = '\0';

    printf("client: received '%s'\n",buf);
*/
    close(sockfd);
}
////////////////////////////////////////send to bidder 2 TCP////////////////////////////////////////

printf("End of Phase 3 for Auction Server\n");
return 0;
}

int exist_function(char *c1,char *c2, char *c3, char *c4, char *a1,char *a2,char *a3 ){
	int add_item;
//	add_item = (char *)malloc(50 *sizeof(char) );
	strcat(a1,a2);
	if ( strncmp(a1,c1,11*sizeof(char) )==0 ) add_item = 0;
	else if ( strncmp(a1,c2,11*sizeof(char) )==0 ) add_item = 0;
	else if ( strncmp(a1,c3,11*sizeof(char) )==0 ) add_item = 0;
	else if ( strncmp(a1,c4,11*sizeof(char) )==0 ) add_item = 0;
	
	else{ 
		strcat(a1,",");
		strcat(a1,a3);
		
		add_item = 1;
	}
return add_item;	
}

char *cmp_function(char *b1,char *b2, char *b3,char* a1,char*a2,char*a3,char*a4,char*a5,char*a6,char*a7,char*a8,char*a9){
	
	char *return_cmp =(char *)malloc(200*sizeof(char));
	if ( (strcmp(b1,a1)||strcmp(b2,a2)) ==0 ){
//		if( strcmp(b3,a3)>=0 ) {strcat(b1,b2); return_cmp = strcat(b1,b3);}
		if( strcmp(b3,a3)>=0 ) {strcat(b1,b2); strcat(b1,","); strcat(b1,b3); return_cmp = strcat(b1,",");}
		else {strcat(a1,a2);strcat(a1,","); strcat(a1,a3); return_cmp = strcat(a1,",");}
	}
	else if( (strcmp(b1,a4)||strcmp(b2,a5)) ==0 ){
		if( strcmp(b3,a6)>=0 ) {strcat(b1,b2); strcat(b1,","); strcat(b1,b3); return_cmp = strcat(b1,",");}
		else {printf("b_6 is :%s\n",b3);strcat(a4,a5);strcat(a4,",");strcat(a4,a6);return_cmp = strcat(a4,",");}
	}
	else if( (((strcmp(b1,a7))||(strcmp(b2,a8)))) ==0 ){
		if( strcmp(b3,a9)>=0 ) {strcat(b1,",");strcat(b1,b2);strcat(b1,","); strcat(b1,b3);return_cmp = strcat(b1,",");}
		else {strcat(a7,",");strcat(a7,a8); strcat(a7,",");strcat(a7,a9);return_cmp = strcat(a7,",");}
	}
	else{
		strcat(b1,b2);strcat(b1,","); strcat(b1,b3);return_cmp = strcat(b1,",");
//		return_cmp = strcat(b1,b3);
	}
//	printf("return_cmp is : %s\n", return_cmp);
	return return_cmp;
}

char * add0(char *p1,char*p2){
	char *p3 = (char *)malloc(100*sizeof(char));
	if( strlen(p2) >=3 ){
		p3 = p2;
	}
	else{
	
	strcat(p1,p2);
	strcpy(p3,p1);
	p1[1]='\0';
	p1[2]='\0';
	
	}
	return p3;
}

void print_ip(char* a)
{
    char   *ptr, **pptr;
    struct hostent *hptr;
    char   str[32];
    ptr = "nunki";
    if((hptr = gethostbyname(ptr)) == NULL)
    {
        //printf(" gethostbyname error for host:%s\n", ptr);
        //return 0; 
    }

 //   printf("official hostname:%s\n",hptr->h_name);
 //   for(pptr = hptr->h_aliases; *pptr != NULL; pptr++)
 //       printf(" alias:%s\n",*pptr);

    switch(hptr->h_addrtype)
    {
        case AF_INET:
        case AF_INET6:
            pptr=hptr->h_addr_list;
            for(; *pptr!=NULL; pptr++)
				if(a ==PORT){
			   printf("Phase1 Auction server has TCP port number %s and IP address %s \n", 
                       PORT, inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str)));
					 }
            //printf(" first address: %s\n", 
             //          inet_ntop(hptr->h_addrtype, hptr->h_addr, str, sizeof(str)));
//			else if ((a == "Mary")||(a=="James")||(a=="John")||(a=="Matt")||(a=="Taylor")||(a=="Julia"))
			else if ( !(strcmp(a,"Mary")&strcmp(a,"James")&strcmp(a,"John")&strcmp(a,"Matt")&strcmp(a,"Taylor")&strcmp(a,"Julia")) )
			printf("Phase1: Auction Server IP Address: %s , PreAuction Port Number is %s sent to the<user %s> \n",inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str)),PORT,a);
			else if (a == "1893")
			printf ("Phase2: Auction Server IP Address %s PreAuction TCP  port Number 1893\n",inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str)));
//			else if (a == "phase3_ip_port")
//			printf("phase 3: Auction Server IP Address: %s Auction UDP Port Number: %s ",inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str)), );
		break;
        default:
            printf("unknown address type\n");
        break;
    }

	//return 0;
}
