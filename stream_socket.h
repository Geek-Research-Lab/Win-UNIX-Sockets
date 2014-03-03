#ifndef STREAM_SOCKET_H
#define STREAM_SOCKET_H
#include "resource.h"
#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<UtilLib.h>
#include<memory.h>
#include<string.h>
#include<atlsocket.h>
#include<InetReg.h>
#include<InetSDK.h>
#include<Winineti.h>
#include<dbnetlib.h>
#include<sys\types.h>
#include<infocard.h>
const int BACKLOG_NUM=5;
class sock
{
private:
	/*Socket descriptor*/
	int sid;
	/*socket domain*/
	int domain;
	/*Socket type*/
	int socktype;
	/*member function return status code*/
	int rc;
	/*Build internet socket name based on host name and port number*/
	int constr_name(struct sockaddr_in& addr, const char* hostnm, int port)
	{
		addr.sin_family=domain;
		if(!hostnm)
			addr.sin_addr.s_addr=INADDR_ANY;
		else
		{
			struct hostent *hp = gethostbyname(hostnm);
			if(hp==0)
			{
				perror("gethostbyname");
				return -1;
			}
			memcpy((char*)&addr.sin_addr,(char*)hp->h_addr,hp->h_length);
		}
		addr.sin_port=htons(port);
		return sizeof(addr);
	};
	/*Build an UNIX domain socket name based on a path name*/
	int constr_name(struct sockaddr& addr, const char* Pathnm)
	{
		addr.sa_family=domain;
		strcpy(addr.sa_data,Pathnm);
		return sizeof(addr.sa_family) + strlen(Pathnm) + 1;
	};
	/*Convert an IP address to a character string host name*/
	char* ip2name(const struct in_addr in)
	{
		u_long laddr;
		if((int)(laddr=inet_addr(inet_ntoa(in)))==-1)
			return 0;
			struct hostent *hp = gethostbyaddr((char*)&laddr, sizeof(laddr), AF_INET);
			if(!hp)
				return 0;
			for(char **p=hp->h_addr_list;*p!=0;p++)
			{
				if(hp->h_name)
					return hp->h_name;
	}
	return 0;
};
public:
	/*sock object constructor function*/
	sock(int dom, int type, int protocol=0) : domain(dom), socktype(type)
	{
		if((sid=socket(dom,type,protocol))<0)
			perror("socket");
	};
	/*sock object destruction function*/
	~sock()
	{
		/*discard a socket*/
		shutdown();
		close(sid);
	};
	/*return a socket's id*/
	int fd()
	{
		return sid;
	};
	/*check sock object status*/
	int good()
	{
		return sid>=0;
	};
	/*assign UNIX or internet name to a socket*/
	int bind(const char* name, int port=-1)
	{
		if(port==-1)
		{
			/*UNIX domain socket*/
			struct sockaddr addr;
			int len=constr_name(addr,name);
			if((rc=::bind(sid,&addr,len))<0)
				perror("bind");
		}
		else
		{
			struct sockaddr_in addr;
			int len=constr_name(addr,name,port);
			if((rc=::bind(sid,(struct sockaddr *)&addr, len))<0 || (rc=getsockname(sid,(struct sockaddr*)&addr,&len))<0)
				perror("bind or getsockname");
			else
				cout<<"socket port:"<<ntohs(addr.sin_port)<<endl;
		}
		/*setup connection backlog threshold for a STREAM socket*/
		if(rc!=-1 && socktype!=SOCK_DGRAM && (rc=listen(sid,BACKLOG_NUM))<0)
			perror("listen");
		return rc;
	};
	/*A server socket accepts a client connection request*/
	int accept(char *name, int* port_p)
	{
		if(!name)
			return ::accept(sid,0,0);
		if(!port_p || *port_p==-1)
		{
			/*UNIX domain socket*/
			struct sockaddr addr;
			int size=sizeof(addr);
			if((rc=::accept(sid,&addr,&size))>-1)
				strncpy(name,addr.sa_data,size),name[size]='\0';
		}
		else
		{
			/*Internet domain socket*/
			struct sockaddr_in addr;
			int size=sizeof(addr);
			if((rc=::accept(sid,(struct sockaddr*)&addr, &size))>-1)
			{
				if(name)
					strcpy(name,ip2name(addr.sin_addr));
				if(port_p)
					*port_p=ntohs(addr.sin_port);
			}
		}
		return rc;
	};
	/*A client socket initiates a connection request to a server socket*/
	int connect(const char* hostnm,int port=-1)
	{
		if(port==-1)
		{
			/*UNIX domain socket*/
			struct sockaddr addr;
			int len=constr_name(addr,hostnm);
			if((rc=::connect(sid,&addr,len))<0)
				perror("bind");
		}
		else
		{
			/*internet socket domain*/
			struct sockaddr_in addr;
			int len=constr_name(addr,hostnm,port);
			if((rc=::connect(sid,(struct sockaddr *)&addr,len))<0)
				perror("bind");
		}
		return rc;
	};
	/*Writes a message to a connected stream socket*/
	int write(const char* buf,int len,int flag=0,int nsid=-1)
	{
		return ::send(nsid==-1?sid:nsid,buf,len,flag);
	};
	/* reads a message from a connected stream socket */
	int read(char* buf,int len,int flag=0,int nsid=-1) /*read a message*/
	{
		return ::recv(nsid==-1?sid:nsid,buf,len,flag);
	};
	/*write to a socket of the given socket name*/
	int writeto(const char* buf,int len,int flag,const char* name,const int port,int nsid=-1)
	{
		if(port==-1)
		{
			/*UNIX domain socket*/
			struct sockaddr addr;
			int size=constr_name(addr,name);
			return ::sendto(nsid==-1?sid:nsid,buf,len,flag,&addr,size);
		}
		else
		{
			/*Internet domain socket*/
			struct sockaddr_in addr;
			char buf1[80];
			if(!name)
			{
				/*use local host*/
				if(sysinfo(SI_HOSTNAME,buf1,sizeof buf1)==-1L)
					perror("sysinfo");
				name=buf1;
			}
			int size=constr_name(addr,name,port);
			return ::sendto(nsid==-1?sid:nsid,buf,len,flag,(struct sockaddr*)&addr,size);
		}
	};
	/*recives a message from a socket*/
	int readfrom(char* buf,int len,int flag,char* name,int *port_p,int nsid=-1)
	{
		if(!port_p||*port_p==-1)
		{
			/*UNIX domain socket*/
			struct sockaddr addr;
			int size=sizeof(addr);
			if((rc=::recvfrom(nsid==-1?sid:nsid,buf,len,flag,&addr,&size))>-1 && name)
				strncpy(name,addr.sa_data,rc),name[rc]='\0';
		}
		else
		{
			/*Internet domain socket*/
			struct sockaddr_in addr;
			int size=sizeof(addr);
			if((rc=::recvfrom(nsid==-1?sid:nsid,buf,len,flag,(struct sockaddr*)&addr,&size))>-1)
			{
				if(name)
					strcpy(name,ip2name(addr.sin_addr));
				if(port_p)
					*port_p=ntohs(addr.sin_port);
			}
		}
		return rc;
	};
	/*Shut down connection of a socket*/
	int shutdown(int mode=2)
	{
		return ::shutdown(sid,mode);
	};
	};
#endif
