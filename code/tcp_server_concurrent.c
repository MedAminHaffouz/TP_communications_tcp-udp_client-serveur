#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <time.h>

#define PORT 8888

// Fonction pour envoyer les messages à un client
void handle_client(int client_socket) {
    char buffer[1024];
    time_t rawtime;
    struct tm *timeinfo;

    // Envoi de 60 messages avec un délai de 1 seconde
    for (int i = 0; i < 60; i++) {
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        strftime(buffer, sizeof(buffer), "Il est %H:%M:%S\n", timeinfo);
        send(client_socket, buffer, strlen(buffer), 0);
        sleep(1);
    }
    close(client_socket);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;

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
    listen(server_fd, 5); // File d'attente de 5 clients

    printf("Serveur en écoute sur le port %d...\n", PORT);

    while (1) {
        new_socket = accept(server_fd, NULL, NULL);
        if (new_socket < 0) {
            perror("Erreur d'acceptation");
            continue;
        }

        // Créer un processus enfant pour le client
        if (fork() == 0) {
            close(server_fd); // Ferme la socket d'écoute (inutile pour l'enfant)
            handle_client(new_socket); // Gère les 60 messages
            exit(EXIT_SUCCESS);
        }

        // Parent ferme la socket client et continue
        close(new_socket);
        waitpid(-1, NULL, WNOHANG); // Nettoie les processus terminés (évite les zombies)
    }

    close(server_fd);
    return 0;
}
