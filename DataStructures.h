//
// Created by 13614 on 4/10/2026.
//

#ifndef UNTITLED_DATASTRUCTURES_H
#define UNTITLED_DATASTRUCTURES_H

#include <vector>
#include <string>

struct Class {
    int id;
    int enrollment;
    std::string days; // e.g., "1010100"
    int startSlot;
    int length;
};

struct Room {
    int id;
    int capacity;
    int x, y;
};

struct Individual {
    // This vector is your Chromosome: phi = [roomID1, roomID2, ...]
    std::vector<int> chromosome;
    double fitness;

    Individual(int numClasses) : chromosome(numClasses), fitness(0.0) {}
};

#endif //UNTITLED_DATASTRUCTURES_H
