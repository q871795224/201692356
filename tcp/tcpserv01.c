#include	"unp.h"
#include "stdlib.h"
void
Str_echo(int sockfd)
{
	ssize_t		n;
	char		buf[MAXLINE];
	int k1,k2,k3,i;
	double num;
	char cs[100];
	//printf("ook\n");

again:
	while ( (n = read(sockfd, buf, MAXLINE)) > 0){
		int n1,n2;
		sscanf(buf,"%d %d", &n1,&n2);
		k1=n1+n2;
		k2=n1-n2;
		k3=n1*n2;
		i=sprintf(cs,"%d %d %d ",k1,k2,k3);
		if(n2 == 0){
			sprintf(cs+i, "0 get not be divisor\n");
			
		}else{
		    num =(double) n1/n2;
                
			sprintf(cs+i, "%lf\n", num);
		
		
		}
		sleep(2);
		Writen(sockfd, cs, strlen(cs));
		
    }
	if (n < 0 && errno == EINTR)
		goto again;
	else if (n < 0)
		err_sys("str_echo: read error");
}

char*
byteorder(char *c)
{
	char buf[MAXLINE];

	
	//printf("%s: ", CPU_VENDOR_OS);
	if (sizeof(short) == 2) {
		if (c[0] == 1 && c[1] == 2)
			sprintf(buf,"big-endian. ");
		else if (c[0] == 2 && c[1] == 1)
			sprintf(buf,"little-endian. ");
		else
			sprintf(buf,"unknown. ");
	} else{
		int i = sizeof(short);
		sprintf(buf,"sizeof(short) =%d . ",i);

	}
	return buf;
}

int
main(int argc, char **argv)
{
	int					listenfd, connfd;
	pid_t				childpid;
	socklen_t			clilen;
	struct sockaddr_in	cliaddr, servaddr;
    union {
	  short  s;
      char   c[sizeof(short)];
    } un;
	char obuf[MAXLINE];
	char buff[MAXLINE];
	int n1;

	un.s = 0x0102;
	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(SERV_PORT);

	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);
	printf("%s\n",un.c);
	printf("%s\n",byteorder(un.c));
	for ( ; ; ) {
		clilen = sizeof(cliaddr);
		connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);
		if((n1=read(connfd,obuf,MAXLINE))>0){
			int i,j,k,l;
			i=sprintf(buff,"Server type is ");
			j=sprintf(buff+i,byteorder(un.c));
			k=sprintf(buff+i+j,"Client type is ");
			l=sprintf(buff+i+j+k,byteorder(obuf));
			sprintf(buff+i+j+k+l,"\n");
			Writen(connfd,buff,sizeof(buff));
		}
		//printf("okok\n");
		if ( (childpid = Fork()) == 0) {	/* child process */
			//printf("okk\n");
			Close(listenfd);	/* close listening socket */
			Str_echo(connfd);	/* process the request */
			exit(0);
		}
		Close(connfd);			/* parent closes connected socket */
	}
}
