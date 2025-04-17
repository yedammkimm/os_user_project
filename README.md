# Sherlock 13 – Projet OS USER

## Présentation du jeu

**Sherlock 13** est un jeu de déduction pour 2 à 4 joueurs, où chaque participant tente d’identifier le personnage coupable parmi 13 suspects. Chaque personnage est associé à un ensemble unique d’objets, et les joueurs doivent éliminer des suspects en posant des questions sur les objets visibles chez leurs adversaires.

## Règles du jeu

- Chaque joueur reçoit **3 cartes suspects**, le **13e** est le coupable inconnu.
- Les **cartes sont associées à des objets** (pipe, loupe, collier...).
- À son tour, un joueur peut :
  - **Demander à un joueur** combien de fois un objet apparaît dans ses cartes.
  - **Demander combien de fois un objet apparaît chez les autres joueurs**.
  - **Accuser un personnage** s’il pense avoir identifié le coupable.
- Une mauvaise accusation élimine le joueur. Une bonne accusation termine la partie avec une victoire.

## Objectifs du projet

Le but du projet est de concevoir une **version multijoueur du jeu Sherlock 13** en utilisant les outils vus dans le module **OS USER**, notamment :

- **Sockets TCP** pour la communication client-serveur
- **Processus et threads** pour gérer plusieurs clients
- **Mutex** pour synchroniser l'accès aux données partagées
- **SDL2** pour le rendu graphique interactif
- **Structures et gestion mémoire en C**

## Ce que contient le projet

- `sh13.c` : le **client graphique** basé sur SDL2
- `server.c` : le **serveur multijoueur** qui gère la logique du jeu
- `Makefile` : pour compiler facilement le projet
- Dossiers `images/` ou fichiers PNG des cartes et objets
- `README.md` (ce fichier)

## Compilation et exécution

### Prérequis :

- `libsdl2-dev`, `libsdl2-image-dev`, `libsdl2-ttf-dev` installés
- `gcc`, `make` installés

### Compilation :

```bash
make
```

### Lancer une partie :

1. Lancer le serveur :
```bash
./server 12345
```

2. Lancer 4 clients dans 4 terminaux séparés :
```bash
./sh13 127.0.0.1 12345 127.0.0.1 <PORT_CLIENT> <NomJoueur>
```
Exemple :
```bash
./sh13 127.0.0.1 12345 127.0.0.1 3000 Sherlock
```

## Ce que j’ai fait

### Architecture logicielle :

- J’ai mis en place un **serveur TCP** capable de gérer jusqu'à **4 clients simultanés**, avec un système FSM (`fsmServer`) pour gérer les phases du jeu.
- Côté client, j’ai construit une **interface graphique interactive** avec SDL2, permettant de :
  - Sélectionner des objets, des joueurs ou faire des accusations
  - Afficher dynamiquement les informations reçues du serveur (cartes visibles, résultats de questions, statut de victoire/défaite)

### Concepts OS USER utilisés :

| Concept              | Utilisation concrète                       |
|----------------------|---------------------------------------------|
| **Socket TCP**       | Communication entre le serveur et chaque client |
| **Thread**           | Le client écoute en parallèle avec un thread pour la réception des messages |
| **Mutex**            | Synchronisation des variables partagées comme `synchro` |
| **FSM**              | Gestion de l'état du serveur (attente de connexions, gestion de partie) |
| **Processus client/serveur** | Le serveur est autonome, les clients se connectent à distance |

## Ce que j’ai réussi à faire

- Intégration complète des fonctionnalités attendues (accusation, question ciblée, question globale)
- Synchronisation entre clients et serveur
- Interface SDL fluide avec images, textes dynamiques, boutons cliquables
- Implémentation propre et modulaire, compilable sans erreur

## Structure du dépôt

```
.
├── Makefile
├── README.md
├── sh13.c          # Code client
├── server.c        # Code serveur
├── SH13_*.png      # Images des cartes
├── SH13_*.png      # Objets (pipe, poing, etc.)
├── gobutton.png    # Bouton Go
├── connectbutton.png
└── sans.ttf        # Police TTF utilisée
```

## Rapport

Le projet m’a permis de combiner de nombreuses notions vues en TP, dans un cadre concret et motivant. Travailler sur Sherlock 13 m’a forcé à réfléchir à l’aspect **asynchrone et multijoueur**, tout en assurant la **réactivité graphique et l’ergonomie** du client. Le plus grand défi a été de gérer la logique du jeu côté serveur sans erreur d'état, ainsi que le rendu SDL en temps réel côté client.

## Merci

Merci aux encadrants du module OS USER pour les TP progressifs qui ont permis d’atteindre ce niveau d’autonomie.
