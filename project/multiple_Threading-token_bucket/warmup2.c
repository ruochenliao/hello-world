#include<stdio.h>
#include <pthread.h>
#include<stdlib.h>
#include <sys/time.h>
#include<string.h>
#include <unistd.h>
#include "my402list.h"
#include "cs402.h"
#include <signal.h>
//#include <math.h>

	pthread_t *Q1_ptr;
	pthread_t *P_ptr;
	pthread_t *Q3_ptr;

struct timeval start_time;
My402List *Q1_list, *Q2_list, *server;
int stop;
sigset_t new;
struct sigaction act;

typedef struct{
	double lambda;
	double mu;
	double r;
	int B;
	int P;
	int n;
	int mode;
//	char * t;
	char t[100];
	double arrive_time;
	double inter_arrive_time;
	double time_enter_Q1;
	int num;
	double enterQ2_time;
	double enter_server_time;
//	double service_time;
//	double system_time;
	double time_in_Q1;
	double time_in_Q2;
}input_parameter_struct;

input_parameter_struct input_parameter;

pthread_mutex_t m;
pthread_cond_t cv;


double inter_arrival_time_s = 0;
double service_time_s;
double system_time_s;
double time_in_Q1_s;
double time_in_Q2_s;
//double time_in_S;
int arrive_num_s;
int token_drop_s;
int token_produce_s;
int packet_drop_s;
double square_diff_s;
int token_g;
int removed_from_Q2_s;
typedef struct{
	int a;
//	int token;
//	input_parameter_struct * Q1_bottle;
	input_parameter_struct * server_bottle;
	int b;
//	printf("you can use x b1,b2 ... bn;\n");
}global;
global global_varible;


// parameter initialization
void parseinput( int argc, char *argv[] ){
//	input_parameter = {0.5,0.35,1.5,10,3,20,0};
	input_parameter.lambda  = 0.5;
	input_parameter.mu = 0.35;
	input_parameter.r = 1.5;
	input_parameter.B = 10;
	input_parameter.P = 3;
	input_parameter.n = 20;
	input_parameter.mode = 0;

	global_varible.a = 0;
//	global_varible.token = 0;
//	global_varible.Q1_bottle = NULL;
	global_varible.server_bottle = NULL;
	global_varible.b = 0;
	if( argc> 15){
		fprintf(stderr,"input format error\n");
		exit(1);
	}
		
	int i;
	for(i=1; i<= (argc -1 ) ;i++){
		
		if( strcmp(argv[i],"-lambda") ==0 ){
			input_parameter.lambda = atof(argv[++i]);
			if( input_parameter.lambda < 0 ){
				fprintf(stderr,"lambda is less than 0\n");
				exit(1);
			}
			if(input_parameter.lambda <0.1 ){
				input_parameter.lambda = 0.1;
			}
		}
		else if ( strcmp(argv[i],"-mu") == 0 ){
			input_parameter.mu = atof(argv[++i]);
//			printf("input_parameter.mu is %f\n",input_parameter.mu);
			if( input_parameter.mu <0 ){
				fprintf(stderr,"mu is less than 0\n");
				exit(1);
			}
			if (input_parameter.mu < 0.1){
				input_parameter.mu = 0.1;
			}
		}	
		else if ( strcmp(argv[i],"-r") == 0 ){
			input_parameter.r = atof(argv[++i]);
			if( input_parameter.r <0 ){
				fprintf(stderr,"r is less than 0\n");
				exit(1);
			}
		}
			
		else if ( strcmp(argv[i],"-B") == 0 ){
			input_parameter.B = atoi(argv[++i]);
			if( input_parameter.B > 2147483647 ){
				fprintf(stderr,"B is greater than 2147483647\n");
				exit(1);
			}
		}
			
		else if ( strcmp(argv[i],"-P") == 0 ){
			input_parameter.P = atoi(argv[++i]);
			if(input_parameter.P > 2147483647){
				fprintf(stderr,"P is greater than 2147483647\n");
				exit(1);			
			}	
		}	
		else if ( strcmp(argv[i],"-n") == 0 ){
			input_parameter.n = atoi(argv[++i]);
			if(input_parameter.n > 2147483647){
				fprintf(stderr,"n is greater than 2147483647\n");
				exit(1);			
			}
		}	
		else if ( strcmp(argv[i],"-t") == 0 ){
			input_parameter.mode = 1;
			//printf("\n testfile is %s\n", argv[++i]);
			strcpy(input_parameter.t, argv[++i]);
		//	input_parameter.t = strdup(argv[++i]);
		}	
		else {
		//	printf("argv[%d] is : %s\n",i,argv[i]);
			fprintf(stderr,"can not find the input command\n");
			exit(1);
			
		}	
	
	}
}
/////////display the data to the screem////////////////////////////////////////////////
FILE *load_data(){
	if(input_parameter.mode == 1){
		char buf[100];
		FILE *fp;
//		int i = 0;
//		int j;
		//char *tab_ptr;
		
		printf("input_parameter.t is %s\n",input_parameter.t);
		if ( (fp = fopen(input_parameter.t,"r")) == NULL ){
			fprintf(stderr,"couldn't open the file\n");
			exit(1);
		}
		
		fgets(buf, sizeof(buf), fp);
		input_parameter.n = atoi(buf);
		printf("Emulation Parameters:\n");
		printf("\tnumber to arrive = %d\n",input_parameter.n);		
		printf("input_parameter.n is %d\n",input_parameter.n);
		printf("\tr = %g\n",input_parameter.r);
		printf("\tB = %d\n",input_parameter.B);
		printf("\ttsfile = %s\n\n",input_parameter.t);

//		fclose(fp);
		return (fp);
	}
	else{
		printf("Emulation Parameters:\n\tnumber to arrive = %d\n\tlambda = %g\n\tmu = %g\n\tr = %g\n\tB = %d\n\tP = %d\n\n",
		input_parameter.n,input_parameter.lambda,input_parameter.mu,input_parameter.r,input_parameter.B,input_parameter.P);
		if(input_parameter.n == 2147483647 ) input_parameter.n = 3000;
		return NULL;
	}
	
}


double Diff_time(struct timeval *start_time, struct timeval *end_time){
	
	double diff_time = ((end_time->tv_sec - start_time->tv_sec)*1000.0 + (end_time->tv_usec-start_time->tv_usec)/1000.0);
//	printf("start time is : %f\n",(end_time->tv_sec - start_time->tv_sec)*1000.0);
	return diff_time;
}


//double Diff_time(struct timeval start, struct timeval end) {
 //   return ((end.tv_sec-start.tv_sec)*1000.0 + (end.tv_usec-start.tv_usec)/1000.0);
//}

////////////////////Thread Arrive//////////////////////////////////////////////////////////
void *Arrive(void *arg){
	struct timeval current_time;//,current_time_1;
	FILE *fp = load_data();
//	gettimeofday(&start_time,NULL);	
//	printf("%012.3fms: emulation begins\n",0.0);
	
	int line;
	input_parameter_struct * node[input_parameter.n];


//////////////create packet by reading from the file////////////////////////////////

	if( fp != NULL ){

		char buf[100];
		line = 0;
		int cn;
		
		//input_parameter_struct * node[input_parameter.n];

		while ( fgets(buf, sizeof(buf), fp)!= NULL ){
			
			//line = 0;
			node[line] = (input_parameter_struct *)malloc(sizeof(input_parameter_struct) );
			if ( node[line] == NULL ){
				fprintf(stderr,"no space allocated by malloc\n");
				exit(1);
			}
			//parameter_copy();
			node[line] -> n = input_parameter.n;
			node[line] -> r = input_parameter.r;
			//printf("node[line] -> r is %f\n",node[line] -> r);
			node[line] -> B = input_parameter.B;
			node[line] -> num = line;
			strcpy( node[line] -> t, input_parameter.t );			
			//printf("node[line]->n is :%d\n",node[line]->n);
			char * pch;
			cn = 0;
			//int k = 0;
			pch = strtok (buf," \t");
			while (pch != NULL){
				
				//parameter_copy(); lambada and mu are time (ms)
				switch (cn){
					case 0: node[line] -> lambda = atof(pch); break;  //unit is s
					case 1: node[line] -> P = atoi(pch);  break;
					case 2: node[line] -> mu = atof(pch);  break;
					default: printf("error happens in switch\n"); break;
				}
				//printf("n is %d\n",node[line] -> n);
				cn ++;
				pch = strtok (NULL, " \t");
			}
			
				//printf("test:\n\tnumber to arrive = %d\n\tlambda = %g\n\tmu = %g\n\tr = %g\n\tB = %d\n\tP = %d\n",
				//node[line]->n,node[line]->lambda,node[line]->mu,node[line]->r,node[line]->B,node[line]->P);

			line ++;
		}
			
	}
//////create the packet by reading from keyboard or from default value//////////
	else{	
	
		for(line =0; line < input_parameter.n ; line++){
			  if(input_parameter.n == 3000){
				break;
			  }
			  node[line] = (input_parameter_struct *)malloc(sizeof(input_parameter_struct) );
			//node[line] = (input_parameter_struct *)malloc(sizeof(input_parameter_struct) );
			if ( node[line] == NULL ){
				fprintf(stderr,"no space allocated by malloc\n");
				exit(1);
			}
		
			node[line] -> n = input_parameter.n;
			node[line] -> r = input_parameter.r;
			node[line] -> B = input_parameter.B;
			node[line] -> num = line;			
//printf("lambda is %f, %f,%p\n",input_parameter.lambda/1000,node[line] -> lambda,node);
			node[line] -> lambda = 1000/(input_parameter.lambda);
			node[line] -> P = input_parameter.P;
			node[line] -> mu = 1000/(input_parameter.mu);
		}
	}
///////////////////////////////////////////////////////////////////////////////////////////////////
                               //emulation begins and Packet arrives//
///////////////////////////////////////////////////////////////////////////////////////////////////

	pthread_mutex_lock (&m);
	global_varible.a =1;
	pthread_cond_broadcast(&cv);
	pthread_mutex_unlock (&m);
	int sleep_time;	
	double inter_arrive_time;

	double leave_time;
	input_parameter_struct * Q1_bottle;
	double enterQ2_time;	


	Q1_list = (My402List *)malloc(sizeof(My402List));
	My402ListInit(Q1_list);
	//Q2_list = (My402List *)malloc(sizeof(My402List));

	line = 0;
//	for (line = 0; line < input_parameter.n ; line ++){
	gettimeofday(&start_time,NULL);	
	printf("%012.3fms: emulation begins\n",0.0);

	while( line !=input_parameter.n && stop ==0){
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
		

		if( input_parameter.n  == 3000 ){
			node[line] = (input_parameter_struct *)malloc(sizeof(input_parameter_struct) );
			if ( node[line] == NULL ){
				fprintf(stderr,"no space allocated by malloc\n");
				exit(1);
				printf("hello world\n");
			}
		
			//node[line] -> n = 0;
			node[line] -> r = input_parameter.r;
			node[line] -> B = input_parameter.B;
			node[line] -> num = line;			
//printf("lambda is %f, %f,%p\n",input_parameter.lambda/1000,node[line] -> lambda,node);
			node[line] -> lambda = 1000/(input_parameter.lambda);
			node[line] -> P = input_parameter.P;
			node[line] -> mu = 1000/(input_parameter.mu);			
			//printf("hello world\n");
		}
		
		
		
		
		if(line < input_parameter.n ){
			//line = 0;
			sleep_time = (int)(1000*node[line] -> lambda);
			//printf("(int)(node[line] -> lambda) is %d\n",(int)(node[line] -> lambda));
			
			usleep( sleep_time );		
			gettimeofday(&current_time,NULL);
			//arrive time
			node[line] -> arrive_time = Diff_time(&start_time,&current_time);
			printf("%012.3fms: ",node[line] -> arrive_time );
			
			if(line == 0){
				inter_arrive_time = node[line] -> arrive_time;
			}
			else{
				inter_arrive_time = node[line] -> arrive_time - node[line -1] -> arrive_time;
			}
			printf("p%d arrives, needs %d tokens, inter-arrival time = %.3fms",line+1,node[line] -> P,inter_arrive_time);

//////////////////////////////////drop packet//////////////////////////////////////
			if( node[line] -> P > input_parameter.B){
				printf(" droped");
			//	pthread_mutex_lock(&m);
				
				packet_drop_s ++;
				line++;
				inter_arrival_time_s = 	inter_arrival_time_s + inter_arrive_time;
				arrive_num_s = line;
				
			//	pthread_cond_broadcast(&cv);
			//	pthread_mutex_unlock(&m);				
				printf("\n");
				continue;
			}
			printf("\n");
			
////////////////////////packet enters Q1///////////////////////////////////////
			pthread_mutex_lock(&m);
			inter_arrival_time_s = 	inter_arrival_time_s + inter_arrive_time;
			arrive_num_s = line +1;
			My402ListPrepend(Q1_list, (void*)(node[line]));
			gettimeofday(&current_time,NULL);
			node[line] -> time_enter_Q1 = Diff_time(&start_time,&current_time);
			//printf("node[line] -> time_enter_Q1 is %f\n",node[line] -> time_enter_Q1);
			printf("%012.3fms: p%d enters Q1\n",Diff_time(&start_time,&current_time),line+1);










		if(Q1_list -> num_members>=1 && token_g  >= ((input_parameter_struct *)(Q1_list ->anchor.prev -> obj))-> P ){

			token_g = token_g - ((input_parameter_struct *)(Q1_list -> anchor.prev ->obj)) -> P;

			gettimeofday(&current_time,NULL);
			leave_time = Diff_time(&start_time,&current_time);
			
			printf("%012.3fms: ",Diff_time(&start_time,&current_time));
			//printf("p%d leaves Q1 ",++leave);
			printf("p%d leaves Q1 ", ((input_parameter_struct *)(Q1_list -> anchor.prev ->obj)) -> num + 1);
			printf("time in Q1 = %.3fms ",leave_time-((input_parameter_struct *)(Q1_list ->anchor.prev -> obj)) ->time_enter_Q1 );
			((input_parameter_struct *)(Q1_list -> anchor.prev ->obj)) -> time_in_Q1 = leave_time-((input_parameter_struct *)(Q1_list ->anchor.prev -> obj)) ->time_enter_Q1 ;
			printf("token bucket now has %d token\n",token_g);

			Q1_bottle = ((input_parameter_struct *)(Q1_list -> anchor.prev -> obj));
		
			My402ListUnlink(Q1_list,Q1_list -> anchor.prev);
			

////////////////////////////////////p enter Q2 ////////////////////////////////	

			My402ListPrepend(Q2_list, (void*)(Q1_bottle) );
			gettimeofday(&current_time,NULL);
			enterQ2_time = Diff_time(&start_time,&current_time);	
			printf("%012.3fms: ",enterQ2_time);
			printf("p%d enters Q2\n",((input_parameter_struct *)(Q1_bottle )) -> num+1);
			((input_parameter_struct *)(Q1_bottle )) -> enterQ2_time = enterQ2_time;

			if(My402ListEmpty(Q2_list) == FALSE){
				global_varible.b = 1;

			}	
		}









			pthread_cond_broadcast(&cv);
			pthread_mutex_unlock(&m);
			line ++;
			//printf("hello world0\n");


		}
		
	}
	sleep(10);	
	pthread_exit((void*)1);
//	return ( (void*) 2);
}


void handler(){
	struct timeval current_time;
	
	pthread_cancel(*Q1_ptr);	
	pthread_cancel(*P_ptr);
//	printf("hello world\n");
	pthread_mutex_lock(&m);
	stop  =  1;
	double square_root;
	
//	printf("\nStatistics:\n");
	while(global_varible.server_bottle){
		pthread_cond_wait(&cv, &m);
//		printf("hello world\n");
	}
	gettimeofday(&current_time,NULL);
	int i,j;
//	printf("My402ListLength(Q1_list) is :%d\n",My402ListLength(Q1_list));
	//int removed_packet_Q1 = arrive_num_s;
//	int removed_packet_Q2 = removed_from_Q2_s;
	My402ListElem * P1,*P2;
	P1 = &(Q1_list -> anchor);
	P2 = &(Q2_list -> anchor);
	for(i = My402ListLength(Q1_list); i>0;i--){
		//removed_packet_Q1 = arrive_num_s - i ;
		P1 = P1 -> prev;
		printf("%012.3fms: ",Diff_time(&start_time,&current_time));
		printf("p%d removed from Q1 (after Control + C)\n", ((input_parameter_struct *)(P1 -> obj)) -> num+1);
	}
	
	for(j = My402ListLength(Q2_list); j >0; j--){
		P2 = P2 -> prev;
		printf("%012.3fms: ",Diff_time(&start_time,&current_time));
		printf("p%d removed from Q2 (after Control + C)\n", ((input_parameter_struct *)(P2 -> obj)) -> num+1);
	}
	printf("%012.3fms: emulation ends\n",Diff_time(&start_time,&current_time) );
	double end_time = Diff_time(&start_time,&current_time);	

	printf("\nStatistics:\n\n");
	//printf("\tinter_arrival_time_s is %f\n",inter_arrival_time_s);
	if(inter_arrival_time_s == 0) printf("\taverage packet inter-arrival time = N/A (no packet arrived in Q1)\n");
	else printf("\taverage packet inter-arrival time = %.6gs\n",inter_arrival_time_s/arrive_num_s/1000);

	if (service_time_s == 0) printf("\taverage packet inter-arrival time = N/A (no packet has been served)\n\n");
	else printf("\taverage packet service time = %.6gs\n\n",service_time_s/server->num_members/1000 );
	
	if(time_in_Q1_s == 0) printf("\taverage number of packets in Q1 = 0\n");
	else printf("\taverage number of packets in Q1 = %.6g\n",time_in_Q1_s/end_time);

	if( time_in_Q2_s == 0) printf("\taverage number of packets in Q2 = 0\n");
	else printf("\taverage number of packets in Q2 = %.6g\n",time_in_Q2_s/end_time);
	
	if( service_time_s == 0) printf("\taverage number of packets at S = 0\n\n");
	else printf("\taverage number of packets at S = %.6g\n\n",service_time_s/end_time);

	square_root = square_diff_s/server->num_members - system_time_s*system_time_s/server->num_members/server->num_members/1000000;
	if(server->num_members == 0) printf("\taverage time a packet spent in system = N/A\n\n");
	else printf("\taverage time a packet spent in system = %.6gs\n",system_time_s/server->num_members/1000);
	
	if( server->num_members == 0 )printf("\tstandard deviation for time spent in system = N/A(no packet has gone through the whole system)\n");
	else printf("\tstandard deviation for time spent in system = %.6g\n\n",square_root);

	printf("\ttoken drop probability = %.6g\n",(double)token_drop_s/(double)token_produce_s );
//	printf("\tpacket drop probability = %.6g\n",(double)(arrive_num_s - server->num_members + packet_drop_s)/(double)arrive_num_s );

	if(inter_arrival_time_s ==0 ) printf("\tpacket drop probability = N/A (no packet arrived)\n");
//	else printf("\tpacket drop probability = %.6g\n",(double)(arrive_num_s - server->num_members)/(double)arrive_num_s );
	else printf("\tpacket drop probability = %.6g\n",(double)packet_drop_s/(double)arrive_num_s );
	pthread_cond_broadcast(&cv);
	pthread_mutex_unlock(&m);
	exit(1);
}
///////////////////////////////////////////////////////////////////////////////////////////
				  //Token//
///////////////////////////////////////////////////////////////////////////////////////////
void *Token(void *arg){


	pthread_mutex_lock(&m);
	while(global_varible.a  !=1 ){
		pthread_cond_wait(&cv, &m);
//		printf("Q2 is still waiting for signal\n");
	}

	pthread_cond_broadcast(&cv);
	pthread_mutex_unlock(&m);

	double r = input_parameter.r;
	int B = input_parameter.B;
	
	int i =0;
	token_g = 0;
	int token_drop =0;
	struct timeval current_time;
	double leave_time;
	struct timeval current_time_1;
	input_parameter_struct * Q1_bottle;
	double enterQ2_time;
	Q2_list = (My402List *)malloc(sizeof(My402List));
	My402ListInit(Q2_list);

	while( stop ==0 ){
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
///////////////////////////token enter Bucket/////////////////////////////////////////
		usleep(1000000/r);

		pthread_mutex_lock(&m);
		if( token_g < B ){
//			usleep(1000000/r);
			
			token_g ++;			
			i++;
			gettimeofday(&current_time,NULL);
			printf("%012.3fms: ",Diff_time(&start_time, &current_time) );
			printf("token t%d arrives, token bucket now has %d token\n",i,token_g);
		}
		else{
//			usleep(1000000/r);
			i++;
			token_drop ++;
			token_drop_s = token_drop;
			gettimeofday(&current_time,NULL);
			printf("%012.3fms: ",Diff_time(&start_time, &current_time) );
			printf("token t%d arrives, dropped\n",i);
		}

/////////////////////////////packet leaves Q1/////////////////////////////////////////////	
		// ((input_parameter_struct *)(Q1_list ->anchor.prev -> obj))	
	//	pthread_mutex_lock(&m);
		//printf("hello world1\n");
		
		token_produce_s = i;
		//token_drop_s = token_drop;
		if(Q1_list -> num_members>=1 && token_g  >= ((input_parameter_struct *)(Q1_list ->anchor.prev -> obj))-> P ){

			token_g = token_g - ((input_parameter_struct *)(Q1_list -> anchor.prev ->obj)) -> P;
			gettimeofday(&current_time_1,NULL);
			leave_time = Diff_time(&start_time,&current_time_1);
			
			printf("%012.3fms: ",Diff_time(&start_time,&current_time_1));
			//printf("p%d leaves Q1 ",++leave);
			printf("p%d leaves Q1 ", ((input_parameter_struct *)(Q1_list -> anchor.prev ->obj)) -> num + 1);
			printf("time in Q1 = %.3fms ",leave_time-((input_parameter_struct *)(Q1_list ->anchor.prev -> obj)) ->time_enter_Q1 );
			((input_parameter_struct *)(Q1_list -> anchor.prev ->obj)) -> time_in_Q1 = leave_time-((input_parameter_struct *)(Q1_list ->anchor.prev -> obj)) ->time_enter_Q1 ;
			printf("token bucket now has %d token\n",token_g);

			Q1_bottle = ((input_parameter_struct *)(Q1_list -> anchor.prev -> obj));
		
			My402ListUnlink(Q1_list,Q1_list -> anchor.prev);
			

////////////////////////////////////p enter Q2 ////////////////////////////////	

			My402ListPrepend(Q2_list, (void*)(Q1_bottle) );
			gettimeofday(&current_time,NULL);
			enterQ2_time = Diff_time(&start_time,&current_time);	
			printf("%012.3fms: ",enterQ2_time);
			printf("p%d enters Q2\n",((input_parameter_struct *)(Q1_bottle )) -> num+1);
			((input_parameter_struct *)(Q1_bottle )) -> enterQ2_time = enterQ2_time;

			if(My402ListEmpty(Q2_list) == FALSE){
				global_varible.b = 1;

			}	
		}
		//printf("My402ListEmpty(Q1_list) is %d\n",My402ListEmpty(Q1_list));
		//printf("arrive_num_s == input_parameter.n = %d\t%d\n",arrive_num_s,input_parameter.n);
		if(My402ListEmpty(Q1_list) == TRUE && arrive_num_s == input_parameter.n){
			//printf("hello world\n");
			pthread_cond_broadcast(&cv);
			pthread_mutex_unlock(&m);			
			break;
		}
		
		pthread_cond_broadcast(&cv);
		pthread_mutex_unlock(&m);
		//printf("hello world1\n");
	}
	sleep(100);
	return 0;
}


///////////////////////////////////////////////////////////////////////////////////////////
				  //Server//
///////////////////////////////////////////////////////////////////////////////////////////

void *Server(void *arg){
//	act.sa_handler = handler;
//	sigaction(SIGINT,&act,NULL);
//	pthread_sigmask(SIG_UNBLOCK,&new,NULL);


	struct timeval current_time;
//	double enterQ2_time;	
	double enter_server_time;
	double leave_server_time;
	int sleep_time;
	server = (My402List *)malloc(sizeof(My402List));
	My402ListInit(server);

	while(stop == 0){
		pthread_mutex_lock(&m);
		//printf("hello world2\n");
		while(global_varible.b  != 1 ){
			pthread_cond_wait(&cv, &m);
		}	
//////////////////////////leave Q2 and enter server//////////////////////////////////
//		
		if( My402ListEmpty(Q2_list) == FALSE && global_varible.server_bottle == NULL){

			//pthread_mutex_lock(&m);
			global_varible.server_bottle = ((input_parameter_struct *)(Q2_list -> anchor.prev -> obj));
			My402ListUnlink(Q2_list,Q2_list -> anchor.prev);
			gettimeofday(&current_time,NULL);
			enter_server_time = Diff_time(&start_time,&current_time);
			//printf("hello world\n");
			printf("%012.3fms: ",enter_server_time);
			printf("p%d leaves Q2, ",global_varible.server_bottle -> num +1);
			printf("time in Q2 = %.3fms, begin service at S\n",enter_server_time - ((input_parameter_struct *)(global_varible.server_bottle)) -> enterQ2_time);
			time_in_Q2_s = time_in_Q2_s + enter_server_time - ((input_parameter_struct *)(global_varible.server_bottle)) -> enterQ2_time;			
			global_varible.server_bottle -> enter_server_time = enter_server_time;
			removed_from_Q2_s = global_varible.server_bottle -> num;
			pthread_mutex_unlock(&m);
//////////////////////////////  server serve P ///////////////////////////////////////////////

			sleep_time = (int)(1000*global_varible.server_bottle -> mu);
			usleep(sleep_time);

			pthread_mutex_lock(&m);
			gettimeofday(&current_time,NULL);
			leave_server_time = Diff_time(&start_time,&current_time);
			printf("%012.3fms: ",leave_server_time);
			printf("p%d departs from S, ",global_varible.server_bottle ->num +1);
			printf("service time = %.3fms, ",leave_server_time - enter_server_time);
			printf("time in system = %.3fms\n",leave_server_time - global_varible.server_bottle ->arrive_time);
			service_time_s = service_time_s + leave_server_time - enter_server_time;
			system_time_s = system_time_s + leave_server_time - global_varible.server_bottle ->arrive_time;
			
			time_in_Q1_s = time_in_Q1_s + global_varible.server_bottle -> time_in_Q1;
			square_diff_s = square_diff_s + (leave_server_time - global_varible.server_bottle ->arrive_time)*(leave_server_time - global_varible.server_bottle ->arrive_time)/1000000;
			//printf("global_varible.server_bottle -> time_in_Q1 is %f\n",time_in_Q1_s);
			//global_varible.server_bottle ->service_time = leave_server_time - enter_server_time;
			//global_varible.server_bottle -> system_time = leave_server_time - global_varible.server_bottle ->arrive_time;
			My402ListPrepend(server, (void*)( global_varible.server_bottle ) );

			global_varible.server_bottle = NULL;
//			pthread_cond_broadcast(&cv);
//			pthread_mutex_unlock(&m);
//			printf("hello world2\n");
		}
		if(My402ListEmpty(Q1_list) == TRUE && My402ListEmpty(Q2_list) == TRUE && arrive_num_s == input_parameter.n ){
			//printf("hello world!!\n");
			pthread_cond_broadcast(&cv);
			pthread_mutex_unlock(&m);			
			handler();
			
		}
		pthread_cond_broadcast(&cv);
		pthread_mutex_unlock(&m);	
	}	


	return 0;
}


int main(int argc, char *argv[]){
	
	parseinput(argc,argv);
	stop = 0;
	//printf("the start time is:%f,%f\n", start_time.tv_sec,start_time.tv_usec );
	pthread_mutex_init(&m, NULL);
	pthread_cond_init (&cv, NULL);
  
//	pthread_t *interupt_id = (pthread_t*)malloc(sizeof(pthread_t));


	Q1_ptr = (pthread_t*)malloc(sizeof(pthread_t));
	P_ptr = (pthread_t*)malloc(sizeof(pthread_t));
	Q3_ptr = (pthread_t*)malloc(sizeof(pthread_t));

	//signal catch
	sigemptyset(&new);
	sigaddset(&new, SIGINT);
	pthread_sigmask(SIG_BLOCK, &new, NULL);
	act.sa_handler = handler;
	sigaction(SIGINT,&act,NULL);
	pthread_sigmask(SIG_UNBLOCK,&new,NULL);


//	pthread_create(interupt_id,0,interupt,NULL);
	pthread_create(Q1_ptr,0,Arrive,NULL);
	pthread_create(P_ptr,0,Token,NULL);
	pthread_create(Q3_ptr,0,Server,NULL);
	
	void *result;

//	(void)pthread_join(*interupt_id, 0);

	(void)pthread_join(*Q1_ptr,(void**)&result);
	(void)pthread_join(*P_ptr,0);
	(void)pthread_join(*Q3_ptr,0);	

	return 0;
}



