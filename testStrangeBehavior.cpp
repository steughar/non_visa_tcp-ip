// testStrangeBehavior.cpp : Defines the entry point for the console application.
//
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "5024"
#define address "192.168.1.20"


int strlen(char* str)
{
	int result = 0;
	while (*str++)
	{
		result++;
	}
	return(result);
}

void truncateStr(char *strBuf)
{
	char *OnePastLast_character = strBuf;
	while(*strBuf++ != '>')
	{
			OnePastLast_character = strBuf + 1;
	}
	*(OnePastLast_character + 1) = '\0';
}

// this will send command to the instrument
// WARNING! NO WARRANTY! NO ADDITIONAL ERROR CHECKING!
void SendCommand(char *strBuf, SOCKET ConnectSocket)
{
	// Send an initial buffer
	int iResult = send(ConnectSocket, strBuf, (int)strlen(strBuf), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
	}
}

// this will send command and get the answer 
// WARNING! NO WARRANTY! NO ADDITIONAL ERROR CHECKING!
void SendQueryCommand(char *strBuf, SOCKET ConnectSocket)
{
	// Send an initial buffer
	int iResult = send(ConnectSocket, strBuf, (int)strlen(strBuf), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
	}
	Sleep(1000);
	char recvBuf[DEFAULT_BUFLEN];
	int recvBufLen = DEFAULT_BUFLEN;
	iResult = recv(ConnectSocket, recvBuf, recvBufLen, 0);
	truncateStr(recvBuf);
	printf(recvBuf);
}


int main()
{
#pragma region SOCKET_initialization_and_connection_to_the_instrument

	WSADATA wsaData;

	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}

	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;



	// Resolve the server address and port
	iResult = getaddrinfo(address, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	SOCKET ConnectSocket = INVALID_SOCKET;

	// Attempt to connect to the first address returned by
	// the call to getaddrinfo
	ptr = result;

	// Create a SOCKET for connecting to server
	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
		ptr->ai_protocol);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Connect to server.
	iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
	}

	// Should really try the next address returned by getaddrinfo
	// if the connect call failed
	// But for this simple example we just free the resources
	// returned by getaddrinfo and print an error message

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}
#pragma endregion

	// I do not know why this instrument at first send welcome message
	// but he do, so blank query for welcome message at start
	Sleep(1000);
	char recvBuf[DEFAULT_BUFLEN];
	int recvBufLen = DEFAULT_BUFLEN;
	iResult = recv(ConnectSocket, recvBuf, recvBufLen, 0);
	truncateStr(recvBuf);
	printf(recvBuf);

	SendQueryCommand("*IDN?\r\n", ConnectSocket);
	SendQueryCommand("*OPC?\r\n", ConnectSocket);

	SendCommand("SENS:FREQ:STAR 1.2E+9\r\n", ConnectSocket);
	Sleep(3000);
	SendCommand("*RST\r\n", ConnectSocket);

	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();

	return 0;
}


