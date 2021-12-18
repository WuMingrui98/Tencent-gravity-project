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

#define SERV_PORT 8082
#define IPADDRESS "110.40.193.165"
#define KEY_DEV_PATH "/dev/input/event2"
#define MOU_DEV_PATH "/dev/input/event0"
#define FDSIZE 10

struct epoll_event ev;//LT Module
struct epoll_event events[10];
vector<struct input_event> kamevent={{{0},0}};
struct input_event last = {{0},0};

void handle_connection(int sockfd);
void handle_events(int epollfd,struct epoll_event* events,int num, int sockfd, vector<struct input_event>kamevent);
void do_read(int epollfd,int fd,int sockfd,vector<struct input_event>kamevent);
void do_write(int epollfd, int fd, vector<struct input_event>kamevent);
void add_event(int epollfd, int fd, int state);
void delete_event(int epfd,int fd,int state);
int kbfd;
int msfd;

int main(int argc, char** argv){
    int sockfd;
    struct sockaddr_in servaddr;
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET,IPADDRESS,&servaddr.sin_addr);
    if(connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr))<0){
        perror("Connect ");
        exit(errno);
    }
    printf("server connected\n");

    handle_connection(sockfd);
    close(sockfd);
    return 0;
}

void handle_connection(int sockfd){
    int epollfd;
    int ret;
    epollfd = epoll_create(FDSIZE);

    kbfd = open(KEY_DEV_PATH, O_RDONLY | O_NONBLOCK);
    msfd = open(MOU_DEV_PATH,O_RDONLY | O_NONBLOCK);
    add_event(epollfd,kbfd,EPOLLIN);
    add_event(epollfd,msfd,EPOLLIN);
    while(1){
        ret = epoll_wait(epollfd,events,FDSIZE,-1);
        handle_events(epollfd,events,ret,sockfd,kamevent);
    }
    close(epollfd);
}

void handle_events(int epollfd, struct epoll_event*events,int num, int sockfd, vector<struct input_event>kamevent){
    int fd;
    int i;
    for(i = 0; i<num;i++){
        fd = events[i].data.fd;
        if(events[i].events&EPOLLIN)
            do_read(epollfd,fd,sockfd,kamevent);
        /*
        else if(events[i].events&EPOLLOUT)
            do_write(epollfd,sockfd,kamevent);
        */
    }
}

void do_read(int epollfd, int fd, int sockfd,vector<struct input_event> kamevent){
    int nread;
    struct input_event et;
    nread = read(fd,&et,sizeof(input_event));
    
    if(nread == -1){
        perror("read error: ");
        exit(1);
    }

    if(fd == msfd){
        et.type += 5;
    }

    //kamevent.push_back(et);
    //printf("read\n type%d, code%d, value%d\n",et.type,et.code,et.value);
    int nwrite;
    if(et.type == 5||et.type  == 7||et.type  == 8||et.type  == 6||et.type== 1){
        if(send(sockfd,&et,sizeof(input_event),0)<0){
            perror("send error: ");
            exit(1);
        }else{
            printf("send\n %d %d %d\n",et.type,et.code,et.value);
            //kamevent.erase(kamevent.begin());
        }
    }
    delete_event(epollfd, fd,EPOLLIN);
    add_event(epollfd,fd,EPOLLIN);
}
/*
void do_write(int epollfd,int sockfd,vector<struct input_event>kamevent){
    int nwrite;
    if(kamevent[0].type == 7||kamevent[0].type == 8||kamevent[0].type == 6||kamevent[0].type == 1){
        if(send(sockfd,&kamevent[0],sizeof(input_event),0)<0){
            perror("send error: ");
            exit(1);
        }else{
            printf("send\n %d %d %d\n",kamevent[0].type,kamevent[0].code,kamevent[0].value);
            kamevent.erase(kamevent.begin());
        }
    }else{
        kamevent.erase(kamevent.begin());
    }
    delete_event(epollfd,sockfd,EPOLLOUT);
    add_event(epollfd,sockfd,EPOLLOUT);

}
*/
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