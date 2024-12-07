# Makefile unifié

CC = gcc
CFLAGS = -Wall -Wextra  -I/usr/include/SDL2 -Iinclude

# Cibles pour l'OCR
OCRB_SRCS = src/mainBw.c src/load_image.c src/image_processing.c # Enlève src/preprocessor.c si non utilisé
OCRB_OBJS = $(OCRB_SRCS:.c=.o)
OCRB_TARGET = bin/image_loaderB

OCRG_SRCS = src/mainGray.c src/load_image.c src/image_processing.c # Enlève src/preprocessor.c si non utilisé
OCRG_OBJS = $(OCRG_SRCS:.c=.o)
OCRG_TARGET = bin/image_loaderG

OCRContrast_SRCS = src/mainContrast.c src/load_image.c src/image_processing.c # Enlève src/preprocessor.c si non utilisé
OCRContrast_OBJS = $(OCRContrast_SRCS:.c=.o)
OCRContrast_TARGET = bin/image_loaderContrasted

OCRFilter_SRCS = src/mainFilter.c src/load_image.c src/image_processing.c # Enlève src/preprocessor.c si non utilisé
OCRFilter_OBJS = $(OCRFilter_SRCS:.c=.o)
OCRFilter_TARGET = bin/image_loaderFiltered

OCRInvert_SRCS = src/mainReverseColor.c src/load_image.c src/image_processing.c # Enlève src/preprocessor.c si non utilisé
OCRInvert_OBJS = $(OCRInvert_SRCS:.c=.o)
OCRInvert_TARGET = bin/image_loaderReversed

# Cibles pour detection
DET_SRCS = src/mainDet.c src/load_image.c src/image_processing.c src/detect.c # Enlève src/preprocessor.c si non utilisé
DET_OBJS = $(DET_SRCS:.c=.o)
DET_TARGET = bin/image_Det

# Cibles pour le module Neural
NEURAL_SRCS = src/neural_net.o src/main_neural_net.o src/png_to_array.o 
NEURAL_TARGET = bin/neural_test

# Cibles pour le module Solver
SOLVER_SRCS = src/solver.c src/main_solver.c
SOLVER_TARGET = bin/solver

.PHONY: all ocr neural solver clean

# Cible par défaut
all: ocrb ocrg ocrc det neural solver

# Cible pour l'OCR
ocrb: $(OCRB_TARGET)

$(OCRB_TARGET): $(OCRB_OBJS)
	mkdir -p bin
	$(CC) -o $(OCRB_TARGET) $(OCRB_OBJS) -lSDL2 -lSDL2_image -lm

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)


ocrg: $(OCRG_TARGET)

$(OCRG_TARGET): $(OCRG_OBJS)
	mkdir -p bin
	$(CC) -o $(OCRG_TARGET) $(OCRG_OBJS) -lSDL2 -lSDL2_image -lm

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)
	

ocrc: $(OCRContrast_TARGET)

$(OCRContrast_TARGET): $(OCRContrast_OBJS)
	mkdir -p bin
	$(CC) -o $(OCRContrast_TARGET) $(OCRContrast_OBJS) -lSDL2 -lSDL2_image -lm

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

ocrf: $(OCRFilter_TARGET)

$(OCRFilter_TARGET): $(OCRFilter_OBJS)
	mkdir -p bin
	$(CC) -o $(OCRFilter_TARGET) $(OCRFilter_OBJS) -lSDL2 -lSDL2_image -lm

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

ocri: $(OCRInvert_TARGET)

$(OCRInvert_TARGET): $(OCRInvert_OBJS)
	mkdir -p bin
	$(CC) -o $(OCRInvert_TARGET) $(OCRInvert_OBJS) -lSDL2 -lSDL2_image -lm

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)


det: $(DET_TARGET)

$(DET_TARGET): $(DET_OBJS)
	mkdir -p bin
	$(CC) -o $(DET_TARGET) $(DET_OBJS) -lSDL2 -lSDL2_image -lm

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

# Cible pour le module Neural
neural: $(NEURAL_TARGET)

$(NEURAL_TARGET): $(NEURAL_SRCS)
	$(CC) $(CFLAGS) -o $(NEURAL_TARGET) $(NEURAL_SRCS)  -lSDL2 -lSDL2_image -lm

# Cible pour le module Solver
solver: $(SOLVER_TARGET)

$(SOLVER_TARGET): $(SOLVER_SRCS)
	$(CC) $(CFLAGS) -o $(SOLVER_TARGET) $(SOLVER_SRCS)

# Cible de nettoyage
clean:
	rm -f $(OCRB_OBJS) $(DET_OBJS) $(DET_TARGET) $(OCRB_TARGET) $(OCRG_OBJS) $(OCRG_TARGET) $(OCRContrast_OBJS) $(OCRContrast_TARGET) $(OCRFilter_OBJS) $(OCRFilter_TARGET) $(OCRInvert_OBJS) $(OCRInvert_TARGET) $(NEURAL_SRCS:.c=.o) $(NEURAL_TARGET) $(SOLVER_TARGET)
	rm -f letterGrid/*.png letterGrid/*.bmp letterList/*.png letterList/*.bmp
