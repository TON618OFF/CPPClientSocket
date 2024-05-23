#define WIN32_LEAN_AND_MEAN // макрос

#include <iostream>
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
using namespace std;

int main()
{
	WSADATA wsaData;
	ADDRINFO hints;
	ADDRINFO* addrResult;
	SOCKET ConnectSocket = INVALID_SOCKET;
	char recvBuffer[512];

	const char* sendBuffer1 = "Hello from client 1";
	const char* sendBuffer2 = "Hello from client 2";

	WSAStartup(MAKEWORD(2, 2), &wsaData);
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) {
		cout << "WSAStartup failed result" << endl;
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints)); //нам необходимо изначально занулить память, 1-ый параметр, что зануляем, 2-ой сколько
	hints.ai_family = AF_INET; // 4-байтный Ethernet
	hints.ai_socktype = SOCK_STREAM; //задаём потоковый тип сокета
	hints.ai_protocol = IPPROTO_TCP; // используем протокол TCP



	// функция хранит в себе адрес, порт, семейство структур, адрес сокета.
	result = getaddrinfo("localhost", "666", &hints, &addrResult);
	if (result != 0) {
		cout << "getaddrinfo failed with error" << endl;
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}

	ConnectSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
	if (ConnectSocket == INVALID_SOCKET) {
		cout << "socket creation error" << endl;
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}

	result = connect(ConnectSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
	if (result == SOCKET_ERROR) {
		cout << "connection error to server" << endl;
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}

	result = send(ConnectSocket, sendBuffer1, (int)strlen(sendBuffer1), 0);
	if (result == SOCKET_ERROR) {
		cout << "send failed error" << endl;
		closesocket(ConnectSocket);
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}

	cout << "Sent" << result << "bytes" << endl;

	result = send(ConnectSocket, sendBuffer2, (int)strlen(sendBuffer2), 0);
	if (result == SOCKET_ERROR) {
		cout << "Send failed, error: " << result << endl;
		closesocket(ConnectSocket);
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}
	cout << "Sent: " << result << " bytes" << endl;

	result = shutdown(ConnectSocket, SD_SEND);
	if (result == SOCKET_ERROR) {
		cout << "Shutdown failed, error: " << result << endl;
		closesocket(ConnectSocket);
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}

	do {
		ZeroMemory(recvBuffer, 512);
		result = recv(ConnectSocket, recvBuffer, 512, 0);
		if (result > 0) {
			cout << "Received " << result << " bytes" << endl;
			cout << "Received data: " << recvBuffer << endl;
		}
		else if (result == 0) {
			cout << "Connection closed" << endl;
		}
		else {
			cout << "Recv failed, error: " << WSAGetLastError() << endl;
		}
	} while (result > 0);

	closesocket(ConnectSocket);
	freeaddrinfo(addrResult);
	WSACleanup();
	return 0;
}
