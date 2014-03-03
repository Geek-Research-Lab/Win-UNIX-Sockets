#include"stream_socket.h"
#define QUIT_CMD 2
int main(int argc, char* argv[])
{
	if(argc<2)
	{
		cerr<<"usage:"<<argv[0]<<"sockname | port> [<host>] \n";
		return 1;
	}
	int port=-1,rc;
	/*Check if port number of socket name is specified*/
	(void)sscanf(argv[1],"%d",&port);
	/*"host" may be a socket nameor a host name*/
	char buf[80], *host=(port==-1)?argv[1]:argv[2],socknm[80];
	/*Create a client socket*/
	sock sp(port!=-1?AF_INET:AF_UNIX,SOCK_STREAM);
	if(!sp.good())
		return 1;
	/*connect to a server socket*/
	if(sp.connect(host,port)<0)
		return 8;
	/*Send commands 0->2 to server*/
	for(int cmd=0;cmd<3;cmd++)
	{
		/*Compose a command to server*/
		sprintf(buf,"%d",cmd);
		if(sp.write(buf,strlen(buf)+1)<0)
			return 9;
		/*exit the loop if QUIT_CMD*/
		if(cmd==QUIT_CMD)
			break;
		/*read reply from server*/
		if(sp.read(buf,sizeof buf)<0)
			return 10;
		cerr<<"client:recv""<<buf<<""\n";
	}
	sp.shutdown();
	return 0;
}
