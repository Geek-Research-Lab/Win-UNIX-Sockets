#include"stream_socket.h"
const char* MSG2="hello MSG2";
const char* MSG4="hello MSG4";
int main(int argc,char* argv[])
{
	int port=-1,rc;
	char buf[80],socknm[80];
	if(argc<2)
	{
		cerr<<"usage:"<<argv[0]<<"<sockname!port>[<remote-host>] \n";
		return 1;
	}
	/*Check if port number or socket name is required*/
	(void)sscanf(argv[1],"%d",&port);
	/*Create a datagram socket*/
	sock sp(port==-1?AF_UNIX:AF_INET,SOCK_DGRAM);
	if(!sp.good())
		return 1;
	/*Assign a name to the socket*/
	if(sp.bind(port==-1?argv[1]:argv[2],port)<0)
		return 2;
	/*read MSG1 from peer*/
	if((rc=sp.readfrom(buf,sizeof buf,0,socknm,&port,-1))<0)
		return 1;
	cerr<<"server:recvfrom from""<<socknm<<""msg:"<<buf<<endl;
	/*write MSG2 to peer*/
	if((rc=sp.writeto(MSG2,strlen(MSG2)+1,0,socknm,port,-1))<0)
		return 2;
	/*Establish a default client address*/
	if((rc=sp.connect(socknm,port))<0)
		return 3;
	/*read MSG3 from peer*/
	if((rc=sp.read(buf,sizeof buf, 0))<0)
		return 4;
	cerr<<"server:receive msg""<<buf<<""\n";
	/*write MSG4 to peer*/
	if(write(sp.fd(),MSG4,strlen(MSG4)+1)<0)
		return 5;
}
