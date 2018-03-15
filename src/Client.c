/* Client.c: Core Server Systen that support socket communication */
/* Author: Yifan Xu(Max) */
/* Version: 3.0, 03/19/2017 */
/* V1.0: File Create */
/* V1.1: Initial (Finished) Version, no compile no debug */
/* V1.2: Add diagnostics support code */
/* V1.3£ºSocket Communication Work, no calling core system part code */
/* V1.4: Add ID recognition system */
/* V1.5: Add ID Advnace */
/* V1.6: Add RebuildString */
/* V1.7: Get_Position Loop */
/* V2.0: Add Core Management System Call */
/* V2.1: Add GTK support */
/* V2.2: Add Arrival Reminder */
/* V2.3: Add LoopControl */
/* V2.4: Switch to Standard Protocol, Simple tests Pass */
/* V2.5: Add price display */
/* V2.6: Fix bug that User can be pick up twice */
/* V2.7: Server-Client Version pair */
/* V2.8: Advanced Error-Response Solutions */
/* V2.9: New Gtk with auto-complete support */
/* V3.0: RTM, fix bugs, final test */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include "map.h"
#include "GTKMain.h"
#include "constants.h"
#include "ClientMap.h"

/*** global variables ****************************************************/

const char *Program = NULL; /* program name for descriptive diagnostics */
int SocketFD,	/* socket file descriptor */
PortNo;		/* port number */
struct sockaddr_in
	ServerAddress;	/* server address we connect with */
struct hostent
	*Server;		/* server host */

char SendBuf[256];	/* message buffer for sending a response */
char RecvBuf[256];	/* message buffer for receiving a response */
char Temp[256];	/* temp message buffer for sending a response */

char ID[OrderLength + 1];/* Store Order Number */
char TaxiNumber[100]; /* Store Taxi Number */
char PickUpLocation[100];
char Destination[100];
char CurrentPosition[100];
char LastPosition[100] = "INITIALIZATION";
char Message[256];

Map *map;					/* Globle pointer to the whole Map structure */
Point taxi;					/* Structure variable to save taxi's location */
BigP position;		/* Return value of the first GTK Window(Request Window) */
char RealRe[SLEN];	/* Global Variable share with Request/GTK */
int FF;				/* Global Variable share with Confirmation/GTK */
char GTKbuffer[100];/* Global Message buffer to communicate with Map Window */
int Jsig;
GtkWidget *Window;
GtkLabel *GTKMessage = NULL;
GtkWidget *GTKMap = NULL;
int GTKMaplength = 1000;
int GTKMapwidth = 600;

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

/* Connect Temp with ID/Taxi Number, stored in SendBuf */
void RebuildString(int i) {
	int length;

	memset(SendBuf, 0, sizeof(SendBuf));
	strncpy(SendBuf, Temp, sizeof(Temp));
	length = strlen(SendBuf);
	SendBuf[length] = 32;	/* Add a space */
	SendBuf[length + 1] = 0;
	/* ID */
	if (i == 0) {
		strcat(SendBuf, ID);
	}
	else {	/* Taxi Number */
		strcat(SendBuf, TaxiNumber);
	}

	length = strlen(SendBuf);
	SendBuf[length] = 0;
}

/* Handle Decline=1/Error=2/Invalid=3 Response */
int HandleErrorMessage(char *Response) {
	char ErrorTemp[10];
	char Reason[256];

	memset(Reason, 0, sizeof(Reason));
	memset(ErrorTemp, 0, sizeof(ErrorTemp));
	strncpy(ErrorTemp, Response, 5);
	ErrorTemp[5] = 0;

	/* Decline */
	if (0 == strcmp(ErrorTemp, "DECLI")) {
		strncpy(Reason, Response + 8, sizeof(Reason));
		GTK4(0, 0, "Request Declined!", Reason);
		return 1;
	}
	/* Error */
	if (0 == strcmp(ErrorTemp, "ERROR")) {
		strncpy(Reason, Response + 6, sizeof(Reason));
		GTK4(0, 0, "Error!", Reason);
		return 2;
	}
	/* Invalid */
	if (0 == strcmp(ErrorTemp, "INVAL")) {
		return 3;
	}

	return 0; /* No Error */
}

/* Acquire Taxi # and Oreder # From Response */
int HandleRequestResponse(char *Response) {
	int length;
	int i, j;
	int errorcode;

	memset(Temp, 0, sizeof(Temp));
	strcpy(Temp, Response);
	length = strlen(Temp);
	Temp[length] = 0;	/*Set End Mark */

	errorcode = HandleErrorMessage(Response);

	if (errorcode != 0) {	/* Error/Invalid/Decline */
		return errorcode;
	}
	else {	/* An Available Taxi Found! */
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
	}
	return 0;
}

/* Acquire the Current Location of the Assigned Taxi */
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

/* Generate Readable Text-Base Message */
void GenReadableSring(char *Response) {
	char Res[256];
	char NoticeMark[20];
	char PickUpTime[10] = "00:00";
	char DropOffTime[10] = "00:00";
	char TotalCost[20];
	int i, j, k, l;
	int length;

	memset(Res, 0, sizeof(Res));
	memset(NoticeMark, 0, sizeof(NoticeMark));
	//memset(PickUpTime, 0, sizeof(PickUpTime));
	//memset(DropOffTime, 0, sizeof(DropOffTime));
	memset(TotalCost, 0, sizeof(TotalCost));
	strcpy(Res, Response);
	length = strlen(Res);
	Res[length] = 0;


	/* Acquire Pick Up Time */
	for (i = 0; i < length; i++) {
		if (Res[i] == ':') {
			strncpy(PickUpTime, Res + i - 2, 5);
			break;
		}
	}
	/* Acquire Drop Off Time */
	for (j = i; j < length; j++) {
		if (Res[j] == ':') {
			strncpy(DropOffTime, Res + j - 2, 5);
			break;
		}
	}
#ifdef DEBUG
	printf("j: '%d'\n", j);
#endif
	/* Acquire Ride Price */
	for (k = 0; k < length; k++) {
		if (Res[k] == 36) {
			break;
		}
	}
#ifdef DEBUG
	printf("k: '%d'\n", k);
#endif
	for (l = k; l < length; l++) {
		if (Res[l] == 32) {
			strncpy(TotalCost, Res + k, l - k);
			break;
		}
	}
#ifdef DEBUG
	printf("l: '%d'\n", l);
#endif
#ifdef DEBUG
	printf("Pick Up Time: '%s'\n", PickUpTime);
	printf("Drop off Time: '%s'\n", DropOffTime);
	printf("Total Cost: '%s'\n", TotalCost);
#endif
	memset(Message, 0, sizeof(Message));
	strcpy(Message, "Order:");
	strcat(Message, ID);
	length = strlen(Message);
	Message[length] = 32;
	Message[length + 1] = 0;
	strcat(Message, TaxiNumber);
	length = strlen(Message);
	Message[length] = 32;	/* Space */
	Message[length + 1] = 0;
	strcpy(NoticeMark, "will pick you up around ");
	strcat(Message, NoticeMark);
	length = strlen(Message);
	Message[length] = 0;
	strcat(Message, PickUpTime);
	length = strlen(Message);
	Message[length + 1] = 0;
	memset(NoticeMark, 0, sizeof(NoticeMark));
	strcpy(NoticeMark, ". Estimated Arrival is around ");
	strcat(Message, NoticeMark);
	length = strlen(Message);
	Message[length] = 0;
	strcat(Message, DropOffTime);
	length = strlen(Message);
	Message[length] = '.';
	Message[length + 1] = 32;
	Message[length + 2] = 0;
	memset(NoticeMark, 0, sizeof(NoticeMark));
	strcpy(NoticeMark, "Total cost is ");
	strcat(Message, NoticeMark);
	length = strlen(Message);
	Message[length] = 0;
	strcat(Message, TotalCost);
	length = strlen(Message);
	Message[length] = 0;
	length = strlen(Message);
	Message[length] = '.';
#ifdef DEBUG
	printf("Message Generated: '%s'\n", Message);
#endif
}

void CalTaxiCoordinate(char* CurrPos) {
	char Te[100];

	if (strcmp(CurrPos, "") != 0) {
		taxi.x = (int)(CurrPos[0] - 65);
		memset(Te, 0, sizeof(Te));
		strcpy(Te, CurrPos + 1);
		taxi.y = atoi(Te) - 1;
	}
	else {
		taxi.x = 0;
		taxi.y = 0;
		strcpy(GTKbuffer, "Taxi Position Unknown!");
	}
#ifdef DEBUG
	printf("Current Location: '%s'\n", CurrPos);
	printf("taxi at: (%d,%d)\n", taxi.x, taxi.y);
#endif
}

/* Main Guiding Loop */
int ClientMainLoop()
{
	char *Response = NULL;
	int ShutDown = 0;
	int length;
	int errorcode;

	/* Initialization */
	memset(Temp, 0, sizeof(Temp));
	memset(SendBuf, 0, sizeof(SendBuf));
	memset(ID, 0, sizeof(ID));
	memset(TaxiNumber, 0, sizeof(TaxiNumber));
	memset(RealRe, 0, sizeof(RealRe));

	/* I.a Send Taxi Request */
	position = GTK1(0, 0);	/* Call GTK Window 1, acquire request info */
	if (RealRe[0] == 0) {	
		return 0;	/* User Close the Window intentionally */
	}
		/* Store Destination and Pickup Location */
	strcpy(PickUpLocation, position.posi_R);
	strcat(PickUpLocation, position.posi_C);
	length = strlen(PickUpLocation);
	PickUpLocation[length] = 0;	/* Set an End Mark */
	strcpy(Destination, position.desti_R);
	strcat(Destination, position.desti_C);
	length = strlen(Destination);
	Destination[length] = 0;	/* Set an End Mark */

	strcpy(SendBuf, RealRe);
#ifdef DEBUG
	printf("Pick Up Location: '%s'\n", PickUpLocation);
	printf("Destination: '%s'\n", Destination);
	printf("%s: Original Message: '%s' \n", Program, RealRe);
	printf("%s: Copied Message: '%s' \n", Program, SendBuf);
	/* Debug Code, ShutDown Server, Classfied */
	if (SendBuf[13] == '*') {
		if (SendBuf[14] == '@') {
			memset(SendBuf, 0, sizeof(SendBuf));
			strcpy(SendBuf, "Bye");
			Response = Socket();
			exit(0);
		}
	}
#endif
	/* I.b Receive Response */
	Response = Socket();
	/* I.c Check if a Taxi Available */
	errorcode = HandleRequestResponse(Response);
	if (errorcode != 0) {	/* Error/Decline */
		ShutDown = 1;
		return 2;	/* Restart Client Main Loop */
	}

	/* II. Confirm an Order */
	printf("Please Respond CONFIRM or CANCEL:\n");
	/* II.a Generate Readable Sentences */
	GenReadableSring(Response);
	/* II.b Call GTK2 */
	GTK2(0, 0, Message);	/* Fix ME! More Information */
	memset(SendBuf, 0, sizeof(SendBuf));
	memset(Temp, 0, sizeof(Temp));
	if (FF == 1) {
		strcpy(Temp, "CONFIRM");
		Temp[7] = 0;
		ShutDown = 0;
	}
	else /* FF = 0 */
	{
		strcpy(Temp, "CANCEL");
		Temp[6] = 0;
		ShutDown = 1;	/* User Cancel a Taxi Request, End */
		GTK4(0, 0, "Order Cancelled!", "Thank you for choosing Uder!");
		Response = Socket();
		return 0;
	}
	RebuildString(0);	/* Add Order Number to the tail */
#ifdef DEBUG
	printf("%s: Message to be sent: '%s' \n", Program, SendBuf);
#endif
	Response = Socket();
	/* II.c Handle TimeOut, otherwise find exact Pick Up Time */
	errorcode = HandleErrorMessage(Response);
	if (errorcode != 0) {
		if (errorcode == 3) {	/* Invalid */
			GTK4(0, 0, "Invalid!", "You seem to miss the Confirm Window. Resubmit a new request pls.");
			ShutDown = 1;
			return 2;	/* Restart Client Main Loop */
		}
		else {	/* Error */
			ShutDown = 1;
			return 2;	/* Restart Client Main Loop */
		}
	}
	else {
	   /* Acquire Exact Pick Up Time/Taxi Location */
		AnalyCurrentLocation(Response);
		if (0 == strcmp(CurrentPosition, PickUpLocation)) { /* Reday for pick up */
			printf("Taxi Arrive at pre-set Pickup Location!\n");
			GTK3(0, 0, "Ready for Pick Up");
			memset(PickUpLocation, 0, sizeof(PickUpLocation));
			strcpy(PickUpLocation, "MX");	/* Avoid arrive at Pick Up loation twice */
		}
		/* GTK1'(0£¬0£¬"You Taxi is on the way~"); */
	}

	/* III. Accquire Taxi position every X second */
	/* III.a CreateWindow and Set Taxi */
	strcpy(GTKbuffer, "Your Taxi is on the way!");
	CalTaxiCoordinate(CurrentPosition);
	Window = CreateWindow(0, 0);
	/* III.b Enter Loop */
	do {
		memset(Temp, 0, sizeof(Temp));
		strcpy(Temp, "REQUEST_POSITION");
		RebuildString(1);
		Response = Socket();
/* FIX ME, Acquire Exact Pick Up Time */
		if (HandleErrorMessage(Response) != 0) {
			continue;
		}
		/* Acquire Current Position */
		AnalyCurrentLocation(Response);
#ifdef DEBUG
		printf("Current Location: '%s'\n", CurrentPosition);
		printf("Previous Location: '%s'\n", LastPosition);
#endif
		CalTaxiCoordinate(CurrentPosition);
		/* Analyses Order Status */
		if (0 == strcmp(CurrentPosition, PickUpLocation)) { /* Reday for pick up */
			GTK3(0, 0, "Ready for Pick Up");
			strcpy(GTKbuffer, "Your Taxi is on its way to the destination!");
			UpdateWindow();
			memset(PickUpLocation, 0, sizeof(PickUpLocation));
			strcpy(PickUpLocation, "MX");	/* Avoid arrive at Pick Up loation twice */
		}
		if (0 == strcmp(CurrentPosition, Destination)) {	/* Arrive at destination */
			if (0 == strcmp(PickUpLocation, "MX")) {	/* Avoid arrive at Destination before Pick Up*/
				GTK3(0, 0, "Arrive at Destination, Thanks!");
				gtk_widget_destroy(Window);
				Jsig = 0;/* Keep Request Position to send Taxi Back */
			}
		}
		if (0 == strcmp(CurrentPosition, LastPosition)) {	/* Taxi stop */
			printf("Taxi Back to a stop!\n");
			ShutDown = 1;
			return 0;
		}

		/* GTK Map Refresh */
		if (GTKMessage)
		{
			gtk_label_set_label(GTKMessage, GTKbuffer);
		}
		if (GTKMap)
		{
			gdk_window_invalidate_rect(GTKMap->window,	/* update it! */
				&GTKMap->allocation, /* invalidate entire region */
				FALSE);
		}
		UpdateWindow();
		if (Jsig) {
			break;
		}
		ShutDown = Jsig;
		sleep(LoopIntervalConstant);
	} while (!ShutDown);
	return 0;
}

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
	printf("Loading Map Successfully!\n");
	return 1;
}

int main(int argc, char *argv[])
{
	Program = argv[0];	/* publish program name (for diagnostics) */
	int errorcode;

	/* Load Map */
	errorcode = LoadMapFromFile();
	assert(errorcode);
	GTKMaplength = map->col * 25;
	GTKMapwidth = map->row * 25;

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
	errorcode = 0;
	do {
		errorcode = ClientMainLoop();
	} while (errorcode == 2);
	printf("%s: Exiting...\n", Program);
	return 0;
}
