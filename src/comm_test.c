// comm_test.c: test communication module
// with multiple test cases, can be adjusted and extended
// check the makefile for more detailed usage
//
// Huan Chen, 3/9/2017
// updated 3/10/2017, add sleep intervals, support multi_threading, collect cost information; verify costs to be added

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>  // read(), write(), close()
#include<string.h>
#include<netinet/in.h>  // internet domain addresses
#include<netdb.h>
#include<time.h>
#include<sys/select.h>
#include<arpa/inet.h> 
#include<assert.h>
#include<pthread.h> // multithreading
#include<regex.h>   // for regular expression
#include<limits.h>  // INT_MAX

const int SLEEP_TIME = 250000; // number of microseconds to sleep during each request (taxi request, position request)
const int SLEEP_INTERVAL = 3000000; // number of microseconds to sleep between tests
const int MULTI_THREAD = 0; // multithreading flag, if set to 1, taxi requests will be made concurrently, otherwise, sequentially
const int maxStepCnt = 100;   // max number of steps allowed, stop requesting positions over this limit to prevent endless loop

typedef struct thread_data {
    int clientId;
    const char *host;
    int portNo;
    const char *origin;
    const char *dest; // destination
    int stepCnt;
    //float cost; 
} thread_data_t;

void setThreadData(int clientId, const char *host, int portNo, const char *origin, const char *dest, int stepCnt, thread_data_t *data) {
    assert(data);
    data->clientId = clientId;
    data->host = host;
    data->portNo = portNo;
    data->origin = origin;
    data->dest = dest;
    data->stepCnt = stepCnt;
}

void verifyCost(float cost, float expected) {
    // if (cost != expected)
    // printf("Incorrect cost.\n"); 
}

int match(const char *string, char *pattern) {
    int status;
    regex_t re;

    if (regcomp(&re, pattern, REG_EXTENDED | REG_NOSUB) != 0) {
        return 0;   // report error
    }

    status = regexec(&re, string, (size_t) 0, NULL, 0);
    regfree(&re);
    //printf("status = %d\n", status);
    if (status == 0) {
        return 1;   // matched!
    }
    else if (status == REG_NOMATCH) {
        return 0;   // not match!
    }
    return 2; // unknown error
}

void error(const char *msg) {
    perror(msg);
    exit(1);
}

// given port number, return a integer (server socket file descriptor)
int makeServerSocket(uint16_t portNo) {
    int serverSocketFD;
    struct sockaddr_in serverSocketName;    // internet address

    // create the socket
    serverSocketFD = socket(PF_INET, SOCK_STREAM, 0);   // protocol family addr, stream socket, 
    if (serverSocketFD < 0) { // return -1 if socket call fails
        error("Service socket creation failed.");
    }

    // bind the socket to server
    serverSocketName.sin_family = AF_INET;
    serverSocketName.sin_port = htons(portNo);  // host byte order to network byte order
    serverSocketName.sin_addr.s_addr = htonl(INADDR_ANY);   // IP address of the host
    if (bind(serverSocketFD, (struct sockaddr*) &serverSocketName, sizeof(serverSocketName)) < 0) {
        error("Binding the server to a socket failed");
    }

    // listen to the pocket
    if (listen(serverSocketFD, 5) < 0) {
        error("Listen on the socket failed.");
    }
    return serverSocketFD;
}

void sendRequest(int clientNo, const char *hostName, int portNo, char sendBuf[], char recvBuf[]);

void getTaxiName(const char *buffer, char taxiName[]) {
    // buffer = "OK Taxi7 POSITION..."
    memset(taxiName, 0, sizeof(taxiName));
    char *pos1 = strstr(buffer, " ");
    char *pos2 = strstr(pos1 + 1, " ");
    strncpy(taxiName, pos1 + 1, pos2 - pos1);
}

void getCurrLoc(const char *buffer, char currLoc[]) {
    // buffer = "OK <taxi> POSITION <position> [ETA <position> <time>]"
    memset(currLoc, 0, sizeof(currLoc));
    const char *KEY = "POSITION";
    char *pos1 = strstr(buffer, KEY);
    char *pos2 = strstr(pos1 + strlen(KEY) + 1, " ");
    strncpy(currLoc, pos1 + strlen(KEY) + 1, pos2 - strlen(KEY) - 1 - pos1);
}

float getCostFromMsg(const char *buffer) {
    // example buffer: OK Taxi7 PICKUP S8 10:15 DROPOFF D4 11:42 $8.75 CONFIRM
    float cost = 0.0;
    char *pos1 = strstr(buffer, "$");
    char *pos2 = strstr(pos1, ".");
    int len = pos2 - pos1 - 1;
    char str[256] = {0};
    strncpy(str, pos1 + 1, len);
    int x = atoi(str);
    pos1 = strstr(pos2, " ");
    len = pos1 - pos2 + 1;
    memset(str, 0, sizeof(str));
    strncpy(str, pos2 + 1, len);
    int y = atoi(str);
    cost = x + y / 100.0;
    return cost;
}

// mock taxi request to server, from university to the airport (NewIrvine.map)
// return cost (float)
float testRequestOneTaxi(int clientNo, const char *hostName, int portNo, const char *origin, const char *destination) {
    printf("-- Client #%d: %s -> %s --\n", clientNo, origin, destination);
    char sendBuf[256] = {0};
    char recvBuf[256] = {0};  // reply from server

    sprintf(sendBuf, "REQUEST_TAXI %s TO %s", origin, destination);
    //strcpy(sendBuf, "REQUEST_TAXI S8 TO D4");
    sendRequest(clientNo, hostName, portNo, sendBuf, recvBuf);
    // example expected reply: OK Taxi7 PICKUP S8 10:15 DROPOFF D4 11:42 $8.75 CONFIRM
    // check reply msg 

    //assert(strncmp(recvBuf, "OK", 2) == 0);
    //char *confirmStr = strstr(recvBuf, "CONFIRM");
    //assert(confirmStr && strcmp(confirmStr, "CONFIRM") == 0);
    
    // use regular expression to verify the format of the received message
    char *timePattern = "[0-9]+:[0-9]+";    // time pattern: need to be digits, i.e. 9:36 or 13:00
    char *locPattern = "[A-Z]+[0-9]+";  // returned location pattern: need to be uppercase letters and digits, i.e. "D12"
    char *moneyPattern = "\\$[0-9]+.[0-9]{2}";  // money pattern: need to be dollars and cents, i.e. $5.00 or $8.75

    //char *testMsg = "OK Taxi7 PICKUP S8 10:15 DROPOFF D4 11:42 $8.75 CONFIRM #10042";
    char reqTaxiPattern[256] = {0};
    sprintf(reqTaxiPattern, "OK Taxi[0-9]+ PICKUP %s %s DROPOFF %s %s %s CONFIRM #[0-9]+", locPattern, timePattern, locPattern, timePattern, moneyPattern);
    //char *reqTaxiPattern = "OK Taxi[0-9]+ PICKUP [A-Z]+[0-9]+ [0-9]+:[0-9]+ DROPOFF [A-Z]+[0-9]+ \\$[0-9]+.[0-9]{2} CONFIRM";
    //assert(match(testMsg, reqTaxiPattern) == 1);
    assert(match(recvBuf, reqTaxiPattern) == 1);

    // get taxi cost from the received reply
    float cost = getCostFromMsg(recvBuf);

    // confirm the taxi order
    char *confirmNum = strstr(recvBuf, "#");    // send "CONFIRM #10042" to server
    sprintf(sendBuf, "CONFIRM %s", confirmNum);
    //strcpy(sendBuf, "CONFIRM");
    sendRequest(clientNo, hostName, portNo, sendBuf, recvBuf);
    //assert(strncmp(recvBuf, "OK", 2) == 0);
    //assert(strcmp(recvBuf, "OK Taxi7 POSITION D8 ETA S8 10:15") == 0);
    //char *confirmPattern = "OK Taxi[0-9]+ POSITION [A-Z]+[0-9]+ ETA [A-Z]+[0-9]+ [0-9]+:[0-9]+";
    char confirmPattern[256] = {0};
    sprintf(confirmPattern, "OK Taxi[0-9]+ POSITION %s ETA %s %s", locPattern, locPattern, timePattern);
    assert(match(recvBuf, confirmPattern) == 1);


    // request taxi position, assume the origin is different from the destination
    //char *taxiName = "Taxi7";   // for demo purpose
    char currLoc[256] = {0};
    char taxiName[256] = {0};
    getTaxiName(recvBuf, taxiName);

    sprintf(sendBuf, "REQUEST_POSITION %s", taxiName);
    //strcpy(sendBuf, "REQUEST_POSITION Taxi7");
    sendRequest(clientNo, hostName, portNo, sendBuf, recvBuf);
    //assert(strncmp(recvBuf, "OK", 2) == 0);
    //assert(strcmp(recvBuf, "OK Taxi7 POSITION J2 ETA D4 11:45") == 0);
    char reqPosPattern[256] = {0};
    char reqPosPattern2[256] = {0};
    sprintf(reqPosPattern, "OK Taxi[0-9]+ POSITION %s ETA %s %s", locPattern, locPattern, timePattern);
    sprintf(reqPosPattern2, "OK Taxi[0-9]+ POSITION %s", locPattern);
    assert(match(recvBuf, reqPosPattern) == 1 || match(recvBuf, reqPosPattern2) == 1);

    getCurrLoc(recvBuf, currLoc);
    printf("Returned taxi location: %s\n", currLoc);

    // TODO
    // int expectedStepCnt == getStepCnt(origin, destination, map);
    // or passed in from arguments?
    // uncomment the following line to run test properly
    int expectedStepCnt = 3;
    //int expectedStepCnt = INT_MAX;    // now it does not affect while loop
    
    int stepCnt = 1;
    while (strcmp(currLoc, destination) != 0) {   
        //request position until reaching destination
        sprintf(sendBuf, "REQUEST_POSITION %s", taxiName);
        sendRequest(clientNo, hostName, portNo, sendBuf, recvBuf);
        assert(match(recvBuf, reqPosPattern) == 1 || match(recvBuf, reqPosPattern2) == 1);
        //assert(match(recvBuf, reqPosPattern) == 1);

        getCurrLoc(recvBuf, currLoc);
        printf("Returned taxi location: %s\n", currLoc);
        stepCnt++;
        // in real test scenarios, comment out the following line
        if (stepCnt == expectedStepCnt) break;   // for demo use only
        if (stepCnt >= maxStepCnt) {
            printf("%s -> %s: ", origin, destination);
            printf("%d steps passed, fail to reach destination, optimize your route!\n", maxStepCnt);
            break;
        }
    }

    assert (stepCnt == expectedStepCnt);
    return cost;
}

void sendRequest(int clientNo, const char *hostName, int portNo, char sendBuf[], char recvBuf[]) {
    // verify host name and port number
    struct hostent* server = NULL;
    if ((server = gethostbyname(hostName)) == NULL) {
        fprintf(stderr, "%s: no such host named '%d'\n", hostName, portNo);
        exit(2);
    }

    if (portNo <= 2000) {
        error("Host number should be greater than 2000");
    }

    memset(recvBuf, 0, 256);

    int socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFD < 0) { // return -1 if socket call fails
        error("Service socket creation failed.");
    }

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(portNo);
    serverAddress.sin_addr = *(struct in_addr *) server->h_addr_list[0];
    printf("\n");
    printf("Client #%d: Connecting to host '%s':'%d'...\n", clientNo, hostName, portNo);

    if (connect(socketFD, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
        error("Connect failed.");
    }

    printf("Send: %s\n", sendBuf); 
    int n = write(socketFD, sendBuf, strlen(sendBuf));
    if (n < 0) {
        error("Error writing to socket.");
    }

    n = read(socketFD, recvBuf, 255);
    //n = read(socketFD, recvBuf, sizeof(recvBuf) - 1);
    if (n < 0) {
        error("Error reading from socket.");
    }
    //printf("n = %d\n", n);
    printf("Received: %s\n", recvBuf);
    close(socketFD);
    memset(sendBuf, 0, sizeof(sendBuf));

    fflush(stdout); // print log immediately, without buffering

    // sleep for 0.25 seconds during each request
    usleep(SLEEP_TIME);
}

void myThreadFunc(void *vargp) {
    int myId = (int) (uintptr_t) vargp;
    printf("Client ID: %d \n", myId);
    testRequestOneTaxi(myId, "laguna", 2001, "S8", "D4");
}

void* thread_func(void *arg) {
    assert(arg);
    thread_data_t *data = (thread_data_t *) arg;
    printf("Thread # %ld\n", pthread_self());
    //printf("Client ID: %d, %s -> %s\n", data->clientId, data->origin, data->dest);
    testRequestOneTaxi(data->clientId, data->host, data->portNo, data->origin, data->dest);
    // dummy returned stepCnt
    data->stepCnt = data->clientId + 10;   // test output of threads call
    return data;    // pass output out if any
}

void testOneClientWithOneRequest(const char *host, int portNo) {
    printf("== One client with one request ==\n\n");
    // one client requests one taxi from ORIGIN to DEST

    //int numClients = 7;
    // test origins and destinations
    const char *origins[] = {"S8", "D8", "S8", "Taxi Stand B", "University of New Irvine (UNI)", "S8", "CORNER 8th Street AND Stand Fourth"};
    const char *dests[] = {"D4", "D4", "D12", "D12", "Santa Claus Airport (SCA)", "CORNER Doc Arthur Blvd AND 12th Street"};

    int numTests = sizeof(origins) / sizeof(const char *);
    float *costs = malloc(sizeof(int) * numTests);
    printf("%d tests\n", numTests);
    int i;
    for (i = 0; i < numTests; i++) {
        // testRequestOneTaxi(i + 1, host, portNo, origins[i], dests[i]);
        costs[i] = testRequestOneTaxi(i + 1, host, portNo, origins[i], dests[i]);
        verifyCost(costs[i], 0);
    }
    free(costs);

    /* 
       int clientId = 100;

       testRequestOneTaxi(clientId, host, portNo, "S8", "D4");

       testRequestOneTaxi(clientId, host, portNo, "D8", "D4");   
    // expectedStepCnt = 4 + (include taxiStand -> origin and destination -> taxiStand ?)

    // all the following tests request the taxi from the university to the airport
    testRequestOneTaxi(clientId, host, portNo, "S8", "D12");

    testRequestOneTaxi(clientId, host, portNo, "Taxi Stand B", "D12");

    testRequestOneTaxi(clientId, host, portNo, "University of New Irvine (UNI)", "D12");

    testRequestOneTaxi(clientId, host, portNo, "S8", "Santa Claus Airport (SCA)");

    testRequestOneTaxi(clientId, host, portNo, "CORNER 8th Street AND Stand Fourth", "CORNER Doc Arthur Blvd AND 12th Street");
    */

    printf("== Success. ==\n\n");
}

void testMultiClientsWithSameRequests(const char *host, int portNo) {
    // multiple clients request taxis from the same origin to the same destination 
    printf("== Multiple clients with the same requests ==\n\n");
    int numClients = 3;
    int i;
    float *costs = malloc(sizeof(int) * numClients);
    // if no multi threading set, make taxi requests sequentially
    if (!MULTI_THREAD) {
        for (i = 0; i < numClients; i++) {
            // testRequestOneTaxi(i + 1, host, portNo, "S8", "D4");
            costs[i] = testRequestOneTaxi(i + 1, host, portNo, "S8", "D4");
            verifyCost(costs[i], 0);
        }
    }
    // otherwise, make taxi requests concurrently
    else {
        pthread_t tid;
        thread_data_t a[numClients];
        for (i = 0; i < numClients; i++) {
            setThreadData(i + 1, host, portNo, "S8", "D4", 19, &a[i]);
            pthread_create(&tid, NULL, thread_func, (void *) &a[i]);
            //pthread_create(&tid, NULL, (void *) &myThreadFunc, (void *) (uintptr_t) (i + 1));
        }

        pthread_exit(NULL);
    }
    free(costs);
    printf("== Success. ==\n\n");
}

void testMultiClientsWithDiffRequests(const char *host, int portNo) {
    // multiple clients request taxis from different origins to different destinations
    printf("== Multiple clients with different requests ==\n\n");
    int numClients = 3;
    int i;
    int *costs = malloc(sizeof(int) * numClients);
    thread_data_t a[numClients];
    setThreadData(1, host, portNo, "D8", "D4", 4, &a[0]);
    setThreadData(2, host, portNo, "E24", "D4", 25, &a[1]);
    setThreadData(3, host, portNo, "Q20", "D4", 29, &a[2]);

    // if no multi threading set, make taxi requests sequentially
    if (!MULTI_THREAD) {
        for (i = 0; i < numClients; i++) {
            // testRequestOneTaxi(i + 1, host, portNo, a[i].origin, a[i].dest); 
            costs[i] = testRequestOneTaxi(i + 1, host, portNo, a[i].origin, a[i].dest); 
            verifyCost(costs[i], 0);
        }
    }
    // otherwise, make taxi requests concurrently
    else {
        pthread_t threads[numClients];
        thread_data_t *b[numClients];   // stores the result of threads call
        for (i = 0; i < numClients; i++) {
            pthread_create(&threads[i], NULL, thread_func, (void *) &a[i]);
        }

        // wait for the completion of the threads
        for (i = 0; i < numClients; i++) {
            pthread_join(threads[i], (void **) &b[i]);
            printf("Client #%d: returned step cnt = %d\n", b[i]->clientId, b[i]->stepCnt);
        }
        pthread_exit(NULL);
    }
    free(costs);
    printf("== Success. ==\n\n");
}

void testGetCost() {
    const char *buffer = "OK Taxi7 PICKUP S8 10:15 DROPOFF D4 11:42 $8.75 CONFIRM";
    float cost = getCostFromMsg(buffer);
    assert(cost == 8.75);
}

void printWaitStatus() {
    const char *padding = "##############################################";
    printf("%s\n", padding);
    printf("Wait the server for %f seconds to process requests...\n", SLEEP_INTERVAL / 1E6);
    fflush(stdout);
    usleep(SLEEP_INTERVAL);
}

int main(int argc, char *argv[]) {
    // send a valid request to server to request taxi
    if (argc < 3) {
        printf("Usage: %s <hostname> <port>\n", argv[0]);
        exit(1);
    } 

    const char *host = argv[1];
    int portNo = atoi(argv[2]);

    // a third argument can be used to run different tests
    //int testId = argv[3];

    //if (testId == 1)
    testOneClientWithOneRequest(host, portNo);

    printWaitStatus();

    // uncomment this line to run testMultiClientsWithSameRequests()
    // once uncommented, testMultiClientsWithDiffRequests() will not run in multithreading mode

    // if (testId == 2) 
    testMultiClientsWithSameRequests(host, portNo);

    printWaitStatus();

    // if (testId == 3)
    testMultiClientsWithDiffRequests(host, portNo);

    printf("-- Tests finished. --\n");

    return 0;
}
