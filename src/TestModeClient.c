/* TestServer.c: Socket-Only Test Module, built to test Server */
/* Author: Yifan Xu(Max) */
/* Version: 1.2, 03/18/2017 */
/* V1.0: File Create */
/* V1.1: Fix Bug */
/* V1.2: Delete unrelated part */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include "map.h"
#include "constants.h"

/*** global variables ****************************************************/

const char *Program = NULL; /* program name for descriptive diagnostics */
int SocketFD,	/* socket file descriptor */
PortNo;		/* port number */
char SendBuf[256];	/* message buffer for sending a response */
char RecvBuf[256];	/* message buffer for receiving a response */
char Temp[256];	/* temp message buffer for sending a response */

struct sockaddr_in
	ServerAddress;	/* server address we connect with */
struct hostent
	*Server;	/* server host */

char ID[OrderLength + 1];/* Store Order Number */
char TaxiNumber[100]; /* Store Taxi Number */
char PickUpLocation[100] = "S8";
char Destination[100] = "D4";
char CurrentPosition[100];
char LastPosition[100] = "INITIALIZATION";

/*** support functions ****************************************************/

/* print error diagnostics and abort */
void FatalError(const char *ErrorMsg)
{
	fputs(Program, stderr);
	fputs(": ", stderr);
	perror(ErrorMsg);
	fputs(Program, stderr);
	fputs(": Exiting!\n", stderr);
	exit(20);
} /* end of FatalError */

  /* Socket Communication£¬ send data in Sendbug() */
char *Socket()
{
	int l, n;

	memset(RecvBuf, 0, sizeof(RecvBuf));
	/* II.Socket Connecting */
#ifdef DEBUG
	printf("%s: Creating a socket...\n", Program);
#endif
	SocketFD = socket(AF_INET, SOCK_STREAM, 0);
	if (SocketFD < 0)
	{
		FatalError("Socket creation failed");
	}
#ifdef DEBUG
	printf("%s: Preparing the server address...\n", Program);
#endif
	memset(&ServerAddress, 0, sizeof(ServerAddress));
	ServerAddress.sin_family = AF_INET;
	ServerAddress.sin_port = htons(PortNo);
	memcpy(&ServerAddress.sin_addr.s_addr,
		Server->h_addr_list[0], Server->h_length);
	printf("%s: Connecting to the server...\n", Program);
	if (connect(SocketFD, (struct sockaddr*)&ServerAddress,
		sizeof(ServerAddress)) < 0)
	{
		FatalError("Connecting to server failed");
	}

	/* III.Socket Communicating */
#ifdef DEBUG
	printf("%s: Message to send to the server: %s\n", Program, SendBuf);
#endif	
	l = strlen(SendBuf);
	if (SendBuf[l - 1] == '\n')
	{
		l = l - 1;
		SendBuf[l] = 0;
	}
	if (l)
	{
		printf("%s: Sending message '%s'...\n", Program, SendBuf);
		n = write(SocketFD, SendBuf, l);
		if (n < 0)
		{
			FatalError("Writing to socket failed");
		}
#ifdef DEBUG
		printf("%s: Waiting for response...\n", Program);
#endif
		n = read(SocketFD, RecvBuf, sizeof(RecvBuf) - 1);
		if (n < 0)
		{
			FatalError("Reading from socket failed");
		}
		RecvBuf[n] = 0;
		printf("%s: Received response: '%s'\n", Program, RecvBuf);
	}
	else {
		printf("%s: No send string get!\n", Program);
	}
	close(SocketFD);
	return RecvBuf;
}

int HandleRequestResponse(char *Response) {
	int length;
	int i, j;

	memset(Temp, 0, sizeof(Temp));
	strcpy(Temp, Response);
	length = strlen(Temp);
	Temp[length] = 0;	/*Set End Mark */

	/* An Available Taxi Found! */
			/*Acquire Taxi Number */
		for (i = 3; i < length; i++) {
			if (Temp[i] == 32) {
				strncpy(TaxiNumber, Temp + 3, i - 3);
				break;
			}
		}
		/*Acquire Order Number */
		for (j = 0; j < length; j++) {
			if (Temp[j] == '#') {
				strncpy(ID, Temp + j, OrderLength + 1);
				break;
		}
	}
#ifdef DEBUG
		printf("%s: Taxi Number: '%s' \n", Program, TaxiNumber);
		printf("%s: Order Number: '%s' \n", Program, ID);
#endif
	return 0;
}

void AnalyCurrentLocation(char *Response) {
	int length;
	int i, j;

	/* Save Current Location at Last Location */
	memset(LastPosition, 0, sizeof(LastPosition));
	strcpy(LastPosition, CurrentPosition);

	/* Understand Current Location */
	memset(Temp, 0, sizeof(Temp));
	memset(CurrentPosition, 0, sizeof(CurrentPosition));

	strcpy(Temp, Response);
	length = strlen(Temp);
	Temp[length] = 0;	/* Set an End Mark */
#ifdef DEBUG
	printf("Length: '%d'\n", length);
	printf("Temp String: '%s'\n", Temp);
#endif
	/* Find the position of the first letter of Current Location, 2 unit after N of POSITION */
	for (i = 0; i < length; i++) {
		if (Temp[i] == 78) { /* 'N' = 78 */
			j = i + 2;
			break;
		}
	}
	/* Find the position of the last letter of Current Location */
	for (i = j; i < length; i++) {
		if (Temp[i] == 32) { /* ' ' = 32 */
			break;
		}
	}

	strncpy(CurrentPosition, Temp + j, i - j);
	length = strlen(CurrentPosition);
	CurrentPosition[length] = 0; /* Set an End Mark */
}


/* Main Guiding Loop */
int ClientMainLoop()
{
	char *Response = NULL;
	int ShutDown = 0;

	/* Initialization */
	memset(SendBuf, 0, sizeof(SendBuf));
	memset(ID, 0, sizeof(ID));

	/* I Send Taxi Request */
	strcpy(SendBuf, "REQUEST_TAXI S8 TO D4");
	Response = Socket();
	HandleRequestResponse(Response);

	/* II. Confirm an Order */
	memset(SendBuf, 0, sizeof(SendBuf));
	strcpy(SendBuf, "CONFRIM ");
	strcat(SendBuf, ID);
	Response = Socket();
	AnalyCurrentLocation(Response);
#ifdef DEBUG
	printf("Current Location: '%s'\n", CurrentPosition);
	printf("Previous Location: '%s'\n", LastPosition);
#endif

	/* III. Accquire Taxi position every X second */
	do {
		memset(SendBuf, 0, sizeof(SendBuf));
		strcpy(SendBuf, "REQUEST_POSITION ");
		strcat(SendBuf, TaxiNumber);
		Response = Socket();

		/* Acquire Current Position */
		AnalyCurrentLocation(Response);
		printf("Current Location: '%s'\n", CurrentPosition);
		printf("Previous Location: '%s'\n", LastPosition);
		/* Analyses Order Status */
		if (0 == strcmp(CurrentPosition, PickUpLocation)) { /* Reday for pick up */
			printf("Taxi Arrive at pre-set Pickup Location!\n");
			memset(PickUpLocation, 0, sizeof(PickUpLocation));
			strcpy(PickUpLocation, "MX");
		}
		if (0 == strcmp(CurrentPosition, Destination)) {	/* Arrive at destination */
			printf("Arrive at Destination! Thanks for Choosing Uder!\n");
		}
		if (0 == strcmp(CurrentPosition, LastPosition)) {	/* Taxi stop */
			printf("Taxi Back to a stop!\n");
			ShutDown = 1;
		}
		sleep(LoopIntervalConstant);
	} while (!ShutDown);
	return 0;
}


int main(int argc, char *argv[])
{
	Program = argv[0];	/* publish program name (for diagnostics) */

	/* I.Socket Preparing */
#ifdef DEBUG
	printf("%s: Starting...\n", argv[0]);
#endif
	if (argc < 3)
	{
		fprintf(stderr, "%s: Pls provide hostname and port\n", Program);
		exit(10);
	}
#ifdef DEBUG
	printf("%s: Looking up host %s on the network...\n", Program, argv[1]);
#endif
	Server = gethostbyname(argv[1]);
	if (Server == NULL)
	{
		fprintf(stderr, "%s: no such host named '%s'\n", Program, argv[1]);
		exit(10);
	}
	PortNo = atoi(argv[2]);
#ifdef DEBUG
	printf("%s: Using port %d...\n", Program, PortNo);
#endif
	if (PortNo <= 2000)
	{
		fprintf(stderr, "%s: invalid port number %d, should be greater 2000\n",
			argv[0], PortNo);
		exit(10);
	}

	/* Loop Start Here */
	ClientMainLoop();
	printf("%s: Exiting...\n", Program);
	return 0;
}
