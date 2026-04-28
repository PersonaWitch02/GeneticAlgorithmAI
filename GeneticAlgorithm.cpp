#include "GeneticAlgorithm.h"
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <vector>
#include <iostream>

#include <chrono>

#include "CSVParser.h"

using namespace std;

long long iterations {0};
int overlap = 0;
int capacity = 0;
int totalViolations = 0;

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
    return (max(a.startSlot, b.startSlot) < min(endA, endB));
}

void randomBaseline(vector<Individual> &population, int size,
                          const vector<Class> &classes,
                          const vector<Room> &rooms) {
    srand(42);
    for (int i = 0; i < size; ++i) {
        Individual ind(classes.size());
        for (size_t j = 0; j < classes.size(); ++j) {

            ind.chromosome[j] = rand() % rooms.size();
        }
        population.push_back(ind);
    }
}

void randomBaselineGeneration(int populationSize) {
    vector<Class> classes = loadClasses("classes_demand.csv");
    vector<Room> rooms = loadRooms("rooms_pool.csv");

    vector<Individual> population;
    initializePopulation(population, populationSize, classes, rooms);

    cout<<"********Random Baseline******** \n";

    //first 5 room assignments of the first individual
    cout << "First 5 genes (Room IDs) of Individual 0: ";
    for (int i = 0; i < 5; i++) {
        cout << population[0].chromosome[i] << " ";
    }
    cout << endl;
    int random = rand() % populationSize;
    calculateFitness(population[random], classes, rooms);
    cout << "Capacity violations: " << capacity << endl;
    cout << "Overlap violations:  " << overlap << endl;
    cout << "Hard violations: " << capacity+overlap << endl;
    cout<<"**************************** \n";
}

//attempted heuristic initialization
void initializePopulation(vector<Individual> &population, int size,
                          const vector<Class> &classes,
                          const vector<Room> &rooms) {
    srand(42);

    vector<int> classOrder(classes.size());
    for (size_t i = 0; i < classes.size(); i++) classOrder[i] = i;
    sort(classOrder.begin(), classOrder.end(), [&](int a, int b) {
        return classes[a].enrollment > classes[b].enrollment;
    });

    vector<int> roomOrder(rooms.size());
    for (size_t i = 0; i < rooms.size(); i++) roomOrder[i] = i;
    sort(roomOrder.begin(), roomOrder.end(), [&](int a, int b) {
        return rooms[a].capacity > rooms[b].capacity;
    });

    for (int i = 0; i < size; ++i) {
        Individual ind(classes.size());
        for (size_t j = 0; j < classes.size(); ++j) {
            if (i == 0) {
                ind.chromosome[classOrder[j]] = roomOrder[j % rooms.size()];
            } else {
                ind.chromosome[j] = rand() % rooms.size();
            }
        }
        population.push_back(ind);
    }
}

void calculateFitness(Individual &ind, const vector<Class> &classes, const vector<Room> &rooms) {
    int hardViolations = 0;
    double softViolations = 0.0;
    int numRooms = rooms.size();

    iterations++;

    vector<vector<int> > roomAssignments(numRooms);

    //HARD CONSTRAINT: Capacity
    for (size_t i = 0; i < classes.size(); i++) {
        int roomIdx = ind.chromosome[i];
        if (classes[i].enrollment > rooms[roomIdx].capacity) {
            hardViolations++;
        }
        roomAssignments[roomIdx].push_back(i);
    }

    capacity = hardViolations;

    overlap = 0;
    //HARD CONSTRAINT: Overlap
    for (int r = 0; r < numRooms; r++) {
        const auto &assigned = roomAssignments[r];
        for (size_t i = 0; i < assigned.size(); i++) {
            for (size_t j = i + 1; j < assigned.size(); j++) {
                if (isOverlapping(classes[assigned[i]], classes[assigned[j]])) {
                    hardViolations++;
                    overlap++;
                }
            }
        }
    }

    //soft constraint
    for (int day = 0; day < 7; day++) {
        vector<int> classDays;

        for (size_t i = 0; i < classes.size(); i++) {
            if (classes[i].days[day] == '1') {
                classDays.push_back(i);
            }
        }

        sort(classDays.begin(), classDays.end(), [&](int a, int b) {
            return classes[a].startSlot < classes[b].startSlot;
        });



        for (size_t t = 0; t + 1 < classDays.size(); t++) {
            const Room &r1 = rooms[ind.chromosome[classDays[t]]];
            const Room &r2 = rooms[ind.chromosome[classDays[t + 1]]];
            softViolations += classes[classDays[t]].enrollment * sqrt(((r2.x - r1.x) * (r2.x - r1.x)) + ((r2.y - r1.y) * (r2.y - r1.y)));
        }
    }

    double ws = 1.0;
    double wh = pow(10, 5);

    ind.fitness = 1.0 / (1.0 + (wh * hardViolations) + (softViolations * ws));
}

//GA
Individual geneticAlgo(int populationSize, int tournamentSize, double crossoverRate, double mutationRate,
                       int maximumGeneration, double probability) {

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
    cout << endl;

    calculateFitness(population[0], classes, rooms);

    auto initialFitness = population[0].fitness;
    cout << "Initial Random Schedule Fitness: " << initialFitness << endl;

    vector<Individual> newPopulation;
    vector<Individual> topFive;

    const int tournamentProbability = static_cast<int>(probability * 1000);
    const int mutationRateScaled =static_cast<int>(mutationRate * 1000);

    for (int i = 0; i < maximumGeneration; i++) {
        newPopulation.clear();

        //elitism
        //sorting population so we can find the best fitness
        sort(population.begin(), population.end(), [](const Individual &a, const Individual &b) {
            return a.fitness > b.fitness;
        });

        for (int j=0; j<5; j++) {
            newPopulation.push_back(population[j]);
        }

        while (newPopulation.size() < populationSize) {
            Individual parent1 = tournamentSelection(population, tournamentSize, tournamentProbability);
            Individual parent2 = tournamentSelection(population, tournamentSize, tournamentProbability);

            singlePointCrossOver(parent1, parent2, parent1.chromosome.size(), crossoverRate);

            randomResettingMutation(parent1.chromosome, mutationRateScaled, rooms.size());
            randomResettingMutation(parent2.chromosome, mutationRateScaled, rooms.size());

            newPopulation.push_back(parent1);

            //so that population size isnt exceeded
            if (newPopulation.size() < populationSize) {
                newPopulation.push_back(parent2);
            }
        }

        population = newPopulation;

        for (Individual &individual: population) {
            calculateFitness(individual, classes, rooms);
        }

        if (i % 100 == 0) {
            Individual &topFitness = population[0];
            for (Individual &ind: population)
                if (ind.fitness > topFitness.fitness) topFitness = ind;
            cout << "Generation " << i << ", fitness: " << topFitness.fitness <<", violations: "<< capacity+overlap <<endl;
        }
    }

    Individual bestChromosome = population[0];
    for (const Individual &individual: population) {
        if (individual.fitness > bestChromosome.fitness) {
            bestChromosome = individual;
        }
    }

    calculateFitness(bestChromosome, classes, rooms);
    cout << "Capacity violations: " << capacity << endl;
    cout << "Overlap violations:  " << overlap << endl;
    cout << "Hard violations: " << capacity+overlap << endl;

    cout<<"Difference between best and initial:"<< abs(bestChromosome.fitness - initialFitness)<<endl;

    return bestChromosome;
}

Individual tournamentSelection(vector<Individual> &population, const int tournamentSize,
                               const int tournamentProbability) {
    const size_t populationSize = population.size();

    vector<Individual *> tournamentSet(tournamentSize);

    //selecting randoms for tournament set
    for (int i = 0; i < tournamentSize; i++) {
        const int randomIndividual = rand() % static_cast<int>(populationSize);

        tournamentSet[i] = &population[randomIndividual];
    }

    //sorting the tournament set, lamba allows custom sort
    sort(tournamentSet.begin(), tournamentSet.end(), [](Individual *a, Individual *b) {
        return a->fitness > b->fitness;
    });

    const int randomNumber = rand() % 1000;

    if (randomNumber < tournamentProbability) {
        return *tournamentSet[0];
    } else {
        const int randomIndex = (rand() % (tournamentSize));
        return *tournamentSet[randomIndex];
    }
}


void singlePointCrossOver(Individual &parent1, Individual &parent2, const size_t length, double crossoverRate) {
    const int crossOverPoint = rand() % static_cast<int>(length);

    if (static_cast<double>(rand() % 1000) / 1000.0 <= crossoverRate) {
        for (int i = crossOverPoint; i < length; i++) {
            swap(parent1.chromosome[i], parent2.chromosome[i]);
        }
    }
}

void randomResettingMutation(vector<int> &chromosome, const double mutationProbability, const size_t roomsSize) {
    for (int i = 0; i < chromosome.size(); i++) {
        const double randomNumber = rand() % 1000;

        if (randomNumber < mutationProbability) {
            const int randomValue = rand() % static_cast<int>(roomsSize);
            chromosome[i] = randomValue;
        }
    }
}
