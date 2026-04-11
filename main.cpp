#include "CSVParser.h"
#include "GeneticAlgorithm.h"
#include <vector>
#include <iostream>

int main() {
    std::vector<Class> classes = loadClasses("classes_demand.csv");
    std::vector<Room> rooms = loadRooms("rooms_pool.csv");

    std::vector<Individual> population;
    int populationSize = 100;

    initializePopulation(population, populationSize, classes, rooms);

    // --- ADD THIS TO VERIFY ---
    std::cout << "Population initialized successfully!" << std::endl;
    std::cout << "Number of individuals: " << population.size() << std::endl;

    // Print the first 5 room assignments of the first individual
    std::cout << "First 5 genes (Room IDs) of Individual 0: ";
    for(int i = 0; i < 5; i++) {
        std::cout << population[0].chromosome[i] << " ";
    }
    std::cout << std::endl;
    // ---------------------------

    return 0;
}