#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/time.h>
#include<sys/select.h>
#include<netinet/ip.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include"header"
#define PORT_NO 3000

int main(void)
{

	struct sockaddr_in v,v1;
	int sfd,len,bk_no=0;
	FILE *fp;
	if((sfd=socket(AF_INET,SOCK_DGRAM,0))<0)
	{
		perror("socket:");
		return ;
	}

	v.sin_family=AF_INET;
	v.sin_port=htons(PORT_NO);
	v.sin_addr.s_addr=inet_addr("0.0.0.0");
	len=sizeof(v);

	if(bind(sfd,(const struct sockaddr*)&v,len)<0)
	{
		perror("bind:");
		return;
	}
	recvfrom(sfd,&u,sizeof(u),0,(struct sockaddr*)&v1,&len);
	printf("%d\n%s",ntohs(v1.sin_port),inet_ntoa(v1.sin_addr));
	
	fd_set s;
	switch(u.r1.opcode)
	{
		case 1: printf("rrq\n ");
			if((fp=fopen(u.r1.file,"r"))==0)
			{
				u.r5.opcode=5;
				u.r5.errno=1;
				strcpy(u.r5.errdata,"File Not Found");
				sendto(sfd,&u,sizeof(u),0,(struct sockaddr*)&v1,len);
				return;
			}
			int i,r,d_len;
			struct timeval t;
			if(strcmp(u.r1.mode,"netascii")==0)
			{	
				while(fread(u.r3.data,1,512,fp))
				{
					u.r3.opcode=3;
					u.r3.bck_no=++bk_no;
					d_len=strlen(u.r3.data);
					//printf("%d   %d  \n%s\n  %d\n",(int)strlen(u.r3.data),i,u.r3.data,bk_no);
			RESEND_DATA:
					printf("%d\n",bk_no);
					sendto(sfd,&u,sizeof(u),0,(struct sockaddr*)&v1,len);
					perror("sendto:");
					t.tv_sec=1;
					t.tv_usec=100;
					FD_ZERO(&s);
					FD_SET(sfd,&s);
					r=select(5,&s,0,0,&t);
					if(FD_ISSET(sfd,&s))
					{
					ACK_WAIT:
						printf("ack\n");
						recvfrom(sfd,&u,sizeof(u),0,(struct sockaddr*)&v1,&len);
						if(u.r4.opcode!=4)
						{
							printf("Error in connection\n");
							return;
						}
						if(u.r4.bck_no<bk_no)
							goto ACK_WAIT;	
					        
					}
					else if(r==0)
					{
						printf("Timeout\n");
						goto RESEND_DATA;
					}
					bzero(u.r3.data,sizeof(u.r3.data));
				}
			}
			//if last block contain 512 characters transmit block contain 0 data
			if(d_len==512)
			{
					u.r3.opcode=3;
					u.r3.bck_no=++bk_no;
					sendto(sfd,&u,sizeof(u),0,(struct sockaddr*)&v1,len);
			}	
			fclose(fp);
			break;
		case 2: printf("wrq \n");
			break;
		default:printf("Error in data\n");
			return ;
	}
}
