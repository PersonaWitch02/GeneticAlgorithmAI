//
// Created by 13614 on 4/10/2026.
//

#pragma once


#include "DataStructures.h"
#include <vector>

void randomBaseline(std::vector<Individual> &population, int size,
                          const std::vector<Class> &classes,
                          const std::vector<Room> &rooms);

void randomBaselineGeneration(int populationSize);

void initializePopulation(std::vector<Individual>& population,
                          int size,
                          const std::vector<Class>& classes,
                          const std::vector<Room>& rooms);


void calculateFitness(Individual& ind,
                        const std::vector<Class>& classes,
                        const std::vector<Room>& rooms);

bool isOverlapping(const Class& a, const Class& b);


//Genetic Algo using tournament selection and random resetting mutation
Individual geneticAlgo( int populationSize, int tournamentSize, double crossoverRate, double mutationRate, int maximumGeneration, double tournamentRate);

// population = P, tournamentSize = k, probability= p
Individual tournamentSelection(std::vector<Individual> &population, int tournamentSize, int tournamentProbability);

void singlePointCrossOver(Individual &parent1, Individual &parent2, size_t length, double crossoverRate);

void randomResettingMutation( std::vector<int> &chromosome, double mutationProbability, size_t roomsSize);

uint8_t applyMask(std::string byte);

bool haveCommonDay(const Class& a, const Class& b);

