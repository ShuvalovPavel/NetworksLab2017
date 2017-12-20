#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <netdb.h>  
#include <arpa/inet.h>
#include <stdbool.h>
#include <ctype.h>

#define PORT 5056
#define MAXCLIENTS 255
#define BUFSIZE 100

char shutdown_serv[] = "SHUTDOWN";
char deactivate[] = "DEACT";
int clientsCount = -1;
int result = -1;
int listener;
int currentId = 0;
int maxUserId = -1;

struct repo {
    char description[BUFSIZE];
    char developerName[BUFSIZE];
    char projectName[BUFSIZE]; 
    char status;                              //  ['B']ug,  ['F']ixed,  ['C']losed
} *repositories;

struct users{
    char userName[BUFSIZE/2-2];
    char userType;  //['T']ester, ['D']eveloper
    char password[BUFSIZE/2-2];
} *registredUsers;

struct client {
    char* ip;
    int port;
    int socketDesc;
    bool isActive;
    int userId;
} *connectingClients;

void ShutdownServer() {
    for (int i = 0; i <= clientsCount; i++) {
        close(connectingClients[i].socketDesc);
    }
}

void KickClient(int id) {
    printf("User: %s disconnected.\n", registredUsers[connectingClients[id].userId].userName);
    SendToClient(connectingClients[id].socketDesc, "KICKED");
    close(connectingClients[id].socketDesc);
    connectingClients[id].isActive = false;
    clientsCount--;
}

int SendToClient(int socket, char* message, void* currentIndex) {
    if (send(socket, message, BUFSIZE, 0) < 0) {
        if (currentIndex != "NULL"){
            perror("Message not sent to client. Socket was closed");
            KickClient((int)currentIndex);
            pthread_exit(NULL);
            return -1;
        }
    }
}

int curClientId() {
    for (int i = 0; i < MAXCLIENTS + 1; i++) {
        if (!connectingClients[i].isActive)
            return i;
    }
    return -1;
}

int createNewProject(char *developerName, char *projectName, char *description, char status) {
    for (int i=0; i<BUFSIZE; i++){
        repositories[currentId].developerName[i]=developerName[i];
        repositories[currentId].projectName[i]=projectName[i];
        repositories[currentId].description[i]=description[i];
    }
    repositories[currentId].status = status;                           //  ['B']ug,  ['T']est,  ['F']ixed,  ['C']losed
    currentId ++;
    return currentId-1;
}

workTester(int socket, void* currentIndex, char *userName){
    char buf[BUFSIZE]; //Buffer
    for(;;){
            bzero(buf, BUFSIZE+1);   
                
                if (recv(socket, buf, BUFSIZE, 0) < 0) {
                    KickClient((int)currentIndex);
                    pthread_exit(NULL);
                }
                if (buf[0] == '1') {
                        for (int i = 0; i < currentId; i++){
                            if (repositories[i].status == 'F'){
                                SendToClient(socket, repositories[i].developerName, currentIndex);
                                SendToClient(socket, repositories[i].projectName, currentIndex);
                                SendToClient(socket, repositories[i].description, currentIndex);

                                char tmp1[BUFSIZE];
                                sprintf(tmp1, "%d", i);
                                SendToClient(socket, tmp1, currentIndex);
                            }
                        }
                        SendToClient(socket, "END", currentIndex);
                        buf[0] = '0';
                }

                if (buf[0] == '2') {
                        for (int i = 0; i < currentId; i++){
                            if (repositories[i].status == 'B'){
                                SendToClient(socket, repositories[i].developerName, currentIndex);
                                SendToClient(socket, repositories[i].projectName, currentIndex);
                                SendToClient(socket, repositories[i].description, currentIndex);
                                char tmp1[BUFSIZE];
                                sprintf(tmp1, "%d", i);
                                SendToClient(socket, tmp1, currentIndex);
                            }
                        }
                        SendToClient(socket, "END", currentIndex);
                        buf[0] = '0';
                }
                if (buf[0] == '3') {
                   
                        char *developerName;
                        char *projectName; 
                        char *description;
                        char *istr;
                        char *error = "N";
                        istr = strtok (buf," ");
                        developerName = strtok (NULL," ");
                        if ((developerName == "") || (developerName == NULL)){
                            error = ("Incorrect developer name.");
                        }
                        projectName = strtok (NULL," ");
                        if ((projectName == "") || (projectName == NULL)){
                            error = ("Incorrect project name.");
                        }
                        description = strtok (NULL," ");
                        if ((description == "") || (description == NULL)){
                            error = ("Incorrect description.");
                        }
                        if (*istr != '3'){
                            error = "Incorrect request.";
                        }

                        SendToClient(socket, error, currentIndex);


    
                        int id = createNewProject(developerName, projectName, description, 'B'); 
            

                        char tmp1[BUFSIZE];
                        bzero(tmp1, BUFSIZE+1);
                        sprintf(tmp1, "%d", id);
                        SendToClient(socket, tmp1, currentIndex);
                        buf[0] = '0';
                }

                if (buf[0] == '4') {
                        char tmp1[BUFSIZE];
                        for (int i = 0; i < currentId; i++){
                            if (repositories[i].status == 'F'){
                                sprintf(tmp1, "%d", i);
                                SendToClient(socket, tmp1, currentIndex);
                            }
                        }
                        SendToClient(socket, "N", currentIndex);

                        if (recv(socket, buf, BUFSIZE, 0) < 0) {
                            KickClient((int)currentIndex);
                            pthread_exit(NULL);
                        }
                        char curStatus = buf[0];
                        for (int g=0; g<5; g++)
                            buf[g] = buf[g+1];
                        if (curStatus != 'N'){
                            int selectedId = atoi(buf);
                            repositories[selectedId].status = curStatus;
                            SendToClient(socket, "Status been changed.\n", currentIndex);
                        }        

                        buf[0] = '0';
                }

                if (buf[0] == '5'){
                            KickClient((int)currentIndex);
                            pthread_exit(NULL);
                            buf[0] = '0';
                }
          
        }
}

workDeveloper(int socket, void* currentIndex, char *userName){
    char buf[BUFSIZE]; //Buffer
        SendToClient(socket, "PROCESS", currentIndex);
        
        if (recv(socket, buf, BUFSIZE, 0) < 0) {
            KickClient((int)currentIndex);
            pthread_exit(NULL);
        }
        
        if (buf[0] == '1') {
                for (int i = 0; i < currentId; i++){
                    if ((repositories[i].status == 'B') && (*repositories[i].developerName == *userName)){
                        SendToClient(socket, repositories[i].projectName, currentIndex);
                        SendToClient(socket, repositories[i].description, currentIndex);
                        char tmp1[BUFSIZE];
                        sprintf(tmp1, "%d", i);
                        SendToClient(socket, tmp1, currentIndex);
                    }
                }
                SendToClient(socket, "END", currentIndex);
                buf[0] = '0';
        }
        if (buf[0] == '2') {
                char tmp1[BUFSIZE];
                for (int i = 0; i < currentId; i++){
                    if ((repositories[i].status == 'B') && (*repositories[i].developerName == *userName)){
                        sprintf(tmp1, "%d", i);
                        printf("tmp1 %s\n", tmp1);
                        SendToClient(socket, tmp1, currentIndex);
                    }
                }
                SendToClient(socket, "N", currentIndex);
                
                if (recv(socket, buf, BUFSIZE, 0) < 0) {
                    KickClient((int)currentIndex);
                    pthread_exit(NULL);
                }
                
                if (buf[0] != 'Q') {
                    int curId = atoi(buf);
                    repositories[curId].status = 'F';

                    SendToClient(socket, "Sucsess.\n", currentIndex);

                    buf[0] = '0';
                }
        }
        if (buf[0] == '3'){
                    KickClient((int)currentIndex);
                    pthread_exit(NULL);
                    buf[0] = '0';
        }
}

void *ClientHandler(void* currentIndex) {
    int socket = connectingClients[(int) currentIndex].socketDesc;
    connectingClients[(int)currentIndex].isActive = true;
    char buf[BUFSIZE]; //Buffer
    char userName[BUFSIZE/2-2];
    int curentUserId = 0;

    do{
        bzero(buf, BUFSIZE);
        if (recv(socket, buf, BUFSIZE, 0) < 0) {
            KickClient((int)currentIndex);
            pthread_exit(NULL);
        }

        char loginBuffer[BUFSIZE/2-2]; 
        char passwordBuffer[BUFSIZE/2-2]; 
        for (int i=0; i<BUFSIZE/2-3; i++){
                if (buf[i+2] == '\n')
                    break;
                loginBuffer[i] = buf[i+2];
                userName[i] = loginBuffer[i];
        }
        for (int i=0; i<BUFSIZE/2-3; i++){
                if (buf[i+BUFSIZE/2] == '\n')
                    break;
                passwordBuffer[i] = buf[i+BUFSIZE/2];
        }
        

        if (buf[0] == '1'){
            char sendBuffer[BUFSIZE];
            int isUserExist = 0;
            for (int m = 0; m <= maxUserId; m++) {
                if (*registredUsers[m].userName == *loginBuffer){
                    sendBuffer[0] = registredUsers[m].userType;
                    isUserExist = 1;
                    curentUserId = m;
                    break;
                }
            }
                if (isUserExist == 0){
                    SendToClient(socket, "!Username not exist.\n", currentIndex);
                    buf[0] = '0';
                    continue;
                }

                if (*registredUsers[curentUserId].password != *passwordBuffer){
                    SendToClient(socket, "!Incorrect password.\n", currentIndex);
                    buf[0] = '0';
                    continue;
                }

                SendToClient(socket, sendBuffer, currentIndex);
        }

        if (buf[0] == '2'){
                int errorExisenceUser = 0;
                for (int m = 0; m <= maxUserId; m++) {
                    if (*registredUsers[m].userName == *loginBuffer){
                        SendToClient(socket, "!Username already exist.\n", currentIndex);
                        buf[0] = '0';
                        errorExisenceUser = 1;
                        break;
                    }
                }
                if (errorExisenceUser == 0){
                    maxUserId++;

                    for (int v=0; v<BUFSIZE/2-2; v++){
                        registredUsers[maxUserId].userName[v] = loginBuffer[v];
                        registredUsers[maxUserId].password[v] = passwordBuffer[v];
                    }
                    registredUsers[maxUserId].userType = buf[1];
                    SendToClient(socket, " Success registration.\n", currentIndex);
                }
        }

        if (buf[0] == 'Q'){
                KickClient((int)currentIndex);
                pthread_exit(NULL);
                buf[0] = '0';    
        }
    }while(buf[0] != '1');

    if (registredUsers[curentUserId].userType == 'D') {
        printf("Developer: %s login.\n", userName);
        for(;;) 
                workDeveloper(socket, currentIndex, userName);
    }
    else if (registredUsers[curentUserId].userType == 'T') {
        printf("Tester: %s login.\n", userName);
	for(;;) 
        	workTester(socket, currentIndex, userName);
           
    }

}


void *ServerHandler(void* empty) {
    char input[BUFSIZE]; //buffer
    for(;;) {
        bzero(input, BUFSIZE+1);
        fgets(input, BUFSIZE, stdin);
        strtok(input, "\n");
        if (strstr(input, deactivate) != NULL) {
            int k = sizeof(deactivate);
            for (k ; k < sizeof(input); k++) {
                if (!isdigit(input[k]))
                    break;
            }
            char *id = malloc(k - sizeof(deactivate));
            for (int i = 0; i < sizeof(id); i++)
                id[i] = input[sizeof(deactivate)+i];
            if (connectingClients[atoi(id)].isActive) {
                KickClient(atof(id));
                printf("Kick client with id %d.\n", atoi(id));
            } else {
                printf("Client with id %d offline.\n", atoi(id));
            }
        }
        if (strstr(input, shutdown_serv) != NULL) {
            if (input[sizeof(shutdown_serv)-1] == '\0') {
                ShutdownServer();
                close(listener);
                printf("All clients are disconnected.\nServer socket are closed");
                exit(0);
            } else
                printf("No command '%s' found, did you mean: command '%s'\n", input, shutdown_serv);
        }
    }
}

int main(void){
    ShutdownServer();
    connectingClients = (struct client *) malloc(MAXCLIENTS + 1);
    repositories = (struct repo *) malloc(1000);
    registredUsers = (struct users*) malloc(MAXCLIENTS + 1);
    
    if (connectingClients == NULL) {
        perror("Could not allocate memory");
        exit(1);
    }

    int newfd;        // newly accepted socket descriptor
    socklen_t addrlen;
    struct sockaddr_in serv_addr, cli_addr;

    listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0) {
        perror("ERROR opening listening socket");
        exit(1);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT);


    /* Now bind the host address using bind() call.*/
    if (bind(listener, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }

    pthread_attr_t threadAttr;
    pthread_attr_init(&threadAttr);
    pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_DETACHED);
    //thread for server
    pthread_t serv_thread;
    pthread_create(&serv_thread, &threadAttr, ServerHandler, NULL);
    listen(listener, 5);
    addrlen = sizeof cli_addr;
    for(;;) {
        newfd = accept(listener, (struct sockaddr *) &cli_addr, &addrlen);
        if (newfd < 0) {
            perror("ERROR on accept");
            exit(1);
        }
            int currentClientId = curClientId();
            connectingClients[currentClientId].ip = inet_ntoa(cli_addr.sin_addr);
            connectingClients[currentClientId].port = ntohs(cli_addr.sin_port);
            connectingClients[currentClientId].socketDesc = newfd;

            printf("Socket %d connected\n", newfd);
            SendToClient(newfd, "CONNECTED", "NULL");
            clientsCount++;
            pthread_t client_thread;
            if (pthread_create(&client_thread, &threadAttr, ClientHandler, currentClientId) != 0)
                perror("Creating thread false");
        
    }
}
