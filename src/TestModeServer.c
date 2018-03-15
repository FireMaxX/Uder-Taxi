/* TestServer.c: Socket-Only Test Module, built to test Client */
/* Author: Yifan Xu(Max) */
/* Version: 1.2, 03/09/2017 */
/* V1.0: File Create */
/* V1.1: Fix Bug */
/* V1.2: Delete unrelated part */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <assert.h>
#include "Response.h"
#include "constants.h"
#include "map.h"
#include "utils.h"
#include "Pathfind.h"
#include "ServerManagement.h"
#include "GetTime.h"


/*** global variables ****************************************************/

const char *Program = NULL;	/* program name for descriptive diagnostics */
int Shutdown = 0;			/* Key to Stop the whole Program */
int total = 0;				/* Variable for multiply clients recognition */
char *res = NULL;			/* Pointer to the response from Core Management System */

/*** BuiltIn Command ****************************************************/

char OkayTaxi[256] = "OK Taxi7 PICKUP S8 10:15 DROPOFF D4 11:42 $8.75 CONFIRM #10042";
char LocationI[256] = "OK Taxi7 POSITION J2 ETA D4 12:45";
char LocationII[256] = "OK Taxi7 POSITION S8 ETA D4 12:45";
char LocationIII[256] = "OK Taxi7 POSITION D4";
char ErrorMessage[256] = "ERROR Test_Error_Message";
char InvalidMessage[256] = "INVALID Test_INVALID_Message";
char DeclineMessageI[256] = "DECLINE Test_Overtime_CONFRIM";
char DeclineMessageII[256] = "DECLINE Test_No_Taxi_Available";

/*** support functions ****************************************************/

/* Print error diagnostics and abort */
void FatalError(const char *ErrorMsg)
{
	fputs(Program, stderr);
	fputs(": ", stderr);
	perror(ErrorMsg);
	fputs(Program, stderr);
	fputs(": Exiting!\n", stderr);
	exit(20);
}

/*  Initialize the Socket Server */
int CreateServerSocket(uint16_t PortNo)
{
	int ServSocketFD;
	struct sockaddr_in ServSocketName;
	int on = 1;

	/* Create */
	ServSocketFD = socket(PF_INET, SOCK_STREAM, 0);
#ifdef DEBUG
	printf("%s: ServSocketFD = %d\n",Program, ServSocketFD);
#endif
	if (ServSocketFD < 0)
	{
		FatalError("Fail to create Socket Server");
	}
	ServSocketName.sin_family = AF_INET;
	ServSocketName.sin_port = htons(PortNo);
	ServSocketName.sin_addr.s_addr = htonl(INADDR_ANY);
#ifdef DEBUG
	printf("%s: PortNo = %d\n",Program, PortNo);
#endif
	/* Enable Reuse */
	if ((setsockopt(ServSocketFD, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0)
	{
		perror("SetReuse Fail");
		exit(EXIT_FAILURE);
	}
	/* Bind */
	if (bind(ServSocketFD, (struct sockaddr*)&ServSocketName,sizeof(ServSocketName)) < 0)
	{
		FatalError("Fail to bind Server to Socket");
	}
	/* Listen */
	if (listen(ServSocketFD, 5) < 0)
	{
		FatalError("Fail to listen on Socket");
	}
	return ServSocketFD;
}

/* Fetch Request, call core system, send response */
void ProcessRequest(int DataSocketFD)
{
	int n;
	int length;
	int swit;
	char switc[100];

    char RecvBuf[256];	/* message buffer for receiving a message */
    char SendBuf[256];	/* message buffer for sending a response */
	char DebugBuf[256];

	memset(RecvBuf, 0, sizeof(RecvBuf));
	n = read(DataSocketFD, RecvBuf, sizeof(RecvBuf));
    if (n < 0) 
    {   FatalError("Reading from data socket failed");
    }
    RecvBuf[n] = 0;	/* Set Ending Mark */
#ifdef DEBUG
	printf("%s: Received message: '%s'\n", Program, RecvBuf);
#endif

	/* Debug Command, ShutDown Server, Classfied */
	if (0 == strcmp("Bye", RecvBuf)) {
		Shutdown = 1;
		strncpy(SendBuf, "Bye Client", sizeof(SendBuf));
	}
	/* Normal Command: Call Core system */
	/* Core system Call if no DebugMode, only availabe in SocetServer.c */
	else {
		/* DEBUG MODE, type response back */
		printf("Choose a built in command to response:\n");
		printf("1: OK Taxi7 PICKUP S8 10:15 DROPOFF D4 11:42 $8.75 CONFIRM #10042\n");
		printf("2: OK Taxi7 POSITION J2 ETA D4 12:45 --Random Location\n");
		printf("3: OK Taxi7 POSITION S8 ETA D4 12:45 --PickUp Location\n");
		printf("4: OK Taxi7 POSITION D4 ETA D4 12:45 --Destination \n");
		printf("5: ERROR Test_Error_Message\n");
		printf("6: INVALID Test_INVALID_Message\n");
		printf("7: DECLINE Test_Overtime_CONFRIM\n");
		printf("8: DECLINE Test_No_Taxi_Available\n");
		printf("0: **Type a new command**\n");
		printf("Type response for this request:\n");
		fgets(switc, sizeof(switc), stdin);
		swit = atoi(switc);
#ifdef DEBUG
		printf("swit = %d\n", swit);
#endif
		memset(DebugBuf, 0, sizeof(DebugBuf));
		switch (swit)
		{
		case 1: {
			strcpy(DebugBuf, OkayTaxi);
			break;
		}
		case 2: {
			strcpy(DebugBuf, LocationI);
			break;
		}
		case 3: {
			strcpy(DebugBuf, LocationII);
			break;
		}
		case 4: {
			strcpy(DebugBuf, LocationIII);
			break;
		}
		case 5: {
			strcpy(DebugBuf, ErrorMessage);
			break;
		}
		case 6: {
			strcpy(DebugBuf, InvalidMessage);
			break;
		}
		case 7: {
			strcpy(DebugBuf, DeclineMessageI);
			break;
		}
		case 8: {
			strcpy(DebugBuf, DeclineMessageII);
			break;
		}
		default: {
			printf("Type the command that you want to send:\n");
			fgets(DebugBuf, sizeof(DebugBuf), stdin);
			length = strlen(DebugBuf);
			DebugBuf[length - 1] = 0;
			printf("Message type in: '%s'", DebugBuf);
			res = DebugBuf;
			memset(SendBuf, 0, sizeof(SendBuf));
			strcpy(SendBuf, res);
			length = strlen(SendBuf);
			SendBuf[length] = 0;
			break;
		}
		}
		memset(SendBuf, 0, sizeof(SendBuf));
		strcpy(SendBuf, DebugBuf);
		length = strlen(SendBuf);
		SendBuf[length] = 0;
	}

#ifdef DEBUG
	printf("%s: Sending response: ‘%s’.\n", Program, SendBuf);
#endif
	length = strlen(SendBuf);
	/* Send Buf to Client */
	n = write(DataSocketFD, SendBuf, length);
	if (n < 0)
	{
		FatalError("Writing to data socket failed");
	}
} /* EOF ProcessRequest() */

/* Server main loop to handle request one by one */
void ServerMainLoop(
	int ServSocketFD,		/* server socket to wait on */
	int Timeout)			/* timeout in micro seconds */
{
	int DataSocketFD;	/* socket for a new client */
    socklen_t ClientLen;
    struct sockaddr_in ClientAddress;	/* client address we connect with */
    fd_set ActiveFDs;	/* socket file descriptors to select from */
    fd_set ReadFDs;	/* socket file descriptors ready to read from */
    struct timeval TimeVal;
    int res, i;

    FD_ZERO(&ActiveFDs);		/* set of active sockets */
    FD_SET(ServSocketFD, &ActiveFDs);	/* server socket is active */
    while(!Shutdown)
    {
	ReadFDs = ActiveFDs;
	TimeVal.tv_sec  = Timeout / 1000000;	/* seconds */
	TimeVal.tv_usec = Timeout % 1000000;	/* microseconds */
	/* Block until input arrives on active sockets or until timeout */
	res = select(FD_SETSIZE, &ReadFDs, NULL, NULL, &TimeVal);
	if (res < 0)	/* Case 1: Unexpected Error occrured */
	{   FatalError("Wait for input or timeout (select) failed");
	}
	if (res == 0)	/* Case 2: Timeout occurred */
	{
/******** Fix Me! What to do if Timeout! ***********/
#ifdef DEBUG
	    /* printf("%s: Timeout Occured...\n", Program); */
#endif
/******** Fix Me! What to do if Timeout! ***********/
	}
	else		/* Case 3: FDs have data ready to read, accept a new Client */
	{   for(i=0; i<FD_SETSIZE; i++)
	    {   if (FD_ISSET(i, &ReadFDs))
		{   if (i == ServSocketFD)
		    {	/* connection request on server socket */
#ifdef DEBUG
			printf("%s: Accepting new client %d...\n", Program, i);
#endif
			ClientLen = sizeof(ClientAddress);
			DataSocketFD = accept(ServSocketFD, (struct sockaddr*)&ClientAddress, &ClientLen);
			if (DataSocketFD < 0)
			{   FatalError("Data socket creation (accept) failed");
			}
#ifdef DEBUG
			printf("%s: Client %d connected from %s:%hu.\n", Program, i, inet_ntoa(ClientAddress.sin_addr), ntohs(ClientAddress.sin_port));
#endif
			FD_SET(DataSocketFD, &ActiveFDs);
		    }
		    else
		    {   /* active communication with a client */
#ifdef DEBUG
			printf("%s: Dealing with client %d...\n", Program, i);
#endif
			ProcessRequest(i);
#ifdef DEBUG
			printf("%s: Closing client %d connection.\n", Program, i);
#endif
			close(i);
			FD_CLR(i, &ActiveFDs);
		    }
		}
	    }
	}
    }/* end of While */
} /* end of ServerMainLoop */

/*** main function ****************************************************/
int main(int argc, char *argv[])
{
	int ServSocketFD;	/* socket file descriptor for service */
	int PortNo;		/* port number */
        
	Program = argv[0];	/* publish program name (for diagnostics) */
	total = 0;				/* intialize ID recognition system */
	
#ifdef DEBUG
	printf("%s: Starting...\n", Program);
#endif
	if (argc < 2)	/* No port number provided*/
	{
		fprintf(stderr, "%s: Pls provide port number \n", Program);
		exit(10);
	}
	PortNo = atoi(argv[1]);	/* get the port number */
	if (PortNo <= 2000)
	{
		fprintf(stderr, "%s: invalid port number %d, should be >2000\n", Program, PortNo);
		exit(10);
	}
#ifdef DEBUG
	printf("%s: Creating the server socket...\n", Program);
#endif
	ServSocketFD = CreateServerSocket(PortNo);
#ifdef DEBUG
	printf("%s: Listening on port %d...\n", Program, PortNo);
#endif
	ServerMainLoop(ServSocketFD, TimeOutConstant);
	printf("\n%s: Shutting down.\n", Program);
	close(ServSocketFD);
	return 0;
}

/* EOF SocketServer.c */

