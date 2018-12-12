#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>



int main()
{
	int srvfd;
	int cltfd;
	int reuseon=1;
	struct sockaddr_in srvaddr;
	struct sockaddr_in cltaddr;
	socklen_t cltaddr_len;
	
	
	char cltip[INET_ADDRSTRLEN];
	char portnr[16];
	
	if ((srvfd=socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("Err creating socket\n");
		return -1;
	}
	
	if (setsockopt(srvfd, SOL_SOCKET, SO_REUSEADDR, &reuseon, sizeof(int)) < 0)
	{
		printf("cannot set sock reuse\n");
		return -1;
	}
	
	memset (&srvaddr, 0, sizeof(srvaddr));
	
	srvaddr.sin_family=AF_INET;
	
	//////////////////////////////////
	// default address
	// port 9088
	/////////////////////////////////
	
	
	
	//srvaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	srvaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	srvaddr.sin_port = htons(9088);
	
	if (bind(srvfd, (struct sockaddr*)&srvaddr, sizeof(srvaddr))<0)
	{
		printf("bind error\n");
		return -1;
	}
	
	//backlog 10
	if (listen(srvfd, 10)<0)
	{
		printf("cannot let kernel listen\n");
		return -1;
	}
	
	
	//test version no daemon
	//no fork
	
	//future 2 times fork
	cltaddr_len=sizeof(cltaddr);
	int ertime=0;
	while(1)
	{
		if ((cltfd=accept(srvfd, (struct sockaddr*)&cltaddr, &cltaddr_len))<0)
		{
			printf("accept failed\n");
			ertime++;
			if(ertime>=3000)
			{
				printf("continue to happen\n");
				return -1;
			}
			continue;
		}
		
		
		//get ip
		if (inet_ntop(AF_INET, &(cltaddr.sin_addr), cltip, INET_ADDRSTRLEN)==0)
		{
			printf("get ip error\n");
			close(cltfd);
			ertime++;
			if(ertime>=3000)
			{
				printf("continue to happen(ip))\n");
				return -1;
			}
			continue;
		}
		
		
		//get port
		sprintf(portnr, "%d", ntohs(cltaddr.sin_port));
		
		
		
		//send "ip:"
		if(send(cltfd, "ip:", 3, 0)<0)
		{
			printf("send err 1\n");
			close(cltfd);
			ertime++;
			if(ertime>=3000)
			{
				printf("continue to happen(send1))\n");
				return -1;
			}
			continue;
		}
		
		//send ip 
		if(send(cltfd, cltip, strlen(cltip), 0)<0)
		{
			printf("send err 2\n");
			close(cltfd);
			ertime++;
			if(ertime>=3000)
			{
				printf("continue to happen(send2))\n");
				return -1;
			}
			continue;
		}
		
		
		//send \r\n
		if(send(cltfd, "\r\n", 2, 0)<0)
		{
			printf("send err 3\n");
			close(cltfd);
			ertime++;
			if(ertime>=3000)
			{
				printf("continue to happen(send3))\n");
				return -1;
			}
			continue;
		}
		
		
		
		
		//send "port:"
		if(send(cltfd, "port:", 5, 0)<0)
		{
			printf("send err 1\n");
			close(cltfd);
			ertime++;
			if(ertime>=3000)
			{
				printf("continue to happen(send1))\n");
				return -1;
			}
			continue;
		}
		
		//send port 
		if(send(cltfd, portnr, strlen(portnr), 0)<0)
		{
			printf("send err 2\n");
			close(cltfd);
			ertime++;
			if(ertime>=3000)
			{
				printf("continue to happen(send2))\n");
				return -1;
			}
			continue;
		}
		
		
		//send \r\n
		if(send(cltfd, "\r\n", 2, 0)<0)
		{
			printf("send err 3\n");
			close(cltfd);
			ertime++;
			if(ertime>=3000)
			{
				printf("continue to happen(send3))\n");
				return -1;
			}
			continue;
		}
		
		//char str[] = "FUCk \r\n";
    	//write(cltfd, str, sizeof(str)+1);
		ertime=0;
		close(cltfd);
	}
	
	
	
}

