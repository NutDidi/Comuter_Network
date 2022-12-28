#include <iostream>
#include <string>
#include <vector>
#include <winsock.h>
#include<time.h>


#define ServerIP "127.9.22.98"
#define ClientIP "127.9.22.99"
const int Max_DataLen = 8192;
const int Max_waitTime = 500;
SOCKADDR_IN Server_addr,Client_addr;
unsigned short Server_Port = 5001;
unsigned short Client_Port = 5002;
const int Max_window = 10;
const int loss_rate = 5;
const int MSS = 5;
const int sleeptime = 0;

int package = 0;

void printTime()
{	
	SYSTEMTIME sys; 
    GetLocalTime( &sys ); 
    printf( "[%4d/%02d/%02d %02d:%02d:%02d] ",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond); 
}

char* getTime()
{	
	char* c;
	SYSTEMTIME sys; 
    GetLocalTime( &sys ); 
    sprintf( c,"[%4d/%02d/%02d %02d:%02d:%02d] ",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond); 
	return c;
}

struct Message
{
    unsigned short src_port;
    unsigned short dst_port;
    unsigned int seq;
    unsigned int ack;
	unsigned int datalen = 0;
	unsigned int Winsize ;
    unsigned short Flags = 0;
    unsigned short CheckSum;
    char Data[8192]{};

    void setFin()
    {
        Flags = Flags | 0x0001;
    };
    bool getFin()
    {
        return Flags & 0x0001;
    };

    void setSyn()
    {
        Flags = Flags | 0x0002;
    };
    bool getSyn()
    {
        return Flags & 0x0002;
    };

    void setRst()
    {
        Flags = Flags | 0x0004;
    };
    bool getRst()
    {
        return Flags & 0x0004;
    };

    void setPsh()
    {
        Flags = (Flags | 0x0008);
    };
    bool getPsh()
    {
        return (Flags & 0x0008);
    };

    void setAck()
    {
        Flags = Flags | 0x0010;
    };
    bool getAck()
    {
        return Flags & 0x0010;
    };

	void setName()
	{
		Flags = Flags | 0x0020;
	};
    bool getName()
    {
        return Flags & 0x0020;
    };



    void setData(char* s)
    {
        memset(Data, 0, 8192);
		
        memcpy(Data, s, 8192);
    };
	
    bool Check(struct Header* h);
	void setChecksum(struct Header* h);
};

struct Header
{
    unsigned long src_IP;
    unsigned long dst_IP;
    char zero = 0;
    int Protocol = 17;
    int length = sizeof(struct  Message);
};

void Message::setChecksum(struct Header* h)
{
	this->CheckSum = 0;
	unsigned long sum = 0;
	int i=0;
	int count = sizeof(struct Header) / 2;
	while(count--)
	{
		sum += ((unsigned short*)h)[i];
		i++;
		if (sum & 0xffff0000)
		{
			sum &= 0x0000ffff;
			sum++;
		}
	}
	i=0;
	count = sizeof(struct Message) / 2;
	while(count--)
	{
		sum += ((unsigned short*)this)[i];
		i++;
		if (sum & 0xffff0000)
		{
			sum &= 0x0000ffff;
			sum++;
		}
	}		
	this->CheckSum = ~(sum & 0xffff);
}

bool Message::Check(struct Header* h)
{
	unsigned long sum = 0;
	int i=0;
	int count = sizeof(struct Header) / 2;
	while(count--)
	{
		sum += ((unsigned short*)h)[i];
		i++;
		if (sum & 0xffff0000)
		{
			sum &= 0x0000ffff;
			sum++;
		}
	}
	i=0;
	count = sizeof(struct Message) / 2;
	while(count--)
	{
		sum += ((unsigned short*)this)[i];
		i++;
		if (sum & 0xffff0000)
		{
			sum &= 0x0000ffff;
			sum++;
		}
	}
	return sum == 0x0000ffff;	
}

void printFlags(struct Message m)
{
	std::cout<<"[";
	if(m.getFin()) std::cout<<" FIN ";
	if(m.getSyn()) std::cout<<" SYN ";
	if(m.getRst()) std::cout<<" RST ";
	if(m.getPsh()) std::cout<<" PSH ";
	if(m.getAck()) std::cout<<" ACK ";

	std::cout<<"] ";
}

void printMess(struct Message m ,bool stc)
{
	printTime();
	if(stc)
	{
		std::cout<<"Server -> Client";
	}
	else
	{
		std::cout<<"Client -> Server";
	}
	printFlags(m);
	if(m.getAck())
	{
		std::cout<<"Seq="<<m.seq<<"  Ack="<<m.ack<<"   Len="<<m.datalen<<"   Win="<<m.Winsize<<std::endl;
	}
	else
	{
		std::cout<<"Seq="<<m.seq<<"   Len="<<m.datalen<<"   Win="<<m.Winsize<<std::endl;
	}
}

bool LossPackage() 
{
    
	Sleep(sleeptime);
	// return false;
	 return rand() % 100 < loss_rate;
}