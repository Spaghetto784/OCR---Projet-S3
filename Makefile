CC = gcc
CFLAGS = -Wall -Wextra -I/usr/include/SDL2 -Iinclude
SRCS = src/main.c src/load_image.c src/image_processing.c  # Enlève src/preprocessor.c si non utilisé
OBJS = $(SRCS:.c=.o)

TARGET = bin/image_loader

all: $(TARGET)

$(TARGET): $(OBJS)
	mkdir -p bin
	$(CC) -o $(TARGET) $(OBJS) -lSDL2 -lSDL2_image

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f $(OBJS) $(TARGET)
