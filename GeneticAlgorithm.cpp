#include "GeneticAlgorithm.h"
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <vector>
#include <iostream>

#include <chrono>

#include "CSVParser.h"


std::chrono::microseconds timeFitness(0);
std::chrono::nanoseconds timeGA(0);

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

void calculateFitness(Individual &ind, const std::vector<Class> &classes, const std::vector<Room> &rooms) {
    int hardViolations = 0;
    double softViolations = 0.0;
    int numRooms = rooms.size();

    std::vector<std::vector<int> > roomAssignments(numRooms);

    //HARD CONSTRAINT: Capacity
    for (size_t i = 0; i < classes.size(); i++) {
        int roomIdx = ind.chromosome[i];
        if (classes[i].enrollment > rooms[roomIdx].capacity) {
            hardViolations++;
        }
        roomAssignments[roomIdx].push_back(i);
    }

    //HARD CONSTRAINT: Overlap
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

    //soft constraint
    for (int day = 0; day < 7; day++) {
        std::vector<int> classDays;

        for (size_t i = 0; i < classes.size(); i++) {
            if (classes[i].days[day] == '1') {
                classDays.push_back(i);
            }
        }

        std::ranges::sort(classDays, [&](int a, int b) {
            return classes[a].startSlot < classes[b].startSlot;
        });

        for (size_t t = 0; t + 1 < classDays.size(); t++) {
            const Room &r1 = rooms[ind.chromosome[classDays[t]]];
            const Room &r2 = rooms[ind.chromosome[classDays[t + 1]]];
            softViolations += classes[classDays[t]].enrollment * std::sqrt(
                (r2.x - r1.x) * (r2.x - r1.x) + (r2.y - r1.y) * (r2.y - r1.y));
        }
    }

    ind.fitness = 1.0 / (1.0 + (1000.0 * hardViolations) + (softViolations / 1000000.0));
}


//GA
Individual geneticAlgo(int populationSize, int tournamentSize, double crossoverRate, double mutationRate,
                       int maximumGeneration, double probability) {
    using namespace std;

    vector<Class> classes = loadClasses("classes_demand.csv");
    vector<Room> rooms = loadRooms("rooms_pool.csv");

    vector<Individual> population;
    initializePopulation(population, populationSize, classes, rooms);


    cout << "Population initialized successfully!" << endl;
    cout << "Number of individuals: " << population.size() << endl;

    //first 5 room assignments of the first individual
    cout << "First 5 genes (Room IDs) of Individual 0: ";
    for (int i = 0; i < 5; i++) {
        cout << population[0].chromosome[i] << " ";
    }
    cout << std::endl;

    calculateFitness(population[0], classes, rooms);

    auto initialFitness = population[0].fitness;

    cout << "Initial Random Schedule Fitness: " << initialFitness << endl;

    vector<Individual> newPopulation;

    const int tournamentProbability = probability * 1000;
    const int mutationRateScaled = mutationRate * 1000;

    for (int i = 0; i < maximumGeneration; i++) {
        newPopulation.clear();

        //elitism
        Individual best = population[0];
        for (const Individual &ind: population) {
            if (ind.fitness > best.fitness) {
                best = ind;
            }
        }
        newPopulation.push_back(best);

        auto startGA = chrono::steady_clock::now();
        while (newPopulation.size() < populationSize) {
            Individual parent1 = tournamentSelection(population, tournamentSize, tournamentProbability);
            Individual parent2 = tournamentSelection(population, tournamentSize, tournamentProbability);

            if (static_cast<double>(rand() % 1000) / 1000.0 < crossoverRate) {
                singlePointCrossOver(parent1, parent2, parent1.chromosome.size());
            }

            randomResettingMutation(parent1.chromosome, mutationRateScaled, rooms.size());
            randomResettingMutation(parent2.chromosome, mutationRateScaled, rooms.size());

            newPopulation.push_back(parent1);
            newPopulation.push_back(parent2);
        }

        population = newPopulation;

        timeGA += chrono::duration_cast<chrono::nanoseconds>(chrono::steady_clock::now() - startGA);

        auto startFit = chrono::steady_clock::now();
        for (Individual &individual: population) {
            calculateFitness(individual, classes, rooms);
        }
        timeFitness += chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now() - startFit);

        if (i % 500 == 0) {
            Individual &topFitness = population[0];
            for (Individual &ind: population)
                if (ind.fitness > topFitness.fitness) topFitness = ind;
            cout << "Generation " << i << ", fitness: " << topFitness.fitness << endl;
        }
    }

    auto startGA = chrono::steady_clock::now();

    Individual bestChromosome = population[0];
    for (const Individual &individual: population) {
        if (individual.fitness > bestChromosome.fitness) {
            bestChromosome = individual;
        }
    }

    timeGA += chrono::duration_cast<chrono::nanoseconds>(chrono::steady_clock::now() - startGA);

    cout << "Genetic Algorithm took: " << timeGA.count() / 1000000000.0 << " seconds" << endl;
    cout << "Fitness took: " << timeFitness.count() / 1000000.0 << " seconds" << endl;

    // Diagnostic: count violations in best solution
    int hardViolations = 0;
    int numRooms = rooms.size();

    std::vector<std::vector<int> > roomAssignments(numRooms);
    for (size_t i = 0; i < classes.size(); i++) {
        int roomIdx = bestChromosome.chromosome[i];
        if (classes[i].enrollment > rooms[roomIdx].capacity) {
            hardViolations++;
        }
        roomAssignments[roomIdx].push_back(i);
    }

    int overlapViolations = 0;
    int capacityViolations = hardViolations;
    for (int r = 0; r < numRooms; r++) {
        const auto &assigned = roomAssignments[r];
        for (size_t i = 0; i < assigned.size(); i++)
            for (size_t j = i + 1; j < assigned.size(); j++)
                if (isOverlapping(classes[assigned[i]], classes[assigned[j]])) {
                    overlapViolations++;
                    hardViolations++;
                }
    }
    cout << "Capacity violations: " << capacityViolations << endl;
    cout << "Overlap violations:  " << overlapViolations << endl;
    cout << "Total hard violations: " << hardViolations << endl;


    cout<<"Difference between best and initial:"<< abs(bestChromosome.fitness - initialFitness)<<endl;

    return bestChromosome;
}

Individual tournamentSelection(std::vector<Individual> &population, const int tournamentSize,
                               const int tournamentProbability) {
    const size_t populationSize = population.size();

    std::vector<Individual *> tournamentSet(tournamentSize);

    //selecting randoms for tournament set
    for (int i = 0; i < tournamentSize; i++) {
        const int randomIndividual = rand() % static_cast<int>(populationSize);

        tournamentSet[i] = &population[randomIndividual];
    }

    //sorting the tournament set, lamba allows custom sort
    std::sort(tournamentSet.begin(), tournamentSet.end(), [](Individual *a, Individual *b) {
        return a->fitness > b->fitness;
    });

    const int randomNumber = rand() % 1000;

    if (randomNumber < tournamentProbability) {
        return *tournamentSet[0];
    } else {
        const int randomIndex = rand() % (tournamentSize - 1);
        return *tournamentSet[randomIndex + 1];
    }
}


void singlePointCrossOver(Individual &parent1, Individual &parent2, const size_t length) {
    const int crossOverPoint = rand() % static_cast<int>(length);

    for (int i = crossOverPoint; i < length; i++) {
        std::swap(parent1.chromosome[i], parent2.chromosome[i]);
    }
}

void randomResettingMutation(std::vector<int> &chromosome, const double mutationProbability, const size_t roomsSize) {
    for (int i = 0; i < chromosome.size(); i++) {
        const double randomNumber = rand() % 1000;

        if (randomNumber < mutationProbability) {
            const int randomValue = rand() % static_cast<int>(roomsSize);
            chromosome[i] = randomValue;
        }
    }
}
