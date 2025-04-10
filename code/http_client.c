#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8888  // On a utilisé ce port car le port de HTTP est inutilisable

int main() {
    int sock;
    // objet pour le serveur
    struct sockaddr_in server;
    char request[1024], response[4096];

    //création de socket
    //SOCK_STREAM car le TCP est en flux de packets
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Erreur de création de socket");
        exit(EXIT_FAILURE);
    }

    //famille d'adresses : IPv4
    server.sin_family = AF_INET;
    //port "en format réseau"
    server.sin_port = htons(PORT);
    //adresse IP en format local
    server.sin_addr.s_addr = inet_addr("127.0.0.1");  // Serveur local

    //Connection (établissement du 3-way handshake)
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Erreur de connexion");
        close(sock);
        exit(EXIT_FAILURE);
    }

    //Input of HTTP querry
    printf("Entrez la requête HTTP (ex: GET / HTTP/1.1): ");
    fgets(request, sizeof(request), stdin);

    //pour l'entrée
    if (strchr(request, '\n') == NULL) {
        strcat(request, "\r\n");  
    }
    strcat(request, "\r\n");

    //envoi de requête
    send(sock, request, strlen(request), 0);
    printf("Requête envoyée: %s", request);

    // Réception du message de la part de serveur
    char full_response[4096] = {0};
    ssize_t bytes_received;
    while ((bytes_received = recv(sock, response, sizeof(response)-1, 0)) > 0) {
        response[bytes_received] = '\0';
        strcat(full_response, response);
    }
    printf("%s", full_response);  // Affiche la réponse complète

    //fermeture de socket
    close(sock);
    return 0;
}