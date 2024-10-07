# OCR Word Search Solver

## Groupe : **SegFaultless**

- **Matthieu Greiner**
- **Julien Texeira**
- **Dylan Barros**
- **Pierre-Louis Chambon**

---

## Description du Projet

L'objectif de ce projet est de développer un logiciel capable de résoudre des grilles de mots cachés à partir d'une image. Le projet utilise la reconnaissance optique de caractères (OCR) pour identifier les lettres dans une grille, puis un algorithme de résolution de grille en C pour rechercher les mots.

Le logiciel prend en entrée une image représentant une grille de mots cachés et renvoie la grille résolue. Cette application combine plusieurs technologies, dont :
- **Prétraitement d'image** : Conversion des images en niveaux de gris et redressement pour améliorer la qualité des images.
- **Réseau de neurones** : Reconnaissance des lettres présentes dans la grille à partir des images des lettres.
- **Solver de grille** : Algorithme en C permettant de rechercher un mot dans une grille et d'afficher sa position.

## Utilisation

### 1. Clonage du dépôt
Clonez le dépôt GIT contenant le projet :

```bash
git clone https://github.com/votre-repo.git
cd OCR-Word-Search-Solver
```

### 2. Compilation du programme

Le projet contient un fichier `Makefile` pour faciliter la compilation. Pour compiler l'application, exécutez la commande suivante dans le terminal :

```bash
make all
```

### 3. Utilisation du Solver en ligne de commande
Le solver permet de rechercher un mot dans une grille de caractères fournie sous forme de fichier texte. Voici un exemple d'utilisation :

```bash
./solver grid.txt horizontal
```

Où grid.txt contient la grille à résoudre, et horizontal est le mot à rechercher dans la grille.

Le programme affichera les coordonnées de la première et de la dernière lettre du mot dans la grille, par exemple :
```bash
(0,0)(9,0)
```
Si le mot n'est pas trouvé, il affichera :
```bash
Not Found
```

### 4. Nettoyage
Pour supprimer les fichiers compilés, utilisez la commande suivante :
```bash
make clean
```
