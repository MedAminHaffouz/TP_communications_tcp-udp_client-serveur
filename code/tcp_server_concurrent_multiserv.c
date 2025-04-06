#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <time.h>

#define PORT 8888

void handle_client(int client_socket) {
    char request[1024];
    read(client_socket, request, sizeof(request)); // Lire la requête du client
    request[strcspn(request, "\n")] = 0; // Supprimer le '\n' final

    if (strncmp(request, "HEURE", 5) == 0) {
        // Service HEURE : 60 messages avec l'heure
        char buffer[1024];
        time_t rawtime;
        struct tm *timeinfo;

        for (int i = 0; i < 60; i++) {
            time(&rawtime);
            timeinfo = localtime(&rawtime);
            strftime(buffer, sizeof(buffer), "Il est %H:%M:%S\n", timeinfo);
            send(client_socket, buffer, strlen(buffer), 0);
            sleep(1);
        }
    } else if (strncmp(request, "PROCESSUS", 9) == 0) {
        // Service PROCESSUS : Nombre de processus
        char response[1024];
        FILE *fp = popen("ps -ef | wc -l", "r");
        if (fp) {
            fgets(response, sizeof(response), fp);
            send(client_socket, response, strlen(response), 0);
            pclose(fp);
        } else {
            send(client_socket, "Erreur: Impossible d'exécuter la commande.\n", 45, 0);
        }
    } else if (strncmp(request, "SYSTEM_INFO", 11) == 0) {
        // Service SYSTEM_INFO : Informations système (uptime, CPU, etc.)
        char response[1024];
        FILE *fp = popen("uptime", "r");
        if (fp) {
            fgets(response, sizeof(response), fp);
            send(client_socket, response, strlen(response), 0);
            pclose(fp);
        } else {
            send(client_socket, "Erreur: Impossible d'exécuter la commande.\n", 45, 0);
        }
    } else {
        send(client_socket, "Service non disponible. Choisissez HEURE, PROCESSUS, ou SYSTEM_INFO.\n", 65, 0);
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
    listen(server_fd, 5); // File d'attente de 5 connexions

    printf("Serveur en écoute sur le port %d...\n", PORT);

    while (1) {
        new_socket = accept(server_fd, NULL, NULL);
        if (new_socket < 0) {
            perror("Erreur d'acceptation");
            continue;
        }

        // Créer un processus enfant pour le client
        if (fork() == 0) {
            close(server_fd); // Fermer la socket d'écoute dans l'enfant
            handle_client(new_socket);
            exit(EXIT_SUCCESS);
        }

        // Parent ferme la socket client et nettoie les zombies
        close(new_socket);
        waitpid(-1, NULL, WNOHANG); // Évite les zombies
    }

    return 0;
}
