#include"stream_socket.h"
const char* MSG1="Hello MSG1";
const char* MSG3="Hello MSG3";
int main(int argc, char* argv[])
{
	char buf[80],socknm[80];
	int nlen,port=-1,rc;
	if(argc<2)
	{
		cerr<<"usage:"<<argv[0]<<"<sockname | port> [<remote-host>] \n";
		return 1;
	}
	/*Check if port number or socket name is specified*/
	(void)sscanf(argv[1],"%d",&port);
	/*Create a datagram socket*/
	sock sp(port==-1?AF_UNIX:AF_INET,SOCK_DGRAM);
	if(port==-1)
	{
		/*UNIX domain socket*/
		sprintf(buf,"%s%d",argv[1],getpid()); /*Construct client socket name*/
		/*assign name to socket*/
		if(sp.bind(buf,port)<0)
			return 2;
	}
	else if(sp.bind(0,0)<0)
		return 2;
	/*write MSG1 to peer*/
	if((rc=sp.writeto(MSG1,strlen(MSG1)+1,0,port==-1?argv[1]:argv[2],port,-1))<0)
		return 6;
	/*read MSG2 from peer*/
	if((rc=sp.readfrom(buf,sizeof buf,0,socknm,&port,-1))<0)
		return 7;
	cerr<<"client:recvfrom""<<socknm<<""msg:"<<buf<<endl;
	/*Establish a default peer socket address*/
	if(sp.connect(socknm,port)<0)
		return 8;
	/*write MSG3 to peer*/
	if(sp.write(MSG3,strlen(MSG3)+1)<0)
		return 9;
	/*read MSG4 from peer*/
	if((rc=read(sp.fd(),buf,sizeof buf))==-1)
		return 10;
	cerr<<"client:read msg:"<<buf<<endl;
	sp.shutdown();
}
