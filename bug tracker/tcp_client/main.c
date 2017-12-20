#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <unistd.h>

#include <string.h>
#include <ctype.h>

#define BUFSIZE 100

char userType = ' ';

int testerMode(int socket) {
    char buffer[BUFSIZE];
    bzero(buffer, BUFSIZE+1);

    for(;;) {
            printf("\nChose action.\n1.Show fixed bugs.\n2.Show current bugs.\n3.Push new bug.\n4.Change status.\n5.Quit.\n");
        
            bzero(buffer, BUFSIZE+1);
            fgets(buffer, BUFSIZE, stdin);
            
            int requestLength = 0;
            char createRequest[BUFSIZE];
            createRequest[0] = buffer[0];
            
            if (strstr(buffer, "1") != NULL) {
                    write(socket, createRequest, 1);
                    for(;;){
                        read(socket, buffer, BUFSIZE);
                        if (strstr(buffer, "END") == NULL){
                            printf("DeveloperName: %s. ", buffer);
                            read(socket, buffer, BUFSIZE);
                            printf("projectName: %s. ", buffer);
                            read(socket, buffer, BUFSIZE);
                            printf("Description: %s. ", buffer);
                            read(socket, buffer, BUFSIZE);
                            printf("ID: %s.\n", buffer);
                        }else{
                            break;
                        }
                    }
            }
            if (strstr(buffer, "2") != NULL) {
                    write(socket, createRequest, 1);
                    for(;;){
                        read(socket, buffer, BUFSIZE);
                        if (strstr(buffer, "END") == NULL){
                            printf("DeveloperName: %s. ", buffer);
                            read(socket, buffer, BUFSIZE);
                            printf("projectName: %s. ", buffer);
                            read(socket, buffer, BUFSIZE);
                            printf("Description: %s. ", buffer);
                            read(socket, buffer, BUFSIZE);
                            printf("Id: %s.\n", buffer);
                        }else{
                            break;
                        }
                    }
            }
            
            if (strstr(buffer, "3") != NULL) {
                    for (int j=0; j<3; j++){
                            printf("Specify developer name, project name, bug description. Use space to separate.\n");

                            bzero(buffer, BUFSIZE+1);                              //  developerName 
                            fgets(buffer, BUFSIZE, stdin);                          //  projectName      
                                                                                     //  description
                            createRequest[1] = ' ';                                   
                            for (int i = 0 ; i < BUFSIZE-2; i++) {                  
                                createRequest[i+2] = buffer[i];
                                if (buffer[i] == '\n'){
                                    createRequest[i+2] =' ';
                                    requestLength = i+2;
                                    break;
                                }
                            }

                            write(socket, createRequest, requestLength);
                            bzero(buffer, BUFSIZE+1);     
                            read(socket, buffer, BUFSIZE);
                            if (buffer[0] != 'N') {
                                printf("%s\n", buffer);
                            }else {
                                bzero(buffer, BUFSIZE+1);     
                                read(socket, buffer, BUFSIZE);
                                printf("Sucsess. Bug id: %s.\n", buffer);
                                break;
                            }
                    }
            }
            
            if (strstr(buffer, "4") != NULL) {
                    int arrayOfIds[1000];
                    int currIndex = 0;
                    write(socket, createRequest, 1);
                    printf("Input id(");
                    for (;;){
                        read(socket, buffer, BUFSIZE);
                        if (buffer[0] == 'N')
                            break;
                        arrayOfIds[currIndex] = atoi(buffer);
                        printf(" %d", arrayOfIds[currIndex]);
                        currIndex++;
                    }

                    printf(")\n");
                    bzero(buffer, BUFSIZE+1);
                    fgets(buffer, BUFSIZE, stdin);
                    int curId = atoi(buffer);
                    int checkId = 0;
                    for (int l=0; l<currIndex; l++){
                        if (arrayOfIds[l] == curId){
                            checkId = 1;
                            break;
                        }
                    }
                
                    if (checkId == 0){
                        printf("Incorrect id.\n");
                        write(socket, "N", 1);
                    } else{
                        printf("Chose action.\n1.Accept fix.\n2.Decline fix.\n");
                        char buf[BUFSIZE];
                        bzero(buf, BUFSIZE+1);                              
                        fgets(buf, BUFSIZE, stdin); 
                        for (int g=4; g>=0; g--)
                            buffer[g+1] = buffer[g];
                        if (buf[0] == '1')
                            buffer[0]='C';
                        else 
                            buffer[0]='B';
                        
                        write(socket, buffer, 5);
                        read(socket, buffer, BUFSIZE);
                        printf("%s", buffer);
                    }
            }
            
            if (strstr(buffer, "5") != NULL) {
                    write(socket, createRequest, 1);
                    close(socket);
                    exit(0);
            }
    }
}

int developerMode(int socket) {
    char buffer[BUFSIZE];
    bzero(buffer, BUFSIZE+1);

    for(;;) {
        read(socket, buffer, BUFSIZE);
        
        
        if (strstr(buffer, "PROCESS") != NULL) {
            printf("\nChose action.\n1.Show my bugs.\n2.Push fix.\n3.Quit.\n");
        
            bzero(buffer, BUFSIZE+1);
            fgets(buffer, BUFSIZE, stdin);
            
            int requestLength = 0;
            char createRequest[BUFSIZE];
            createRequest[0] = buffer[0];
            
            if (strstr(buffer, "1") != NULL) {
                    write(socket, createRequest, 1);
                    for(;;){
                        read(socket, buffer, BUFSIZE);
                        if (strstr(buffer, "END") == NULL){
                            printf("projectName: %s. ", buffer);
                            read(socket, buffer, BUFSIZE);
                            printf("Description: %s. ", buffer);
                            read(socket, buffer, BUFSIZE);
                            printf("ID: %s.\n", buffer);
                        }else{
                            break;
                        }
                    }
            }
            if (strstr(buffer, "2") != NULL) {
                    int arrayOfIds[1000];
                    int currIndex = 0;
                    write(socket, createRequest, 1);
                    printf("Input id(");
                    for (;;){
                        read(socket, buffer, BUFSIZE);
                        if (buffer[0] == 'N')
                            break;
                        arrayOfIds[currIndex] = atoi(buffer);
                        printf(" %d", arrayOfIds[currIndex]);
                        currIndex++;
                    }

                    printf(").\n");

                    bzero(buffer, BUFSIZE+1);
                    fgets(buffer, BUFSIZE, stdin);
                    int curId = atoi(buffer);
                    int checkId = 0;
                    for (int l=0; l<currIndex; l++){
                        if (arrayOfIds[l] == curId){
                            checkId = 1;
                            break;
                        }
                    }

                    if (checkId == 0){
                        printf("Incorrect id.\n");
                        write(socket, "Q", 1);
                    } else{
                        char tmp1[BUFSIZE];
                        sprintf(tmp1, "%d", curId);
                        write(socket, tmp1, 5);
                    }
                    read(socket, buffer, BUFSIZE);
                    printf("%s", buffer);
            }
            if (strstr(buffer, "3") != NULL) {
                    write(socket, createRequest, 1);
                    close(socket);
                    exit(0);
            }
        }
    }
}

int registerNewClient(int socket) {
    char buffer[BUFSIZE];
    for(;;) {
        printf("Choose role:\n1. Tester\n2. Developer\n3. Exit\n");
        bzero(buffer, BUFSIZE+1);
        fgets(buffer, BUFSIZE, stdin);
        if (strstr(buffer, "1") != NULL) {
            userType = 'T';
        }
        if (strstr(buffer, "2") != NULL) {
            userType = 'D';
        }
        if (strstr(buffer, "3") != NULL) {
            write(socket, "Q", 1);
            close(socket);
            exit(0);
        }
        if (userType == 'T' || userType == 'D'){
            char loginBuffer[BUFSIZE/2-2]; 
            char passwordBuffer[BUFSIZE/2-2]; 
            printf("Input login:\n");
            bzero(loginBuffer, BUFSIZE/2-1);
            fgets(loginBuffer, BUFSIZE/2-2, stdin);
            printf("Input password:\n");
            bzero(passwordBuffer, BUFSIZE/2-1);
            fgets(passwordBuffer, BUFSIZE/2-2, stdin);
            buffer[0] = '2';
            buffer[1] = userType;
            for (int i=0; i<BUFSIZE/2-1; i++){
                buffer[i+2] = loginBuffer[i];
                buffer[i+BUFSIZE/2] = passwordBuffer[i];
            }
            write(socket, buffer, BUFSIZE);
            bzero(buffer, BUFSIZE);
            read(socket, buffer, BUFSIZE);
            if (buffer[0] = '!'){
                buffer[0] = ' ';
                printf("%s", buffer);
                return -1;
            } else {
                return 1;
            }
        }
        
        printf("No role");
    }
}


int authentication(int socket) {
    char loginBuffer[BUFSIZE/2-2]; 
    char passwordBuffer[BUFSIZE/2-2];
    char buffer[BUFSIZE];
    for(;;) {
        printf("Input login:\n");
        bzero(loginBuffer, BUFSIZE/2-1);
        fgets(loginBuffer, BUFSIZE/2-2, stdin);
        printf("Input password:\n");
        bzero(passwordBuffer, BUFSIZE/2-1);
        fgets(passwordBuffer, BUFSIZE/2-2, stdin);
        
        bzero(buffer, BUFSIZE);
        buffer[0] = '1';
        for (int i=0; i<BUFSIZE/2-1; i++){
            buffer[i+2] = loginBuffer[i];
            buffer[i+BUFSIZE/2] = passwordBuffer[i];
        }
        
        write(socket, buffer, BUFSIZE);
        bzero(buffer, BUFSIZE);
        read(socket, buffer, BUFSIZE);
            if (buffer[0] == '!'){
                buffer[0] = ' ';
                printf("%s", buffer);
                return;
            } 
        userType = buffer[0];
        
        if (userType == 'T'){
            return testerMode(socket);
        }
        if (userType = 'D'){
            return developerMode(socket);
        }
        printf("No");
    }
}


int startWork(int socket){
    char buffer[BUFSIZE]; 
    for(;;){
        printf("Chose option:\n1. Log in.\n2. Register\n3. Exit.\n");
        bzero(buffer, BUFSIZE+1);
        fgets(buffer, BUFSIZE, stdin);
        if (strstr(buffer, "1") != NULL) 
            authentication(socket);
        if (strstr(buffer, "2") != NULL) 
            registerNewClient(socket);
        if (strstr(buffer, "3") != NULL) {
                write(socket, "3", 1);
                close(socket);
                exit(0);
        }
    }
}

int main(int argc, char *argv[]) {
    int sockfd, n;
    uint16_t portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    
    argv[1] = "localhost";
    argv[2] = "5056";

    portno = (uint16_t) atoi(argv[2]);

    /* Create a socket point */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    server = gethostbyname(argv[1]);

    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(server->h_addr, (char *) &serv_addr.sin_addr.s_addr, (size_t) server->h_length);
    serv_addr.sin_port = htons(portno);

    /* Now connect to the server */
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        exit(1);
    }


    bzero(buffer, BUFSIZE+1);
    read(sockfd, buffer, BUFSIZE);

    if (strstr(buffer, "CONNECTED") != NULL) {
        bzero(buffer, BUFSIZE+1);
        if (startWork(sockfd) < 0)
            exit(1);
    }
    return 0;
}
