#include <stdio.h>
#include <stdlib.h>
#include "neural_net.h"

int main(int argc, char *argv[]) {
    // Initialisation du réseau
    init_network();

    // Entraînement du réseau sur les données de vérité
    for (int epoch = 0; epoch < 10000; epoch++) {
        train(0, 0, 0);
        train(0, 1, 0);
        train(1, 0, 1);
        train(1, 1, 1);
    }

    // Vérification des arguments
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <A (0 ou 1)> <B (0 ou 1)>\n", argv[0]);
        return 1;
    }

    int A = atoi(argv[1]);
    int B = atoi(argv[2]);

    // Vérifie si A et B sont bien 0 ou 1
    if ((A != 0 && A != 1) || (B != 0 && B != 1)) {
        fprintf(stderr, "Valeur incorrecte : A et B doivent être 0 ou 1.\n");
        return 1;
    }

    // Affiche la prédiction
    float result = predict(A, B);
    printf("Résultat pour A = %d et B = %d : %.2f\n", A, B, result);

    return 0;
}

