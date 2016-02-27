#include <stdio.h>
#include <string.h>

#include <string>

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <ev.h>

#include "lib.h"
#include "http.h"

FILE * log;
int id;
static const char * home_directory;

void read_data_cb(struct ev_loop *loop,ev_io *w,int revents){
	char buf[1024];
	
	//log=fopen("/home/auser/log.txt","a");
	//fprintf(log,"worker %d recv sd: %d",id,w->fd);

	bzero(buf,sizeof(buf));
	int size=recv(w->fd,buf,sizeof(buf),MSG_NOSIGNAL);
	//fprintf(log,"size:%d, data:%s\n",size,buf);
	
	if(size<=0){
	//	fprintf(log,"Worker %d. Client %d disconnect\n",id,w->fd);
	//	fclose(log);
	
		close(w->fd);
		ev_io_stop(loop,w);
		delete w;
		return ;
	}

	//fprintf(log,"worker %d recv data from sd: %d\n",id,w->fd);
	//fclose(log);
	
	std::string response=http_request(buf,home_directory);
	if(response.size()!=0)
		send(w->fd,response.c_str(),response.length(),MSG_NOSIGNAL);
	shutdown(w->fd,SHUT_WR);
	close(w->fd);
}

void new_client_cb(struct ev_loop* loop,ev_io * w, int revents){
	int sd;
	char buf[1024];
	
	//log=fopen("/home/auser/log.txt","a");
	
	ssize_t size=sock_fd_read(w->fd,buf,sizeof(buf),&sd);
	if(size<0){
	//	fprintf(log,"Error buffer read\n");
	//	fclose(log);
		return ;
	}
	if(sd<0){
	//	fprintf(log,"Error fd read\n");
	//	fclose(log);
		return ;
	}
	
	//fprintf(log,"Worker %d. Client %d connect\n",id,sd);
	//fclose(log);

	ev_io * w_read=new struct ev_io;
	ev_io_init(w_read,read_data_cb,sd,EV_READ);
	ev_io_start(loop,w_read);
}

void worker(int sv,int pid,const char * home_dir){
	//printf("worker %d, sv: %d\n",pid,sv);

	id=pid;
	home_directory=home_dir;

	struct ev_loop * loop=ev_loop_new(0);
	ev_io w_new_client;
	ev_io_init(&w_new_client,new_client_cb,sv,EV_READ);
	ev_io_start(loop,&w_new_client);
	ev_run(loop);
}
