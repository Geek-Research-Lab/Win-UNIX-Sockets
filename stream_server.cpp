#include "stream_socket.h"
const char* MSG2="hello MSG2";
const char* MSG4="hello MSG4";
int main(int argc,char* argv[])
{
	char buf[80], socknm[80];
	int port=-1, nsid, rc;
	if(argc<2)
	{
		cerr<<"usage:"<<argv[0]<<"sockname|port>[<host>] \n";
		return 1;
	}
	/*Check if port number of a socket name is specified*/
	(void)sscan(argv[1],"%d",&port);
	/*create a stream socket*/
	sock sp(port!=-1?AF_INET:AF_UNIX,SOCK_STREAM);
	if(!sp.good())
		return 1;
	/*Bind a name to the server socket*/
	if(sp.bind(port==-1?argv[1]:argv[2],port)<0)
		return 2;
	/*Accept a connection request from a client socket*/
	if((nsid=sp.accept(0,0))<0)
		return 1;
	/*read MSG1 from a client socket*/
	if((rc=sp.read(buf,sizeof buf,0,nsid))<0)
		return 5;
	cerr<<"server:receive msg:""<<buf<<""\n";
	/*write MSG2 to a client socket */
	if(sp.write(MSG2,strlen(MSG2)+1,0,nsid)<0)
		return 6;
	/*read MSG3 from a client socket*/
	if(sp.readfrom(buf,sizeof buf,0,socknm,&port,nsid)>0)
		cerr<<"server:recvfrom"<<socknm<<"msg:""<<buf<<""\n";
	/*write MSG4 to a client socket*/
	if(write(nsid,MSG4,strlen(MSG4)+1)==-1)
		return 7;
}
