//
// Created by 13614 on 4/10/2026.
//

#pragma once

#include <cstdint>
#include <vector>
#include <string>

struct Class {
    int id;
    int enrollment;
    std::string days;
    int startSlot;
    int length;
};

struct Room {
    int id;
    int capacity;
    int x, y;
};

struct Individual {
    std::vector<int> chromosome;
    double fitness;

    Individual(int numClasses) : chromosome(numClasses), fitness(0.0) {}
};

