# TP Sockets TCP/UDP - GL2 - Architecture des Réseaux

---

**NB :** le serveur donné dans le TP n'a pas fonctionné et il nous indique `Connection timed out`

```bash
$ telnet 10.250.101.1 80
Trying 10.250.101.1...
telnet: Unable to connect to remote host: Connection timed out
$ telnet 10.250.101.1 80
Trying 10.250.101.1...
telnet: Unable to connect to remote host: Connection timed out
```

Structure du projet :

```
/
├── README.md    # Explication du projet et instructions et les observations
├── code/        #implémentations des clients et des serveurs
├── captures/    #les captures des packets effectués
└── logs/        #sorties des clients et des serveurs mis en fichiers textes      
```

### 2.2 Client HTTP en mode connecté

On a implémenté le client HTTP dans le fichier `http_client.c` et pour absence de serveur dans notre cas , on a implementé un serveur de test dans le fichier : `http_servr_test.c`

Après observation de la capture `./captures/http_8888.pcap` et de l'output du client et du serveur `http_client.txt` et `http_servr_test.txt` , on remarque que :

* La connexion HTTP s'établit quand le client envoie une requête HTTP au serveur : `GET / HTTP /1.1`  (packets 4 et 8)

* La connexion TCP s'établit par le **3-way handshake** ; le client envoie un paquet `SYN` et il reçoit un paquet `SYN-ACK` de la part du serveur et il envoie un autre paquet `ACK` (packets 5,6 et 7)

* Selon la capture , le client est dans le port 35934 et le serveur dans le port 8888

Le chronogramme se trouve dans `./assets/chronogramme1.jpeg`

Si au lieu du `http_client` , on utilise `telnet` , on observant la capture dans `./captures/http_8888_3.pcap` , on remarque que la requête HTTP est envoyée par TCP et non par HTTP , le contenu de la requête s'envoie dans le datagramme

Le chronogramme se trouve dans `./assets/chronogramme2.jpeg`

### 3. Transfert de données en mode connecté

En implémentant `tcp_client.c` et `tcp_server.c` pour que le serveur envoie 60 fois des messages au client pour l'informer à propos du temps et en capturant les packets via `./captures/tcp_60_messages.pcap` , on remarque bien que à chaque seconde , le server envoie via un packet avec des flags [PSH+ACK] le message temps pendant 60s càd 60 messages on étaient envoyés puis la fermeture de connexion commencée par le serveur 

Le chronogramme se trouverai dans `./assets/chronogramme3.jpeg`

On augmentant le temps d'attente `tcp_server_delaied.c` , la capture `./captures/tcp_60_messages_delaied.pcap` va enregistrer tous les 60 messages mais dans le double de temps donc **il n'y a pas de perte de messages**

Pour simuler le débranchement réseau en `localhost` dans le cas d'une seule machine , on utilise la commande `killall tcp_server`

Le débranchement engendre la fermeture de connexion (FIN,ACK)

Mais si on essaye de redémarrer (on n'était pas capable de totalement simuler le rebranchement) , le serveur va envoyer (RST,ACK) vers le client et le client fait une ACK et la connexion se rétablit une autre fois : voir captures : `./captures/tcp_60_messages_stop.pcap` et `./captures/tcp_60_messages_restart.pcap`

Avec notre implémentation actuelle , il était impossible de hoster plus qu'un seul client . Dans la capture `./captures/tcp_60_messages_manyCL_1server.pcap` , on remarque qu'à chaque fois un client autre que celui du début (39032) le client fait avec lui le 3 way handshake mais pas d'échanges de data (PSH) . Après l'envoi de 60 messages , le client (39032) et le serveur (8888) intéragissent des (FIN,ACK) mais ici le client envoie des packets (RST,ACK) aux autres clients (39048,37080 et 37096) sans réponse pour raison de terminaison de communication par le serveur

### 4. Transfert de données en mode non connecté

Maintenant , on travaillerai sur le protocole UDP au lieu de TCP . On a implémenté une version de serveur et client en UDP dans `udp_server.c` et `udp_client.c` et on a fait la capture des packets dans `./captures/udp_60_messages.pcap` . On sait que la UDP est un protocole non orienté-connexion d'où chaque envoie de message contenait le destinataire

Dans la capture , on peut voir l'absence de l'établissement et la fermeture de la connexion . On ne voit que 61 packets . Chaque packet contient le message envoyé par le serveur au client . <u>Mais le premier packet était envoyé par le client au serveur et il contient le message `START` </u>. ***C'est comment la communication commence en UDP***    

Si on débranche le serveur (en localhost à travers `killall`) , on remarque que la connexion s'arrête directement

Si on débranche puis rebranche directement , on voit un nouveau client qui envoie le START au serveur et il lui envoie les données

Même observations pour la manip de multiclients, les clients envoient le START mais le serveur n'intéragit pas avec eux

### 5. Serveur en mode concurrent

On modifie le code du serveur pour obtenir une sorte de **serveur en mode concurrent** , càd , un serveur qui fait `fork()` pour les différents processus (des exécutions) du code `client` et les manipulent simultanément . On observe bien que chaque fois un client entre en communication avec le serveur via le 3-way handshake , le serveur le met en une sorte de file d'attente et à chaque fois envoie un packet de données à l'in d'eux un par un . Quand le serveur finit les 60 messages avec un certain client , il le remouve de la file d'attente et passe au next

Si on ajoute plus de services aux serveurs (on a choisit d'ajouter la fonction d'afficher le nombre de processus actifs et les infos du système) , on remarque que à chaque fois l'un des clients établit la connexion puis il attend que le client lui envoie en PSH+ACK le type de service demandé pour qu'il lui répond directement avec l'information appropriée et si il s'agit d'une demande d'affichage des 60 messages il le met dans la file de replys comme le cas de monoservice
