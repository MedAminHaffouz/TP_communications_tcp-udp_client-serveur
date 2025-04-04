#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8888  // Port non privilégié

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    char buffer[1024];
    char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nBonjour du serveur local !";

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Erreur de création de socket");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // **Problème** : Pas de vérification des erreurs pour `bind()` et `listen()`
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Erreur de bind");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("Erreur de listen");
        exit(EXIT_FAILURE);
    }

    printf("Serveur en écoute sur le port %d...\n", PORT);

    new_socket = accept(server_fd, NULL, NULL);
    if (new_socket < 0) {
        perror("Erreur d'acceptation");
        exit(EXIT_FAILURE);
    }

    // **Problème** : Lecture d'une seule fois → Peut manquer le corps de la réponse
    // **Correction** : Lire jusqu'à ce que `read` renvoie 0
    ssize_t bytes = read(new_socket, buffer, 1024);
    if (bytes > 0) {
        buffer[bytes] = '\0';
        printf("Message reçu: %s", buffer);
    }

    send(new_socket, response, strlen(response), 0);
    printf("Réponse envoyée.\n");

    close(new_socket);
    close(server_fd);
    return 0;
}