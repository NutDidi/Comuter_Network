#include<iostream>
#include<winsock.h>
#include<string.h>
#include<time.h> 
#include<sstream>
#include<list>
#include <algorithm>
#pragma comment(lib, "Ws2_32.lib")

using namespace std;
#define max_buf 1062
const int PORT = 5001;//端口号
#define IP "127.0.0.1"//IP
const int maxlen = 10;

int client_num=0;
list<SOCKET>  Client_Socket;


void getTime(char* s)
{	
	SYSTEMTIME sys; 
    GetLocalTime( &sys ); 
    sprintf( s," %4d/%02d/%02d %02d:%02d:%02d ",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond); 
}

void printTime()
{	
	SYSTEMTIME sys; 
    GetLocalTime( &sys ); 
    printf( "[%4d/%02d/%02d %02d:%02d:%02d] ",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond); 

}

void writebuf(char* sendbuf, char* msg, char* timemsg,int flag)
{
    if (flag == 0) sendbuf[0]='2';//来自服务器
	else sendbuf[0] = '1';//允许退出
	int index=1;
	for(int i=0;i<strlen(msg);i++)
	{
		sendbuf[index]=msg[i];
		index++;
	}
	sendbuf[index]='(';
    for(int i=0;i<strlen(timemsg);i++)
    {
        index++;
        sendbuf[index]=timemsg[i];
    }    
    index++;
    sendbuf[index]=')';   
    return; 
    
}
//聊天线程
DWORD WINAPI CommunicateThread(LPVOID X)
{
	SOCKET clientsocket =(SOCKET)X;//当前接收消息的客户
	char sendtime[19];
	char recbuf[max_buf];//接收消息缓冲区
	char sendbuf[max_buf];

	memset(sendtime,'\0',sizeof(sendtime));
	memset(recbuf, '\0', sizeof(recbuf));
    memset(sendbuf, '\0', sizeof(sendbuf));


	int rec=0;

	//接收消息
	while(1)
	{
		//清空缓冲区
		memset(sendtime,'\0',sizeof(sendtime));
		memset(recbuf, 0, max_buf);
		rec = recv(clientsocket, recbuf, 1024, 0);
		if(string(recbuf).length() <= 0) continue;
		if (rec == SOCKET_ERROR)//接收失败,断开连接
		{
			printTime();
			cout<<"Failed to receive message from "<<clientsocket<<endl;//log
			printTime();
			cout<<"Connect with Client "<<clientsocket<<" breaks"<<endl;//log
			client_num--;
			list<SOCKET>::iterator itePos = find(Client_Socket.begin(),Client_Socket.end(),clientsocket);
			itePos = Client_Socket.erase(itePos);//请出链表
			printTime();
			cout<<"There are "<<client_num<<" people online"<<endl;//log										
			closesocket(clientsocket);//关闭客户套接字		
			return -1;	

		}
		else
		{	
			if(recbuf[0] == '1') //接收的是退出请求
			{
				client_num--;
				getTime(sendtime);
				writebuf(sendbuf,"Exit successfully ! Thanks for using.",sendtime, 1);
				if (send(clientsocket, sendbuf, 1024, 0) == SOCKET_ERROR)
				{
					printTime();
					cout<<"[SendError]send server mseeage to "<< clientsocket <<" failed "<<endl;
					continue;					
				}
				else
				{
					printTime();
					cout<<"[Send]send server message to "<< clientsocket<<endl;
					printTime();
					cout<<"Client "<<clientsocket<<" exits"<<endl;
				}
				//断连，移出链表，连接数-1
				list<SOCKET>::iterator itePos = find(Client_Socket.begin(),Client_Socket.end(),clientsocket);
				itePos = Client_Socket.erase(itePos);
				printTime();
				cout<<"There are "<<client_num<<" people online"<<endl;												
				return 0;					
			}		
			else 
			if(recbuf[0] == '0') //接收的是聊天消息
			{
				printTime();
				cout<<"[Recieve]";
				for(int i=1;i<strlen(recbuf);i++) cout<<recbuf[i];
				cout<<endl;

				for (list<SOCKET>::iterator i = Client_Socket.begin(); i !=  Client_Socket.end();i++)
				{		
					if(send(*i, recbuf, strlen(recbuf), 0) == SOCKET_ERROR)
					{
						printTime();
						cout<<"[SendError]send chat message to "<< *i <<" failed "<<endl;
						continue;
					}
					else 
					{
						printTime();
						cout<<"[Send]send chat message to "<< *i <<endl;	
					}				
				}
			}
			else
			if(recbuf[0] == '2')//接收的是登陆消息
			{	
				getTime(sendtime);
				writebuf(sendbuf,"Success to login in!",sendtime, 0);
				if (send(clientsocket, sendbuf, 1024, 0) == SOCKET_ERROR)
				{
					printTime();
					cout<<"[SendError]send login mseeage to "<< clientsocket <<" failed "<<endl;
					continue;
				}
				else
				{
					printTime();
					cout<<"[Send]send to "<< clientsocket<<endl;
					printTime();
					cout<<"[Login]Client "<<clientsocket<<" log in"<<endl;					
				}

			}	
		}

	}
	closesocket(clientsocket);
	return 0;
}


int main()
{
	cout<<"================================================================================="<<endl;
	cout<<"================================= Server ========================================"<<endl;
	cout<<"============================== IP : 127.0.0.1 ==================================="<<endl;
	cout<<"=============================== Port : 5001 ====================================="<<endl;
	cout<<"================================================================================="<<endl;


	//检查连接
	WSADATA wsad;
	if (WSAStartup(MAKEWORD(2, 2), &wsad) != 0)
	{
		printTime();
		cout<<"Failed to load Winsock"<<endl;
		system("pause");
		return -1;
	}
	else 
	{
		printTime();
		cout<<"Success to load Winsock"<<endl;
	}
	//创建监听socket
	SOCKET listen_socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (listen_socket == INVALID_SOCKET)
	{
		printTime();
		cout<<"Failed to creat socket : "<<WSAGetLastError()<<endl;
		system("pause");
		return -1;
	}
	else 
	{
		printTime();
		cout<<"Success to creat socket"<<endl;
	}
	//进行绑定	
	SOCKADDR_IN listen_addr;
	listen_addr.sin_family = AF_INET;
	listen_addr.sin_addr.S_un.S_addr = inet_addr(IP);
	listen_addr.sin_port = htons(PORT);	
	if (bind(listen_socket,(LPSOCKADDR)&listen_addr, sizeof(listen_addr)) == SOCKET_ERROR)
	{
		printTime();
		cout<<"Failed to bind : "<<WSAGetLastError()<<endl;
		system("pause");
		return -1;
	}
	else 
	{
		printTime();
		cout<<"Success to bind"<<endl;
	}
	//进行监听
	if (listen(listen_socket,maxlen) == SOCKET_ERROR)
	{
		printTime();
		cout<<"Failed to listen : "<<WSAGetLastError()<<endl;
		system("pause");
		return -1;
	}
	else 
	{
		printTime();
		cout<<"Begin to listen"<<endl;;
	}

	//服务端接收客户端请求
	while(1)
	{
		if(client_num>1024) continue;
		SOCKET client =accept(listen_socket,0,0);
		if (client == INVALID_SOCKET )//不合法
		{
			printTime();
			cout<< "Invalid Socket!"<<endl;
			closesocket(listen_socket);
			WSACleanup();
			system("pause");
			return -1;
		}
		else 
		{
			//合法
			client_num++;
			HANDLE hThread;
			printTime();
			Client_Socket.push_back(client);
			cout<<"Client "<< client <<" has connect to server successfully"<<endl;
			hThread = CreateThread(NULL, NULL,&CommunicateThread, (LPVOID)client, 0, NULL);
			CloseHandle(hThread);
		}
		
	}
	closesocket(listen_socket);
	WSACleanup();
	system("pause");
	return 0;
}