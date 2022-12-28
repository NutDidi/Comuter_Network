#include<iostream>
#include <winsock.h>
#include <fstream>
#include<time.h>
#include <string>
#include<queue>
#include<vector>
#include "mess.h"

using namespace std;
#pragma comment(lib, "Ws2_32.lib")



const bool SEND = true;

SOCKET Server;

int cur_recvnum = 0;

// unsigned int SEQ = 0 ;
unsigned int lastrecvSEQ = 0;
// unsigned int ACK = 0 ;
unsigned int lastsendSEQ = 0 ;
unsigned int lastrecvACK = 0;
int sendwin;
int curwin = Max_window;
int lastwinsize;
int beginseq = 0;
clock_t filetimer;


struct Message sendbuf{Server_Port,Client_Port, 0, 0};
struct Message recvbuf;

// struct Message lastsend{Server_Port,Client_Port, 0, 0}; 
// std::queue<struct Message> lastsendQ;
std::vector<struct Message> sendQ;


struct Header sendHeader{inet_addr(ServerIP), inet_addr(ClientIP)};
struct Header recvHeader{inet_addr(ClientIP), inet_addr(ServerIP)};

void connect_shake();
bool recv_name(char *File_name);
void recv_content(char* &File_content,int& curlen);
void disconnect_wave();

int main()
{
    //检查连接
	WSADATA wsad;
	if (WSAStartup(MAKEWORD(2, 2), &wsad) != 0)
	{
		printTime();
		std::cout<<"[ Error ] Failed to load Winsock"<<endl;
		system("pause");
		return -1;
	}
	else 
	{
		printTime();
		std::cout<<"[ Log ] Success to load Winsock"<<endl;
	}


	//创建监听socket
	Server = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);

	if (Server == INVALID_SOCKET)
	{
		printTime();
		std::cout<<"[ Error ] Failed to creat socket : "<<WSAGetLastError()<<endl;
		system("pause");
		return -1;
	}
	else 
	{
		printTime();
		std::cout<<"[ Log ] Success to creat socket"<<endl;
	}

    //非阻塞
    int iMode = 1; 
    ioctlsocket(Server, FIONBIO, (u_long FAR*) & iMode);

	//进行绑定	
	Client_addr.sin_family = AF_INET;
    Client_addr.sin_addr.S_un.S_addr = inet_addr(ClientIP);
	Client_addr.sin_port = htons(Client_Port);	

	Server_addr.sin_family = AF_INET;
	Server_addr.sin_addr.S_un.S_addr = inet_addr(ServerIP);
	Server_addr.sin_port = htons(Server_Port);	
	if (bind(Server,(LPSOCKADDR)&Server_addr, sizeof(Server_addr)) == SOCKET_ERROR)
	{
		printTime();
		std::cout<<"[ Error ] Failed to bind : "<<WSAGetLastError()<<endl;
		system("pause");
		return -1;
	}
	else 
	{
		printTime();
		std::cout<<"[ Log ] Success to bind"<<endl;
	}


	std::cout<<"   Please Input 1 To Establish Connection:   ";
	char makesure[2];
	cin.getline(makesure,2);
	if(string(makesure) != "1")
	{
		printTime();
		std::cout<<"[ Error ] Connection rejected by server, program will be closed soon"<<endl;
		Sleep(2000);
		return 0;
	}

	clock_t endTime;
	clock_t beginTime;
	connect_shake();
	std::cout<<"-------------------------Transfer-------------------------"<<endl;
	std::cout<<endl;
	while(1)
	{

		// while(!lastsendQ.empty()) lastsendQ.pop();
		// sendQ.clear();
		beginseq = lastsendSEQ + 1;
		std::cout<<endl;
		std::cout<<"Waiting for the Client to select the file to send."<<endl;
		char Filename[100];
		memset(Filename, 0 ,100);
		char *Filecontent=new char[100000000];
		memset (Filecontent,0,100000000);
		int file_len =0;
		cur_recvnum = 0;
		curwin = Max_window;
		beginTime = clock();
		if (!recv_name(Filename)) break;
		printTime();
		std::cout<<"[ Log ] Recieving File ";
		std::cout<<Filename <<endl;

		recv_content(Filecontent,file_len);
		endTime = clock();

		char dir[100] = "./3-Server/";

		sprintf(dir,"%s%s",dir,Filename);
		ofstream outputfile(dir,ios::binary|ios::app);

		for (int i = 0; i <file_len; i++) 
		{
			outputfile << Filecontent[i];
		}
		outputfile.flush();
		outputfile.close();

		printTime();
		std::cout<<"[ Log ] File ";
		std::cout<<Filename ;
		std::cout<<" has been written to destination folder. "<<endl;;		
	}
	
	std::cout<<endl;
	disconnect_wave();

	closesocket(Server);
	WSACleanup();
	system("pause");

	
}


void connect_shake()
{
	clock_t startTime;
	while(1)
	{
	//Frist Handshake-recvfrom
	//Seq = 0, Flags(Syn)
		int l = sizeof(Client_addr);
		while(recvfrom(Server, (char*)&recvbuf, sizeof(struct Message), 0, (sockaddr*)&Client_addr, &l) == SOCKET_ERROR);

		if (recvbuf.Flags == 2 && recvbuf.Check(&recvHeader))   //正确
		{
			printTime();
			std::cout<<"[ Log ] Receive First Handshake"<<endl;
			printMess(recvbuf,!SEND);
			// ACK = recvbuf.seq + 1;
			lastrecvSEQ = recvbuf.seq;
		}
		else
		{
			if(!recvbuf.Check(&recvHeader))
			{
				printTime();
				std::cout<<"[Error] Checksum Error"<<endl;
			}
			else
			{
				printTime();
				std::cout<<"[ Error ] Wrong Message Handshake1"<<endl;
			}
			continue;                            //  校验和出错，等待重传 || 接收到的不是First Handshake，等待重传
		}
		while(1)
		{
		//Second Handshake-sendto
		//Seq = 0, Ack = 0(seq), Flags(Syn,Ack)
			sendbuf.seq = lastsendSEQ + 1;
			sendbuf.ack = lastrecvSEQ;
			sendbuf.Flags = 0;
			sendbuf.setAck();
			sendbuf.setSyn();
			sendbuf.setChecksum(&sendHeader);
			int s = sendto(Server, (char*)&sendbuf, sizeof(struct Message), 0, (sockaddr*)&Client_addr, l);
			if (s == SOCKET_ERROR)
			{
				printTime();
				std::cout<<"[ Error ] Filed To Send Second Handshake Message"<<endl;
				continue;                     //发送失败，重发
			}
			else
			{
			//发送成功
				printTime();
				std::cout<<"[ Log ] Send Second Handshake"<<endl;
				printMess(sendbuf,SEND);
				// SEQ++;
				startTime = clock();//计时
				lastsendSEQ = sendbuf.seq;
				break;
			}
		}


		while (1)
		{
		//Third Handshake-recvfrom
		//Seq = 1, Ack = 1, Flags(Syn,Ack)
			while(recvfrom(Server, (char*)&recvbuf, sizeof(struct Message), 0, (sockaddr*)&Client_addr, &l) == SOCKET_ERROR)
			{
				if(clock() - startTime >= Max_waitTime)
				{
					printTime();
					std::cout<<"[ Log ] Timeout! Retransmit data."<<endl;
					//重传
					while(1)
					{
						int s = sendto(Server, (char*)&sendbuf, sizeof(struct Message), 0, (sockaddr*)&Client_addr, l);
						if (s == SOCKET_ERROR)
						{
							printTime();
							std::cout<<"[ Error ] Filed To Send Second Handshake Message"<<endl;
							continue;                     //发送失败，重发
						}
						else
						{
						//发送成功
							printTime();
							std::cout<<"[ Log ] Resend Second Handshake"<<endl;
							printMess(sendbuf,SEND);
							// SEQ++;
							lastsendSEQ = sendbuf.seq;
							startTime = clock();            //计时
							break;                          //跳出send
						}
					}
				}
			}

			//接收成功
			if (recvbuf.Flags == 16 &&  recvbuf.Check(&recvHeader) && recvbuf.seq ==lastrecvSEQ+1 && recvbuf.ack == lastsendSEQ)
			{
				printTime();
				std::cout<<"[ Log ] Receive Third Handshake"<<endl;
				printMess(recvbuf,!SEND);	
				// ACK = recvbuf.seq +1;
				lastrecvSEQ = recvbuf.seq;
				return;
			}
			else
			{
				startTime = clock(); //计时
				if(!recvbuf.Check(&recvHeader))
				{
					printTime();
					std::cout<<"[ Error ] Checksum Error"<<endl;
				}
				else
				{
					printTime();
					// std::cout<<recvbuf.Flags<<" "<<recvbuf.seq<<" "<<recvbuf.ack<<endl;
					// cout<<"lastrecvSEQ:    "<<lastrecvSEQ<<"sendbuf.seq +1       "<<sendbuf.seq +1<<"SEQ        "<<SEQ<<endl;
					std::cout<<"[ Error ] Wrong Message  Handshake3"<<endl;
				}

				continue;                      //校验和出错，等待重传 || 接收到的不是Third Handshake，等待重传
			}			
		}
		


	}
}

bool recv_name(char *File_name)
{
	struct Message recvname {};
	int l = sizeof(Client_addr);
	bool esc = false;
	while(1)
	{
		while(recvfrom(Server, (char*)&recvname, sizeof(struct Message), 0, (sockaddr*)&Client_addr, &l) == SOCKET_ERROR)
		{
			if(clock() - filetimer >=10*Max_waitTime)
			{
				int  s = sendto(Server, (char*)&sendQ.back(), sizeof(struct Message), 0, (sockaddr*)&Client_addr, l);
			}
		}
		if((recvname.Check(&recvHeader)) && (recvname.Flags == 48) && (string(recvname.Data) == "Exit") && (recvname.seq ==lastrecvSEQ + 1) )
		{
			printTime();
			std::cout<<"[ Log ] Delivery interruption request received"<<endl;
			printMess(recvname,!SEND);	
			esc = true;
			lastrecvSEQ = recvname.seq;
			lastrecvACK = recvname.ack;
		}
		else
		{
			if( (recvname.Check(&recvHeader)) && (recvname.Flags == 48) && (string(recvname.Data) != "Exit"))
			{
				printTime();
				std::cout<<"[ Log ] Receive File Named"<<recvname.Data<<endl;
				printMess(recvname,!SEND);

				memset(File_name,0,sizeof(File_name));
				memcpy(File_name,recvname.Data,recvname.datalen);

				lastrecvSEQ = recvname.seq;
			}
			else
			{
				if(!recvname.Check(&recvHeader))
				{
					printTime();
					std::cout<<"[ Error ] Checksum Error"<<endl;
				}
				continue;
			}
		}

		sendQ.clear();

		struct Message sendname{Server_Port , Client_Port ,  lastsendSEQ + 1 , lastrecvSEQ};
		sendname.setName();
		sendname.setAck();
		sendname.setChecksum(&sendHeader);

		while(1)
		{
			int s = sendto(Server, (char*)&sendname, sizeof(struct Message), 0, (sockaddr*)&Client_addr, l);
			if (s == SOCKET_ERROR)
			{
				printTime();
				std::cout<<"[ Error ] Filed To Send File Name Ack"<<endl;
			}
			else
			{
				if(!esc)
				{
					printTime();
					std::cout<<"[ Log ] Send File Name Ack"<<endl;
					printMess(sendname,SEND);
					// SEQ++;
					lastsendSEQ = sendname.seq;
					sendQ.push_back(sendname);
					// lastsend = sendname;
					filetimer = clock();  //不退出，计时
					return true;
				}
				else if(esc)
				{
					printTime();
					std::cout<<"[ Log ] Send Exit Ack"<<endl;
					printMess(sendname,SEND);
					// SEQ++;
					lastsendSEQ = sendname.seq;
					return false;				
				}
			}
		}

	}	
}

void recv_content(char* &File_content,int& curlen)
{
	int resendcount = 0;
	int packnum = 0;
	char* Content = new char [100000000];
	memset(Content, 0 ,100000000);
	curlen =0;
	while(1)
	{
		if(curwin>0)
		{
			struct Message recvcontent {};
			int l = sizeof(Client_addr);
			int r = recvfrom(Server, (char*)&recvcontent, sizeof(struct Message), 0, (sockaddr*)&Client_addr, &l);
			//非尾
			if( r != SOCKET_ERROR && (recvcontent.Check(&recvHeader)) && (recvcontent.Flags == 16) && recvcontent.seq == lastrecvSEQ + 1 )
			{
				packnum++;
				printMess(recvcontent,!SEND);
				memcpy(Content+curlen,recvcontent.Data,recvcontent.datalen);
				curlen += recvcontent.datalen;
				lastrecvSEQ = recvcontent.seq;
				lastrecvACK = recvcontent.ack;
				cur_recvnum++;
				filetimer = clock();
                

					struct Message sendcontent{Server_Port , Client_Port ,  lastsendSEQ + 1 , lastrecvSEQ};
					sendcontent.Flags = 0;
					sendcontent.setAck();
					sendcontent.Winsize = curwin;
					sendcontent.setChecksum(&sendHeader);

						int s = sendto(Server, (char*)&sendcontent, sizeof(struct Message), 0, (sockaddr*)&Client_addr, l);
							
							printMess(sendcontent,SEND);
							lastsendSEQ = sendcontent.seq;
							sendQ.push_back(sendcontent);
							cur_recvnum = 0;
							
				lastwinsize = recvcontent.Winsize;
				continue;
			}
			else 
			{
				//尾部
				if( r != SOCKET_ERROR && (recvcontent.Check(&recvHeader)) && (recvcontent.Flags == 24) && recvcontent.seq == lastrecvSEQ+1  && recvcontent.ack <= lastsendSEQ)
				{
					
					packnum++;
					
					printMess(recvcontent,!SEND);
					memcpy(Content+curlen,recvcontent.Data,recvcontent.datalen);
					curlen += recvcontent.datalen;
					lastrecvSEQ = recvcontent.seq;
					lastrecvACK = recvcontent.ack;
					cur_recvnum++;
					filetimer = clock();

						struct Message sendcontent{Server_Port , Client_Port ,  lastsendSEQ + 1 , lastrecvSEQ};
						sendcontent.Flags = 0;
						sendcontent.setAck();
						sendcontent.Winsize = curwin;
						sendcontent.setChecksum(&sendHeader);
					
							int s = sendto(Server, (char*)&sendcontent, sizeof(struct Message), 0, (sockaddr*)&Client_addr, l);

								printMess(sendcontent,SEND);
								lastsendSEQ = sendcontent.seq;
								sendQ.push_back(sendcontent);

								cur_recvnum = 0;
					
					File_content = Content;
					return;
				}
				else
				{
					if(clock()-filetimer > Max_waitTime)
					{	
						int s = sendto(Server, (char*)&sendQ.back(), sizeof(struct Message), 0, (sockaddr*)&Client_addr, l);
						std::cout<<"[ Log ] Resend File Data Block Ack"<<endl;
						printMess(sendQ.back(),SEND);				
						continue;
					}
				}
			}		
			
		}
	}
}

void disconnect_wave()
{
	clock_t startTime;
	while(1)
	{
//Frist Handwave-recvfrom
//Seq = x, Ack = ?, Flags(Fin Ack)
		int l = sizeof(Client_addr);
		while(recvfrom(Server, (char*)&recvbuf, sizeof(struct Message), 0, (sockaddr*)&Client_addr, &l) == SOCKET_ERROR);

		if (recvbuf.Flags == 17 && recvbuf.Check(&recvHeader) && recvbuf.seq == lastrecvSEQ+1 && recvbuf.ack == lastsendSEQ)
		{
			printTime();
			std::cout<<"Receive First Handwave"<<endl;
			printMess(recvbuf,!SEND);
			// ACK = recvbuf.seq +1;
			lastrecvSEQ = recvbuf.seq;
		}
		else
		{
			if(!recvbuf.Check(&recvHeader))
			{
				printTime();
				std::cout<<"[ Error ] Checksum Error"<<endl;
			}
			else
			{
				printTime();
				std::cout<<"[ Error ] Wrong Message"<<endl;
			}
			continue;
		}

//Second Handwave-sendto
//Seq = ?, Ack = x+1, Flags(Ack)
		sendbuf.seq = lastsendSEQ + 1;
		sendbuf.ack = lastrecvSEQ ;
		sendbuf.Flags = 0;
		sendbuf.setAck();
		sendbuf.setChecksum(&sendHeader);
		while(1)
		{
			int s = sendto(Server, (char*)&sendbuf, sizeof(struct Message), 0, (sockaddr*)&Client_addr, l);
			if (s == SOCKET_ERROR)
			{
				printTime();
				std::cout<<"[ Error ] Filed To Send Second Handwave Message"<<endl;
				continue;
			}
			else
			{
				printTime();
				std::cout<<"[ Log ] Send Second Handwave"<<endl;
				printMess(sendbuf,SEND);
				lastsendSEQ = sendbuf.seq;
				break;
				// SEQ++;
			}
		}


//Third Handwave-sendto
//Seq = ?, Ack = x+1, Flags(Fin Ack)
		// struct Message Sendbuf	{Server_Port , Client_Port ,  recvbuf.ack , recvbuf.seq +1};
		sendbuf.seq = lastsendSEQ + 1;
		sendbuf.ack = lastrecvSEQ;
		sendbuf.Flags = 0;		
		sendbuf.setAck();
		sendbuf.setFin();
		sendbuf.setChecksum(&sendHeader);
		while(1)
		{
			int S = sendto(Server, (char*)&sendbuf, sizeof(struct Message), 0, (sockaddr*)&Client_addr, l);
			if (S == SOCKET_ERROR)
			{
				printTime();
				std::cout<<"[ Error ] Filed To Send Third Handwave Message"<<endl;
				continue;
			}
			else
			{
				printTime();
				std::cout<<"[ Log ] Send Third Handwave"<<endl;
				printMess(sendbuf,SEND);
				lastsendSEQ = sendbuf.seq;
				startTime = clock();
				break;
			}
		}


//Fourth Handwave-recvfrom
//Seq = x+1(ack), Ack = ?+1(seq+1), Flags(Ack)
		while(1)
		{
			int l = sizeof(Client_addr);
			while(recvfrom(Server, (char*)&recvbuf, sizeof(struct Message), 0, (sockaddr*)&Client_addr, &l) == SOCKET_ERROR)
			{
				if( clock()- startTime >= Max_waitTime)
				{
					printTime();
					std::cout<<"[ Log ] Timeout! Retransmit data."<<endl;
					//重传
					sendbuf.seq = lastsendSEQ + 1;
					sendbuf.ack = lastrecvSEQ;
					sendbuf.Flags = 0;		
					sendbuf.setAck();
					sendbuf.setFin();
					sendbuf.setChecksum(&sendHeader);
					while(1)
					{
						int S = sendto(Server, (char*)&sendbuf, sizeof(struct Message), 0, (sockaddr*)&Client_addr, l);
						if (S == SOCKET_ERROR)
						{
							printTime();
							std::cout<<"[ Error ] Filed To Send Third Handwave Message"<<endl;
							continue;
						}
						else
						{
							printTime();
							std::cout<<"[ Log ] Send Third Handwave"<<endl;
							printMess(sendbuf,SEND);
							lastsendSEQ = sendbuf.seq;
							startTime = clock();
							break;
						}
					}

				}
					
				


			}

			if (recvbuf.Flags == 16 && recvbuf.Check(&recvHeader) && recvbuf.seq == lastrecvSEQ+1 && recvbuf.ack == lastsendSEQ)
			{
				printTime();
				std::cout<<"[ Log ] Receive Fourth Handwave"<<endl;
				printMess(recvbuf,!SEND);
				// ACK = recvbuf.seq +1;
				return;
			}
			else
			{
				if(!recvbuf.Check(&recvHeader))
				{
					printTime();
					std::cout<<"[ Error ] Checksum Error"<<endl;	
				}
				else{
					printTime();
					std::cout<<"[ Error ] Wrong Message"<<endl;						
				}
				continue;
			}
		}
	}
}

