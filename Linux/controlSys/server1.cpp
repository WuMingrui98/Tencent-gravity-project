#include<stdio.h>
#include<string.h>
#include<linux/input.h>
#include<fcntl.h>
#include<errno.h>
#include<sys/wait.h>
#include<sys/socket.h>
#include<sys/epoll.h>
#include<resolv.h>//操作系统域名解释器
#include<stdlib.h>
#include<netinet/in.h>//定义数据结构sockaddr_in
#include<arpa/inet.h>//ip地址转换函数
#include<unistd.h>
#include<vector>

using namespace std;

#define SERV_PORT 6667
#define IPADDRESS "127.0.0.1"
#define KEY_DEV_PATH "/dev/input/event1"
#define MOU_DEV_PATH "/dev/input/event2"
#define FDSIZE 10

struct epoll_event ev;//LT Module
struct epoll_event events[10];
vector<struct input_event> kamevent = {{{0},0}};
struct input_event last = {{0},0};


class Capture{
    public:
        int epfd;
        int kbfd;
        int msfd;

        int socket_bind(const char* ip, int port){
            int listenfd;
            struct sockaddr_in servaddr;
            listenfd = socket(AF_INET, SOCK_STREAM,0);
            if(listenfd == -1){
                perror("socket error: ");
                exit(1);
            }else{
                printf("socket created\n");
            }
            bzero(&servaddr,sizeof(servaddr));
            servaddr.sin_family = AF_INET;
            //inet_pton(AF_INET,ip ,&servaddr.sin_addr);
            servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
            servaddr.sin_port = htons(port);
            if(bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr)) == -1){
                perror("bind error: ");
                exit(1);
            }else{
                printf("bind success\n");
            }
            return listenfd;
        }
        void do_epoll(int listenfd){
            int ret;

            epfd = epoll_create(10);
            add_event(epfd,listenfd,EPOLLIN);
            


            while(1){
                ret = epoll_wait(epfd,events,FDSIZE,-1);
                handle_events(epfd,events,ret,listenfd,kamevent);
            }
            close(epfd);
        }
        void add_event(int epfd,int fd,int state){
            struct epoll_event ev;
            ev.events = state;
            ev.data.fd = fd;
            epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&ev);
        }
        void delete_event(int epfd,int fd,int state){
            struct epoll_event ev;
            ev.events = state;
            ev.data.fd = fd;
            epoll_ctl(epfd,EPOLL_CTL_DEL,fd,&ev);
        }

        void handle_events(int epfd, struct epoll_event * events, int num, int listenfd, vector<struct input_event> kamevent){
            int i;
            int fd;
            for(i = 0; i<num; i++){
                fd = events[i].data.fd;
                if((fd == listenfd)&& (events[i].events & EPOLLIN))
                    handle_accpet(epfd,listenfd);//accept socket
                else if(events[i].events&EPOLLIN)
                    do_read(epfd,fd,kamevent);//readable from client
                /*
                else if((fd == msfd)&&(events[i].events&EPOLLOUT))
                    do_write_ms(epfd,fd,kamevent);//writeable to input_event
                else if((fd == kbfd)&&(events[i].events&EPOLLOUT))
                    do_write_kb(epfd,fd,kamevent);
                */
                
            }
        }

        void handle_accpet(int epfd, int listenfd){
            int clifd;
            struct sockaddr_in cliaddr;
            socklen_t cliaddrlen = sizeof(cliaddr);
            clifd = accept(listenfd,(struct sockaddr*)&cliaddr,&cliaddrlen);
            if(clifd == -1){
                perror("accpet error; ");
            }
            else{
                printf("accepting\n");
                add_event(epfd, clifd, EPOLLIN);//add one that can recv()
                //after recv add writable event to input-event
            }

        }

        void do_read(int epfd, int fd, vector<struct input_event> kamevent){
            int rlen;
            char buffer[1024];
            memset(buffer,0,1024);
            struct input_event *t;
            if((rlen = recv(fd,buffer,sizeof(input_event),0)) == -1){
                perror("recv error");
                close(fd);
            }
            else if(rlen ==sizeof(input_event)){
                
                t = (input_event*)buffer;
                //printf("recv\n %d %d %d\n",t->type,t->code,t->value);
                //kamevent.push_back(*t);

                kbfd = open(KEY_DEV_PATH, O_WRONLY | O_NONBLOCK);
                msfd = open(MOU_DEV_PATH, O_WRONLY | O_NONBLOCK);
                if(kbfd == -1 || msfd == -1){
                    printf("open kb or mouse fail!\n");
                    exit(1);
                }
                else{
                    //printf("open kb or mouse\n");
                    /*
                    ev.data.fd = kbfd;
                    ev.events = EPOLLOUT;
                    int i = epoll_ctl(epfd,EPOLL_CTL_ADD, kbfd, &ev);
                    ev.data.fd = msfd;
                    ev.events = EPOLLOUT;
                    int j = epoll_ctl(epfd,EPOLL_CTL_ADD, msfd, &ev);
                    if(i == -1 || j == -1){
                        printf("add device in epoll fail\n");
                    }
                    */
                    int wlen;
                    if(t->type == EV_KEY){
                        if(write(kbfd,t,sizeof(input_event))!=sizeof(input_event)){
                            printf("press key error");
                            exit(1);
                        }else{
                            //printf("write\n %d %d %d\nsucceed\n",kamevent[0].type,kamevent[0].code,kamevent[0].value);

                            if(write(kbfd,&last,sizeof(last))!=sizeof(last)){
                                printf("write last unsucceed\n");
                                exit(1);
                            }
                            else{
                                    //printf("write \n %d %d %d\nsucceed\n",last.type,last.code,last.value);
                            }
                            //kamevent.erase(kamevent.begin());//delete the first event
                            close(kbfd);
                            //delete_event(epfd,fd,EPOLLOUT);//delete event
                            kbfd = open(KEY_DEV_PATH, O_WRONLY | O_NONBLOCK);//flush
                            //add_event(epfd,kbfd,EPOLLOUT);
                        }
                    }else if(t->type == 5||t->type == 8){
                        t->type -= 5;
                        if(write(msfd,t,sizeof(input_event))!= sizeof(input_event)){
                            printf("mouse error");
                            exit(1);
                        }else{
                            //printf("write\n %d %d %d\nsucceed\n",t->type,t->code,t->value);
                            //kamevent.erase(kamevent.begin());//delete the first event
                            close(msfd);
                            //delete_event(epfd,fd,EPOLLOUT);//delete event
                            msfd = open(MOU_DEV_PATH, O_WRONLY | O_NONBLOCK);//flush
                            //add_event(epfd,msfd,EPOLLOUT);
                            usleep(10);
                        }
                        
                    }else if(t->type == 6){
                        t->type -= 5;
                        if(write(msfd,t,sizeof(input_event))!=sizeof(input_event)){
                            printf("mouse error");
                            exit(1);
                        }else{
                            write(msfd,&last,sizeof(last));
                            close(msfd);
                            msfd = open(MOU_DEV_PATH, O_WRONLY | O_NONBLOCK);
                        }
                        //kamevent.erase(kamevent.begin());//not kbevent or mouse event, delete
                    }
                }
            }
        }
/*
        void do_write_kb(int epfd, int fd, vector<struct input_event>kamevent){
            
            

        }

        void do_write_ms(int epfd, int fd, vector<struct input_event>kamevent){
            int wlen;
            if(kamevent[0].type == 5||kamevent[0].type == 6||kamevent[0].type == 7||kamevent[0].type == 9){//input/mice event0,1,2,4 + 5;
                
            }
            else{
                kamevent.erase(kamevent.begin());//not kbevent or mouse event, delete
            }
        }
*/

};

int main(int argc, char ** argv){
    Capture controlinput;
    int listenfd = 0;
    listenfd = controlinput.socket_bind(IPADDRESS,SERV_PORT);
    listen(listenfd,10);
    controlinput.do_epoll(listenfd);
    return 0;
}