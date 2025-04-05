#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 8888

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    char buffer[1024];
    time_t rawtime;
    struct tm *timeinfo;

    // Création de la socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Erreur de création de socket");
        exit(EXIT_FAILURE);
    }

    // Configuration
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind et écoute
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Erreur de bind");
        exit(EXIT_FAILURE);
    }
    listen(server_fd, 3);

    printf("Serveur en écoute sur le port %d...\n", PORT);

    // Acceptation du client
    new_socket = accept(server_fd, NULL, NULL);
    if (new_socket < 0) {
        perror("Erreur d'acceptation");
        exit(EXIT_FAILURE);
    }

    // Envoi de 60 messages avec un délai de 1 seconde
    for (int i = 0; i < 60; i++) {
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        strftime(buffer, sizeof(buffer), "Il est %H:%M:%S\n", timeinfo);
        send(new_socket, buffer, strlen(buffer), 0);
        sleep(2); // Pause de 1 seconde entre chaque message
    }

    close(new_socket);
    close(server_fd);
    return 0;
}
