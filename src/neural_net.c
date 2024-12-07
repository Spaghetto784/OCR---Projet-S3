#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h> // Required for srand(time(0))
#include "neural_net.h"
#include <err.h>
#include "png_to_array.h"

// Activation function (sigmoid)
double sigmoid(double x) {
    return 1.0 / (1.0 + exp(-x));
}

// Derivative of activation function
double sigmoid_derivative(double x) {
    return x * (1.0 - x);
}


// Initialization of the neural network with random values 
void initialize_network(NeuralNetwork *nn) 
{
    srand(time(0));
    
    nn->weights_input_hidden = (double **)malloc(INPUT_SIZE * sizeof(double *));
    nn->weights_hidden_output = (double **)malloc(HIDDEN_SIZE * sizeof(double *));
    nn->bias_hidden = (double *)malloc(HIDDEN_SIZE * sizeof(double));
    nn->bias_output = (double *)malloc(OUTPUT_SIZE * sizeof(double));
    nn->hidden_output = (double *)malloc(HIDDEN_SIZE * sizeof(double));
    nn->output = (double *)malloc(OUTPUT_SIZE * sizeof(double));

    for (int i = 0; i < INPUT_SIZE; i++) {
	nn->weights_input_hidden[i] = (double *)malloc(HIDDEN_SIZE * sizeof(double));
        for (int j = 0; j < HIDDEN_SIZE; j++) {
            nn->weights_input_hidden[i][j] = ((double) rand() / RAND_MAX) - 0.5;
        }
    }
    for (int i = 0; i < HIDDEN_SIZE; i++) {
	nn->weights_hidden_output[i] = (double *)malloc(OUTPUT_SIZE * sizeof(double));
        nn->bias_hidden[i] = ((double) rand() / RAND_MAX) - 0.5;
        for (int j = 0; j < OUTPUT_SIZE; j++) {
            nn->weights_hidden_output[i][j] = ((double) rand() / RAND_MAX) - 0.5;
        }
    }
    for (int i = 0; i < OUTPUT_SIZE; i++) {
        nn->bias_output[i] = ((double) rand() / RAND_MAX) - 0.5;
    }
}

// Front propagation
void forward(NeuralNetwork *nn, double inputs[INPUT_SIZE]) {
    // Calcul of hidden neurons bias
    for (int i = 0; i < HIDDEN_SIZE; i++) {
        double sum = nn->bias_hidden[i];
        for (int j = 0; j < INPUT_SIZE; j++) {
            sum += inputs[j] * nn->weights_input_hidden[j][i];
        }
        nn->hidden_output[i] = sigmoid(sum);
    }
    // Calcul of output neurons bias 
    for (int i = 0; i < OUTPUT_SIZE; i++) {
        double sum = nn->bias_output[i];
        for (int j = 0; j < HIDDEN_SIZE; j++) {
            sum += nn->hidden_output[j] * nn->weights_hidden_output[j][i];
        }
        nn->output[i] = sigmoid(sum);
    }
}

// Back propagation
void backward(NeuralNetwork *nn, double inputs[INPUT_SIZE], double target[OUTPUT_SIZE]) {
    double output_error[OUTPUT_SIZE];
    double output_delta[OUTPUT_SIZE];

    // Error rate and delta of output layer
    for (int i = 0; i < OUTPUT_SIZE; i++) {
        output_error[i] = target[i] - nn->output[i];
        output_delta[i] = output_error[i] * sigmoid_derivative(nn->output[i]);
    }

    // Error rate and delta of hidden layer
    double hidden_error[HIDDEN_SIZE];
    double hidden_delta[HIDDEN_SIZE];
    for (int i = 0; i < HIDDEN_SIZE; i++) {
        hidden_error[i] = 0.0;
        for (int j = 0; j < OUTPUT_SIZE; j++) {
            hidden_error[i] += output_delta[j] * nn->weights_hidden_output[i][j];
        }
        hidden_delta[i] = hidden_error[i] * sigmoid_derivative(nn->hidden_output[i]);
    }

    // Updating bias and weights from hidden to output layer
    for (int i = 0; i < HIDDEN_SIZE; i++) {
        for (int j = 0; j < OUTPUT_SIZE; j++) {
            nn->weights_hidden_output[i][j] += nn->hidden_output[i] * output_delta[j] * LEARNING_RATE;
        }
    }
    for (int i = 0; i < OUTPUT_SIZE; i++) {
        nn->bias_output[i] += output_delta[i] * LEARNING_RATE;
    }

    // Updating bias and weights from input to hidden layer
    for (int i = 0; i < INPUT_SIZE; i++) {
        for (int j = 0; j < HIDDEN_SIZE; j++) {
            nn->weights_input_hidden[i][j] += inputs[i] * hidden_delta[j] * LEARNING_RATE;
        }
    }
    for (int i = 0; i < HIDDEN_SIZE; i++) {
        nn->bias_hidden[i] += hidden_delta[i] * LEARNING_RATE;
    }
}
void save_network(const char *path, NeuralNetwork *nn, int input_size, int hidden_size, int output_size) {
    FILE *file = fopen(path, "wb");
    if (!file) {
        fprintf(stderr, "Failed to open file for writing: %s\n", path);
        return;
    }

    // Écriture des métadonnées
    if (fwrite(&input_size, sizeof(int), 1, file) != 1 ||
        fwrite(&hidden_size, sizeof(int), 1, file) != 1 ||
        fwrite(&output_size, sizeof(int), 1, file) != 1) {
        fprintf(stderr, "Failed to write network metadata.\n");
        fclose(file);
        return;
    }

    // Écriture des poids et des biais
    if ((size_t)fwrite(nn->weights_input_hidden, sizeof(double), input_size * hidden_size, file) != (size_t)(input_size * hidden_size) ||
        (size_t)fwrite(nn->weights_hidden_output, sizeof(double), hidden_size * output_size, file) != (size_t)(hidden_size * output_size) ||
        (size_t)fwrite(nn->bias_hidden, sizeof(double), hidden_size, file) != (size_t)hidden_size ||
        (size_t)fwrite(nn->bias_output, sizeof(double), output_size, file) != (size_t)output_size) {
        fprintf(stderr, "Failed to write network weights or biases.\n");
        fclose(file);
        return;
    }

    fclose(file);
    printf("Network saved successfully to %s\n", path);
}


int load_network(const char *path, NeuralNetwork *nn, int input_size, int hidden_size, int output_size) {
    FILE *file = fopen(path, "rb");
    if (!file) {
        fprintf(stderr, "Error: Unable to open file %s\n", path);
        return 0; // Échec
    }

    // Charger les dimensions
    if (fread(&input_size, sizeof(int), 1, file) != 1 ||
        fread(&hidden_size, sizeof(int), 1, file) != 1 ||
        fread(&output_size, sizeof(int), 1, file) != 1) {
        fclose(file);
        fprintf(stderr, "Error: Unable to read network dimensions\n");
        return 0; // Échec
    }

    // Charger les poids et les biais
    if ((size_t)fread(nn->weights_input_hidden, sizeof(double), input_size * hidden_size, file) != (size_t)(input_size * hidden_size) ||
        (size_t)fread(nn->weights_hidden_output, sizeof(double), hidden_size * output_size, file) != (size_t)(hidden_size * output_size) ||
        (size_t)fread(nn->bias_hidden, sizeof(double), hidden_size, file) != (size_t)hidden_size ||
        (size_t)fread(nn->bias_output, sizeof(double), output_size, file) != (size_t)output_size) {
        fclose(file);
        fprintf(stderr, "Error: Failed to read network weights or biases\n");
        return 0; // Échec
    }

    fclose(file);
    return 1; // Succès
}


void free_network(NeuralNetwork *nn) {
    for (int i = 0; i < INPUT_SIZE; i++) {
        free(nn->weights_input_hidden[i]);
    }
    free(nn->weights_input_hidden);

    for (int i = 0; i < HIDDEN_SIZE; i++) {
        free(nn->weights_hidden_output[i]);
    }
    free(nn->weights_hidden_output);

    free(nn->bias_hidden);
    free(nn->bias_output);
    free(nn->hidden_output);
    free(nn->output);
}

// Neural Network Training
void train(NeuralNetwork *nn, double inputs[][INPUT_SIZE], double targets[][OUTPUT_SIZE], int samples) {
    for (int epoch = 0; epoch < EPOCHS; epoch++) {
        double total_loss = 0.0;

        for (int i = 0; i < samples; i++) {
            forward(nn, inputs[i]);
            backward(nn, inputs[i], targets[i / 50]);

            // Calcul de la perte
            for (int j = 0; j < OUTPUT_SIZE; j++) {
                double error = targets[i / 50][j] - nn->output[j];
                total_loss += error * error;
            }
        }

        total_loss /= samples; // Moyenne de la perte
        printf("Epoch %d, Loss = %.6f\n", epoch + 1, total_loss);

        // Critère d'arrêt
        if (total_loss < LOSS_THRESHOLD) {
            printf("Training converged after %d epochs.\n", epoch + 1);
            break;
        }
    }
}

// Neural Network Testing
void test(NeuralNetwork *nn, double inputs[][INPUT_SIZE], double targets[][OUTPUT_SIZE], int samples) {
    (void)targets; // Marque `targets` comme inutilisé si vous n'en avez pas besoin

    for (int i = 0; i < samples; i++) {
        forward(nn, inputs[i]);
        int predicted = 0;
        double max_value = nn->output[0];
        for (int j = 1; j < OUTPUT_SIZE; j++) {
            if (nn->output[j] > max_value) {
                max_value = nn->output[j];
                predicted = j;
            }
        }

        printf("Prediction = %c\n", predicted + 'A');
    }
}

char predict_char(char *image_path, char *data) {
    // Load the trained neural network
    NeuralNetwork nn;
    initialize_network(&nn);
    load_network(data, &nn, INPUT_SIZE, HIDDEN_SIZE, OUTPUT_SIZE);
    

    // Convert the image to an input array
    double *temp = Output_Array(image_path);
    if (!temp) {
        fprintf(stderr, "Error: Unable to process image %s\n", image_path);
        free_network(&nn);
        return '\0';
    }

    double input[INPUT_SIZE];
    for (int i = 0; i < INPUT_SIZE; i++) {
        input[i] = temp[i];
    }
    free(temp);

    // Perform forward propagation
    forward(&nn, input);

    // Determine the predicted character
    int predicted_class = 0;
    double max_value = nn.output[0];
    for (int i = 1; i < OUTPUT_SIZE; i++) {
        if (nn.output[i] > max_value) {
            max_value = nn.output[i];
            predicted_class = i;
        }
    }

    // Free the network
    free_network(&nn);

    // Convert the predicted class to a character
    char predicted_char = 'A' + predicted_class;
    printf("Prediction for image %s: %c\n", image_path, predicted_char);
    return predicted_char;
}



