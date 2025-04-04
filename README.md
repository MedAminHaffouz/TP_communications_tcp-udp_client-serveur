# TP Sockets TCP/UDP - GL2 - Architecture des Réseaux

---

**NB :** le serveur donné dans le TP n'a pas fonctionné et il nous indique `Connection timed out`

```bash
medamin_hfz@med-amin-haffouz-HP-Laptop:~/Documents/My Files/MEga GL2 2024-2025/doc/S2/RX/TP/TP_Communications_TCP_UDP_client_serveur$ telnet 10.250.101.1 80
Trying 10.250.101.1...
telnet: Unable to connect to remote host: Connection timed out
medamin_hfz@med-amin-haffouz-HP-Laptop:~/Documents/My Files/MEga GL2 2024-2025/doc/S2/RX/TP/TP_Communications_TCP_UDP_client_serveur$ telnet 10.250.101.1 80
Trying 10.250.101.1...
telnet: Unable to connect to remote host: Connection timed out
```

Structure du projet :

```
GL2-TP-Socket-2025/
├── README.md    # Explication du projet et instructions et les observations
├── code/        #implémentations des clients et des serveurs
├── captures/    #les captures des packets effectués
└── logs/        #sorties des clients et des serveurs mis en fichiers textes      
```

### 2.2 Client HTTP en mode connecté

On a implémenté le client HTTP dans le fichier `http_client.c` et pour absence de serveur dans notre cas , on a implementé un serveur de test dans le fichier : `http_servr_test.c`

Après observation de la capture `/captures/http_8888.pcap` et de l'output du client et du serveur `http_client.txt` et `http_servr_test.txt` , on remarque que :

* La connexion HTTP s'établit quand le client envoie une requête HTTP au serveur : `GET / HTTP /1.1`  (packets 4 et 8)

* La connexion TCP s'établit par le **3-way handshake** ; le client envoie un paquet `SYN` et il reçoit un paquet `SYN-ACK` de la part du serveur et il envoie un autre paquet `ACK` (packets 5,6 et 7)

* Selon la capture , le client est dans le port 35934 et le serveur dans le port 8888

![](/home/medamin_hfz/snap/marktext/9/.config/marktext/images/2025-04-04-23-35-24-image.png)
