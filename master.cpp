#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include <ev.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib.h"

#define BIND_PORT 1234
#define HOME_DIRECTORY "."

int numCPU;
int ** sv;

struct globalArgs_t{
	const char * host;
	int port;
	const char * directory;
	int daemon;
} globalArgs;

static const char * optString="h:p:d:s";

void send_in_worker(int sd){
	static int nextWorker = 0;
	char b[]="1";
	sock_fd_write(sv[nextWorker][0],b,1,sd);
	nextWorker=(nextWorker+1)%numCPU;
}

void accept_cb(struct ev_loop * loop,ev_io * w,int revents){
	int	client_sd=accept(w->fd,0,0);
	if(client_sd==-1){
		perror("Accept error");
		return;
	}

	send_in_worker(client_sd);
	printf("Accept. Client socket:%d\n",client_sd);
};

int daemon(){
	numCPU=sysconf(_SC_NPROCESSORS_ONLN);
	printf("%d\n",numCPU);
	
	int * pid=new int[numCPU];
	sv=new int* [numCPU];
	
	int MasterSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(MasterSocket==-1){
		perror("Socket error");
		return 1;
	}

	struct sockaddr_in addr;
	bzero(&addr,sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_port=htons(globalArgs.port);
	addr.sin_addr.s_addr=inet_addr(globalArgs.host);
	if(bind(MasterSocket,(struct sockaddr *)&addr,sizeof(addr))==-1){
		perror("Bind error");
		close(MasterSocket);
		return 2;
	}

	if(listen(MasterSocket,SOMAXCONN)==-1){
		perror("Listen error");
		shutdown(MasterSocket,2);
		return 3;
	}

	for(int i=0;i<numCPU;i++){
		sv[i]=new int[2];
		if(socketpair(AF_LOCAL,SOCK_STREAM,0,sv[i])<0){
			perror("Socketpair error");
			shutdown(MasterSocket,2);
			close(MasterSocket);
			return 4;
		}
	}

	for(int i=0;i<numCPU;i++){

		switch((pid[i]=fork())){
			case 0:
				close(sv[i][0]);
				worker(sv[i][1],i,globalArgs.directory);
				return 0;
			case -1:
				perror("Fork error");
				break;
			default:
				close(sv[i][1]);
		}
	}

	struct ev_loop * loop=ev_default_loop(0);
	ev_io w_accept;
	ev_io_init(&w_accept,accept_cb,MasterSocket,EV_READ);
	ev_io_start(loop,&w_accept);
	ev_run(loop);

	delete[] pid;
	return 0;
}

int main(int argc,char **argv){
	int opt;	
	pid_t pid;
		
	globalArgs.host=NULL;
	globalArgs.port=BIND_PORT;
	globalArgs.directory=HOME_DIRECTORY;

	opt=getopt(argc,argv,optString);
	while(opt!=-1){
		switch(opt){
			case 'h':
				globalArgs.host=optarg;
				break;
			case 'p':
				globalArgs.port=atoi(optarg);
				break;
			case 'd':
				globalArgs.directory=optarg;
				break;
			case 's':
				globalArgs.daemon=1;
				break;
		}
		opt=getopt(argc,argv,optString);
	}
	
	if(globalArgs.daemon){
		if((pid=fork())<0){
			perror("Fork error");
			return 1;
		}else if(pid!=0)
		return 0;

		setsid();
	}
	daemon();
	
	return 0;
}
