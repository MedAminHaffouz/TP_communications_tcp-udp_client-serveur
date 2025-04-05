#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 8888

int main() {
    int sock;
    struct sockaddr_in server, client;
    socklen_t client_len = sizeof(client);
    char buffer[1024];
    time_t rawtime;
    struct tm *timeinfo;

    // Création de la socket UDP
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("Erreur de création de socket");
        exit(EXIT_FAILURE);
    }

    // Configuration du serveur
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    // Bind
    if (bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Erreur de bind");
        exit(EXIT_FAILURE);
    }

    // Attendre une requête initiale du client
    char req[1024];
    recvfrom(sock, req, sizeof(req), 0, (struct sockaddr *)&client, &client_len);
    printf("Client connecté : %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));

    // Envoi de 60 messages avec un délai de 1 seconde
    for (int i = 0; i < 60; i++) {
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        strftime(buffer, sizeof(buffer), "Il est %H:%M:%S\n", timeinfo);
        sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&client, client_len);
	sleep(2);
    }

    close(sock);
    return 0;
}
