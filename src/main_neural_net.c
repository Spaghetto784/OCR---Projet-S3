#include <stdio.h>
#include <stdlib.h>

#include "neural_net.h"

int main(int argc, char *argv[])
{
    // Initialize the network
    init_network();

    // Train the network on the truth data
    for (int epoch = 0; epoch < 10000; epoch++)
    {
        train(0, 0, 0);
        train(0, 1, 0);
        train(1, 0, 1);
        train(1, 1, 1);
    }

    // Check command-line arguments
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <A (0 or 1)> <B (0 or 1)>\n", argv[0]);
        return 1;
    }

    int A = atoi(argv[1]);
    int B = atoi(argv[2]);

    // Verify that A and B are either 0 or 1
    if ((A != 0 && A != 1) || (B != 0 && B != 1))
    {
        fprintf(stderr, "Incorrect value: A and B must be 0 or 1.\n");
        return 1;
    }

    // Display the prediction
    float result = predict(A, B);
    printf("Result for A = %d and B = %d: %.2f\n", A, B, result);

    return 0;
}
