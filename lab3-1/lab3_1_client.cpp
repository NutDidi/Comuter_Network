#include<iostream>
#include <winsock.h>
#include <fstream>
#include<time.h>
#include <string>
#include<math.h>
#include "mess.h"

using namespace std;
#pragma comment(lib, "Ws2_32.lib")
#define IP "127.9.22.98"
#define PORT 5002
const bool SEND = false;

SOCKET Client;

unsigned int lastrecvSEQ = 0;
unsigned int lastsendSEQ = 0 ;

struct Message sendbuf{Client_Port , Server_Port, 0, 0};
struct Message recvbuf;
struct Message lastsend;

struct Header sendHeader{inet_addr(IP), inet_addr(IP)};
struct Header recvHeader{inet_addr(IP), inet_addr(IP)};

int l = sizeof(Server_addr);

void connect_shake();
void send_data(char*,char *,int);
void send_name(char* name, bool esc);
void disconnect_wave();

int main()
{
    WSADATA wsad;
	if (WSAStartup(MAKEWORD(2, 2), &wsad) != 0)
	{
		printTime();
		std::cout<<"[ Error ] Failed to load Winsock"<<endl;
		std::system("pause");
		return -1;
	}
	else 
	{
		printTime();
		std::cout<<"[ Log ] Success to load Winsock"<<endl;
	}

	//创建socket
	Client = socket(AF_INET, SOCK_DGRAM, 0);
	if (Client == INVALID_SOCKET)
	{
		printTime();
		std::cout<<"[ Error ] Failed to creat socket : "<<WSAGetLastError()<<endl;
        closesocket(Client);
        WSACleanup();
		std::system("pause");
		return -1;
	}
	else 
	{
		printTime();
		std::cout<<"[ Log ] Success to creat socket"<<endl;
	} 

    //非阻塞
    int iMode = 1; 
    ioctlsocket(Client, FIONBIO, (u_long FAR*) & iMode);
	
	Client_addr.sin_family = AF_INET;
    Client_addr.sin_addr.S_un.S_addr = inet_addr(IP);
	Client_addr.sin_port = htons(Client_Port);	


	Server_addr.sin_family = AF_INET;
	Server_addr.sin_addr.S_un.S_addr = inet_addr(IP);
	Server_addr.sin_port = htons(Server_Port);	


	std::cout<<"   Please Input 1 To Establish Connection:   ";
	char makesure[2];
	cin.getline(makesure,2);
	if(string(makesure) != "1")
	{
		printTime();
		std::cout<<"[ Error ] Connection rejected by client, program will be closed soon"<<endl;
		Sleep(2000);
		return 0;
	}
//建立连接

	connect_shake();

	std::cout<<"-------------------------Transfer-------------------------"<<endl;
	cout<<endl;
	std::cout<<"              Select the file you want to transfer:              "<<endl;
	std::cout<<"  0 ---  Finish  1 ---  1.jpg  2 --- 2.jpg  3 --- 3.jpg  4 --- helloworld.txt "<<endl;

	clock_t endTime;
	clock_t beginTime;
//读取文件
	while(1)
	{
		char *File_content;
		File_content = new char [100000000];             //文件内容
		memset(File_content, 0, 100000000);
		int filelen = 0;                                 //文件长度

		int file_num;                                    //文件序号（读入）
		cout<<endl;
		printTime();
		std::cout<<"Input the index of file : ";
		cin>>file_num;
	//根据序号，选择读入。
		switch (file_num)
		{
			case 0:
			{
				printTime();
				std::cout<<"File Transfer Stopped."<<endl;
				char File_name[]="Exit";
				send_name(File_name,true);
				break;
			}
			case 1:
			{
				printTime();
				std::cout<<"[ Log ] About to transfer file 1.jpg..."<<endl;
				ifstream readfile("F:/computer_network/3-Client/1.jpg",std::ios::in|ios::binary);
				if(!readfile)
				{
					printTime();
					std::cout<<"[ Error ] Filed To Open 1.jpg"<<endl;
					continue;
				}
				
				BYTE b = readfile.get();
				while(readfile)
				{
					File_content[filelen++] = b;
					b = readfile.get();
				}
				readfile.close();

				printTime();
				std::cout<<"[ Log ] 1.jpg has been opened, total length is "<<filelen<<endl;
				char File_name[]="1.jpg";
				beginTime = clock();
				send_data(File_name,File_content,filelen);
				endTime = clock();
				printTime();
				std::cout<<"[ Log ] 1.jpg transmission completed. Cost "<<((double)(endTime-beginTime))/CLK_TCK<<"s"<<endl;
				break;
			}
			case 2:
			{
				printTime();
				std::cout<<"[ Log ] About to transfer file 2.jpg..."<<endl;
				ifstream readfile("F:/computer_network/3-Client/2.jpg",std::ios::in|ios::binary);
				if(!readfile)
				{
					printTime();
					std::cout<<"[ Error ] Filed To Open 2.jpg"<<endl;
					continue;
				}
				
				BYTE b = readfile.get();
				while(readfile)
				{
					File_content[filelen++] = b;
					b = readfile.get();
				}
				readfile.close();

				printTime();
				std::cout<<"[ Log ] 2.jpg has been opened, total length is "<<filelen<<endl;
				char File_name[]="2.jpg";
				beginTime = clock();
				send_data(File_name, File_content,filelen);
				endTime = clock();
				printTime();
				std::cout<<"[ Log ] 2.jpg transmission completed. Cost "<<((double)(endTime-beginTime))/CLK_TCK<<"s"<<endl;
				break;
			}
			case 3:
			{
				printTime();
				std::cout<<"[ Log ] About to transfer file 3.jpg..."<<endl;
				ifstream readfile("F:/computer_network/3-Client/3.jpg",std::ios::in|ios::binary);
				if(!readfile)
				{
					printTime();
					std::cout<<"[ Error ] Filed To Open 3.jpg"<<endl;
					continue;
				}
				
				BYTE b = readfile.get();
				while(readfile)
				{
					File_content[filelen++] = b;
					b = readfile.get();
				}
				readfile.close();

				printTime();
				std::cout<<"[ Log ] 3.jpg has been opened, total length is "<<filelen<<endl;
				char File_name[]="3.jpg";
				beginTime = clock();
				send_data(File_name, File_content,filelen);
				endTime = clock();
				printTime();
				std::cout<<"[ Log ] 3.jpg transmission completed. Cost "<<((double)(endTime-beginTime))/CLK_TCK<<"s"<<endl;

				break;
			}
			case 4:
			{
				printTime();
				std::cout<<"[ Log ] About to transfer file helloworld.txt..."<<endl;
				ifstream readfile("F:/computer_network/3-Client/helloworld.txt",std::ios::in|ios::binary);
				if(!readfile)
				{
					printTime();
					std::cout<<"[ Error ] Filed To Open helloworld.txt"<<endl;
					continue;
				}
				
				BYTE b = readfile.get();
				while(readfile)
				{
					File_content[filelen++] = b;
					b = readfile.get();
				}
				readfile.close();

				printTime();
				std::cout<<"[ Log ] helloworld.txt has been opened, total length is "<<filelen<<endl;
				char File_name[]="helloworld.txt";
				beginTime = clock();
				send_data(File_name, File_content,filelen);
				endTime = clock();
				printTime();
				std::cout<<"[ Log ] helloworld.txt transmission completed. Cost "<<((double)(endTime-beginTime))/CLK_TCK<<"s"<<endl;

				break;
			}
			default:
			{
				printTime();
				std::cout<<"[ Error ] Wrong number"<<endl;
				continue;
			}
		}
		if (file_num == 0) break;
	}

	std::cout<<endl;
	disconnect_wave();

	closesocket(Client);
	WSACleanup();
	std::system("pause");
	return 0;
}


void connect_shake()
{
	clock_t startTime;
	while(1)
	{
	//Frist Handshake-sendto
	//Seq = 0, Flags(Syn)
		sendbuf.seq = 0;
		sendbuf.ack = 0;
		sendbuf.Flags = 0;
		sendbuf.setSyn();
		sendbuf.setChecksum(&sendHeader);
		while(1)
		{
			int s = sendto(Client, (char*) &sendbuf, sizeof(struct Message), 0, (sockaddr*)&Server_addr, l );
			if (s == SOCKET_ERROR)
			{
				printTime();
				std::cout<<"[ Error ] Filed To Send First Handshake Message"<<endl;
				continue;                //发送失败，重发
			}
			else
			{
				printTime();
				std::cout<<"[ Log ] Send First Handshake"<<endl;
				printMess(sendbuf,SEND);
				// SEQ++;
				startTime = clock();
				lastsendSEQ = sendbuf.seq;
				break;
			}
		}

		//Second Handshake-recvfrom
		//Seq = 0, Ack = 1, Flags(Syn,Ack)
		while(1)
		{
			int l = sizeof(Server_addr);

			while(recvfrom(Client, (char*) &recvbuf, sizeof(struct Message), 0, (sockaddr*)&Server_addr, &l) == SOCKET_ERROR)
			{
				if(clock() - startTime >= Max_waitTime)
				{
					printTime();
					std::cout<<"[ Log ] Timeout! Retransmit data."<<endl;
					//重传
					while(1)
					{
						int s = sendto(Client, (char*) &sendbuf, sizeof(struct Message), 0, (sockaddr*)&Server_addr, l );
						if (s == SOCKET_ERROR)
						{
							printTime();
							std::cout<<"[ Error ] Filed To Send First Handshake Message"<<endl;
							continue;             //发送失败，重发
						}
						else
						{
						//发送成功
							printTime();
							std::cout<<"[ Log ] Resend First Handshake"<<endl;
							printMess(sendbuf,SEND);
							// SEQ++;
							lastsendSEQ = sendbuf.seq;
							startTime = clock();   //计时
							break;                 //跳出send
						}
					}

				}
			}

			if (recvbuf.Flags == 18 && recvbuf.Check(&recvHeader) )
			{
				printTime();
				std::cout<<"[ Log ] Receive Second Handshake"<<endl;
				printMess(recvbuf,!SEND);
				// ACK = recvbuf.seq + 1;
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
					std::cout<<"[ Error ] Wrong Message Handshake2"<<endl;					
				}
				continue;       //校验和出错，等待重传 || 接收到的不是Second Handshake，等待重传
			}

			//Third Handshake-sendto
			//Seq = 1, Ack = 1, Flags(Ack)
			sendbuf.seq = lastsendSEQ+1;
			sendbuf.ack =  lastrecvSEQ + 1;
			sendbuf.Flags = 0;
			sendbuf.setAck();
			sendbuf.setChecksum(&sendHeader);
			while(1)
			{
				int s = sendto(Client, (char*) &sendbuf, sizeof(struct Message), 0, (sockaddr*)&Server_addr, l);
				if (s == SOCKET_ERROR)
				{
					printTime();
					std::cout<<"[ Error ] Filed To Send Third Handshake Message"<<endl;
					continue;         //重发
				}
				else
				{
					printTime();
					std::cout<<"[ Log ] Send Third Handshake"<<endl;
					printMess(sendbuf,SEND);
					// SEQ++;
					lastsendSEQ = sendbuf.seq;
					return;
				}
			}
		
		}
	}
}

void send_name(char* name, bool esc)
{
	clock_t startTime;
	struct Message sendname{Client_Port , Server_Port ,  lastsendSEQ+1 , lastrecvSEQ + 1};
	sendname.datalen = strlen(name);
	sendname.setName();
	sendname.setAck();
	sendname.setData(name);
	sendname.setChecksum(&sendHeader);

		int s = sendto(Client, (char*) &sendname, sizeof(struct Message), 0, (sockaddr*)&Server_addr, l );

			printTime();
			if(!esc) std::cout<<"[ Log ] Send File Name "<<name<<endl;
			else
			{
				if(esc)
				{
					std::cout<<"[ Log ] Request to end transmission."<<endl;
				}
			}
		printMess(sendname,SEND);
		lastsendSEQ = sendname.seq;
		startTime = clock();

		
	while(1)
	{
		struct Message recvname;
		int r = recvfrom(Client, (char*) &recvname, sizeof(struct Message), 0, (sockaddr*)&Server_addr, &l);

		if (r != SOCKET_ERROR && recvname.Flags == 48 && recvname.Check(&recvHeader) && recvname.seq == lastrecvSEQ+1 && recvname.ack == lastsendSEQ + 1 )
		{
			printMess(recvname,SEND);
			lastrecvSEQ = recvname.seq;
			return;
		}
		else
		{
			if(clock()-startTime >= Max_waitTime)
			{
					int s = sendto(Client, (char*) &sendname, sizeof(struct Message), 0, (sockaddr*)&Server_addr, l );

					printMess(sendname,SEND);
					lastsendSEQ = sendname.seq;
					startTime = clock();								
			}
			continue;			
		}		
	}
}

void send_content(char* content,int contentlen,int i,bool k)//是否为尾
{
	clock_t startTime;
	if( !k)
	{
		struct Message sendcontent{Client_Port , Server_Port ,  lastsendSEQ+1 , lastrecvSEQ + 1};
		sendcontent.datalen = contentlen;
		sendcontent.setAck();
		sendcontent.setData(content);
		sendcontent.setChecksum(&sendHeader);

		bool LOSS = LossPackage();
		int s;	
		if(!LOSS)	
		{
			s = sendto(Client, (char*) &sendcontent, sizeof(struct Message), 0, (sockaddr*)&Server_addr, l );
		} 
		else
		{
			printTime();
			cout<<"Loss Seq"<<lastsendSEQ+1<<endl;			
		}
		if(!LOSS) printMess(sendcontent,SEND);
		lastsendSEQ = sendcontent.seq;
		startTime = clock();
			

		while(1)
		{
			struct Message recvcontent;
			int r = recvfrom(Client, (char*) &recvcontent, sizeof(struct Message), 0, (sockaddr*)&Server_addr, &l);
		
			if (r!= SOCKET_ERROR && recvcontent.Flags == 16 && recvcontent.Check(&recvHeader) && recvcontent.seq == lastrecvSEQ +1 && recvcontent.ack == lastsendSEQ +1)
			{
				printMess(recvcontent,!SEND);
				lastrecvSEQ = recvcontent.seq;
				return;
			}
			else
			{
				if(clock()-startTime >= Max_waitTime)
				{
						int s = sendto(Client, (char*) &sendcontent, sizeof(struct Message), 0, (sockaddr*)&Server_addr, l );

						printMess(sendcontent,SEND);
						lastsendSEQ = sendcontent.seq;
						startTime = clock();								
				}
				continue;
			}		
		}	
			

	}
	else
	{
		struct Message sendcontent{Client_Port , Server_Port ,  lastsendSEQ+1 , lastrecvSEQ + 1};
		sendcontent.datalen = contentlen;
		sendcontent.setAck();
		sendcontent.setPsh();
		sendcontent.setData(content);
		sendcontent.setChecksum(&sendHeader);

		bool LOSS = LossPackage();
		int s;	
		if(!LOSS)	
		{
			s = sendto(Client, (char*) &sendcontent, sizeof(struct Message), 0, (sockaddr*)&Server_addr, l );
		} 
		else
		{
			printTime();
			cout<<"Loss Seq"<<lastsendSEQ+1<<endl;			
		}
		if(!LOSS) printMess(sendcontent,SEND);
		lastsendSEQ = sendcontent.seq;
		startTime = clock();
				
		while(1)
		{
			struct Message recvcontent;
			int r = recvfrom(Client, (char*) &recvcontent, sizeof(struct Message), 0, (sockaddr*)&Server_addr, &l);

			if (r != SOCKET_ERROR && recvcontent.Flags == 16 && recvcontent.Check(&recvHeader) && recvcontent.seq == lastrecvSEQ +1 && recvcontent.ack == sendcontent.seq +1)
			{

				printMess(recvcontent,!SEND); 
				lastrecvSEQ = recvcontent.seq;
				return;
			}
			else
			{
				if(clock()-startTime >= Max_waitTime)
				{
					int s = sendto(Client, (char*) &sendcontent, sizeof(struct Message), 0, (sockaddr*)&Server_addr, l );

						printMess(sendcontent,SEND);
						lastsendSEQ = sendcontent.seq;
						startTime = clock();								
				}					
				continue;
			}		
		}			
	}	
}

void send_data(char* filename,char* file_content, int filelen)
{

	int packnum = ceil((double)filelen/Max_DataLen);
	send_name(filename,false);


	
	for(int i=0;i<packnum;i++)
	{
		int curlen;
		if( i != packnum -1)
		{
		//非尾包
			curlen =Max_DataLen;
			char* filepart = new char[8192];
			memset(filepart, 0 ,8192);
			memcpy(filepart, file_content + i*Max_DataLen, curlen);

			send_content(filepart,curlen,i,false);

		}
		else
		{
		//尾包
			curlen = filelen - (packnum-1)*Max_DataLen;
			char* filepart = new char[8192];
			memset(filepart, 0 ,8192);
			memcpy(filepart, file_content + i*Max_DataLen, curlen);
			send_content(filepart,curlen,i,true);

		}
	}
	return;
}

void disconnect_wave()
{
	clock_t startTime;
	while(1)
	{
//Frist Handwave-sendto
//Seq = x, Ack = ?, Flags(Fin Ack)
		sendbuf.seq = lastsendSEQ+1;
		sendbuf.ack = lastrecvSEQ + 1;
		sendbuf.Flags = 0;
		sendbuf.setAck();
		sendbuf.setFin();
		sendbuf.setChecksum(&sendHeader);
		while(1)
		{
			int s = sendto(Client, (char*) &sendbuf, sizeof(struct Message), 0, (sockaddr*)&Server_addr, l );
			if (s == SOCKET_ERROR)
			{
				printTime();
				std::cout<<"[ Error ] Filed To Send First Handshake Message"<<endl;
				continue;
			}
			else
			{
				printTime();
				std::cout<<"[ Log ] Send First Handwave"<<endl;
				printMess(sendbuf,SEND);
				lastsendSEQ = sendbuf.seq;
				startTime=clock();
				break;
			}
		}


//Second Handwave-recvfrom
//Seq = ?, Ack = x+1, Flags(Ack)
		while(1)
		{
			// struct Message recvbuf{}; 
			int l = sizeof(Server_addr);

			while(recvfrom(Client, (char*) &recvbuf, sizeof(struct Message), 0, (sockaddr*)&Server_addr, &l) == SOCKET_ERROR)
			{
				if(clock()- startTime >= Max_waitTime)
				{
					printTime();
					std::cout<<"[ Log ] Timeout! Retransmit data."<<endl;
					//重传
					while(1)
					{
						int s = sendto(Client, (char*) &sendbuf, sizeof(struct Message), 0, (sockaddr*)&Server_addr, l );
						if (s == SOCKET_ERROR)
						{
							printTime();
							std::cout<<"[ Error ] Filed To Send First Handshake Message"<<endl;
							continue;
						}
						else
						{
							printTime();
							std::cout<<"[ Log ] Send First Handwave"<<endl;
							printMess(sendbuf,SEND);
							lastsendSEQ = sendbuf.seq;
							startTime = clock();
							break;
						}
					}
				}

			}

			if (recvbuf.Flags == 16 && recvbuf.Check(&recvHeader))
			{
				printTime();
				std::cout<<"[ Log ] Receive Second Handwave"<<endl;
				printMess(recvbuf,!SEND);
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

//Third Handwave-recvfrom
//Seq = ?, Ack = x+1, Flags(Fin Ack)
			while(1)
			{
				// struct Message recvbuf{}; 
				int l = sizeof(Server_addr);

				while(recvfrom(Client, (char*) &recvbuf, sizeof(struct Message), 0, (sockaddr*)&Server_addr, &l) == SOCKET_ERROR)
				{
					if(clock()- startTime >= Max_waitTime)
					{
						printTime();
						std::cout<<"[ Log ] Timeout! Retransmit data."<<endl;
						//重传
						while(1)
						{
							int s = sendto(Client, (char*) &sendbuf, sizeof(struct Message), 0, (sockaddr*)&Server_addr, l );
							if (s == SOCKET_ERROR)
							{
								printTime();
								std::cout<<"[ Error ] Filed To Send First Handshake Message"<<endl;
								continue;
							}
							else
							{
								printTime();
								std::cout<<"[ Log ] Send First Handwave"<<endl;
								printMess(sendbuf,SEND);
								lastsendSEQ = sendbuf.seq;
								startTime = clock();
								break;
							}
						}
					}
				}

				if (recvbuf.Flags == 17 && recvbuf.Check(&recvHeader))
				{
					printTime();
					std::cout<<"Receive Third Handwave"<<endl;
					printMess(recvbuf,!SEND);
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
						std::cout<<"[ Error ] Checksum Error"<<endl;
					}
					continue;
				}	

//Fourth Handwave-sendto
//Seq = x+1, Ack = ?+1(seq+1), Flags(Ack)

				sendbuf.seq = lastsendSEQ+1;
				sendbuf.ack = lastrecvSEQ + 1;
				sendbuf.Flags = 0;
				sendbuf.setAck();
				sendbuf.setChecksum(&sendHeader);
				while(1)
				{
					int s = sendto(Client, (char*) &sendbuf, sizeof(struct Message), 0, (sockaddr*)&Server_addr, l );
					if (s == SOCKET_ERROR)
					{
						printTime();
						std::cout<<"[ Error ] Filed To Send Fourth Handshake Message"<<endl;
						continue;
					}
					else
					{
						printTime();
						std::cout<<"Send Fourth Handwave"<<endl;
						printMess(sendbuf,SEND);
						lastsendSEQ = sendbuf.seq;
						return;
					}
				}
				
			}	
		}
	}
}