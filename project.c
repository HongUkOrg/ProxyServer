#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<pthread.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<netdb.h>
#include<errno.h>


char *a;
char *b;
char *c;
char type[100];
char length[100];
int ad_block=-1;

char *data;
char *que;
char *buff;
char logo[99999]="";

char log3[99999]="[CLI <== PRX --- SRV]\n";
char log4[99999]="";
int tog=-1;
int tog2=-1;
int pop=-1;
long cl=0;
int co=0;
int mt=-1;
int k=0;
int logoo=-1;
int popup=-1;
int zip=-1;
char *mesg="HTTP/1.1 200 OK\r\nContent-type:text/html;\r\nConnection: close\r\n\r\n<html><script type=\"text/javascript\">alert(\'this site is hacked\');</script></html>";



int argv_port = 0;

pid_t pid;
struct sockaddr_in addr_in,cli_addr,serv_addr;
struct hostent* host;
int sock,client_sock;



struct serverInfo  //runsocket 함수에 인자를 넘겨줄때 필요한 struct
{  
      int client_fd;  
      char ip[100];  
      char port[100];  
};  
 // A thread function  
 // A thread for each client request  
void *runSocket(void *vargp)   //멀티 스레딩을 위한 함수 선언.
{  
char log[99999]="";
char log2[99999]="";
   struct serverInfo *info = (struct serverInfo *)vargp;  
   client_sock=info->client_fd;
   

sprintf(log,"-------------\n%d\n[CLI connected to  %s: %s\n",co,info->ip,info->port);

strcat(log,"[CLI ==> PRX --- SRV]\n");

struct sockaddr_in host_addr;
int flag=0,proxy_sock,n,port=0,i,server_sock,z,x;
char buffer[99999],t1[99999],t2[99999],t3[99999],buf[99999];
char* temp=NULL;
bzero((char*)buffer,99999);
recv(client_sock,buffer,99999,0);




sscanf(buffer,"%s %s %s",t1,t2,t3);


	sprintf(logo,"%s",t2);
	que=strtok(logo,"?");
	que=strtok(NULL,"?\t\n"); //t2는 get뒤에 오는 사용자가 요청한 url
	//이 url을 ?로파싱하여 밑에서 비교
	
		
	if(que!=NULL)
	{
		
		if(strncmp(que,"start_adblock",13)==0)
		{
			ad_block=1;
			
		}
		if(strncmp(que,"start_popup",11)==0) //팝업이 확인됐을 시 pop을 1로 변경
		{
			
			pop=1;
	
		}
		if(strncmp(que,"stop_popup",10)==0)
		{
		
			pop=-1;
	
		}
		
		if(strncmp(que,"logo",4)==0)
		{
			logoo=1;
	
		}
		
	
		
	}
	
	

if(popup!=1)
{
	
		if(strstr(logo,"ad")!=NULL&&ad_block==1)
		{
			strcat(log,"\n\npopup !!!find \n\n");
			//popup=1;
		
		
		}
	
}




if(strncmp(t1,"GET",3)==0)
{

sprintf(logo,"%s",t2);
sprintf(log2," > GET %s\n",t2);
strcpy(buf,t2);
strcat(log,log2);
//printf(" > %s %s\n",t1,t2);
strcpy(t1,t2);

   


if(strncmp(t2,"http",4)==0)
temp=strtok(t2,"//");

port=80;

temp=strtok(NULL,"/");
	





   
sprintf(t2,"%s",temp);
strcpy(log4,t2);


host=gethostbyname(t2);

   
   
strcat(t1,"^]");
temp=strtok(t1,"//");
temp=strtok(NULL,"/");
if(temp!=NULL)
temp=strtok(NULL,"^]");

if(temp!=NULL)
sprintf(log2,"[SRV connected to %s:%d]\n",t2,port);
else
sprintf(log2,"[SRV connected to %s:%d]\n",log4,port);


strcat(log,log2);
sprintf(log2,"[CLI --- PRX ==> SRV]\ncomp is %d mt is %d\n",zip,mt);   
strcat(log,log2);
 
bzero((char*)&host_addr,sizeof(host_addr));
host_addr.sin_port=htons(port);
host_addr.sin_family=AF_INET;
bcopy((char*)host->h_addr,(char*)&host_addr.sin_addr.s_addr,host->h_length);
   
server_sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
proxy_sock=connect(server_sock,(struct sockaddr*)&host_addr,sizeof(struct sockaddr));

if(proxy_sock<0)
perror("Error in connecting to remote server");

bzero((char*)buffer,sizeof(buffer));
if(zip!=1)
{
if(temp!=NULL)
sprintf(buffer,"GET /%s %s\r\nHost: %s\r\nConnection: Close\r\n\r\n",temp,t3,t2);
else
sprintf(buffer,"GET / %s\r\nHost: %s\r\nConnection: Close\r\n\r\n",t3,t2);
}
else
{
if(temp!=NULL)
sprintf(buffer,"GET /%s %s\r\nHost: %s\r\nConnection: Close\r\nAccept-Encoding: gzip\r\n\r\n",temp,t3,t2); //gzip방식을 사용하기 위한 헤더파일의 변경 부분
else
sprintf(buffer,"GET / %s\r\nHost: %s\r\nConnection: Close\r\nAccept-Encoding: gzip\r\n\r\n",t3,t2);
} // zip=1인경우 헤더파일에 accept-encoding을 추가


n=send(server_sock,buffer,strlen(buffer),0);
sprintf(log2," > GET %s\n",buf);
strcat(log,log2);
if(n<0)
perror("error");
else{

tog2=0;
long file_size=0;
do
{
bzero((char*)buffer,99999);
n=recv(server_sock,buffer,99999,0);
sprintf(buf,"%s",buffer);
data=strstr(buffer,"\r\n\r\n");
if(data==NULL)
file_size+=strlen(buffer);
else
file_size+=strlen(data);


if(strncmp(buffer,"HTTP/1.1 200 OK",15)==0)
{

strcat(log,"[CLI --- PRX <== SRV]\n");
sprintf(log2," > 200 OK\n");

strcat(log,log2);

a=strtok(buf,"\r\n");
k=0;
while(strncmp(a,"Content-Type",12)!=0)
{
	

	a=strtok(NULL,"\r\n");
	if(a==NULL) break;
	if(strncmp(a,"Content-Length",14)==0)
	{
		
		strcpy(length,a);
			
	}
	
	


}

b=strtok(a," ;\n\t");
b=strtok(NULL," \t;");
strcpy(type,b);

	
	if(pop==1&&strncmp(type,"text/html",9)==0)
	{

	
	strcat(log,"\npop is loaded\n");
	


	}
	

sprintf(log2," > %s ",b);
strcat(log,log2);

c=strtok(length," \t\n");
if(c!=NULL)
{
c=strtok(NULL," \t\r\n");

strcpy(length,c);
strcat(log,length);
strcat(log,"bytes\n");
strcat(type," ");
strcat(type,length);
strcat(type,"bytes");
}



}
else
{
	if(strncmp(buffer,"HTTP/1.1",8)==0)
	{
	
	strcpy(log4,buffer);
	strcpy(buf,buffer);
	strcat(log,"[CLI --- PRX <== SRV]\n");
	a=strtok(log4,"\r\n"); 
	b=strtok(a," \t");
	b=strtok(NULL,"\r\n");
	sprintf(log4," > %s\n",b);
	strcat(log,log4);
	
	a=strtok(buf,"\r\n");
	
	while(strncmp(a,"Content-Type",12)!=0)
	{
	

	a=strtok(NULL,"\r\n");
	if(a==NULL) break;
	if(strncmp(a,"Content-Length",14)==0)
	{
		
		strcpy(length,a);
		
		
	
	}
		
	}
	
	b=strtok(a," ;\n\t");
	b=strtok(NULL," \t;");
	strcpy(type,b);
	
	sprintf(log2," > %s ",b);
	strcat(log,log2);
	
	

	c=strtok(length," \t\n");
	if(c!=NULL)
	{
	c=strtok(NULL," \t\r\n");
	
	strcpy(length,c);
	strcat(log,length);
	strcat(log,"bytes\n");
	strcat(type," ");
	strcat(type,length);
	strcat(type,"bytes");
	
	}
	
	
	
	
	

	}
	


}
if(tog2==0)
{
	

	if(strncmp(buffer,"HTTP/1.1 200 OK",15)==0)
	{
		strcpy(log3,"[CLI <== PRX --- SRV]\n");
		strcat(log3," > 200 OK\n");

		

		sprintf(log2," > %s ",type);
		strcat(log3,log2);
		tog2=1;
	}
	if(strncmp(buffer,"HTTP/1.1",8)==0&&strncmp(buffer,"HTTP/1.1 200 OK",15)!=0)
	{
		strcpy(log3,"[CLI <== PRX --- SRV]\n");
		strcat(log3,log4);
		strcat(log3,log2);
		
		tog2=1;
	
	}
	
	
	
}



if(n>0)
{


if(popup==5)
{
	popup=-1;
	strcat(log,"\npopup is blocked\n");
	strcpy(buffer,"HTTP/1.1 404 NOT FOUND\r\n\r\n");
	
}




if(pop==1) //팝업을 위해 클라이언트 소켓에 앞서 선언한 mesg를 보내주는 코드
{
send(client_sock,mesg,strlen(mesg),0);
que=strstr(buffer,"\r\n\r\n"); //뒤에 오는 버퍼들은 \r\n\r\n으로 잘라 뒤에 보내줌
 // 이미 mesg에서 헤더파일 선언
sprintf(buffer,"%s",que);

pop=-1;

}

send(client_sock,buffer,n,0);



}

	


}while(n>0);
if(c==NULL)
{
file_size-=4;
sprintf(log2,"%ld",file_size);

strcat(log,log2);
strcat(log,"bytes");
strcat(log,"\n");

strcat(log3,log2);
strcat(log3,"bytes\n");
strcat(log,log3);
}
else
{

	strcat(log,log3);
	strcat(log,"\n");


}


}
}
else if(strncmp(t1,"CONNECT",7)==0) //Connect Method
{
	

	
	int port2=443;
	
	strcpy(log4,buffer);
	a=strtok(log4,"\r\n");
	strcat(log,a);
	strcat(log,"\n");
	
	a=strtok(t2,":");
	b=strtok(NULL," \t");
	sscanf(b,"%d",&port2);
	

	host=gethostbyname(a);
	bzero((char*)&host_addr,sizeof(host_addr));
	host_addr.sin_port=htons(port2);
	host_addr.sin_family=AF_INET;
	bcopy((char*)host->h_addr,(char*)&host_addr.sin_addr.s_addr,host->h_length);
   
	server_sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	proxy_sock=connect(server_sock,(struct sockaddr*)&host_addr,sizeof(struct sockaddr));
	

	if(proxy_sock<0)
	{
	perror("error");
	
	}
	else
	{	strcat(log,"[CLI <== PRX --- SRV]\n");
		strcat(log," > 200 Connection established\n");
		
		strcat(log,"[CLI ==> PRX ==> SRV]\n");
		strcat(log,"[CLI <== PRX <== SRV]\n");
		strcat(log," > Data is secured\n");
		
		sprintf(log4,"HTTP/1.1 200 Connection established\r\n\r\n");
		send(client_sock,log4,strlen(log4),0);
		
	}

	
   
    fd_set fdset;
    int maxp1, r;
    char buf[10000];
  
    
  
    
    if ( client_sock >= server_sock )
        maxp1 = client_sock + 1;
    else
        maxp1 = server_sock + 1;
    
    while(r>=0)
    {
        FD_ZERO( &fdset );
        FD_SET( client_sock, &fdset );
        FD_SET(server_sock, &fdset );
        r = select( maxp1, &fdset, (fd_set*) 0, (fd_set*) 0, NULL );
        if ( r == 0 ) {
        
            
        }
        else if ( FD_ISSET( client_sock, &fdset ) )
        {
            r = recv( client_sock, buf, sizeof( buf ),0 );
            if ( r <= 0 )
                break;
            r = send( server_sock, buf, r,0 );
            if ( r <= 0 )
                break;
        }
        else if ( FD_ISSET( server_sock, &fdset ) )
        {
            r = recv( server_sock, buf, sizeof( buf ),0 );
            if ( r <= 0 )
                break;
            if(popup==5)
            {	strcpy(buf,"HTTP1.1 404 NOT FOUND\r\n\r\n");
            	popup=-1;
            }
            r = send( client_sock, buf, r ,0);
            if ( r <= 0 )
                break;
        }
    }


	


}




else
{
strcat(log,t1);
strcat(log," ");
strcat(log,t2);
strcat(log,"\n");
strcat(log,"ANOTHER METHOD IS NOT IMPLEMENTED\n");
send(client_sock,"400 : BAD REQUEST\n",18,0);
}
close(server_sock);
close(client_sock);
strcat(log,"[CLI disconnected]\n");
strcat(log,"[SRV disconnected]\n");
close(sock);


strcat(log,"-------------------------\n\n\n");
printf("\n%s\n",log);



exit(0);


   return NULL;  
 }  
 
int main(int argc,char* argv[])
{

int i=0;
pthread_t tid; 

   

 

printf("Starting proxy server on %s\n",argv[1]);
if(argv[2]!=NULL)
{
if(argv[2]!=NULL&&strncmp(argv[2],"-comp",5)==0)
{
	zip=1;
}
else if(argv[2]!=NULL&&strncmp(argv[2],"-COMP",5)==0)
{
	zip=1;
}
if(argv[2]!=NULL&&strncmp(argv[2],"-mt",3)==0)
{
	mt=1;
}
else if(argv[2]!=NULL&&strncmp(argv[2],"-MT",3)==0)
{
	mt=1;
}
if(argv[3]!=NULL&&strncmp(argv[3],"-comp",5)==0)
{
	zip=1;
}
else if(argv[3]!=NULL&&strncmp(argv[3],"-COMP",5)==0)
{
	zip=1;
}
if(argv[3]!=NULL&&strncmp(argv[3],"-mt",3)==0)
{
	mt=1;
}
else if(argv[3]!=NULL&&strncmp(argv[3],"-MT",3)==0)
{
	mt=1;
}
}
   
bzero((char*)&serv_addr,sizeof(serv_addr));
bzero((char*)&cli_addr, sizeof(cli_addr));
   
serv_addr.sin_family=AF_INET;
serv_addr.sin_port=htons(atoi(argv[1]));
serv_addr.sin_addr.s_addr=INADDR_ANY;
   
  
sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
if(sock<0)
perror("error");
   
if(bind(sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr))<0)
perror("error");
  
listen(sock,50); 
int clilen=sizeof(cli_addr);
  
 
 


while(1)
{




client_sock=accept(sock,(struct sockaddr*)&cli_addr,&clilen);
   
if(client_sock<0)
{
perror("Problem in accepting connection");
}

else
{
char* req;

if(fork()==0)
{

		//멀티스레딩을 위한 코드. 기존 메인함수에서 모두 돌아가는것을
		//뜯어고쳐, pthread_creat함수를 사용해, 다수의 스레드를 생성
       
                 struct serverInfo *item = malloc(sizeof(struct serverInfo));  
                 item->client_fd = client_sock;  
                 strcpy(item->ip,inet_ntoa(cli_addr.sin_addr));  
                 sprintf(item->port,"%d",ntohs(cli_addr.sin_port)); //struct에 자료를 넣어줌
		 if(mt==1) //멀티스레딩인경우
		 { 
                 pthread_create(&tid, NULL, runSocket, (void *)item);  
                 sleep(1);  
                 }
                 else //멀티스레딩이 아닌경우는 이전과 동일하게 실행
                 {
                 	runSocket(item);
                 
                 }
	
}
else
{

close(client_sock);
co++;


}

}

}
return 0;
}
