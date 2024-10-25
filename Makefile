# Nom de l'exécutable final
EXEC = ocr_solver

# Compilateur
CC = cc

# Dossiers
SRC_DIR = src
INC_DIR = include
DATA_DIR = data/examples

# Fichiers sources et en-têtes
SRC = $(SRC_DIR)/main.c $(SRC_DIR)/preprocessor.c
OBJ = $(SRC:.c=.o)
INCLUDES = -I$(INC_DIR)

# Options de compilation
CFLAGS = -Wall -Wextra -std=c11

# Règle de compilation principale
all: $(EXEC)

# Création de l'exécutable
$(EXEC): $(OBJ)
	$(CC) -o $(EXEC) $(OBJ)

# Compilation des fichiers objets
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Nettoyage des fichiers objets et de l'exécutable
clean:
	rm -f $(OBJ) $(EXEC)

# Exemple d'utilisation avec l'image de niveau 1
run: $(EXEC)
	./$(EXEC) $(DATA_DIR)/level_1_image_1.png
