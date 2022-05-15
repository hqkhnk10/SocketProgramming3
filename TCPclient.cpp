/*
	Create a TCP socket
*/

#include<stdio.h>
#include<winsock2.h>
#include <windows.h>

#pragma comment(lib,"ws2_32.lib") //Winsock Library
#pragma warning(disable:4996) 


DWORD WINAPI sendMess(LPVOID);
DWORD WINAPI recvMess(LPVOID);

DWORD WINAPI sendMess(LPVOID lpParam) {
	SOCKET s = *(SOCKET*)lpParam;
	char message[100];
	while (1) {
		fgets(message, sizeof(message), stdin);

		if (send(s, (const char*)message, strlen(message), 0) < 0)
		{
			puts("Send failed");
			return 1;
		}


		puts("Data Send\n");
	}
}

DWORD WINAPI recvMess(LPVOID lpParam) {
	SOCKET s = *(SOCKET*)lpParam;
	char	server_reply[2000];
	int recv_size;
	while (1) {
		//Receive a reply from the server
		if ((recv_size = recv(s, server_reply, 2000, 0)) == SOCKET_ERROR)
		{
			puts("recv failed");
		}
		server_reply[recv_size] = '\0';
		puts(server_reply);
	}
}

int main(int argc, char* argv[])
{
	WSADATA wsa;
	SOCKET s;
	struct sockaddr_in server;
	

	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		return 1;
	}

	printf("Initialised.\n");

	//Create a socket
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}

	printf("Socket created.\n");


	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons(8888);

	//Connect to remote server
	if (connect(s, (struct sockaddr*)&server, sizeof(server)) < 0)
	{
		puts("connect error");
		return 1;
	}
	HANDLE myhandle[2];

	myhandle[0] = CreateThread(0, 0, sendMess, &s,  0, 0);
	myhandle[1] = CreateThread(0, 0, recvMess, &s,  0, 0);

	WaitForMultipleObjects(2, myhandle, true, INFINITE);

	CloseHandle(myhandle[0]);
	CloseHandle(myhandle[1]);

	return 0;
}