# Projet de Programmation Objet

> Travailler en binôme

Pour ce projet vous allez travailler avec une autre personne. Vous devez bien sûr travailler ensemble : concevoir le programme ensemble, le taper et le mettre au point ensemble sur l'ordinateur. Il n'est pas question que certains fassent le travail, et que d'autres y ajoutent leur nom ! Vous ne devrez rendre qu'un ensemble de rapports en indiquant vos noms.

> Comment rendre les rapports :

Il faudra rendre un rapport et un programme : c'est-à-dire plusieurs fichiers que vous regrouperez dans une archive ZIP ou RAR ou TGZ; Vous devrez me l'envoyer par mail à l'adresse suivante : pa@lri.fr. N'envoyez **qu'un et un seul** mail par binôme.

> Les buts de ce projet

- Vous apprendre à écrire un programme à partir de spécifications informelles (énoncé en français).
- Vous familiariser avec le fait que vos programmes seront réutilisés par d'autres plus tard; donc à les écrire lisiblement en les **commentant** judicieusement.
- Vous entrainer à **encapsuler** vos données et méthodes pour obtenir des programmes robustes.
- Vous montrer l'intérêt de séparer la partie traitement d'un programme de sa partie **interface utilisateur**.

> Ce qui vous sera fourni

On vous fournira diverses classes et fonctions C++ permettant l'affichage en 3D avec OpenGL du labyrinthe et des diverses textures.

On se propose de réaliser le jeu de type Kill'emAll en beaucoup plus simple, avec un affichage 3D lui aussi simplifié. Le jeu doit pouvoir fontionner en 3 modes distincts:

- **Autonome**: le joueur joue seul contre la machine.
- **Serveur**: le jeu est en réseau (contre un seul client)
- **Client**: le jeu est en réseau (contre un seul serveur)

La machine anime des personnages de type gardien dont le but est de protéger un trésor caché dans une pièce du labyrinthe, que vous, le chasseur, essayez de prendre. Le but restant le même dans le jeu en réseau (vous pouvez vous allier ou pas avec l'autre humain).

Chaque personnage possède un capital initial de survie qui est décrémenté à chaque fois qu'il subit une attaque de l'un de ses adversaires. Ce capital est incrémenté quand le personnage reste un certain temps sans subir de blessure (ou tout autre critère comme par exemple trouver des réserves de santé sous forme de caisses de survie). Le capital initial de chaque type de personnage (gardiens, chasseur) et le temps de récupération devront être des paramètres du programme.

Les gardiens peuvent attaquer le chasseur à distance en tirant dessus avec une arme qui tire en ligne droite, et dont le projectile est arreté par un obstacle quelconque (mur, objet, etc.), la cible du gardien doit donc être visible pour que le tir ait une chance d'être efficace. Pour laisser une chance au chasseur, chaque gardien a un probabilité de manquer sa cible; cette probabilité dépend de l'état de santé du gardien : moins il a de points de vie, plus il tire mal. Le coefficient qui relie l'état de santé d'un gardien à sa capacité à tirer précisément est aussi un paramètre du programme.

Réciproquement, le chasseur peut attaquer les gardiens, il partage avec les gardiens la caractéristique d'avoir une précision de tir variable avec sont état de santé. Le coefficient n'est pas le même que pour un gardien car le tir du chasseur est dirigé par un humain (et est donc bien moins précis que ce que peut faire la machine qui est parfaite!).

Le chasseur se déplace dans le labyrinthe dans une direction qui est indiquée par la souris et/ou le clavier, c'est le joueur humain qui contrôle complètement le chasseur. Par contre, les gardiens sont des incarnations de la machine : ils sont animés par une sorte d'instinct artificiel qui les poussent d'une part à protéger le trésor, et d'autre part à attaquer le chasseur. C'est l'opposition entre ces deux potentiels plus une dose d'aléatoire qui décide de leur comportement à chaque instant. Chaque gardien sait exactement où se trouvent ses congénères et où est le trésor. Un gardien calcule son potentiel de protection en fonction de la distance qui le sépare du trésor, et du potentiel de protection des autres gardiens.

Un gardien va décider d'aller à la recherche du chasseur dès que son potentiel de protection va être en dessous d'un certain seuil. Pour éviter que les gardiens se figent à une distance qui équilibrent leurs potentiels, on va mettre deux seuils : si le potentiel de protection dépasse le seuil haut, le gardien passe à l'attaque, si le potentiel de protection devient inférieur au seuil bas, il reste en défense. Ces seuils sont des paramètres du programme. Enfin, pour rendre réaliste le comportement des gardiens, on tirera un nombre aléatoire qui s'ajoutera (ou se retranchera s'il est négatif) au potentiel calculé; cela devrait éviter aux gardiens d'avoir un comportement trop prédictible!

> Le labyrinthe

Le constructeur de la classe Labyrinthe servira à initialiser un labyrinthe selon la représentation interne du programme de jeu à partir d'une représentation textuelle du labyrinthe (à l'aide des caractères + - |).

Conceptuellement le labyrinthe est en 2D, c'est l'affichage graphique qui va le rendre en 3D, vos personnages évoluent dans un plan où les murs des pièces (ou des couloirs qui ne sont que des pièces particulières) sont des lignes droites horizontales ou verticales. Le labyrinthe est fermé, et possède deux pièces particulières : 
- celle où le chasseur est placé initialement, 
- celle où se trouve le trésor.

![Image of Labyrinthe](https://www.lri.fr/~pa/PROGCXX/labyrinthe.png)

> Structure du programme

Pour que votre code soit lisible vous mettrez un gros commentaire au début de chaque fichier indiquant à quoi servent les classes qui s'y trouvent, comment elles s'articulent pour constituer le module et à quoi sert ce module. Vous mettrez aussi un petit commentaire en tête de chaque méthode indiquant ce qu'elle fait, la signification de chacun de ses arguments, et celle de la valeur de retour. Enfin, mettez un petit commentaire indiquant ce que représente chaque champ de donnée de vos classes. Veillez aussi à respecter les conventions de nommage qui vous ont été indiquées dans le cours.

C'est à vous de concevoir l'architecture de votre programme de façon à ce qu'il ait toutes les qualités exigées d'un bon logiciel : simplicité, concision, clarté, etc.

Entre autres, commencez par identifier les objets impliqués dans le programme, indiquez comment ces objets s'articulent entre eux pour faire ce qu'on vous demande. Parallèlement, commencez à énumérer les caractéristiques des ces objets : attributs puis méthodes, pour identifier leur classe. Ensuite, regardez les relations entre ces classes, peut être partagent-elles des caractéristiques ? Vous pourrez implémenter ces relations avec la dérivation ou l'utilisation.

> Classes dont devront dériver les vôtres

Pour pouvoir profiter de l'affichage 3D avec OpenGL, vos classes correspondant au labyrinthe et aux gardiens/chasseur devront dériver des modèles téléchargeables suivants:

- la classe Environnement qui définit aussi des structures pour les murs, les caisses et éventuellement des affiches.
- la classe Mover qui définit les fonctionnalités minimales des gardiens/chasseur qui seront utilisées par l'interface graphique.
- L'archive [LabyrintheProto.tgz](https://www.lri.fr/~pa/PROGCXX/LabyrintheProto.tgz) (resp. [LabyrintheDebProto.tgz](https://www.lri.fr/~pa/PROGCXX/LabyrintheDebProto.tgz) pour Debian/Ubuntu, [LabyrintheMacProto.tgz](https://www.lri.fr/~pa/PROGCXX/LabyrintheMacProto.tgz) pour MacOSX) qui contient tous les fichiers d'entête (Chasseur.h Environnement.h Gardien.h Mover.h FireBall.h) les fichiers d'exemple (Labyrinthe.h Labyrinthe.cpp Makefile). C'est le fichier objet à linker avec votre code pour obtenir un exécutable. La fonction main est contenue dans ce fichier donc ne la définissez pas vous même!
Cette archive contient aussi toutes les textures (fichiers .jpg) et le modèle de gardien (garde.md2).
- L'archive [sockets.tgz](https://www.lri.fr/~pa/PROGCXX/sockets.tgz) qui contient les fichiers protocol.h et protocol.cc pour la definition du protocole d'envoi de messages; tcp_socket.h et tcp_socket.cc pour les sockets eux-memes; thread.h et thread.cc pour avoir des threads en C++ pour une meilleure gestion du serveur; et enfin deux programmes de test: client_main.cc et server_main.cc qui s'echangent un message.

Bien entendu, il ne faut **pas modifier** ne serait-ce que d'un caractère le contenu de ces fichiers (sauf les programmes de test!).

Pour le labyrinthe, votre classe (dérivée de Environnement) **doit** s'appeller Labyrinthe et posseder un constructeur qui prend un argument de type char* qui est le nom du fichier definissant le labyrinthe.

La constante Environnement::scale est un facteur d'echelle permettant de convertir des coordonnées flottantes de personnages en coordonnées entières dans la carte. Cette conversion se passe de la façon suivante: (par exemple)

    Mover*	m;

    int i = (int) (m -> _x / Environnement::scale);
    int j = (int) (m -> _y / Environnement::scale);
             
Rien de tel qu'un exemple pour comprendre comment utiliser du code que vous n'avez pas écrit vous même. Téléchargez l'archive LabyrintheProto.tgz (voir plus haut):

- Labyrinthe.h et Labyrinthe.cpp qui définissent un labyrinthe trivial.
- Chasseur et Gardien qui sont deux exemples quasi vides de dérivés de la classe Mover
- Makefile qui vous montre les bibliothèques à utiliser pour linker votre application.

La classe Mover (et donc par ricochet ses dérivées Gardien et Chasseur) a été augmentée de l'affichage d'un projectile (boule de feu) qui nécessite l'inclusion du fichier FireBall.h et une méthode virtuelle :

```
bool process_fireball (float dx, float dy)
```

dont un exemple est donné dans Chasseur.h. Cette fonction est appelée automatiquement quand on a initialisé un tir (voir la méthode fire du chasseur). Elle retourne vrai si le déplacement est accepté et faux s'il y a une collision. En cas de collision, la boule explosera automatiquement.
