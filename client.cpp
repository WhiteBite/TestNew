
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <WinSock2.h>
#include <iostream>
#include <string>
#pragma comment(lib, "Ws2_32.lib")
const int BUFFER_SIZE = 128;
using namespace std;
int sendall(int s, char* buf, int len = BUFFER_SIZE, int flags = 0)
{
	int total = 0;
	int n;
	while (total < len)
	{
		n = send(s, buf + total, len - total, flags);
		if (n == -1)
			break;
		total += n;
	}
	return (n == -1 ? -1 : total);
}

int recvall(int s, char* buf, int len = BUFFER_SIZE, int flags = 0)
{
	int total = 0;
	int n;
	while (total < len)
	{
		n = recv(s, buf + total, len - total, flags);
		if (!n)
			break;
		total += n;
	}
	return (!n ? 0 : total);
}

int main(int argc, char** argv)
{
	std::string ip = "192.168.56.101";
	std::string myip = argv[1];
	// Initialize Winsock.
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
		printf("Error at WSAStartup()\n");

	// Create a socket.
	SOCKET m_socket;
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (m_socket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	// Connect to a server.
	struct sockaddr_in clientService;

	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr("192.168.56.101");
	clientService.sin_port = htons(25560);

	if (connect(m_socket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR)
	{
		std::cout << "Failed to connect.\n";
		WSACleanup();
		return 1;
	}

	// Send and receive data.
	int bytesSent;
	int bytesRecv = SOCKET_ERROR;

	char recvbuf[BUFFER_SIZE] = { 0 };
	char* sendbuf = (char*)myip.c_str();
	bytesSent = send(m_socket, sendbuf, 128, 0);
	std::cout << "Bytes Sent: " << bytesSent << "[" << sendbuf << "]" << std::endl;
	std::string tmp1 = "";

	while (tmp1 != "Your place is [-1]") {
		ZeroMemory(recvbuf, 128);
		bytesRecv = recv(m_socket, recvbuf,128,0);
		if (bytesRecv == 0 || bytesRecv == WSAECONNRESET) {
			//	std::cout << "Connection Closed.\n";
		}
		if (bytesRecv < 0)
			return 1;
		if (bytesRecv > 0) {
			std::cout << "Bytes Recv: " << bytesRecv << "\n";
			std::cout << recvbuf << "\n";
		}
		tmp1 = recvbuf;
	//	Sleep(5000);
	//	bytesSent = send(m_socket, "kek", 128, 0);

		std::cout << "Bytes Sent: " << bytesSent << "[" << sendbuf << "]" << std::endl;
	}
	getchar();
	return 0;
}
