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

#endif //UNTITLED_GENETICALGORITHM_H
