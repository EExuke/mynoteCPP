#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 1023

int setnonblocking( int fd )
{
	int old_option = fcntl( fd, F_GETFL );
	int new_option = old_option | O_NONBLOCK;
	fcntl( fd, F_SETFL, new_option );
	return old_option;
}

int unblock_connect( const char* ip, int port, int time )
{
	int ret = 0;
	struct sockaddr_in address;
	bzero( &address, sizeof( address ) );
	address.sin_family = AF_INET;
	inet_pton( AF_INET, ip, &address.sin_addr );
	address.sin_port = htons( port );

	int sockfd = socket( PF_INET, SOCK_STREAM, 0 );
	int fdopt = setnonblocking( sockfd );
	ret = connect( sockfd, ( struct sockaddr* )&address, sizeof( address ) );
	printf("connect ret code = %d\n", ret);
	if ( ret == 0 )
	{
		printf( "connect with server immediately\n" );
		fcntl( sockfd, F_SETFL, fdopt );   //set old optional back
		return sockfd;
	}
	//unblock mode --> connect return immediately! ret = -1 & errno=EINPROGRESS
	else if ( errno != EINPROGRESS )
	{
		printf("ret = %d\n", ret);
		printf( "unblock connect failed!\n" );
		return -1;
	}
	else if (errno == EINPROGRESS)
	{
		printf( "unblock mode socket is connecting...\n" );
	}

	//use select to check write event, if the socket is writable, then
	//connect is complete successfully!
	fd_set readfds;
	fd_set writefds;
	struct timeval timeout;

	FD_ZERO( &readfds );
	FD_SET( sockfd, &writefds );

	timeout.tv_sec = time; //timeout is 10 minutes
	timeout.tv_usec = 0;

	ret = select( sockfd + 1, NULL, &writefds, NULL, &timeout );
	if ( ret <= 0 )
	{
		printf( "connection time out\n" );
		close( sockfd );
		return -1;
	}

	if ( ! FD_ISSET( sockfd, &writefds  ) )
	{
		printf( "no events on sockfd found\n" );
		close( sockfd );
		return -1;
	}

	int error = 0;
	socklen_t length = sizeof( error );
	if( getsockopt( sockfd, SOL_SOCKET, SO_ERROR, &error, &length ) < 0 )
	{
		printf( "get socket option failed\n" );
		close( sockfd );
		return -1;
	}

	if( error != 0 )
	{
		printf( "connection failed after select with the error: %d \n", error );
		close( sockfd );
		return -1;
	}

	//connection successful!
	printf( "connection ready after select with the socket: %d \n", sockfd );
	fcntl( sockfd, F_SETFL, fdopt ); //set old optional back

	 printf("connect ok !\r\n");

	int recbytes;
	int sin_size;
	char buffer[1024]={0};   

	
	if(-1 == (recbytes = read(sockfd,buffer,1024)))  //��������
	{
		printf("read data fail !\r\n");
		return -1;
	}
	printf("read ok:");

	buffer[recbytes]='\0';
	printf("%s\r\n",buffer);






	return sockfd;
}



int main( int argc, char* argv[] )
{
	 
	const char ip[] = "172.16.2.7";
	int port = 10051;

	int sockfd = unblock_connect( ip, port, 1);
	if ( sockfd < 0 )
	{
		printf("sockfd error! return -1\n");
		return 1;
	}
 
	close(sockfd);
	return 0;
}