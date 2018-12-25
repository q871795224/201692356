/* Use standard echo server; baseline measurements for nonblocking version */
#include	"unp.h"
int bigsmall(){
	union{
		short s;
		char  c[sizeof(short)];
	} un;

	un.s = 0x0102;
	if(sizeof(short)==2){
		if(un.c[0]==1&&un.c[1]==2)
			return 1;
		else if(un.c[0]==2&&un.c[1]==1)
			return 2;
		else
			return -1;
	}
	return -1;
}
int file(int sockfd){
	char		buff[MAXLINE];
	char		filename[MAXLINE] = "hi.c";
	bzero(buff,MAXLINE);
	strncpy(buff,filename,strlen(filename));
	write(sockfd, buff, sizeof(buff));
	FILE *fp = fopen(filename, "r");
	if(fp == NULL){
		printf("file:\t%s can not open",filename);
		return -1;
	}else{
		bzero(buff,MAXLINE);
		int length = 0;
		//while(1){
			length = fread(buff, sizeof(char), MAXLINE, fp);
			printf("aa\n");
			if(length > 0){
				printf("length = %d,buf = %s\n",length,buff);
				int k = write(sockfd,buff,length);	
				if(k < 0){
					printf("send file is failed");
					return -1;
				}
				printf("%d\n",k);
				//bzero(buff,MAXLINE);
			//}else if(length == 0){
				//printf("no char\n");
				//break;
			}	
		//}
		//printf("file transfer is finished");
		bzero(buff,MAXLINE);
		sleep(1);
		sprintf(buff,"transform is finish\n");
		Write(sockfd,buff,strlen(buff));
	}
	fclose(fp);
	return 1;
}
void Str_cli(FILE *fp, int sockfd){
	int			maxfdpl,stdineof;
	fd_set		rset;
	//char		sendline[MAXLINE], recvline[MAXLINE];
	int			n;
	char		buf[MAXLINE];
	
	stdineof = 0;
	FD_ZERO(&rset);
	for(;;){
		if(stdineof==0)
			FD_SET(fileno(fp),&rset);
		FD_SET(sockfd,&rset);
		maxfdpl = max(fileno(fp),sockfd)+1;
		Select(maxfdpl, &rset, NULL, NULL, NULL);

		if(FD_ISSET(sockfd, &rset)){
			if((n=Read(sockfd,buf,MAXLINE))==0){
				if(stdineof==1)
					return;
				else
					err_quit("str_cli:server is killed");
			}
			Write(fileno(stdout),buf,n);
		}
		if(FD_ISSET(fileno(fp),&rset)){
			if((n=Read(fileno(fp),buf,MAXLINE))==0){
				stdineof = 1;
				Shutdown(sockfd,SHUT_WR);
				FD_CLR(fileno(fp),&rset);
				continue;
			}
			if(buf[0]=='F'){
				printf("began transfrom\n");
				bzero(buf,MAXLINE);
				if(file(sockfd))
					printf("transform finished\n");
			}else if(buf[0]=='-'){
				int k = bigsmall();
				sprintf(buf,"- %d",k);
				Writen(sockfd,buf,strlen(buf));
			}
			else
				Writen(sockfd,buf,n);
		}

	}

}

int
main(int argc, char **argv)
{
	int					sockfd;
	struct sockaddr_in	servaddr;

	if (argc != 2)
		err_quit("usage: tcpcli <IPaddress>");

	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(9877);
	Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));

	Str_cli(stdin, sockfd);		/* do it all */

	exit(0);
}
