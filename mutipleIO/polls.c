#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <poll.h>

#define MAX_FD  8192
struct pollfd  fds[MAX_FD];
int cur_max_fd = 10;


int main()
{
    int server_sockfd, client_sockfd;
    int server_len, client_len;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    int result;
	
    //fd_set readfds, testfds;
    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);//建立服务器端socket
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(9000);
    server_len = sizeof(server_address);
    bind(server_sockfd, (struct sockaddr*)&server_address, server_len);
    listen(server_sockfd, 5); //监听队列最多容纳5个

    fds[server_sockfd].fd = server_sockfd;
    fds[server_sockfd].events = POLLIN;
    fds[server_sockfd].revents = 0;
    if(cur_max_fd <= server_sockfd)
    {
        cur_max_fd = server_sockfd + 1;
    }


    while (1)
    {
        char ch;
        int i, fd;
        int nread;
       
        printf("server waiting\n");

        /*无限期阻塞，并测试文件描述符变动 */
        result = poll(fds, cur_max_fd, 1000);

        if (result < 0)
        {
            perror("server5");
            exit(1);

        }
        /*扫描所有的文件描述符*/
        for (i = 0; i < cur_max_fd; i++)
        {

            /*找到相关文件描述符*/
            if (fds[i].revents)
            {
                fd = fds[i].fd;
                /*判断是否为服务器套接字，是则表示为客户请求连接。*/
                if (fd == server_sockfd)
                {
                    client_len = sizeof(client_address);
                    client_sockfd = accept(server_sockfd,
                        (struct sockaddr*)&client_address, &client_len);
                    fds[client_sockfd].fd = client_sockfd;//将客户端socket加入到集合中
                    fds[client_sockfd].events = POLLIN;
                    fds[client_sockfd].revents = 0;


                    if(cur_max_fd <= client_sockfd)
                    {
                        cur_max_fd = client_sockfd + 1;
                    }

                    printf("adding client on fd %d\n", client_sockfd);
          
                }
                /*客户端socket中有数据请求时*/
                else
                {
				   if(fds[i].revents&POLLIN)
				   {
						nread = read(fd, &ch, 1);
					  
						if (nread == 0)
						{
							close(fd);
							memset(&fds[i], 0, sizeof(struct pollfd)); //去掉关闭的fd
							printf("removing client on fd %d\n", fd);
						}
					  
						else
						{
							
							sleep(5);
							printf("serving client on fd %d, read: %c\n", fd, ch);
							ch++;
							fds[i].events=POLLOUT;
							//write(fd, &ch, 1);
				  
						}
                    }
					else if(fds[i].revents &POLLOUT){
						 write(fd,&ch,1);
						 fds[i].events=POLLIN;
					}
				}
            }
        }
    }

    return 0;
}
