# Lower-level Makefile
# Yifan Xu, 03/19/2017
# Yan Zhang, 03/19/2017
# Support: make; make test; make clean;
# make debug(create ClientD and ServerD);
# make runs(Run Server); make runc(Run Client);
# make runsd(Run Server with debug mode); make runcd(Run Client with debug mode);


GTKINC	= `PKG_CONFIG_PATH=/usr/share/pkgconfig pkg-config --cflags gtk+-2.0`
GTKLIBS	= `PKG_CONFIG_PATH=/usr/share/pkgconfig pkg-config --libs gtk+-2.0`

INC	= $(GTKINC)
LIBS	= $(GTKLIBS)
CC 	= gcc


CFLAGS	= -Wall -g
DFLAGS	= -Wall -g -DDEBUG


all: Client Server
test: ClientD ServerD TestClientUI TestServerUI TestModeClient TestModeServer AutoTest TestPathfind

clean:
	rm -f *.o
	rm -rf ../bin/Client
	rm -rf ../bin/ClientD
	rm -rf ../bin/Server
	rm -rf ../bin/ServerD
	rm -rf ../bin/TestModeServer
	rm -rf ../bin/TestModeClient
	rm -rf ../bin/TestClientUI
	rm -rf ../bin/TestServerUI
	rm -rf ../bin/AutoTest
	rm -rf ../bin/TestPathfind
cleano:
	rm -f *.o

## For Main Command
runs:
	../bin/Server 2009
runc:
	../bin/Client crystalcove.eecs.uci.edu 2009

## For Debug Command
debug:	ServerD ClientD
runsd:
	../bin/ServerD 2009
runcd:
	../bin/ClientD crystalcove.eecs.uci.edu 2009

## For Test Command
testsui: TestServerUI
	../bin/TestServerUI
testcui: TestClientUI
	../bin/TestClientUI
runst:	TestModeServer
	../bin/TestModeServer 2009
runct: 	TestModeClient
	../bin/TestModeClient crystalcove.eecs.uci.edu 2009
test-comm: AutoTest
	../bin/AutoTest crystalcove.eecs.uci.edu 2009

## For Objects
Client.o: Client.c
	$(CC) -c Client.c $(CFLAGS) $(INC) -o Client.o

ClientD.o: Client.c
	$(CC) -c Client.c $(DFLAGS) $(INC) -o ClientD.o

ClientMap.o: ClientMap.c
	$(CC) -c ClientMap.c $(INC) $(LIBS) $(CFLAGS) -o ClientMap.o

ClientMapD.o: ClientMap.c
	$(CC) -c ClientMap.c $(INC) $(LIBS) $(DFLAGS) -o ClientMapD.o

GetTime.o: GetTime.c GetTime.h
	$(CC) -c GetTime.c $(CFLAGS) -o GetTime.o

GetTimeD.o: GetTime.c GetTime.h
	$(CC) -c GetTime.c $(DFLAGS) -o GetTimeD.o

CallB.o: CallB.c CallB.h constants.h
	$(CC) -c CallB.c $(INC) $(CFLAGS) -o CallB.o

CallBD.o: CallB.c CallB.h constants.h
	$(CC) -c CallB.c $(INC) $(DFLAGS) -o CallBD.o

GTKMain.o: GTKMain.c GTKMain.h CallB.h constants.h
	$(CC) -c GTKMain.c $(INC) $(CFLAGS) $(LIBS) -o GTKMain.o

GTKMainD.o: GTKMain.c GTKMain.h CallB.h constants.h
	$(CC) -c GTKMain.c $(INC) $(DFLAGS) $(LIBS) -o GTKMainD.o

Server.o: SocketServer.c GetTime.h
	$(CC) -c SocketServer.c $(INC) $(CFLAGS) -o Server.o

ServerD.o: SocketServer.c GetTime.h
	$(CC) -c SocketServer.c $(INC) $(DFLAGS) -o ServerD.o

Response.o: Response.c
	$(CC) -c Response.c $(CFLAGS) -o Response.o

ResponseD.o: Response.c
	$(CC) -c Response.c $(DFLAGS) -o ResponseD.o

map.o: map.c
	$(CC) -c map.c $(CFLAGS) -o map.o

mapD.o: map.c
	$(CC) -c map.c $(DFLAGS) -o mapD.o

utils.o: utils.c
	$(CC) -c utils.c $(CFLAGS) -o utils.o

utilsD.o: utils.c
	$(CC) -c utils.c $(DFLAGS) -o utilsD.o

Pathfind.o: Pathfind.c Pathfind.h
	$(CC) -c Pathfind.c $(CFLAGS) -o Pathfind.o

PathfindD.o: Pathfind.c Pathfind.h
	$(CC) -c Pathfind.c $(DFLAGS) -o PathfindD.o

ServerManagement.o: ServerManagement.c ServerManagement.h 
	$(CC) -c ServerManagement.c $(CFLAGS) -o ServerManagement.o

ServerManagementD.o: ServerManagement.c ServerManagement.h 
	$(CC) -c ServerManagement.c $(DFLAGS) -o ServerManagementD.o
 
Dboard.o: Dboard.c Dboard.h
	$(CC) -c Dboard.c $(INC) $(LIBS) $(CFLAGS) -o Dboard.o

DboardD.o: Dboard.c Dboard.h
	$(CC) -c Dboard.c $(INC) $(LIBS) $(DFLAGS) -o DboardD.o

## For Main App
Server: Server.o Response.o map.o utils.o Pathfind.o Dboard.o ServerManagement.o GetTime.o
	$(CC) Server.o Response.o map.o Pathfind.o utils.o Dboard.o ServerManagement.o GetTime.o $(LIBS) -o ../bin/Server

Client: Client.o GTKMain.o CallB.o map.o utils.o ClientMap.o
	$(CC) $(GTK) $(LIBS) Client.o GTKMain.o CallB.o map.o utils.o ClientMap.o -o ../bin/Client

## For Test Objects
# For Main App Test
ServerD: ServerD.o ResponseD.o mapD.o utilsD.o PathfindD.o DboardD.o ServerManagementD.o GetTimeD.o
	$(CC) ServerD.o ResponseD.o mapD.o PathfindD.o utilsD.o DboardD.o ServerManagementD.o GetTimeD.o $(LIBS) -o ../bin/ServerD

ClientD: ClientD.o GTKMainD.o CallBD.o mapD.o utilsD.o ClientMapD.o
	$(CC) $(GTK) $(LIBS) ClientD.o GTKMainD.o CallBD.o mapD.o utilsD.o ClientMapD.o -o ../bin/ClientD

# For test Client only, running a Server without loading map nor calling Response(Core System)
TestModeServer.o: TestModeServer.c
	$(CC) -c TestModeServer.c $(DFLAGS) -o TestModeServer.o
TestModeServer: TestModeServer.o
	$(CC) TestModeServer.o -o ../bin/TestModeServer

# For test Server only, running a Client with automatically request sent module built in
TestModeClient.o: TestModeClient.c
	$(CC) -c TestModeClient.c $(DFLAGS) -o TestModeClient.o
TestModeClient:	TestModeClient.o
	$(CC)  TestModeClient.o -o ../bin/TestModeClient

# For test Server's UI only
TestServerUI.o:	TestServerUI.c
	$(CC) -c TestServerUI.c $(INC) $(DFLAGS) -o TestServerUI.o
TestServerUI:	map.o  TestServerUI.o Dboard.o utils.o ServerManagement.o Pathfind.o
	$(CC) map.o TestServerUI.o Dboard.o utils.o ServerManagement.o Pathfind.o $(LIBS) -o ../bin/TestServerUI

# For test Clinet's UI only
TestClientUI.o: TestGTKMain.c
	$(CC) -c TestGTKMain.c $(INC) $(LIBS) $(DFLAGS) -o TestClientUI.o
TestClientUI: TestClientUI.o CallB.o GTKMain.o ClientMap.o map.o utils.o
	$(CC) TestClientUI.o GTKMain.o CallB.o ClientMap.o map.o utils.o -o $(LIBS) -o ../bin/TestClientUI

# For test Pathfind only
TestPathfind.o: TestPathfind.c	
	$(CC) -c TestPathfind.c $(DFLAGS) -o TestPathfind.o
TestPathfind:	PathfindD.o TestPathfind.o mapD.o utilsD.o ServerManagementD.o
	$(CC) PathfindD.o TestPathfind.o mapD.o utilsD.o ServerManagementD.o $(LIBS) -o ../bin/TestPathfind

# AutoTest Server & Socket, from Huan~
AutoTest: comm_test.c
	$(CC) $(DFLAGS) comm_test.c -o ../bin/AutoTest -lpthread

