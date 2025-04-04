#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8888  // Utilisez 8888 si le serveur distant est down

int main() {
    int sock;
    struct sockaddr_in server;
    char request[1024], response[4096];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Erreur de création de socket");
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");  // Serveur local

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Erreur de connexion");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // **Problème** : La requête HTTP manque de `\r\n` et de la ligne vide
    printf("Entrez la requête HTTP (ex: GET / HTTP/1.1): ");
    fgets(request, sizeof(request), stdin);

    // **Correction** : Ajoutez les caractères de fin de ligne
    if (strchr(request, '\n') == NULL) {
        strcat(request, "\r\n");  // Ajoute \r\n si pas de retour chariot
    }
    strcat(request, "\r\n");  // Ligne vide pour terminer l'entête HTTP

    send(sock, request, strlen(request), 0);
    printf("Requête envoyée: %s", request);

    // **Problème** : La réception est faite en une seule fois
    // **Correction** : Boucle jusqu'à ce que `recv` renvoie 0
    char full_response[4096] = {0};
    ssize_t bytes_received;
    while ((bytes_received = recv(sock, response, sizeof(response)-1, 0)) > 0) {
        response[bytes_received] = '\0';
        strcat(full_response, response);
    }
    printf("%s", full_response);  // Affiche la réponse complète

    close(sock);
    return 0;
}