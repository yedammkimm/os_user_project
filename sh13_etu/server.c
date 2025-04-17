#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

struct _client {
    char ipAddress[40];
    int port;
    char name[40];
} tcpClients[4];

int nbClients = 0;
int fsmServer = 0;
int deck[13] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
int tableCartes[4][8];
char *nomcartes[] = {
    "Sebastian Moran", "irene Adler", "inspector Lestrade",
    "inspector Gregson", "inspector Baynes", "inspector Bradstreet",
    "inspector Hopkins", "Sherlock Holmes", "John Watson", "Mycroft Holmes",
    "Mrs. Hudson", "Mary Morstan", "James Moriarty"
};
int joueurCourant = 0;

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void melangerDeck() {
    for (int i = 0; i < 1000; i++) {
        int index1 = rand() % 13;
        int index2 = rand() % 13;
        int tmp = deck[index1];
        deck[index1] = deck[index2];
        deck[index2] = tmp;
    }
}

void createTable() {
    memset(tableCartes, 0, sizeof(tableCartes));

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 3; j++) {
            int c = deck[i * 3 + j];
            switch (c) {
                case 0: tableCartes[i][7]++; tableCartes[i][2]++; break;
                case 1: tableCartes[i][7]++; tableCartes[i][1]++; tableCartes[i][5]++; break;
                case 2: tableCartes[i][3]++; tableCartes[i][6]++; tableCartes[i][4]++; break;
                case 3: tableCartes[i][3]++; tableCartes[i][2]++; tableCartes[i][4]++; break;
                case 4: tableCartes[i][3]++; tableCartes[i][1]++; break;
                case 5: tableCartes[i][3]++; tableCartes[i][2]++; break;
                case 6: tableCartes[i][3]++; tableCartes[i][0]++; tableCartes[i][6]++; break;
                case 7: tableCartes[i][0]++; tableCartes[i][1]++; tableCartes[i][2]++; break;
                case 8: tableCartes[i][0]++; tableCartes[i][6]++; tableCartes[i][2]++; break;
                case 9: tableCartes[i][0]++; tableCartes[i][1]++; tableCartes[i][4]++; break;
                case 10: tableCartes[i][0]++; tableCartes[i][5]++; break;
                case 11: tableCartes[i][4]++; tableCartes[i][5]++; break;
                case 12: tableCartes[i][7]++; tableCartes[i][1]++; break;
            }
        }
    }
}

void printDeck() {
    for (int i = 0; i < 13; i++) {
        printf("%d %s\n", deck[i], nomcartes[deck[i]]);
    }
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 8; j++)
            printf("%02d ", tableCartes[i][j]);
        puts("");
    }
}

void printClients() {
    for (int i = 0; i < nbClients; i++) {
        printf("%d: %s %5.5d %s\n", i, tcpClients[i].ipAddress,
               tcpClients[i].port, tcpClients[i].name);
    }
}

int findClientByName(char *name) {
    for (int i = 0; i < nbClients; i++)
        if (strcmp(tcpClients[i].name, name) == 0)
            return i;
    return -1;
}

void sendMessageToClient(char *clientip, int clientport, char *mess) {
    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server = gethostbyname(clientip);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(clientport);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("ERROR connecting\n");
        exit(1);
    }

    sprintf(buffer, "%s\n", mess);
    write(sockfd, buffer, strlen(buffer));
    close(sockfd);
}

void broadcastMessage(char *mess) {
    for (int i = 0; i < nbClients; i++) {
        sendMessageToClient(tcpClients[i].ipAddress, tcpClients[i].port, mess);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }

    srand(time(NULL));

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr, cli_addr;
    int portno = atoi(argv[1]);

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    listen(sockfd, 5);
    socklen_t clilen = sizeof(cli_addr);

    printDeck();
    melangerDeck();
    createTable();
    printDeck();

    for (int i = 0; i < 4; i++) {
        strcpy(tcpClients[i].ipAddress, "localhost");
        tcpClients[i].port = -1;
        strcpy(tcpClients[i].name, "-");
    }

    char buffer[256], reply[256];
    while (1) {
        int newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd < 0)
            error("ERROR on accept");

        bzero(buffer, 256);
        int n = read(newsockfd, buffer, 255);
        if (n < 0)
            error("ERROR reading from socket");

        printf("Received packet from %s:%d\nData: [%s]\n\n",
               inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), buffer);

        if (fsmServer == 0 && buffer[0] == 'C') {
            char com, clientIpAddress[256], clientName[256];
            int clientPort;
            sscanf(buffer, "%c %s %d %s", &com, clientIpAddress, &clientPort, clientName);

            strcpy(tcpClients[nbClients].ipAddress, clientIpAddress);
            tcpClients[nbClients].port = clientPort;
            strcpy(tcpClients[nbClients].name, clientName);
            nbClients++;

            printClients();

            int id = findClientByName(clientName);
            sprintf(reply, "I %d", id);
            sendMessageToClient(tcpClients[id].ipAddress, tcpClients[id].port, reply);

            sprintf(reply, "L %s %s %s %s", tcpClients[0].name, tcpClients[1].name, tcpClients[2].name, tcpClients[3].name);
            broadcastMessage(reply);

            if (nbClients == 4) {
                for (int j = 0; j < 4; j++) {
                    int c1 = deck[j * 3];
                    int c2 = deck[j * 3 + 1];
                    int c3 = deck[j * 3 + 2];
                    sprintf(reply, "D %d %d %d", c1, c2, c3);
                    sendMessageToClient(tcpClients[j].ipAddress, tcpClients[j].port, reply);

                    for (int col = 0; col < 8; col++) {
                        sprintf(reply, "V %d %d %d", j, col, tableCartes[j][col]);
                        sendMessageToClient(tcpClients[j].ipAddress, tcpClients[j].port, reply);
                    }
                }
                sprintf(reply, "M %d", joueurCourant);
                broadcastMessage(reply);
                fsmServer = 1;
            }
        } else if (fsmServer == 1) {
            switch (buffer[0]) {
                case 'G': {
                    int id, suspect;
                    sscanf(buffer, "G %d %d", &id, &suspect);
                    int coupable = deck[12];
                    if (suspect == coupable) {
                        sprintf(reply, "W %d", id);
                        broadcastMessage(reply);
                        exit(0);
                    } else {
                        sprintf(reply, "X %d", id);
                        broadcastMessage(reply);
                        joueurCourant = (joueurCourant + 1) % 4;
                        sprintf(reply, "M %d", joueurCourant);
                        broadcastMessage(reply);
                    }
                    break;
                }
                case 'O': {
                    int id, objet;
                    sscanf(buffer, "O %d %d", &id, &objet);
                    int total = 0;
                    for (int i = 0; i < 4; i++)
                        if (i != id)
                            total += tableCartes[i][objet];
                    sprintf(reply, "R %d", total);
                    sendMessageToClient(tcpClients[id].ipAddress, tcpClients[id].port, reply);
                    joueurCourant = (joueurCourant + 1) % 4;
                    sprintf(reply, "M %d", joueurCourant);
                    broadcastMessage(reply);
                    break;
                }
                case 'S': {
                    int id, cible, objet;
                    sscanf(buffer, "S %d %d %d", &id, &cible, &objet);
                    int nb = tableCartes[cible][objet];
                    sprintf(reply, "R %d", nb);
                    sendMessageToClient(tcpClients[id].ipAddress, tcpClients[id].port, reply);
                    joueurCourant = (joueurCourant + 1) % 4;
                    sprintf(reply, "M %d", joueurCourant);
                    broadcastMessage(reply);
                    break;
                }
            }
        }
        close(newsockfd);
    }

    close(sockfd);
    return 0;
}
