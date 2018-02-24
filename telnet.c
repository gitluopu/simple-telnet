
#include<termios.h>
#include<unistd.h>
#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<string.h>
#include<errno.h>
#include<stdlib.h>
#define err_log printf
	int main(int argc, char *argv[])
	{
		pid_t pid;
		if(argc<2){
			err_log("mistaken para.\n");
			exit(-1);
			}
		pid = fork();
		if(pid<0){
			err_log("fork err.\n");
			exit(-1);
			}else if(pid==0){
		struct termios termbuf;
		struct sockaddr_in addr;
		int fd,ret;
		fd_set rfds,tfds;
		tcgetattr(0,&termbuf);
		termbuf.c_lflag &= ~ECHO;
		termbuf.c_lflag &= ~ICANON;
		tcsetattr(0, TCSANOW, &termbuf);
	
		memset(&addr,0,sizeof addr);
		inet_aton(argv[1],&addr.sin_addr);
		addr.sin_family = AF_INET;
		addr.sin_port = htons(atoi(argv[2]));
        fd = socket(AF_INET, SOCK_STREAM, 0);
		if(fd<0){
			err_log("socket err.\n");
			exit(-1);
			}
		ret = connect(fd, (struct sockaddr*)&addr, sizeof addr);
		if(ret<0){
			err_log("connect err=%d,%s.\n",errno, strerror(errno));
			close(fd);
			exit(-1);
			}
		FD_ZERO(&rfds);
		FD_SET(0,&rfds);
		FD_SET(fd,&rfds);
		while(1){
			tfds = rfds;
			char buf[1024] = {0};
			ret = select(fd+1, &tfds, NULL, NULL, NULL);
			if(ret<=0){
				err_log("select err.\n");
				sleep(1);
				continue;
				}
			if(FD_ISSET(0,&tfds)){
				int len = read(0,buf,1024);
				if(len<=0){
					exit(-1);
					}
				write(fd,buf,len);
				}
			if(FD_ISSET(fd,&tfds)){
				int len = read(fd,buf,1024);
				if(len<=0){
					exit(-1);
					}
				write(1,buf,len);
				}
			}
		exit(1);
				}
			close(0);
			wait(NULL);
		return 0;
	}


