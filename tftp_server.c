#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
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
			int i;
			if(strcmp(u.r1.mode,"netascii")==0)
			{	
			while((i=fread(u.r3.data,1,512,fp)))
			{
				u.r3.opcode=3;
				u.r3.bck_no=++bk_no;
				//printf("%d   %d  \n%s\n  %d\n",(int)strlen(u.r3.data),i,u.r3.data,bk_no);
				sendto(sfd,&u,sizeof(u),0,(struct sockaddr*)&v1,len);
				perror("sendto:");
				recvfrom(sfd,&u,sizeof(u),0,(struct sockaddr*)&v1,&len);
				if(u.r4.opcode!=4 || u.r4.bck_no!=bk_no)
				{
					printf("Error in connection\n");
					break;
				}
				bzero(u.r3.data,sizeof(u.r3.data));
			}
			}
		//	printf("%d\n",i);
			fclose(fp);
			break;
		case 2: printf("wrq \n");
			break;
		default:printf("Error in data\n");
			return ;
	}
}
