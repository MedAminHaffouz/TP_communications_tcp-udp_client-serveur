#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8888

int main() {
    int sock;
    struct sockaddr_in server;
    char buffer[1024];
    int message_count = 0;

    // Création de la socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Erreur de création de socket");
        exit(EXIT_FAILURE);
    }

    // Configuration du serveur
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connexion au serveur
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Erreur de connexion");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Réception des 60 messages
    while (1) {
        ssize_t bytes = recv(sock, buffer, sizeof(buffer), 0);
        if (bytes <= 0) break; // Fin de la connexion
        buffer[bytes] = '\0';
        printf("Message %d : %s", ++message_count, buffer);
    }

    printf("Nombre total de messages reçus : %d\n", message_count);
    close(sock);
    return 0;
}
