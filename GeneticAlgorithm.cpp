#include "GeneticAlgorithm.h"
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <vector>
#include <iostream>

// Add this helper function at the top
bool isOverlapping(const Class& a, const Class& b) {
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

void initializePopulation(std::vector<Individual>& population, int size,
                          const std::vector<Class>& classes,
                          const std::vector<Room>& rooms) {
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

double calculateFitness(Individual& ind, const std::vector<Class>& classes, const std::vector<Room>& rooms) {
    int hardViolations = 0;
    double softPenalty = 0.0;
    int numRooms = rooms.size();

    // Group by room index for overlap checking
    std::vector<std::vector<int>> roomAssignments(numRooms);

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
        const auto& assigned = roomAssignments[r];
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
        const Room& r1 = rooms[ind.chromosome[i]];
        const Room& r2 = rooms[ind.chromosome[i+1]];
        softPenalty += std::sqrt(std::pow(r2.x - r1.x, 2) + std::pow(r2.y - r1.y, 2));
    }

    // Final Fitness Score
    ind.fitness = 1.0 / (1.0 + hardViolations + (softPenalty / 1000000.0));
    return ind.fitness;
}