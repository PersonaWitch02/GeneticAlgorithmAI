//
// Created by 13614 on 4/10/2026.
//

#ifndef UNTITLED_GENETICALGORITHM_H
#define UNTITLED_GENETICALGORITHM_H

#include "DataStructures.h"
#include <vector>

// Section 2: Chromosome Representation & Population Init
void initializePopulation(std::vector<Individual>& population,
                          int size,
                          const std::vector<Class>& classes,
                          const std::vector<Room>& rooms);

// Section 3: Fitness Function (Hard & Soft Constraints)
double calculateFitness(Individual& ind,
                        const std::vector<Class>& classes,
                        const std::vector<Room>& rooms);

bool isOverlapping(const Class& a, const Class& b);


//Genetic Algo using tournament selection and random resetting mutation
Individual geneticAlgo( int populationSize, int tournamentSize, double crossoverRate, double mutationRate, int maximumGeneration, double probability,
                        std::vector<Individual> &population, const std::vector<Class> &classes, const std::vector<Room> &rooms);

// population = P, tournamentSize = k, probability= p
Individual tournamentSelection(std::vector<Individual> &population, int tournamentSize, int tournamentProbability);

void singlePointCrossOver(Individual &parent1, Individual &parent2, size_t length);

void randomResettingMutation( std::vector<int> &chromosome, double mutationProbability, size_t roomsSize);

#endif //UNTITLED_GENETICALGORITHM_H
