#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h> // Required for srand(time(0))
#include <err.h>
#include "png_to_array.h"


#define INPUT_SIZE 784
#define HIDDEN_SIZE 256
#define OUTPUT_SIZE 26
#define LEARNING_RATE 0.1    
#define EPOCHS 200   
#define LOSS_THRESHOLD 0.01
double sigmoid(double x);

double sigmoid_derivative(double x);

typedef struct {
    double **weights_input_hidden;
    double **weights_hidden_output;
    double *bias_hidden;
    double *bias_output;
    double *hidden_output;
    double *output;
} NeuralNetwork;


void initialize_network(NeuralNetwork *nn);

void forward(NeuralNetwork *nn, double inputs[INPUT_SIZE]);

void backward(NeuralNetwork *nn, double inputs[INPUT_SIZE], double target[OUTPUT_SIZE]);

void train(NeuralNetwork *nn, double inputs[][INPUT_SIZE], double targets[][OUTPUT_SIZE], int samples);

void test(NeuralNetwork *nn, double inputs[][INPUT_SIZE], double targets[][OUTPUT_SIZE], int samples);

void save_network(const char *filename, NeuralNetwork *nn, int input_size, int hidden_size, int output_size);

int load_network(const char *filename, NeuralNetwork *nn, int input_size, int hidden_size, int output_size);

void free_network(NeuralNetwork *nn);

char predict_char(char *image_path, char *data);

#endif  //NEURALNETWORK_H
