# Makefile unifié

CC = gcc
CFLAGS = -Wall -Wextra -I/usr/include/SDL2 -Iinclude

# Cibles pour l'OCR
OCR_SRCS = src/main.c src/load_image.c src/image_processing.c  # Enlève src/preprocessor.c si non utilisé
OCR_OBJS = $(OCR_SRCS:.c=.o)
OCR_TARGET = bin/image_loader

# Cibles pour le module Neural
NEURAL_SRCS = src/neural_net.o src/main_neural_net.o
NEURAL_TARGET = bin/neural_test

# Cibles pour le module Solver
SOLVER_SRCS = src/solver.c src/main_solver.c
SOLVER_TARGET = bin/solver

.PHONY: all ocr neural solver clean

# Cible par défaut
all: ocr neural solver

# Cible pour l'OCR
ocr: $(OCR_TARGET)

$(OCR_TARGET): $(OCR_OBJS)
	mkdir -p bin
	$(CC) -o $(OCR_TARGET) $(OCR_OBJS) -lSDL2 -lSDL2_image -lm

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

# Cible pour le module Neural
neural: $(NEURAL_TARGET)

$(NEURAL_TARGET): $(NEURAL_SRCS)
	$(CC) $(CFLAGS) -o $(NEURAL_TARGET) $(NEURAL_SRCS) -lm

# Cible pour le module Solver
solver: $(SOLVER_TARGET)

$(SOLVER_TARGET): $(SOLVER_SRCS)
	$(CC) $(CFLAGS) -o $(SOLVER_TARGET) $(SOLVER_SRCS)

# Cible de nettoyage
clean:
	rm -f $(OCR_OBJS) $(OCR_TARGET) $(NEURAL_SRCS:.c=.o) $(NEURAL_TARGET) $(SOLVER_TARGET)
