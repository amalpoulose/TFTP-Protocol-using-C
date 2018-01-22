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

#define PORT_NO 3000


int main(void)
{

	struct sockaddr_in v,v1;
	int sfd,len,d_len,bk_no=0;
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
	//RRQ
	u.r2.opcode=1;
	strcpy(u.r2.file,"amal");
	strcpy(u.r2.mode,"netascii");
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
			fputs(u.r3.data,fp);
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
