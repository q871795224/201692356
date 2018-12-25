#include	"unp.h"
#include	<math.h>
#include    <stdlib.h>
#include	<string.h>
#define maxsize 1000
#define MAX_CLIENT 2


int bigsmall(int sockfd, int k){
	union{
		short s;
		char  c[sizeof(short)];
	} un;
	char buff[MAXLINE];
	int i=0;

	un.s = 0x0102;
	if(sizeof(short)==2){
		if(un.c[0]==1&&un.c[1]==2)
			i=1;
		else if(un.c[0]==2&&un.c[1]==1)
			i=2;
		else
			return -1;
	}
	if(i==k){
		if(k==1){
			sprintf(buff,"we are the same, both big-endian\n");
		}else if(k==2){
			sprintf(buff,"we are the same, both small-endian\n");	
		}else{
			sprintf(buff,"oh no, error!!!!!!!!\n");
			return -1;
		}
	}else if((i+k)==3){
		if(k==1){
			sprintf(buff,"we are different, you big, I small\n");
		}else if(k==2){
			sprintf(buff,"we are different, you small, I big!!\n");
		}else{
			sprintf(buff,"oh no, error!!!!!!!!\n");
			return -1;
		}
	}else{
		sprintf(buff,"nonononono,error!\n");
		return -1;
	}
	Writen(sockfd,buff,strlen(buff));
	bzero(buff,MAXLINE);
	return 1;
}
int file(int sockfd){
	char		filename[MAXLINE];
	char		buff[MAXLINE];
	char		fin[MAXLINE]="transform is finish\n";	
	bzero(filename,MAXLINE);
	bzero(buff,MAXLINE);
	int length = read(sockfd, buff, MAXLINE);
	if(length < 0){
		printf("read file gg\n");
		return -1;
	}
	printf("file:%s\n",buff);
	strncpy(filename,buff,strlen(buff));
	printf("buf:%s\n",filename);
	FILE *fp = fopen(filename, "w");
	if(fp == NULL){
		printf("file:\t%s can not open to write\n",filename);
		return -1;
	}
	bzero(buff,MAXLINE);
	while(1){
		length = read(sockfd, buff, MAXLINE);
		printf("length:%d, buff:%s\n",length,buff);
		if(length < 0){
			printf("receive data from client gg\n");
			return -1;
		}
		if(length == 20&&!strcmp(buff,fin)){
			printf("%lu %d",strlen(fin),strcmp(buff,fin));
			break;
		}
		int write_length = fwrite(buff,sizeof(char),length,fp);
		printf("write_length:%d\n",write_length);
		if(write_length < length){
			printf("file:%s write failed\n",filename);
			return -1;
		}
		bzero(buff,MAXLINE);
	}
	printf("receive file:%s finished",filename);
	fclose(fp);
	return 1;
}
int Str_echo(int sockfd){
	ssize_t		n;
	char		buf[MAXLINE];
	
	while( (n = read(sockfd, buf, MAXLINE)) > 0){
		if(buf[0]=='Q')
			return 1;
		printf("%s",buf);
		Writen(sockfd, buf, n);
	}
	return 0;
}

typedef struct{
     char data[1000];
     int t;
}sStack;

typedef struct{
     char pExp[200];
     char iExp[100];
     int  flag;
	 double n;
}Cal;
int get_pri(char c);
double calL(double num1,double num2,char op,Cal *c);
double Cal_cal(Cal *cal,char *iE);
double read_num( char* s,int *pos);
double Cal_cal_p(Cal *cal);
int Cal_i_to_p(Cal *cal);
int cal(int sockfd,char buf[]){
    
	ssize_t		n;
	char		tishi[MAXLINE];
	Cal c;
    char		cc[MAXLINE]; 
again:
	while((n = read(sockfd, buf, MAXLINE)) > 0){
		if(buf[0]=='Q')
			return 1;
		bzero(cc,MAXLINE);
		strncpy(cc,buf,n-1);
		Cal_cal(&c,cc);
		if(c.flag ==1){
			printf("1\n");
			sprintf(tishi,"sorry, such operations are not supported now\n");
		}else if(c.flag == -1){
			sprintf(tishi,"sorry, 0 can not be the denominator\n");
			printf("2\n");
		}else{
			sprintf(tishi,"%lf\n",c.n);
		}
		c.flag =0;
		writen(sockfd, tishi, strlen(tishi));
		FILE *fp = fopen("log.txt","a+");
		fprintf(fp,"%s = %s",cc,tishi);
		fclose(fp);
		bzero(tishi,MAXLINE);
		
	}
	if (n < 0 && errno == EINTR)
		goto again;
	else if (n < 0)
		err_sys("str_echo: read error");
	return 0;
}
int
main(int argc, char **argv)
{
	int					i,k, listenfd, connfd, sockfd, maxfd, maxi;
	int					nready, client[MAX_CLIENT], retval;
	char				choose;
	ssize_t				n;
	fd_set				rset,allset;
	socklen_t			clilen;
	struct sockaddr_in	cliaddr, servaddr;
	struct hostent		*host;
	struct in_addr		addr;
	time_t				tick;
	char				Time[MAXLINE];
	char				IP[MAXLINE];
	char				buff[MAXLINE];
	char				connection[MAXLINE];

	struct timeval		tv;
	fd_set				rfds;	
	char try[]="plz try again\n";
	char back[] = "already back\n";	
	char close[] = "your server is closed! byebye~\n";	
	char timeout[] = "already 10s, your server is closed! byebye\n";	
	char tishi_p[] = "what you input, what we output. unless input Q\n";
	char tishi_c[] = "we can calculate,input Q to leave\n";
	char tishi_s[] = "this server will be over in 10s," 
						" unless input spmething\n";
	char usage[MAXLINE]="p.print\nc.calculate\nf.file\ns.sleep\nb.byebye\n"
						"-.big-endian or small-endian\n";
	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(SERV_PORT);

	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

	maxfd = listenfd;
	maxi = -1;
	for(i = 0;i < MAX_CLIENT;i++)
		client[i] = -1;
	FD_ZERO(&allset);
	FD_SET(listenfd,&allset);
	for ( ; ; ) {
		rset = allset;
		nready = Select(maxfd+1, &rset, NULL, NULL, NULL);
		if(FD_ISSET(listenfd, &rset)){
			
			clilen = sizeof(cliaddr);
			connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);
			tick = time(NULL);
			//print ip and port
			addr = cliaddr.sin_addr;
			host = gethostbyaddr((void*)&addr, sizeof(addr), AF_INET);
			printf("new client!\nconnection from %s, port %d, host %s\n",
				Inet_ntop(AF_INET,&addr,IP,sizeof(IP)),
				ntohs(cliaddr.sin_port),host->h_name);
			
			for(i = 0;i < MAX_CLIENT;i++){
				if(client[i] < 0){
					client[i] = connfd; //save descriptor
					break;
				}
			}
			if(i==2){
				printf("what a pity\nThere are too many clients\n");
				char refuse[] = 
					"what a pity\nserver is busy, please come here later\n";
				Writen(connfd,refuse,sizeof(refuse));
				Close(connfd);
				continue;
			}
			FD_SET(connfd,&allset);		//add new descriptor to set
						
			//prompt message
			sprintf(connection,"Congratulations!\nsuccessful connection\n");
			Writen(connfd,connection,sizeof(connection));
			
			//write time to client	
			snprintf(Time,sizeof(Time),"%.24s\r\n",ctime(&tick));
			Write(connfd,Time,strlen(Time));
				
			
			//print usage
			//sprintf(usage,"p.print\n""c.calculate\n""f.file\n" "s.sleep\n"
			//				"b.byebye\n");
			Write(connfd,usage,strlen(usage));


			if(connfd > maxfd)
				maxfd = connfd;			//for select
			if(i > maxi)
				maxi = i;				//max index in clent[] array
			if(--nready <= 0)
				continue;				//no more readable descriptors
			
		}
		for(i = 0; i <= maxi; i++){		//check all clients for data
			if((sockfd = client[i]) < 0)
				continue;
			if(FD_ISSET(sockfd, &rset)){
				n = Read(sockfd, buff, MAXLINE);
				if(n  == 0){
					Close(sockfd);		//connection closed by client
					FD_CLR(sockfd, &allset);
					client[i] = -1;
				}else{	
					sscanf(buff,"%c",&choose);
					printf("%c\n",choose);
					switch(choose){
						case 'p':
							printf("1\n");
							Write(connfd,tishi_p,strlen(tishi_p));
							retval = Str_echo(sockfd);
							if(retval){
								Write(connfd,back,strlen(back));
								Write(connfd,usage,strlen(usage));
							}
							printf("ok\n");
							break;
						case 'c':
							printf("2\n");
							Write(connfd,tishi_c,strlen(tishi_c));
							if(cal(sockfd,buff)==1){
								Write(connfd,back,strlen(back));
								Write(connfd,usage,strlen(usage));
							}
							break;
						case 'f':
							if(file(sockfd)){
								Write(connfd,back,strlen(back));
								Write(connfd,usage,strlen(usage));	
								
							}
							break;
						case 's':
							FD_ZERO(&rfds);
							FD_SET(sockfd,&rfds);
							tv.tv_sec = 10;
							tv.tv_usec = 0;
							Write(connfd,tishi_s,strlen(tishi_s));
							for(;;){
								retval=select(maxfd+1,&rfds,NULL,NULL,&tv);
								if(retval == -1){
									printf("sleep error\n");
									break;
								}else if(retval){
									Write(connfd,back,strlen(back));
									Write(connfd,usage,strlen(usage));
									break;	
								}else{
									Writen(sockfd,timeout,sizeof(timeout));
									Close(sockfd);
									FD_CLR(sockfd,&allset);
									client[i] = -1;
									break;
								}
							}
							break;
						case 'b':
							Writen(sockfd,close,sizeof(close));
							Close(sockfd);		
							FD_CLR(sockfd, &allset);
							client[i] = -1;
							break;
						case '-':
							//big-endian or little-endian
							sscanf(buff,"%c %d",&choose,&k);
							if(bigsmall(sockfd,k)){
								Write(connfd,usage,strlen(usage));
								sleep(1);
								continue;
							}
						default: 
							Writen(sockfd,try,strlen(try));
							printf("0\n");
							break; 
					}
				}

				if(--nready <= 0)
					break;				//no more readable descriptors	
				
			}
		}
		
	}
}

typedef struct{
     double data[1000];
     int t;
}dStack;
void dstk_create(dStack *dstk){
    dstk->t=0;
    dstk->data[0]=0;
}
void dstk_clear(dStack *dstk){
    dstk->t=0;
}
double dstk_pop(dStack *dstk){
    if (dstk->t<=0) {
        return dstk->data[0];
    }
    dstk->t--;
    return dstk->data[dstk->t];
}
void dstk_push(dStack *dstk ,double x){
    if(dstk->t>=1000){
        return;
    }
    dstk->data[dstk->t++]=x;
}
double dstk_top(dStack *dstk){
    if(dstk->t<=0){
        return dstk->data[0];
    }
        return dstk->data[dstk->t-1];
    }
int dstk_is_empty(dStack *dstk){
    if(dstk->t<=0) return 1;
    return 0;
}

void sstk_create(sStack *sstk){
    sstk->t=0;
    sstk->data[0]=0;
}
void sstk_clear(sStack *sstk){
    sstk->t=0;
}
char sstk_pop(sStack *sstk){
    if (sstk->t<=0) {
        return sstk->data[0];
    }
    sstk->t--;
    return sstk->data[sstk->t];
}
void sstk_push(sStack *sstk ,char x){
    if(sstk->t>=1000){
        return;
    }
    sstk->data[sstk->t++]=x;
}
char sstk_top(sStack *sstk){
    if(sstk->t<=0){
        return sstk->data[0];
    }
        return sstk->data[sstk->t-1];
    }
int sstk_is_empty(sStack *sstk){
    if(sstk->t<=0) return 1;
    return 0;
}


double Cal_cal_p(Cal *cal);
void Cal_create(Cal *cal,char *s){
    strcpy(cal->iExp,s);
    cal->n=0;
}

double Cal_cal(Cal *cal,char *iE){
    strcpy(cal->iExp, iE);
    if (Cal_i_to_p(cal)){
        return cal->n=Cal_cal_p(cal);
    }else{
        return 0.0;
    }
}
double read_num( char* s,int *pos)
{
    double x=0;
    int flag=0,w=0,h=1,i;
    if(s[(*pos)]=='-') {(*pos)++;h=-1;}
    for ( i=(*pos);i<strlen(s);i++)
    if (s[i]!='#')
        if(s[i]=='.'){
            flag=1;
        }else {
            if(flag) w++;
            x=x*10+s[i]-'0';
    }else  break;
    (*pos)=i;
    return x/pow(10,w)*h;
}
double calL(double num1,double num2,char op,Cal *c)
{
    switch(op){
		case '+':
			return num1+num2;
		case '-':
			return num1-num2;
		case '*':
			return num1*num2;
		case '/':
			if(num2==0)
			{
				c->flag = -1;
				break;
			}else
				return num1/num2;
		default:
			return 0.0;
    }
	return 0;
}

int get_pri(char c){
    switch(c){
      case '+':case '-': return 1;
      case '*':case '/': return 2;
      case '(':return 0;
      case ')':return 0;
      case '#':return -2;
      default:
        return -1;
    }
}
int Cal_i_to_p(Cal *cal)
{
    sStack stk; 
    sstk_create(&stk);
    int a_num_end=0,is_negative=0;
    int i;
    char result[200];
    strcat(result,"#");
    sstk_push(&stk,'#');
    for (i=0;i<strlen(cal->iExp);i++)
    {
        char c = cal->iExp[i];
        if(c=='-'&&(i==0||cal->iExp[i-1]=='(')) {strcat(result,"-"); is_negative=1;continue;}
        if((c<='9'&&c>='0')||c=='.') {
                if(a_num_end&&!is_negative) {strcat(result,"#"); }
                is_negative=0;
                char cc[10];
                cc[0]=c;
                strncat(result,cc,1);
                a_num_end=0;
        }
        else{a_num_end=1;
            if(c=='(') sstk_push(&stk,'(');
            else if (c==')') {
                while (sstk_top(&stk)!='(')
                {
                    strcat(result,"#");
                    char cc = sstk_pop(&stk);
                    strncat(result,&cc,1);
                }
                sstk_pop(&stk);
            }

            else {
             int p = get_pri(c);
             if (p==-1) return 0;
             while (p<=get_pri(sstk_top(&stk))){
                strcat(result,"#");
                char cc = sstk_pop(&stk);
                strncat(result,&cc,1);
             }
             sstk_push(&stk,c);
        }
        }
    }
    while (sstk_top(&stk)!='#'){
        strcat(result,"#");
        char cc = sstk_pop(&stk);
        strncat(result,&cc,1);
    }
    strcat(result,"#");
    strcpy(cal->pExp , result);
    return 1;
}
double Cal_cal_p(Cal *cal)
{
    int l = strlen(cal->pExp);
    dStack stk1;
    dstk_create(&stk1);
    int i;
    for ( i=1;i<l;i++){
        char c = cal->pExp[i];
        if (c=='#') continue;
        if((c=='-'&&cal->pExp[i+1]!='#')||(c>='0'&&c<='9')) dstk_push(&stk1,read_num(cal->pExp,&i));
        else {
           double y  = dstk_pop(&stk1);
           double x = dstk_pop(&stk1);
		   calL(x,y,c,cal);
		   if(cal->flag == 1){
			   return dstk_pop(&stk1);
		   }else if(cal->flag == -1){
			   return dstk_pop(&stk1);
		   }
           dstk_push(&stk1,calL(x,y,c,cal));
        }
    }
   return dstk_pop(&stk1);

}

