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

// Added boolean type
typedef int bool;
enum { false, true };

const char* CONTENT_TYPE = "Content-Type";
const char* CONTENT_LENGTH = "Content-Length";

const char* METHOD_POST = "POST";
const char* METHOD_GET = "GET";
const char* METHOD_CONNECT = "CONNECT";


const char* SERVER_RESPONSE_OK = "HTTP/1.1 200 OK";
const char* SERVER_RESPONSE_NOT_FOUND = "HTTP/1.1 404 NOT FOUND\r\n\r\n";

const char* MESSAGE_PROXTY_TO_CLIENT = "[CLI <== PRX --- SRV]\n";
const char* MESSAGE_PROXTY_TO_SERVER = "[CLI --- PRX ==> SRV]\n";
const char* MESSAGE_CLIENT_TO_PROXY = "[CLI ==> PRX --- SRV]\n";
const char* MESSAGE_SERVER_TO_PROXY = "[CLI --- PRX <== SRV]\n";

const char* CLIENT_DISCONNECTED = "[CLI disconnected]\n";
const char* SERVER_DISCONNECTED = "[SRV disconnected]\n";

char *a, *b, *c;
char type[100];
char length[100];

char *data, *parameter, *buff;
char parameters[99999] = "";

char log3[99999] = "[CLI <== PRX --- SRV]\n";
char log4[99999] = "";

int tog = -1, tog2 = -1;
int packetCount = 0;
int k = 0;
long cl=0;

bool startPopup = false;
bool blockAD = false;
bool enableMultiThreading = false;
bool showPopup = false;
bool enableComppresion = false;

int httpsPortNumber = 443;
int httpPortNumber = 80;

char *mesg = "HTTP/1.1 200 OK\r\nContent-type:text/html;\r\nConnection: close\r\n\r\n<html><script type=\"text/javascript\">alert(\'This site is hacked\');</script></html>";

struct sockaddr_in addr_in, cli_addr, serv_addr;
struct hostent* host;

// Sockets
int sock,client_sock;
pid_t pid;


// Server Information Model
struct serverInfo {
    int client_fd;
    char ip[100];
    char port[100];
};

// A thread function
// A thread for each client request
void *runSocket(void *vargp) {
    
    char log[99999] = "";
    char additionalLog[99999] = "";
    
    struct serverInfo *info = (struct serverInfo *)vargp;
    client_sock = info -> client_fd;
    
    sprintf(log, "-------------\n%d\n[CLI connected to  %s: %s\n", packetCount, info -> ip, info -> port);
    strcat(log, MESSAGE_CLIENT_TO_PROXY);
    
    struct sockaddr_in host_addr;
    int port = 0;
    int proxy_sock, server_sock;
    int n;
    
    char* temp = NULL;
    char buffer[99999], requestMethod[99999], requestUrl[99999], t3[99999], buf[99999];
    
    bzero((char*)buffer,99999);
    
    // receive from client socket
    recv(client_sock, buffer, 99999, 0);
    
    sscanf(buffer,"%s %s %s", requestMethod, requestUrl, t3);
    
    sprintf(parameters, "%s", requestUrl);
    
    // check parameters
    parameter = strtok(parameters, "?");
    parameter = strtok(NULL, "?\t\n");
    
    if(parameter != NULL) {
        if(strncmp(parameter, "start_adblock", 13) == 0) {
            blockAD = true;
        }
        if(strncmp(parameter, "start_popup", 11) == 0) {
            startPopup = true;
        }
        if(strncmp(parameter, "stop_popup", 10) == 0) {
            startPopup = true;
        }
    }
    
    if(showPopup == false) {
        if(strstr(parameters,"ad") != NULL && blockAD == true) {
            strcat(log,"\nPopup found!\n");
        }
    }
    
    if(strncmp(requestMethod, METHOD_GET, 3) == 0) {
        
        sprintf(parameters, "%s", requestUrl);
        sprintf(additionalLog, " > GET %s\n",requestUrl);
        strcpy(buf, requestUrl);
        strcat(log, additionalLog);
        //printf(" > %s %s\n",t1,t2);
        strcpy(requestMethod,requestUrl);
        
        
        if(strncmp(requestUrl, "http", 4) == 0)
            temp=strtok(requestUrl, "//");
        
        port = httpPortNumber;
        temp = strtok(NULL, "/");
        
        sprintf(requestUrl,"%s", temp);
        strcpy(log4, requestUrl);
        
        host = gethostbyname(requestUrl);
        
        strcat(requestMethod, "^]");
        temp=strtok(requestMethod, "//");
        temp=strtok(NULL,"/");
        if(temp != NULL)
            temp=strtok(NULL,"^]");
        
        if(temp != NULL)
            sprintf(additionalLog, "[SRV connected to %s:%d]\n", requestUrl, port);
        else
            sprintf(additionalLog, "[SRV connected to %s:%d]\n", log4, port);
        
        
        strcat(log, additionalLog);
        sprintf(additionalLog, "%scomp is %d mt is %d\n", MESSAGE_PROXTY_TO_SERVER, enableComppresion, enableMultiThreading);
        strcat(log, additionalLog);
        
        bzero((char*)&host_addr, sizeof(host_addr));
        host_addr.sin_port = htons(port);
        host_addr.sin_family = AF_INET;
        bcopy((char*)host -> h_addr, (char*)&host_addr.sin_addr.s_addr, host -> h_length);
        
        server_sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
        proxy_sock = connect(server_sock, (struct sockaddr*)&host_addr, sizeof(struct sockaddr));
        
        if(proxy_sock < 0)
            perror("Error in connecting to remote server");
        
        bzero((char*)buffer, sizeof(buffer));
        if(enableComppresion == false) {
            if(temp!=NULL)
                sprintf(buffer, "GET /%s %s\r\nHost: %s\r\nConnection: Close\r\n\r\n", temp,t3, requestUrl);
            else
                sprintf(buffer, "GET / %s\r\nHost: %s\r\nConnection: Close\r\n\r\n", t3, requestUrl);
        } else {
            if(temp != NULL)
                sprintf(buffer, "GET /%s %s\r\nHost: %s\r\nConnection: Close\r\nAccept-Encoding: gzip\r\n\r\n",temp,t3,requestUrl);
            else
                sprintf(buffer, "GET / %s\r\nHost: %s\r\nConnection: Close\r\nAccept-Encoding: gzip\r\n\r\n", t3, requestUrl);
        }
        
        n = send(server_sock, buffer, strlen(buffer), 0);
        sprintf(additionalLog, " > GET %s\n", buf);
        strcat(log, additionalLog);
        
        if(n < 0)
            perror("error");
        else {
            
            tog2 = 0;
            long file_size=0;
            
            do {
                bzero((char*)buffer, 99999);
                n = recv(server_sock, buffer, 99999, 0);
                sprintf(buf, "%s", buffer);
                
                // file size
                data = strstr(buffer, "\r\n\r\n");
                if(data == NULL)
                    file_size += strlen(buffer);
                else
                    file_size += strlen(data);
                
                if(strncmp(buffer, SERVER_RESPONSE_OK, 15)==0) {
                    
                    strcat(log, MESSAGE_SERVER_TO_PROXY);
                    sprintf(additionalLog," > 200 OK\n");
                    strcat(log,additionalLog);
                    
                    a=strtok(buf,"\r\n");
                    k = 0;
                    while(strncmp(a, CONTENT_TYPE, 12) != 0) {
                        
                        a=strtok(NULL,"\r\n");
                        if(a==NULL) break;
                        if(strncmp(a, CONTENT_LENGTH, 14) == 0) {
                            strcpy(length,a);
                        }
                    }
                    
                    b=strtok(a," ;\n\t");
                    b=strtok(NULL," \t;");
                    strcpy(type,b);
                    
                    if(startPopup==1&&strncmp(type, "text/html", 9)==0) {
                        strcat(log,"\npop is loaded\n");
                    }
                    
                    sprintf(additionalLog," > %s ",b);
                    strcat(log,additionalLog);
                    
                    c = strtok(length," \t\n");
                    if(c != NULL) {
                        c=strtok(NULL," \t\r\n");
                        
                        strcpy(length, c);
                        strcat(log, length);
                        strcat(log, "bytes\n");
                        strcat(type, " ");
                        strcat(type, length);
                        strcat(type, "bytes");
                    }
                } else {
                    if(strncmp(buffer, "HTTP/1.1", 8) == 0) {
                        
                        strcpy(log4, buffer);
                        strcpy(buf, buffer);
                        strcat(log, MESSAGE_SERVER_TO_PROXY);
                        a = strtok(log4, "\r\n");
                        b = strtok(a, " \t");
                        b = strtok(NULL, "\r\n");
                        sprintf(log4, " > %s\n",b);
                        strcat(log, log4);
                        
                        a = strtok(buf, "\r\n");
                        
                        while(strncmp(a, CONTENT_TYPE, 12) != 0) {
                            
                            a=strtok(NULL,"\r\n");
                            if(a == NULL) break;
                            
                            if(strncmp(a, CONTENT_LENGTH, 14) == 0) {
                                strcpy(length,a);
                            }
                        }
                        
                        b=strtok(a, " ;\n\t");
                        b=strtok(NULL, " \t;");
                        strcpy(type, b);
                        
                        sprintf(additionalLog, " > %s ",b);
                        strcat(log, additionalLog);
                        
                        c=strtok(length, " \t\n");
                        if(c != NULL) {
                            c=strtok(NULL, " \t\r\n");
                            
                            strcpy(length, c);
                            strcat(log, length);
                            strcat(log, "bytes\n");
                            strcat(type, " ");
                            strcat(type, length);
                            strcat(type, "bytes");
                        }
                    }
                }
                if(tog2 == 0) {
                    
                    if(strncmp(buffer, SERVER_RESPONSE_OK, 15) == 0) {
                        strcpy(log3, MESSAGE_PROXTY_TO_CLIENT);
                        strcat(log3, " > 200 OK\n");
                        
                        sprintf(additionalLog," > %s ", type);
                        strcat(log3, additionalLog);
                        tog2 = 1;
                    }
                    if(strncmp(buffer, "HTTP/1.1", 8) == 0 && strncmp(buffer, SERVER_RESPONSE_OK, 15) != 0) {
                        strcpy(log3, MESSAGE_PROXTY_TO_CLIENT);
                        strcat(log3, log4);
                        strcat(log3, additionalLog);
                        tog2 = 1;
                    }
                }
                
                if(n > 0) {
                    
                    if(showPopup == 5) {
                        showPopup = false;
                        strcat(log,"\npopup is blocked\n");
                        strcpy(buffer, SERVER_RESPONSE_NOT_FOUND);
                    }
                    if(startPopup == 1) {
                        send(client_sock,mesg, strlen(mesg), 0);
                        parameter=strstr(buffer, "\r\n\r\n"); //뒤에 오는 버퍼들은 \r\n\r\n으로 잘라 뒤에 보내줌
                        // 이미 mesg에서 헤더파일 선언
                        sprintf(buffer, "%s", parameter);
                        
                        startPopup = -1;
                    }
                    send(client_sock, buffer, n, 0);
                }
                
                
            } while(n > 0);
            
            if(c == NULL) {
                
                file_size -= 4;
                sprintf(additionalLog, "%ld", file_size);
                
                strcat(log, additionalLog);
                strcat(log, "bytes");
                strcat(log, "\n");
                
                strcat(log3, additionalLog);
                strcat(log3, "bytes\n");
                strcat(log, log3);
            } else {
                strcat(log, log3);
                strcat(log, "\n");
            }
        }
    }
    else if(strncmp(requestMethod, METHOD_CONNECT, 7) == 0) {
        //Connect Method
        
        strcpy(log4, buffer);
        a = strtok(log4, "\r\n");
        strcat(log, a);
        strcat(log, "\n");
        
        a = strtok(requestUrl, ":");
        b = strtok(NULL, " \t");
        sscanf(b, "%d", &httpsPortNumber);
        
        host = gethostbyname(a);
        bzero((char*)&host_addr, sizeof(host_addr));
        host_addr.sin_port=  htons(httpsPortNumber);
        host_addr.sin_family = AF_INET;
        bcopy((char*)host -> h_addr, (char*)&host_addr.sin_addr.s_addr, host -> h_length);
        
        server_sock=socket(AF_INET, SOCK_STREAM,IPPROTO_TCP);
        proxy_sock=connect(server_sock, (struct sockaddr*)&host_addr, sizeof(struct sockaddr));
        
        
        if(proxy_sock<0) {
            perror("error");
        }
        else {
            strcat(log, MESSAGE_PROXTY_TO_CLIENT);
            strcat(log, " > 200 Connection established\n");
            
            strcat(log, MESSAGE_CLIENT_TO_PROXY);
            strcat(log, MESSAGE_SERVER_TO_PROXY);
            strcat(log, " > Data is secured\n");
            
            sprintf(log4, "HTTP/1.1 200 Connection established\r\n\r\n");
            send(client_sock, log4, strlen(log4), 0);
        }
        
        fd_set fdset;
        int maxp1, r;
        char buf[10000];
        
        if ( client_sock >= server_sock )
            maxp1 = client_sock + 1;
        else
            maxp1 = server_sock + 1;
        
        while(r >= 0) {
            FD_ZERO(&fdset);
            FD_SET(client_sock, &fdset);
            FD_SET(server_sock, &fdset);
            r = select( maxp1, &fdset, (fd_set*) 0, (fd_set*) 0, NULL);
            
            if(r == 0) {
                // Do nothing
            }
            else if(FD_ISSET( client_sock, &fdset )) {
                r = recv(client_sock, buf, sizeof(buf), 0);
                if ( r <= 0 )
                    break;
                r = send(server_sock, buf, r, 0);
                if ( r <= 0 )
                    break;
            }
            else if(FD_ISSET(server_sock, &fdset )) {
                r = recv( server_sock, buf, sizeof( buf ), 0);
                if ( r <= 0 )
                    break;
                if(showPopup == 5) {
                    strcpy(buf, SERVER_RESPONSE_NOT_FOUND);
                    showPopup = -1;
                }
                r = send(client_sock, buf, r ,0);
                if ( r <= 0 )
                    break;
            }
        }
    }
    else {
        strcat(log, requestMethod);
        strcat(log, " ");
        strcat(log, requestUrl);
        strcat(log, "\n");
        strcat(log, "ANOTHER METHOD IS NOT IMPLEMENTED\n");
        send(client_sock, "400: BAD REQUEST\n", 17, 0);
    }
    
    close(server_sock);
    close(client_sock);
    strcat(log, CLIENT_DISCONNECTED);
    strcat(log, SERVER_DISCONNECTED);
    close(sock);
    
    strcat(log, "-------------------------\n\n\n");
    printf("\n%s\n", log);
    
    exit(0);
    return NULL;
}

int main(int argc, char* argv[]) {
    
    pthread_t tid;
    
    printf("Starting proxy server - port: %s\n",argv[1]);
    
    // check options
    if(argv[2] != NULL) {
        
        if(argv[2] != NULL && strncmp(argv[2], "-comp", 5) == 0) {
            enableComppresion = true;
        }
        else if(argv[2] != NULL && strncmp(argv[2], "-COMP", 5) == 0) {
            enableComppresion = true;
        }
        if(argv[2] != NULL && strncmp(argv[2], "-mt", 3) == 0) {
            enableMultiThreading = true;
        }
        else if(argv[2] != NULL && strncmp(argv[2], "-MT", 3) == 0) {
            enableMultiThreading = true;
        }
        if(argv[3] != NULL && strncmp(argv[3], "-comp", 5) == 0) {
            enableComppresion = true;
        }
        else if(argv[3]!=NULL&&strncmp(argv[3], "-COMP", 5) == 0) {
            enableComppresion = true;
        }
        if(argv[3] != NULL && strncmp(argv[3], "-mt", 3) == 0) {
            enableMultiThreading = true;
        }
        else if(argv[3] != NULL && strncmp(argv[3], "-MT", 3) == 0) {
            enableMultiThreading = true;
        }
    }
    
    bzero((char*)&serv_addr,sizeof(serv_addr));
    bzero((char*)&cli_addr, sizeof(cli_addr));
    
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(atoi(argv[1]));
    serv_addr.sin_addr.s_addr=INADDR_ANY;
    
    sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    
    if(sock < 0) {
        printf("Socket error\n");
        perror("error");
    }
    
    if(bind(sock, (struct sockaddr*) & serv_addr, sizeof(serv_addr)) < 0) {
        printf("Binding error\n");
        perror("error");
    }
    
    listen(sock, 50);
    
    unsigned int clilen = sizeof(cli_addr);
    
    while(1) {
        
        client_sock = accept(sock, (struct sockaddr*)&cli_addr, &clilen);
        
        if(client_sock < 0) {
            printf("Client socket error\b");
            perror("Problem in accepting connection");
        }
        else {
            
            // close socket
            if(fork() != 0) {
                close(client_sock);
                packetCount++;
                continue;
            }
            
            struct serverInfo *item = malloc(sizeof(struct serverInfo));
            item -> client_fd = client_sock;
            strcpy(item -> ip, inet_ntoa(cli_addr.sin_addr));
            sprintf(item -> port, "%d", ntohs(cli_addr.sin_port));
            
            // Multi threading: create a new thread
            if(enableMultiThreading == true) {
                pthread_create(&tid, NULL, runSocket, (void *)item);
                sleep(1);
            }
            else {
                runSocket(item);
            }
        }
    }
    return 0;
}
