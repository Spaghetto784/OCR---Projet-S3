#ifndef NEURAL_NET_H
#define NEURAL_NET_H

// Paramètres du réseau
#define INPUT_NODES 2
#define HIDDEN_NODES 2
#define OUTPUT_NODES 1
#define LEARNING_RATE 0.1

// Fonction d'initialisation
void init_network();

// Fonction d'entraînement
void train(int A, int B, int target);

// Fonction de prédiction
float predict(int A, int B);

#endif
