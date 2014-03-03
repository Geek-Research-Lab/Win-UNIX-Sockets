#include"stream_socket.h"
const char* MSG1="hello MSG1";
const char* MSG3="hello MSG3";
int main(int argc, char* argv[])
{
	int port=-1, rc;
	if(argc<2)
	{
		cerr<<"usage:"<<argv[0]<<"sockname|port>[<host>] \n";
		return 1;
	}
	/*Check if port number of socket name is specified*/
	(void)sscanf(argv[1],"%d",&port);
	/* "host" may be a socket name or a host name */
	char buf[80], *host=(port==1)?argv[1]:argv[2],socknm[80];
	/*Create a client socket*/
	sock sp(port!=-1?AF_INET:AF_UNIX,SOCK_STREAM);
	if(!sp.good())
		return 1;
	/*Connect to a server socket*/
	if(sp.connect(host,port)<0)
		return 8;
	/*Send MSG1 to server*/
	if(sp.write(MSG1,strlen(MSG1)+1)<0)
		return 9;
	/*read MSG2 from server*/
	if(sp.read(buf,sizeof buf)<0)
		return 10;
	cerr<<"client:recv""<<buf<<""\n";
	/*Send MSG3 to server*/
	if((rc=sp.writeto(MSG3,strlen(MSG3)+1,0,host,port,-1))<0)
		return 11;
	/*read MSG4 from server*/
	if((rc=read(sp.fd(),buf,sizeof buf))==-1)
		return 12;
	cerr<<"client:read msg:""<<buf<<""\n";
	/*Shut down socket explicitly*/
	sp.shutdown();
}
