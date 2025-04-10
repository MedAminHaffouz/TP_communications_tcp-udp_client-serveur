#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8888  

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    char buffer[1024];
    char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nBonjour du serveur local !";

    //création de socket de com
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Erreur de création de socket");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    //toute adresse d'adresse
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // bind and listen
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Erreur de bind");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("Erreur de listen");
        exit(EXIT_FAILURE);
    }

    printf("Serveur en écoute sur le port %d...\n", PORT);

    //réception du 3-way handshake du client
    new_socket = accept(server_fd, NULL, NULL);
    if (new_socket < 0) {
        perror("Erreur d'acceptation");
        exit(EXIT_FAILURE);
    }

    //READ de la requete du client
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