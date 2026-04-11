#include "GeneticAlgorithm.h"
#include <cstdlib>

void initializePopulation(std::vector<Individual>& population, int size,
                          const std::vector<Class>& classes,
                          const std::vector<Room>& rooms) {
    srand(42); // Project requirement
    for (int i = 0; i < size; ++i) {
        Individual ind(classes.size());
        for (size_t j = 0; j < classes.size(); ++j) {
            ind.chromosome[j] = rooms[rand() % rooms.size()].id;
        }
        population.push_back(ind);
    }
}

double calculateFitness(Individual& ind, const std::vector<Class>& classes, const std::vector<Room>& rooms) {
    // You will implement your constraint checking here
    return 0.0;
}