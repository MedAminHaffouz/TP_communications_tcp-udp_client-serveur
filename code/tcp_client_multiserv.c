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
    char request[1024];

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

    // Choix du service
    printf("Choisissez un service (HEURE, PROCESSUS, SYSTEM_INFO): ");
    fgets(request, sizeof(request), stdin);
    request[strcspn(request, "\n")] = 0; // Supprimer le '\n'

    send(sock, request, strlen(request), 0);

    // Réception de la réponse
    if (strncmp(request, "HEURE", 5) == 0) {
        // Attendre 60 messages
        int message_count = 0;
        while (message_count < 60) {
            ssize_t bytes = recv(sock, buffer, sizeof(buffer), 0);
            if (bytes <= 0) break; // Fin de la connexion
            buffer[bytes] = '\0';
            printf("%s", buffer);
            message_count++;
        }
        printf("Nombre total de messages reçus : %d\n", message_count);
    } else {
        // Réponse unique
        ssize_t bytes = recv(sock, buffer, sizeof(buffer), 0);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            printf("Réponse: %s\n", buffer);
        }
    }

    close(sock);
    return 0;
}
