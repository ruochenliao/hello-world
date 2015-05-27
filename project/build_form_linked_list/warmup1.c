#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <errno.h>
#include <time.h>
#include <sys/stat.h>

#include "cs402.h"
#include "my402list.h"

int globle;
//globle =0;
struct unit{
	char *sign;
	char *date;
	char *amount;
	char *descrition;
};
void input_error(){
	fprintf(stderr,"malformed command\n");
	fprintf (stderr,"The commandline syntax for warmup1 is as follows: \n");
	fprintf (stderr,"warmup1 sort [tfile]\n");
//	fprintf (stderr,"Square bracketed items are optional. If tfile is not specified, your program should read from stdin. Unless otherwise specified, output of your program must go to stdout and error messages must go to stderr. \n");
	exit(1);
}

FILE *read_input(int argc, char *argv[]){
	FILE * fp;
//	char buf[256];
//	printf("hello world1\n");
	if ( argc == 2){
//		FILE * fp;
		fp = stdin;
	}
	
	else if( (fopen(argv[2],"r")) == NULL ){
//		fprintf(stderr,"there is no such file or directory\n");
//		fprintf(stderr,"can not find the input file '%s'  or the input file is not in the right format\n",argv[2]);
		fprintf(stderr, "input file %s\n", strerror(errno));
		exit(1);
	}
	else{
		
		fp = fopen(argv[2],"r");

		struct stat dirChecker;
		stat(argv[2], &dirChecker);
		if (S_ISDIR(dirChecker.st_mode)) {
			fprintf(stderr, "%s is a directory\n", argv[2]);
			exit(1);
		}		
	}
	return fp;
}

int check_buf(char * buf){
	if(buf[0] == '+'||buf[0] == '-' ){
		return 1;
	}
	else return 0;
}

///////////////////////       parse list   //////////////////////////////
My402List* parse(FILE *fp, My402List*list){
	char buf[1026];
//	char * buf;
//	buf = malloc(1026*sizeof(char));
	
	memset(buf,0,sizeof(buf));
//	memset(buf,0,1026*sizeof(char));
	int line =0;
	int cnt;
	char *start_ptr;
	char *tab_ptr;
//	struct unit *line_unit_ptr[line];
	while(fgets(buf, sizeof(buf), fp) != NULL){		
		if(strlen(buf) >= 100){
			fprintf(stderr,"the input data is too large\n");
			exit(1);
		}
		int check1 = check_buf(buf);

		if (check1 == 1){
			
		}
		else{
//			fprintf(stderr,"input file is not in the right format\n");
			fprintf(stderr,"input file is not in the right format: first character should be '+' or '-'\n");
			exit(1);
		}
		struct unit *line_unit_ptr[line];
		cnt = 0;
		line_unit_ptr[line] = (struct unit*)malloc(sizeof(struct unit));		
		if ( line_unit_ptr[line] == NULL ){
			fprintf(stderr,"no space allocated by malloc\n");
			exit(1);
		}
		line_unit_ptr[line]->sign = (char *)malloc(3*sizeof(char));
		if ( line_unit_ptr[line] ->sign == NULL ){
			fprintf(stderr,"no space allocated by malloc\n");
			exit(1);
		}
		line_unit_ptr[line]->date = (char *)malloc(30*sizeof(char));
		if ( line_unit_ptr[line] -> date== NULL ){
			fprintf(stderr,"no space allocated by malloc\n");
			exit(1);
		}
		line_unit_ptr[line]->amount = (char *)malloc(56*sizeof(char));
		if ( line_unit_ptr[line] -> amount == NULL ){
			fprintf(stderr,"no space allocated by malloc\n");
			exit(1);
		}
		line_unit_ptr[line]->descrition = (char *)malloc(128*sizeof(char));
		if ( line_unit_ptr[line] -> descrition == NULL ){
			fprintf(stderr,"no space allocated by malloc\n");
			exit(1);
		}

		 

//		*line_unit_ptr[line] = (struct unit)malloc(sizeof(struct unit));
//		memset(line_unit_ptr[line]->sign,0,sizeof(line_unit_ptr[line]->sign));
//		memset(line_unit_ptr[line]->date,0,sizeof(line_unit_ptr[line]->date));
//		memset(line_unit_ptr[line]->amount,0,sizeof(line_unit_ptr[line]->amount));
//		memset(line_unit_ptr[line]->descrition,0,sizeof(line_unit_ptr[line]->descrition));
		memset(line_unit_ptr[line]->sign,0,3*sizeof(char));
		memset(line_unit_ptr[line]->date,0,30*sizeof(char));
		memset(line_unit_ptr[line]->amount,0,56*sizeof(char));
		memset(line_unit_ptr[line]->descrition,0,128*sizeof(char));


//		memset(line_unit_ptr[line],0,sizeof(struct unit));
//		memset(line_unit,0,sizeof(struct unit));	
		
		while( cnt!= 4){
			
			if(cnt ==0){
				cnt++;
				start_ptr = buf;
//				printf("start_ptr is %s\n",start_ptr);
				tab_ptr = strchr(start_ptr,'\t');
				if( tab_ptr!= NULL){
					*tab_ptr++ ='\0';
				}
				else{
					fprintf(stderr,"input format error: some important data should input\n");
					exit(1);
				}
				strncpy(line_unit_ptr[line]->sign,start_ptr,3*sizeof(char) );
//				printf("%s\n",(line_unit_ptr[line]->sign));
//				printf("%s\n",start_ptr);
			}
			else if (cnt == 1){
				cnt ++;
				start_ptr = tab_ptr;
				tab_ptr = strchr(start_ptr,'\t');
				if(tab_ptr != NULL){
					 *tab_ptr++ ='\0';
				}
				else{
					fprintf(stderr,"input format error: some important data should input\n");
					exit(1);
				}
				strncpy((line_unit_ptr[line]->date),start_ptr,30*sizeof(char) );
				time_t current_time;
//				struct unit u;
				time(&current_time);
				if ( atoi(start_ptr) > (int)(time(&current_time)) ) {
					fprintf(stderr, "the time input is greater than current time\n");
					exit(1);
				}
				
/*				if (strcmp (start_ptr,"2147483647") == 0){ 
					fprintf(stderr,"time input has format error\n");
					exit(1);
				}
*/
				//				printf("%s\n",&(line_unit_ptr[line]->date));
			}
//			start_ptr = tab_ptr;	
			else if (cnt == 2){
				cnt ++;
				start_ptr = tab_ptr;
				tab_ptr = strchr(start_ptr,'\t');
				if(tab_ptr != NULL){
					 *tab_ptr++ ='\0';
				}
				else{
					fprintf(stderr,"input format error: some important data should input\n");
					exit(1);
				}
				strncpy((line_unit_ptr[line]->amount),start_ptr,56*sizeof(char) );
				char * x = strchr(start_ptr,'.');
//				printf("strlen(x) is %d\n",strlen(x));
				if( strlen(x) != 3){
					fprintf(stderr,"amount input has format error\n");
					exit(1);
				} 
			}				
			else if (cnt ==3){
				cnt ++;
				start_ptr = tab_ptr;
				char* tab = strchr(start_ptr,'\t');
				if(tab !=NULL ){
					fprintf(stderr,"too many filed for input\n");
					exit(1);
				}
				tab_ptr = strchr(start_ptr,'\n');
				if(tab_ptr != NULL){
					 *tab_ptr ='\0';
				}
				else{
					fprintf(stderr,"there is malformed input because lack of 'enter' at the end of the input \n");
					exit(1);
				}
				strncpy((line_unit_ptr[line]->descrition),start_ptr,128*sizeof(char) );	//printf("%s\n",(line_unit_ptr[line]->descrition));
			}
		}
		My402ListPrepend(list, line_unit_ptr[line]);
//		printf("%s\n",line_unit_ptr[line]->date);
//		(list -> num_members) = ++line;
		line ++;
//		printf("line is %d\n",line);
		memset(buf,0,sizeof(buf));
	}

return list;
}

void bubble_sort(int a[],int n,My402List * list)//n为数组a的元素个数
{
    int i,j,bottle;
    struct unit * temp;
    struct unit temp_struct ={0,0,0,0};
    temp = &temp_struct;    
//    My402ListElem *p = (list -> anchor).next;
    for(j=0;j<n-1;j++)
    {
        My402ListElem *p = (list -> anchor).next;
        for(i=0;i<n-1-j;i++,p =p -> next)
        {
            if(a[i]>a[i+1])//数组元素大小按升序排列
            {
		  temp->sign = ((struct unit*)(p -> obj)) -> sign;
		  temp->date = ((struct unit*)(p -> obj)) -> date;
		  temp->amount = ((struct unit*)(p -> obj)) -> amount;
		  temp->descrition = ((struct unit*)(p -> obj)) -> descrition;
		  ((struct unit*)(p -> obj)) -> sign = ((struct unit*)(p -> next -> obj)) -> sign;
		  ((struct unit*)(p -> obj)) -> date = ((struct unit*)(p -> next -> obj)) -> date;
		  ((struct unit*)(p -> obj)) -> amount = ((struct unit*)(p -> next -> obj)) -> amount;
		  ((struct unit*)(p -> obj)) -> descrition = ((struct unit*)(p -> next -> obj)) -> descrition;

		  ((struct unit*)(p -> next -> obj)) -> sign = temp -> sign;
		  ((struct unit*)(p -> next -> obj)) -> date = temp -> date;
		  ((struct unit*)(p -> next -> obj)) -> amount = temp -> amount;
		  ((struct unit*)(p -> next -> obj)) -> descrition = temp -> descrition;
                bottle=a[i];
                a[i]=a[i+1];
                a[i+1]=bottle;
            }
//	    p =p -> next;
        }	
     }
//     int k;
//     for(k = 0;k<4;k++)
//	printf("a[%d] is %d\n",k,a[k]);
}

My402List * sortinput(My402List * list){
	char * a[300];
	int b[300];
	int i;
	int num = list -> num_members;
	My402ListElem *p = (list -> anchor).next;
	for(i = 0; num != 0; num --,i++){
		a[i] = ((struct unit *)(p->obj))->date;
		b[i] = atoi(a[i]);
		p = p -> next;
	}	
	
	bubble_sort(b,list -> num_members,list);
//	printf("%s",((struct unit *)(list->anchor.next->obj))->sign);
	return list;
}

void print_process(int len, int x){
	int y;
	int z;

	if(len <=6){
//		printf("%d%s",len,"\t");
		printf("%d%s%d%d",x/100,".",x%100/10,x%10);
	}
	else if (len >6 && len <=11){
//		printf("%d%s",len,"\t");
		y= x/100000;
//		printf("%d%s%d%s%d%d",x/100000,",",(x - y*100000)/100,".",x%100/10,x%10);
		printf("%d%s",x/100000,",");
		if((x - y*100000)/100<100&& (x - y*100000)/100 >9) printf("%s","0");
		else if ((x - y*100000)/100 <=9) printf("%s","00");
		printf("%d%s%d%d",(x - y*100000)/100,".",x%100/10,x%10);
	}
	else if (len ==12) {
		z = x/100000000;
		y = (x-z*100000000)/100000;
//		printf("%d%s%d%s%d%s%d%d",x/100000000,",",(x-z*100000000)/100000,",",(x - z*100000000 - y*100000)/100,".",x%100/10,x%10);
		printf("%d%s",x/100000000,",");
		if( y <100&& y >9) printf("%s","0");
		else if( y <= 9  ) printf("%s","00");
		printf("%d%s",(x-z*100000000)/100000,",");
		if((x - z*100000000 - y*100000)/100 <100&& (x - z*100000000 - y*100000)/100 >9) printf("%s","0");
		else if((x - z*100000000 - y*100000)/100 <= 9) printf("%s","00");
		printf("%d%s%d%d",(x - z*100000000 - y*100000)/100,".",x%100/10,x%10);
	}
	else printf("  ?,???,???.??");	
}

int lenhelper(int x) {
    if(x>=1000000000) return 10;
    if(x>=100000000) return 9;
    if(x>=10000000) return 8;
    if(x>=1000000) return 7;
    if(x>=100000) return 6;
    if(x>=10000) return 5;
    if(x>=1000) return 4;
    if(x>=100) return 3;
    if(x>=10) return 2;
    return 1;
}

void balance(int x, My402ListElem *p){
	int k = 0,j =0;
	int format_len =0;

	if( strcmp( ((struct unit *)(p->obj))->sign, "+" ) ==0 ){
		globle = globle + x;	

	}
	else{
		globle = globle - x;

	}
	
	if(globle >0)
		k = lenhelper(globle);
	else{ 
//		globle = abs ( globle );
		k = lenhelper(abs ( globle ));
	}

	if (k <= 5)
		format_len = k+1;
	else if(k >5 && k <= 8 ){
		format_len = k + 2;
//		print_process(format_len, x);
	}
	else if (k == 9)
		format_len = k + 3;
//	else printf("?,???,???.??");
//	int fix;


	if (globle > 0 && globle < 1000000000){
//	if (globle >0){
		for(j =format_len; (14 - j) != 0; j++ ){
				printf(" ");			
		}
//		printf("format_len is %d\t",format_len);
		print_process(format_len, globle);
		printf("  |");
	}
	else if(globle < 0 && abs(globle) < 1000000000){
//	else {
//		globle = abs ( globle );
		printf(" (");
		for(j = format_len; (12 - j) != 0; j++)
			printf(" ");
		print_process(format_len, abs ( globle ));		
//		printf("%s", ((struct unit *)(p->obj))->amount);
		printf(") |");
	}
	else {
		printf("  ?,???,???.??  |");
		
		
	}
	printf("\n");

}

void print_sign(){
	printf("+-----------------+--------------------------+----------------+----------------+\n|       Date      | Description              |         Amount |        Balance |\n+-----------------+--------------------------+----------------+----------------+\n");	
}


void print_line(My402ListElem *p){
//	My402ListElem *p = list -> anchor.next;
	char *s;
	time_t timer;
	
	char date[16];
	char describle[25];
//	describle = malloc(sizeof(char)*23);
//	memset(describle,0,sizeof(describle));
	char buf[26];
	timer =  (time_t )(atoi(((struct unit *)(p ->obj)) -> date));
	s = ctime(&timer);
//	printf( "%s\n",((struct unit *)(p ->obj)) -> date);
	strncpy(buf,s,sizeof(buf));
//	if(sizeof(buf) >=24){
//		buf[23] ='\0';
//	}
//	printf ( "The current local time: %s\n", buf);
	date[0] = buf[0];date[1] = buf[1];date[2] = buf[2];date[3] = buf[3];date[4] = buf[4];date[5] = buf[5];date[6] = buf[6];date[7] = buf[7];date[8] = buf[8];date[9] = buf[9];date[10] = buf[10];date[11] = buf[20];date[12] = buf[21]; date[13] = buf[22];date[14] = buf[23];date[15] = '\0';
	printf("| ");
	printf("%s",date);
	printf(" | ");
	

	strncpy(describle,((struct unit *)(p->obj))->descrition,sizeof(char)*24);
	if(sizeof(describle) >=24){
		describle[24] = '\0';
	}
	printf("%s", describle);
	int i,j,k;
//	char amount_buf[128];
	i = strlen(describle);
//	printf("i is %d\n",i);
//	j = 23 -i;
	if( i<=24 ){
		for (j =25 -i ; j!=0; j-- )
//		j = 23 -i;
		printf(" ");
	}
	printf("|");
	int amount_len = 0;
	amount_len = strlen( ((struct unit *)(p->obj))->amount) ;
	int format_len = 0;
	format_len = amount_len -1;
//	printf("format_len is %d\t",format_len);
	//*(((struct unit *)(p->obj))->amount) = (*(((struct unit *)(p->obj))->amount)) *100;
//	int x = (int)(100*atof( ((struct unit *)(p->obj))->amount) );
	char *decimal = strchr(((struct unit *)(p->obj))->amount,'.');
	if(decimal !=NULL){
		*decimal ++ = '\0';
	}
	int x = atoi(decimal)+100*atoi(((struct unit *)(p->obj))->amount);
//	printf("x is %d\t",x);
//printf("the x is %d",(int)(100*atof( ((struct unit *)(p->obj))->amount)) );
//printf("the double is %f",100*atof( ((struct unit *)(p->obj))->amount) );
	if (amount_len <= 6)
		format_len = amount_len;
	else if(amount_len >6 && amount_len <= 9 ){
		format_len = amount_len + 1;
//		print_process(format_len, x);
	}
	else if (amount_len == 10)
		format_len = amount_len + 2;
	else  format_len = amount_len + 3;

//	 = strlen((struct unit *)(p->obj))->amount);	
	if( strcmp( ((struct unit *)(p->obj))->sign, "+" ) ==0 ){	
		for(k =format_len; (14 - k) > 0; k++ ){
			printf(" ");
//			printf("%s\n", ((struct unit *)(p->obj))->amount);
			
		}
		print_process(format_len, x);
//		printf("%s",amount_buf);
//		printf("%s", ((struct unit *)(p->obj))->amount);
//		snprintf( ((struct unit *)(p->obj))->amount,5,"%s",);
		printf("  |");
	}
	else{
		printf(" (");
		for(k = format_len; (12 - k) > 0; k++)
			printf(" ");
		print_process(format_len, x);		
//		printf("%s", ((struct unit *)(p->obj))->amount);
		printf(") |");
	}
	balance(x,p);
//	globle = globle + x;
//	printf("%d\n",globle);
	

}



void PrintStatement(My402List *list){
	print_sign();
	int i;
	My402ListElem *p = list -> anchor.next;
	for(i = list ->num_members; i!=0 ;i--){
		print_line(p);
		p = p->next;
	}
	printf("+-----------------+--------------------------+----------------+----------------+");	
	printf("\n");
		
}

void check_time(My402List * list){
	int i = list -> num_members;
	int j;
	char * p;
	
	My402ListElem * elem_ptr = list -> anchor.next;
	My402ListElem * copy_ptr;
	for ( ;(i-1)!=0; i-- ){
		for( p= ((struct unit*)(elem_ptr -> obj)) -> date, j =i,copy_ptr = elem_ptr; (j-1)!=0;j--){
			if( strcmp(p,(((struct unit*)(copy_ptr -> next-> obj)) -> date)) ==0 ){
				printf("error: input shouldn't have the same time stamp\n");
				exit(1);
			}
			 copy_ptr = copy_ptr -> next;
		}
	elem_ptr = elem_ptr -> next;
	}	
}

int main( int argc,char *argv[] ){
//	struct unit x;
	if( (argc <2)||(argc >3)||(strcmp(argv[1],"sort") != 0)  ){
		input_error();
	}
	FILE * fp = read_input(argc,argv);
	My402List *list;
	list = (My402List *)malloc(sizeof(My402List));

	My402ListInit(list);
	list = parse(fp,list);
	check_time(list);
//	printf("%d \n",list -> num_members);
//	printf("%s\n",((struct unit *)(list->anchor.next->next->obj))->date );
	if (fp != stdin) fclose(fp);
	list = sortinput(list);
//	printf("%s",((struct unit *)(list->anchor.next->next->obj))->amount);
	PrintStatement(list);

return 0;
}

