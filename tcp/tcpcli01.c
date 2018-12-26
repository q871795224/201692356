/* Use standard echo server; baseline measurements for nonblocking version */
#include	"unp.h"
void Str_cli(FILE *fp, int sockfd){
	int maxfdpl,stdineof;
	fd_set rset;
	char buf[MAXLINE];
	int n;
	stdineof = 0;
	FD_ZERO(&rset);
	for(;;){
	    if(stdineof == 0)
		    FD_SET(fileno(fp), &rset);
		FD_SET(sockfd, &rset);
		maxfdpl = max(fileno(fp), sockfd)+1;
		Select(maxfdpl, &rset, NULL,NULL,NULL);
		if(FD_ISSET(sockfd, &rset)){
			if((n=Read(sockfd, buf, MAXLINE))==0){
				if(stdineof == 1)
				    return;
				else
                    err_quit("server was killed");
			}
		    Write(fileno(stdout),buf,n);
		}
		if(FD_ISSET(fileno(fp),&rset)){
			if((n=Read(fileno(fp),buf, MAXLINE))==0){
				stdineof =1;
				Shutdown(sockfd, SHUT_WR);
				FD_CLR(fileno(fp),&rset);
				continue;
			}
			//printf("kaiw\n");
			Writen(sockfd, buf, n);
		}
	}
}
int
main(int argc, char **argv)
{
	int					sockfd;
	struct sockaddr_in	servaddr;
	union{
		short s;
		char bto[sizeof(short)];
	}un;
	int n1;
	char recvbuf[MAXLINE];

	if (argc != 2)
		err_quit("usage: tcpcli <IPaddress>");

	un.s = 0x0102;
	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(9877);
	Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));

	Writen(sockfd,un.bto,sizeof(un.bto));
	if((n1=read(sockfd,recvbuf,MAXLINE))>0){
		fputs(recvbuf,stdout);
	}
	Str_cli(stdin, sockfd);		/* do it all */

	exit(0);
}
