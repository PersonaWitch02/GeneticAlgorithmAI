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

    int tournamentSize = 5;
    double crossoverRate = 0.45;
    double mutationRate = 0.7;
    int maximumGeneration = 7;
    double probability = 0.15;

    using std::cout, std::endl;

    cout<< "tournament size: "<<tournamentSize<<endl;
    cout<< "crossover rate: "<<crossoverRate<<endl;
    cout<< "mutation rate: "<<mutationRate<<endl;
    cout<< "maximum generation: "<<maximumGeneration<<endl;
    cout<< "probability: "<<probability<<endl;

    Individual best = geneticAlgo(populationSize, tournamentSize, crossoverRate, mutationRate, maximumGeneration, probability,
        population, classes, rooms);

    cout<<"Best"<< best.fitness<<endl;
    for (int i=0; i<best.chromosome.size(); i++) {
        cout<<best.chromosome[i]<<", ";
    }

    return 0;
}