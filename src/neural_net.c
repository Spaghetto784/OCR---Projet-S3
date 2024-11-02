#include "neural_net.h"
#include <math.h>
#include <stdlib.h>

// Poids et biais du réseau
float weights_input_hidden[INPUT_NODES][HIDDEN_NODES];
float weights_hidden_output[HIDDEN_NODES][OUTPUT_NODES];
float bias_hidden[HIDDEN_NODES];
float bias_output[OUTPUT_NODES];

// Fonction d'activation sigmoïde
float sigmoid(float x) { return 1.0 / (1.0 + exp(-x)); }

// Initialisation des poids et des biais avec des valeurs aléatoires
void init_network() {
  for (int i = 0; i < INPUT_NODES; i++)
    for (int j = 0; j < HIDDEN_NODES; j++)
      weights_input_hidden[i][j] = (float)rand() / RAND_MAX * 2 - 1;

  for (int i = 0; i < HIDDEN_NODES; i++) {
    weights_hidden_output[i][0] = (float)rand() / RAND_MAX * 2 - 1;
    bias_hidden[i] = (float)rand() / RAND_MAX * 2 - 1;
  }
  bias_output[0] = (float)rand() / RAND_MAX * 2 - 1;
}

// Entraînement avec rétropropagation
void train(int A, int B, int target) {
  float inputs[] = {A, B};

  // Propagation vers l'avant
  float hidden[HIDDEN_NODES];
  for (int i = 0; i < HIDDEN_NODES; i++) {
    hidden[i] = bias_hidden[i];
    for (int j = 0; j < INPUT_NODES; j++) {
      hidden[i] += inputs[j] * weights_input_hidden[j][i];
    }
    hidden[i] = sigmoid(hidden[i]);
  }

  float output = bias_output[0];
  for (int i = 0; i < HIDDEN_NODES; i++) {
    output += hidden[i] * weights_hidden_output[i][0];
  }
  output = sigmoid(output);

  // Calcul de l'erreur
  float error = target - output;

  // Rétropropagation
  float gradient_output = error * output * (1 - output);
  for (int i = 0; i < HIDDEN_NODES; i++) {
    weights_hidden_output[i][0] += LEARNING_RATE * gradient_output * hidden[i];
  }
  bias_output[0] += LEARNING_RATE * gradient_output;

  for (int i = 0; i < HIDDEN_NODES; i++) {
    float gradient_hidden = gradient_output * weights_hidden_output[i][0] *
                            hidden[i] * (1 - hidden[i]);
    for (int j = 0; j < INPUT_NODES; j++) {
      weights_input_hidden[j][i] += LEARNING_RATE * gradient_hidden * inputs[j];
    }
    bias_hidden[i] += LEARNING_RATE * gradient_hidden;
  }
}

// Prédiction d'un résultat
float predict(int A, int B) {
  float inputs[] = {A, B};
  float hidden[HIDDEN_NODES];

  for (int i = 0; i < HIDDEN_NODES; i++) {
    hidden[i] = bias_hidden[i];
    for (int j = 0; j < INPUT_NODES; j++) {
      hidden[i] += inputs[j] * weights_input_hidden[j][i];
    }
    hidden[i] = sigmoid(hidden[i]);
  }

  float output = bias_output[0];
  for (int i = 0; i < HIDDEN_NODES; i++) {
    output += hidden[i] * weights_hidden_output[i][0];
  }
  return sigmoid(output);
}
