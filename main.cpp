#include "CSVParser.h"
#include "GeneticAlgorithm.h"

#include <vector>
#include <iostream>  // Add this line to fix the 'cerr' and 'endl' errors

int main() {
    std::vector<Class> classes = loadClasses("classes_demand.csv");
    std::vector<Room> rooms = loadRooms("rooms_pool.csv");

    if (classes.empty() || rooms.empty()) {
        std::cout << "Failed to load data files." << std::endl;
        return -1;
    }

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
    // Calculate fitness for the very first random schedule
    double score = calculateFitness(population[0], classes, rooms);

    std::cout << "Initial Random Schedule Fitness: " << score << std::endl;

    std::vector<int> roomIDPool = createDomain(rooms);

    return 0;
}