#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<signal.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/ip.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include"header"

#define PORT_NO 3001


int main(void)
{

	struct sockaddr_in v,v1;
	int sfd,len,d_len,bk_no=0,option;
	char ch;
	FILE *fp;
	if((sfd=socket(AF_INET,SOCK_DGRAM,0))<0)
	{
		perror("socket:");
		return ;
	}

	v1.sin_family=AF_INET;
	v1.sin_port=htons(PORT_NO);
	v1.sin_addr.s_addr=inet_addr("127.0.0.1");
	len=sizeof(v1);
	printf("\t\t---TFTP SERVER---\n1.read from server\n2.write to server\nchoice? :");
	scanf(" %d",&option);
	if (option==1)
	{
		//RRQ
		u.r1.opcode=1;
		strcpy(u.r1.file,"amal");
		strcpy(u.r1.mode,"netascii");
		sendto(sfd,&u,sizeof(u),0,(struct sockaddr*)&v1,len);
		fp=fopen("amal1","w");
		int flg=1;
		do
		{
			recvfrom(sfd,&u,sizeof(u),0,(struct sockaddr*)&v,&len);
			if(u.r3.opcode!=3)
			{
				printf("error : %s\n",u.r5.errdata);
				return;
			}
			d_len=strlen(u.r3.data);
			printf("%d %d\n",d_len,u.r3.bck_no);
			if(bk_no+1==u.r3.bck_no)
			{
				bk_no=u.r3.bck_no;
				fwrite(u.r3.data,d_len,1,fp);
			}
			u.r4.opcode=4;
			u.r4.bck_no=bk_no;
			if(bk_no==4 && flg==1){
				printf("a\n");	sleep(2);flg=0;}
			sendto(sfd,&u,sizeof(u),0,(struct sockaddr*)&v1,len);
		}while(d_len==512);
		bk_no=0;
		fclose(fp);
	}
	else if(option=2)
	{
		//wrq
		u.r2.opcode=2;
		strcpy(u.r2.file,"amal12");
		strcpy(u.r2.mode,"netascii");
		sendto(sfd,&u,sizeof(u),0,(struct sockaddr*)&v1,len);
		recvfrom(sfd,&u,sizeof(u),0,(struct sockaddr*)&v,&len);
		if(u.r4.opcode==5)
		{
			printf("%d %s\n",u.r5.errno,u.r5.errdata);
			return;
		}
		fp=fopen("amal","r");
		while(fread(u.r3.data,1,512,fp))
		{
			d_len=strlen(u.r3.data);
			u.r3.opcode=3;
			u.r3.bck_no=++bk_no;
			sendto(sfd,&u,sizeof(u),0,(struct sockaddr*)&v1,len);
			recvfrom(sfd,&u,sizeof(u),0,(struct sockaddr*)&v,&len);
			bzero(u.r3.data,sizeof(u.r3.data));
		}
		if(d_len==512)	
		{		
			u.r3.opcode=3;
			u.r3.bck_no=++bk_no;
			sendto(sfd,&u,sizeof(u),0,(struct sockaddr*)&v1,len);
		}
		fclose(fp);
	}
	else
		printf("Invalid choice \n");
}
