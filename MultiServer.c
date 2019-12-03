#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>
void *do_chat(void *); //채팅 메세지를 보내는 함수
int pushClient(int, char *); //새로운 클라이언트가 접속했을 때 클라이언트 정보 추가
int popClient(int); //클라이언트가 종료했을 때 클라이언트 정보 삭제
pthread_t thread;
pthread_mutex_t mutex;

#define MAX_CLIENT 10
#define CHATDATA 1024
#define INVALID_SOCK -1
#define PORT 9000
#define WHISPER "msg"
struct chatNick{
    char nickname[CHATDATA];
};
struct chatNick nick[10];
int    list_c[MAX_CLIENT];
char    escape[ ] = "exit";
char    greeting[ ] = "Welcome to chatting room\n";
char    CODE200[ ] = "Sorry No More Connection\n";
int main(int argc, char *argv[ ])
{
    int c_socket, s_socket;
    struct sockaddr_in s_addr, c_addr;
    int    len;
    int    i, j, n;
    int    res;
    char nickname[CHATDATA];
   
    if(pthread_mutex_init(&mutex, NULL) != 0) {
        printf("Can not create mutex\n");
        return -1;
    }
    s_socket = socket(PF_INET, SOCK_STREAM, 0);
    memset(&s_addr, 0, sizeof(s_addr));
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(PORT);
    if(bind(s_socket, (struct sockaddr *)&s_addr, sizeof(s_addr)) == -1) {
        printf("Can not Bind\n");
        return -1;
    }
    if(listen(s_socket, MAX_CLIENT) == -1) {
        printf("listen Fail\n");
        return -1;
    }
    for(i = 0; i < MAX_CLIENT; i++)
        list_c[i] = INVALID_SOCK;
    while(1) {
        len = sizeof(c_addr);
        c_socket = accept(s_socket, (struct sockaddr *) &c_addr, &len);
        read(c_socket,nickname,sizeof(nickname));

        res = pushClient(c_socket, nickname);
        if(res < 0) { //MAX_CLIENT만큼 이미 클라이언트가 접속해 있다면,
            write(c_socket, CODE200, strlen(CODE200));
            close(c_socket);
        } else {
            write(c_socket, greeting, strlen(greeting));
             pthread_create(&thread, NULL, do_chat, (void*)&c_socket);

            //pthread_create with do_chat function.
        }
    }
}
void *do_chat(void *arg)
{
    int c_socket = *((int *)arg);
    char chatData[CHATDATA];
    int i, n,a;
    while(1) {
        memset(chatData, 0, sizeof(chatData));
        if((n = read(c_socket, chatData, sizeof(chatData))) > 0) {
            char *whistoken=NULL;
            char *nickname=NULL;
            char *msg=NULL;
            if(strncasecmp(chatData,WHISPER,3)==0){
                whistoken=strtok(chatData," ");
                nickname=strtok(NULL," ");
                msg=strtok(NULL,"\0");
                }

            printf("rcvBuffer: %s\n", chatData);
            for(i=0;i<MAX_CLIENT;i++){
                if(list_c[i]!=INVALID_SOCK){
                    if(nickname!=NULL){
                            a=strcasecmp(nick[i].nickname,nickname);//strcasecmp 값비교..
                            printf("nickname %s %s %d\n",nick[i].nickname,nickname,a);
                        if(strcasecmp(nick[i].nickname,nickname)==0)
                            write(list_c[i],msg,strlen(msg));
                    }else{
                write(list_c[i],chatData,sizeof(chatData));
                }
            }
        }
            //write chatData to all clients
            //
            ///////////////////////////////
            if(strstr(chatData, escape) != NULL) {
                popClient(c_socket);
                break;
            }
        }
    }
}
int pushClient(int c_socket, char *nickname) {
        int i;

       for(i=0; i < MAX_CLIENT; i++){
            pthread_mutex_lock(&mutex);
            if(list_c[i]==INVALID_SOCK){
            list_c[i] = c_socket;
                memset(nick[i].nickname,0,CHATDATA);
                strcpy(nick[i].nickname,nickname);
                pthread_mutex_unlock(&mutex);
                return i;
            }
            pthread_mutex_unlock(&mutex);
        }
            if(i==MAX_CLIENT)
                return -1;
    }

          
    //ADD c_socket to list_c array.
    //
    ///////////////////////////////
    //return -1, if list_c is full.
    //return the index of list_c which c_socket is added.

int popClient(int c_socket)
{
    int i;
    close(c_socket);
    pthread_mutex_lock(&mutex);
    for(i=0;i<MAX_CLIENT;i++){
        if(c_socket==list_c[i]){
            list_c[i]=INVALID_SOCK;
            break;
        }
    }
    pthread_mutex_unlock(&mutex);
    return 0;
    //REMOVE c_socket from list_c array.
    //
    ///////////////////////////////////
}  
