#include "GeneticAlgorithm.h"
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <vector>
#include <iostream>

#include <chrono>

std::chrono::milliseconds timeFitness(0);
std::chrono::milliseconds timeGA(0);

// Add this helper function at the top
bool isOverlapping(const Class &a, const Class &b) {
    bool dayOverlap = false;
    for (int k = 0; k < 7; k++) {
        if (a.days[k] == '1' && b.days[k] == '1') {
            dayOverlap = true;
            break;
        }
    }
    if (!dayOverlap) return false;

    int endA = a.startSlot + a.length;
    int endB = b.startSlot + b.length;
    return (std::max(a.startSlot, b.startSlot) < std::min(endA, endB));
}

void initializePopulation(std::vector<Individual> &population, int size,
                          const std::vector<Class> &classes,
                          const std::vector<Room> &rooms) {
    srand(42);
    for (int i = 0; i < size; ++i) {
        Individual ind(classes.size());
        for (size_t j = 0; j < classes.size(); ++j) {
            // Store the INDEX (0 to rooms.size() - 1)
            // This is safer than storing the ID
            ind.chromosome[j] = rand() % rooms.size();
        }
        population.push_back(ind);
    }
}

double calculateFitness(Individual &ind, const std::vector<Class> &classes, const std::vector<Room> &rooms) {
    int hardViolations = 0;
    double softPenalty = 0.0;
    int numRooms = rooms.size();

    // Group by room index for overlap checking
    std::vector<std::vector<int> > roomAssignments(numRooms);

    for (size_t i = 0; i < classes.size(); i++) {
        int roomIdx = ind.chromosome[i]; // This is now a safe index (0 to 126)

        // 1. HARD CONSTRAINT: Capacity
        // We can access rooms[roomIdx] directly and safely!
        if (classes[i].enrollment > rooms[roomIdx].capacity) {
            hardViolations++;
        }
        roomAssignments[roomIdx].push_back(i);
    }

    // 2. HARD CONSTRAINT: Overlap
    for (int r = 0; r < numRooms; r++) {
        const auto &assigned = roomAssignments[r];
        for (size_t i = 0; i < assigned.size(); i++) {
            for (size_t j = i + 1; j < assigned.size(); j++) {
                if (isOverlapping(classes[assigned[i]], classes[assigned[j]])) {
                    hardViolations++;
                }
            }
        }
    }

    // 3. SOFT CONSTRAINT: Distance
    for (size_t i = 0; i < classes.size() - 1; i++) {
        const Room &r1 = rooms[ind.chromosome[i]];
        const Room &r2 = rooms[ind.chromosome[i + 1]];
        softPenalty += std::sqrt(std::pow(r2.x - r1.x, 2) + std::pow(r2.y - r1.y, 2));
    }

    // Final Fitness Score
    ind.fitness = 1.0 / (1.0 + hardViolations + (softPenalty / 1000000.0));
    return ind.fitness;
}



//GA
Individual geneticAlgo(int populationSize, int tournamentSize, double crossoverRate, double mutationRate,
                       int maximumGeneration, double probability,
                       std::vector<Individual> &population, const std::vector<Class> &classes,
                       const std::vector<Room> &rooms) {

    using namespace std;
    //Init. pop.
    //Eval. fit.


    vector<Individual> newPopulation;

    int tournamentProbability = probability * 1000;
    int mutationRateScaled = mutationRate * 1000;

    for (int i = 0; i < maximumGeneration; i++) {
        newPopulation.clear();

        auto startGA = chrono::steady_clock::now();

        while (newPopulation.size() < populationSize) {
            // Individual parent1 = tournamentSelection(population, tournamentSize, probability);
            // Individual parent2 = tournamentSelection(population, tournamentSize, probability);
            Individual parent1 = tournamentSelection(population, tournamentSize, tournamentProbability);
            Individual parent2 = tournamentSelection(population, tournamentSize, tournamentProbability);

            if (static_cast<double>(rand() % 1000) / 1000.0 < crossoverRate) {
                singlePointCrossOver(parent1, parent2, parent1.chromosome.size());
            }

            // randomResettingMutation(parent1.chromosome, mutationRate, roomIDPool);
            // randomResettingMutation(parent2.chromosome, mutationRate, roomIDPool);
            randomResettingMutation(parent1.chromosome, mutationRateScaled, rooms.size());
            randomResettingMutation(parent2.chromosome, mutationRateScaled, rooms.size());

            newPopulation.push_back(parent1);
            newPopulation.push_back(parent2);
        }

        population = newPopulation;

        timeGA += chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - startGA);
        auto startFit = chrono::steady_clock::now();
        for (Individual &individual: population) {
            calculateFitness(individual, classes, rooms);
        }
        timeFitness += chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - startFit);
    }

    auto startGA = chrono::steady_clock::now();

    Individual bestChromosome = population[0];
    for (Individual individual: population) {
        if (individual.fitness > bestChromosome.fitness) {
            bestChromosome = individual;
        }
    }

    timeGA += chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - startGA);

    cout<<"GA took: "<<timeGA.count()/1000.0<<" seconds"<<endl;
    cout<<"Fit took: "<<timeFitness.count()/1000.0<<" seconds"<<endl;

    return bestChromosome;
}

Individual tournamentSelection(std::vector<Individual> &population, const int tournamentSize, const int tournamentProbability) {
    const size_t populationSize = population.size();

    Individual *tournamentSet[tournamentSize];

    //selecting randoms for tournament set
    for (int i = 0; i < tournamentSize; i++) {
        int randomIndividual = rand() % populationSize;

        tournamentSet[i] = &population[randomIndividual];
    }

    //sorting the tournament set, lamba allows custom sort
    std::sort(tournamentSet, tournamentSet + tournamentSize, [](Individual *a, Individual *b) {
        return a->fitness > b->fitness;
    });

    // double randomNumber = static_cast<double>(rand() % 1000)/1000.0;
    //
    // if ( randomNumber < probability) {
    //     return *tournamentSet[0];
    // }else {
    //     int randomIndex = rand() % (tournamentSize-1);
    //     return *tournamentSet[randomIndex+1];
    // }

    int randomNumber = rand() % 1000;

    if (randomNumber < tournamentProbability) {
        return *tournamentSet[0];
    } else {
        int randomIndex = rand() % (tournamentSize - 1);
        return *tournamentSet[randomIndex + 1];
    }
}


void singlePointCrossOver(Individual &parent1, Individual &parent2, const size_t length) {
    int crossOverPoint = rand() % length;

    for (int i = crossOverPoint; i < length; i++) {
        std::swap(parent1.chromosome[i], parent2.chromosome[i]);
    }
}

void randomResettingMutation(std::vector<int> &chromosome, const double mutationProbability, const size_t roomsSize) {
    for (int i = 0; i < chromosome.size(); i++) {
        // double randomNumber = static_cast<double>(rand() % 1000) / 1000.0;
        double randomNumber = rand() % 1000;

        if (randomNumber < mutationProbability) {
            int randomValue = rand() % roomsSize;
            chromosome[i] = randomValue;
        }
    }
}
