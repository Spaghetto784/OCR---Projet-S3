#ifndef NEURAL_NET_H
#define NEURAL_NET_H


#define INPUT_NODES 2
#define HIDDEN_NODES 2
#define OUTPUT_NODES 1
#define LEARNING_RATE 0.1


void init_network();


void train(int A, int B, int target);


float predict(int A, int B);

#endif
