/* SocketServer.c: Core Server System that supports socket communication */
/* Author: Yifan Xu(Max) */
/* Version: 2.4, 03/09/2017 */
/* V1.0: File Create */
/* V1.1: Copy Code from Sample */
/* V1.2: Initial (Finished) Version, no compile no debug */
/* V1.3：Socket Communication Work, no calling core system part code */
/* V1.4: Add ID recognition system */
/* V1.5: Add ID Advnace */
/* V1.6: Correspond to ClientV1.6 */
/* V2.0: Add Core Management System Call */
/* V2.1: Add GTK support */
/* V2.3: Add LoopControl */
/* V2.4: Switch to Standard Protocol, Simple tests Pass */
/* V2.5: Add map configuration. By Yan Zhang */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <assert.h>
#include <math.h>
#include <gtk/gtk.h>
#include "Dboard.h"
#include "Response.h"
#include "constants.h"
#include "map.h"
#include "utils.h"
#include "Pathfind.h"


/*** global variables ****************************************************/

const char *Program = NULL;	/* program name for descriptive diagnostics */
int Shutdown = 0;			/* Key to Stop the whole Program */
int total = 0;				/* Variable for multiply clients recognition */
char *res = NULL;			/* Pointer to the response from Core Management System */
char Temp[256];				/* Temp String buffer for String operation like copy or cut */

Map *map;

GtkWidget *Window;
GtkLabel *GTKMoney = NULL;
GtkWidget *GTKMap = NULL;


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

/* Time accruate system */
void PrintCurrentTime(void)	/*  print/update the current real time */
{
	time_t CurrentTime; /* seconds since 1970 (see 'man 2 time') */
	char   *TimeString;	/* printable time string (see 'man ctime') */
	char   Wheel,
		*WheelChars = "|/-\\";
	static int WheelIndex = 0;
	char ClockBuffer[26]= "";

	CurrentTime = time(NULL);	/* get current real time (in seconds) */
	TimeString = ctime(&CurrentTime);	/* convert to printable format */
	strncpy(ClockBuffer, TimeString, 25);
	ClockBuffer[24] = 0;	/* remove unwanted '/n' at the end */
	WheelIndex = (WheelIndex + 1) % 4;
	Wheel = WheelChars[WheelIndex];
	printf("\rClock: %s %c",	/* print from beginning of current line */
		ClockBuffer, Wheel);	/* print time plus a rotating wheel */
	fflush(stdout);
} /* end of PrintCurrentTime */

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
	if ((setsockopt(ServSocketFD, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)))<0)
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

    char RecvBuf[256];	/* message buffer for receiving a message */
    char SendBuf[256];	/* message buffer for sending a response */


	memset(RecvBuf, 0, sizeof(RecvBuf));
	n = read(DataSocketFD, RecvBuf, sizeof(RecvBuf));
    if (n < 0) 
    {   FatalError("Reading from data socket failed");
    }
    RecvBuf[n] = 0;	/* Set Ending Mark */
#ifdef DEBUG
	printf("%s: Received message: '%s'\n", Program, RecvBuf);
	printf("%c,%c,%c,%c,%c,%c,%c\n", RecvBuf[0], RecvBuf[1], RecvBuf[2], RecvBuf[3], RecvBuf[4], RecvBuf[5], RecvBuf[6]);
#endif

	/* Debug Command, ShutDown Server, Classfied */
	if (0 == strcmp("Bye", RecvBuf)) {
		Shutdown = 1;
		strncpy(SendBuf, "Bye Client", sizeof(SendBuf));
	}
	/* Normal Command: Call Core system */
	else {
#ifdef DEBUG
		printf("Temp: '%s'\n", Temp);
#endif
		memset(Temp, 0, sizeof(Temp));
		strncpy(Temp, RecvBuf, sizeof("REQUEST_TAXI"));
		Temp[12] = 0;	/* Set End Mark */
		/* Core system Call */
		res = Response(RecvBuf);
#ifdef DEBUG
		printf("%s: ** Message acquired: '%s'\n", Program, res);
#endif
		memset(SendBuf, 0, sizeof(SendBuf));
		strcpy(SendBuf, res);
		length = strlen(SendBuf);
		SendBuf[length] = 0;
	}

#ifdef DEBUG
	printf("%s: Sending response: %s.\n", Program, SendBuf);
#endif
	length = strlen(SendBuf);

	/* GTK Refresh */
	if (0 == strcmp(Temp, "REQUEST_TAXI")) {
		if (GTKMap)
		{
			gdk_window_invalidate_rect(GTKMap->window,	/* update it! */
				&GTKMap->allocation, /* invalidate entire region */
				FALSE);
		}
		UpdateWindow();
	}

	/* Send Buf to Client */
	n = write(DataSocketFD, SendBuf, length);
	if (n < 0)
	{
		FatalError("Writing to data socket failed");
	}
	/******** Fix ME! GTK ShutDown **********/
	/******** Fix ME! GTK ShutDown **********/

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

	/* Start GTK Windows */
	Window = CreateWindow(0, 0);

    FD_ZERO(&ActiveFDs);		/* set of active sockets */
    FD_SET(ServSocketFD, &ActiveFDs);	/* server socket is active */
    while(!Shutdown)
    {

		Shutdown = Jsig;	/* When GTK Windows close, shutdown Sever&Socket */

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
#ifdef DEBUG
	    /* printf("%s: Timeout Occured...\n", Program); */
#endif
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

	map = newMap();

#ifdef DEBUG
	printf("starting loading map\n");
#endif
	char *fileName;
	int i;
	fileName="./bin/NewIrvine.map";
	i = loadMap(fileName, map);
#ifdef DEBUG
	printf("LoadMap Return = %d\n", i);
	Point start, end;
	start.x = 0;
	start.y = 0;
	end.x = 10;
	end.y = 10;
	/*
	i = Pathfind(start, end, map);
	*/
	printf("Pathfind Return = %d\n", i);
	printMap(map);
#endif	


	
#ifdef DEBUG
	printf("%s: Starting...\n", Program);
#endif
//	if (argc < 2)	/* No port number provided*/
//	{
//		fprintf(stderr, "%s: Pls provide port number \n", Program);
//		exit(10);
//	}
	PortNo = atoi("2009");	/* get the port number */
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
	deleteMap(map);
	return 0;
}

/* EOF SocketServer.c */

