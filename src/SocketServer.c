/* SocketServer.c: Core Server System that supports socket communication */
/* Author: Yifan Xu(Max) */
/* Version: 3.0, 03/19/2017 */
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
/* V2.6: Add Advanced UI for server */
/* V2.7: Display Taxi */
/* V2.8: First fully functional Version */
/* V2.9: Add necessaey judgement to prevent LoadMap fail */
/* V3.0: RTM, fix bugs, final test */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
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
#include "GetTime.h"
#include "ServerManagement.h"

/*** global variables ****************************************************/

time_t ServerStartTime;		/* Time Start Mark */
const char *Program = NULL;	/* program name for descriptive diagnostics */
int Shutdown = 0;			/* Key to Stop the whole Program */
char *resp = NULL;			/* Pointer to the response from Core Management System */
char Temp[256];				/* Temp String buffer for String operation like copy or cut */

Map *map;					/* Globle pointer to the whole Map structure */
Taxi *taxi[MaxTaxi];		/* Globle pointers array to every taxi */
int TaxiAmount = 0;			/* Total amount of Taxi running on the map */
double Profit = 0.00;			/* Sum of profit that the company earned */

int Jsig;					/* J Signal(Close Signal) from GTK */
GtkWidget *Window;			/* GTK pointer to the shown Window*/
GtkLabel *GTKMoney = NULL;	/* Money Changing display value from GTK */
GtkWidget *GTKMap = NULL;	/* Map Changing display value from GTK */
char buffer[100];

char res[200];						/* Response Buf from Response() */
DriverOrder *DriverOrders[MaxTaxi];	/* Scheduled Taxi Path, array of Point */
Order *Orders[OrderNum];			/* OrderNumber Array */

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
		resp = Response(RecvBuf,map,taxi,DriverOrders);
#ifdef DEBUG
		printf("%s: ** Message acquired: '%s'\n", Program, resp);
#endif
		memset(SendBuf, 0, sizeof(SendBuf));
		strcpy(SendBuf, resp);
		length = strlen(SendBuf);
		SendBuf[length] = 0;
	}
#ifdef DEBUG
	printf("%s: Sending response: %s.\n", Program, SendBuf);
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
    int result, i;

	Jsig = 0;
    FD_ZERO(&ActiveFDs);		/* set of active sockets */
    FD_SET(ServSocketFD, &ActiveFDs);	/* server socket is active */
    while(!Shutdown)
    {
		snprintf(buffer, 12, "%lf", Profit);
		if (GTKMoney)
		{
			gtk_label_set_label(GTKMoney, buffer);
		}
		if (GTKMap)
		{
			gdk_window_invalidate_rect(GTKMap->window,	/* update it! */
				&GTKMap->allocation, /* invalidate entire region */
				FALSE);
		}
		UpdateWindow();
		Shutdown = Jsig;	/* When GTK Windows close, shutdown Sever&Socket */
		ReadFDs = ActiveFDs;
		TimeVal.tv_sec  = Timeout / 1000000;	/* seconds */
		TimeVal.tv_usec = Timeout % 1000000;	/* microseconds */
		/* Block until input arrives on active sockets or until timeout */
		result = select(FD_SETSIZE, &ReadFDs, NULL, NULL, &TimeVal);
		if (result < 0)	/* Case 1: Unexpected Error occrured */
		{   FatalError("Wait for input or timeout (select) failed");
		}
		if (result == 0)	/* Case 2: Timeout occurred */
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

/* Load MapInfo from *.map file */
int LoadMapFromFile() {
	char *fileName;
	int i;

	/* Create a Global Pointer to Map */
	map = newMap();
#ifdef DEBUG
	printf("starting loading map\n");
	printf("Loading Map from %s\n", MapPathI);
#endif
	/* Load Map First Try */
	fileName = MapPathI;
	i = loadMap(fileName, map);
	if (i != 0) {	/* Load Map Second Try */
#ifdef DEBUG
		printf("Failed! Error %d\n", i);
		printf("Loading Map from %s\n", MapPathII);
#endif
		fileName = MapPathII;
		i = loadMap(fileName, map);
	}
	if (i != 0) {	/* Load Map Third Try */
#ifdef DEBUG
		printf("Failed! Error %d\n", i);
		printf("Loading Map from %s\n", MapPathIII);
#endif
		fileName = MapPathIII;
		i = loadMap(fileName, map);
	}
	if (i != 0) {	/* Load Map Fail */
#ifdef DEBUG
		printf("Failed! Error %d\n", i);
#endif
		printf("LoadMap Fail. Abort!\n");
		return 0;
	}
	printf("Find file, Loading Map Successfully!\n");
	return 1;
}

/*** main function ****************************************************/
int main(int argc, char *argv[])
{
	int ServSocketFD;	/* socket file descriptor for service */
	int PortNo;		/* port number */
	int m, i, j;
	TaxiStand *TS;
	double diff;
	time_t ServerShutDownTime;
	Array *arr;

	Program = argv[0];		/* publish program name (for diagnostics) */

	/* Load Map */
	i = LoadMapFromFile();
	assert(i);

	/* Initialize Taxi pointers */
	m = map->stands->size;
	arr = map->stands;
#ifdef DEBUG
	printf("TAXI: m = %d\n", m);
#endif
	TaxiAmount = 0;
	for (i = 0; i < m; i++)
	{
		TS = (TaxiStand *)(arr->items + arr->itemSize * i);
#ifdef DEBUG
		printf("TAXI: TaxiNum in Stand #%d = %d\n", i, TS->numTaxis);
		printf("TAXI: StandLocation is (%d,%d)\n", TS->mark.x, TS->mark.y);
#endif
		for (j = 0; j < TS->numTaxis; j++) {
			taxi[TaxiAmount] = newTaxi(0, TS->mark);
			assert(taxi[TaxiAmount]);
			taxi[TaxiAmount]->num = TaxiAmount;
			taxi[TaxiAmount]->confirm_num[0] = 0;
			TaxiAmount++;
		}
	}
#ifdef DEBUG
	printf("TAXI: Total Taxi Amount = %d\n", TaxiAmount);
#endif
	for (j = 0; j < MaxTaxi; j++) {
		DriverOrders[j]=malloc(sizeof(DriverOrder));
		DriverOrders[j]->confirm_num[0]=0;
		DriverOrders[j]->confirm_num[1]=0;
	}
	for (j = 0; j <OrderNum; j++) {
		Orders[j]=malloc(sizeof(Order));
		Orders[j]->confirm_num=0;
	}


	/* Initialize Socket */
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
	/* Start GTK Windows */
	Window = CreateWindow(0, 0);
	UpdateWindow();

	/* Start Socket Loop */
	ServerStartTime = GetCurrentTime();
	memset(res, 0, sizeof(res));
	ServerMainLoop(ServSocketFD, TimeOutConstant);
	printf("\n%s: Shutting down.\n", Program);
	close(ServSocketFD);

	/* Socket End, Show Time Report */
	ServerShutDownTime = GetCurrentTime();
	diff = difftime(ServerShutDownTime, ServerStartTime);
	printf("Run Time:%lf s\n", diff);
	deleteMap(map);
	return 0;
}

/* EOF SocketServer.c */

