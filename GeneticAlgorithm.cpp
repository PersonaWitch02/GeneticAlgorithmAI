#include "GeneticAlgorithm.h"
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <vector>
#include <iostream>

#include <chrono>

#include "CSVParser.h"

using namespace std;

//for keeping time
std::chrono::milliseconds timeFitness(0);
std::chrono::milliseconds timeGA(0);

//globals for tracking stats
long long iterations {0};
int overlap = 0;
int capacity = 0;
int totalViolations = 0;
int totalSoftViolations = 0;

//This function compares 2 classes to see if the days and slots overlap
bool isOverlapping(const Class &a, const Class &b) {
    bool dayOverlap = false;

    //check over days of the week
    for (int k = 0; k < 7; k++) {
        if (a.days[k] == '1' && b.days[k] == '1') {
            dayOverlap = true;
            break;
        }
    }
    if (!dayOverlap) return false;

    //check for time slot overlap
    int endA = a.startSlot + a.length;
    int endB = b.startSlot + b.length;
    return (max(a.startSlot, b.startSlot) < min(endA, endB));
}

//randomly assign classes to rooms as a baseline
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

//randome baseline
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
    cout<< "Soft violations: " << totalSoftViolations << endl;
    cout << "Fitness: "<<population[random].fitness <<endl;
    cout<<"**************************** \n";
}

//attempted heuristic initialization
//Sortes vectors of classes and rooms by enrollment and capacity
//for first individual try to match largest classes to largest rooms
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
            //i == 0 checks for first individual
            if (i == 0) {
                ind.chromosome[classOrder[j]] = roomOrder[j % rooms.size()];
            } else {
                //all others are randomly filled
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

        //checking and storing classes that a on day i
        for (size_t i = 0; i < classes.size(); i++) {
            if (classes[i].days[day] == '1') {
                classDays.push_back(i);
            }
        }

        //sorting by start slot in ascending order
        sort(classDays.begin(), classDays.end(), [&](int a, int b) {
            return classes[a].startSlot < classes[b].startSlot;
        });

        //calculating sc using the formula given
        for (size_t t = 0; t + 1 < classDays.size(); t++) {
            const Room &r1 = rooms[ind.chromosome[classDays[t]]];
            const Room &r2 = rooms[ind.chromosome[classDays[t + 1]]];
            softViolations += classes[classDays[t]].enrollment * sqrt(((r2.x - r1.x) * (r2.x - r1.x)) + ((r2.y - r1.y) * (r2.y - r1.y)));
            totalSoftViolations = softViolations;
        }
    }

    //fitness constraint weights
    double ws = 1.0; //soft
    double wh = pow(10, 5); //hard

    ind.fitness = 1.0 / (1.0 + (wh * hardViolations) + (softViolations * ws));
}

//GA
Individual geneticAlgo(int populationSize, int tournamentSize, double crossoverRate, double mutationRate,
                       int maximumGeneration, double tournamentRate) {

    vector<Class> classes = loadClasses("classes_demand.csv");
    vector<Room> rooms = loadRooms("rooms_pool.csv");

    vector<Individual> population;
    initializePopulation(population, populationSize, classes, rooms);


    cout << "Population initialized successfully!" << endl;
    cout << "Number of individuals: " << population.size() << endl;

    calculateFitness(population[0], classes, rooms);

    auto initialFitness = population[0].fitness;
    cout << "Initial Random Schedule Fitness: " << initialFitness << endl;

    vector<Individual> newPopulation;
    vector<Individual> topFive;

    const int tournamentProbability = static_cast<int>(tournamentRate * 1000);
    const int mutationRateScaled =static_cast<int>(mutationRate * 1000);

    for (int i = 0; i < maximumGeneration; i++) {
        newPopulation.clear();

        auto startGA = chrono::steady_clock::now();

        //elitism
        //sorting population so we can find the best fitness
        sort(population.begin(), population.end(), [](const Individual &a, const Individual &b) {
            return a.fitness > b.fitness;
        });

        //put the elite genes into the new populaiton
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
        timeGA += chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - startGA);

        auto startFit = chrono::steady_clock::now();
        //calculate the fitness of every individual
        for (Individual &individual: population) {
            calculateFitness(individual, classes, rooms);
        }
        timeFitness += chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - startFit);

        auto startGA2 = chrono::steady_clock::now();
        //print once every hundred generations
        if (i % 100 == 0) {
            Individual &topFitness = population[0];
            for (Individual &ind: population)
                if (ind.fitness > topFitness.fitness) topFitness = ind;
            cout << "Generation " << i << ", fitness: " << topFitness.fitness <<", violations: "<< capacity+overlap <<endl;
        }
        timeGA += chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - startGA2);
    }

    auto startGA = chrono::steady_clock::now();

    //find the best chromosome
    Individual bestChromosome = population[0];
    for (const Individual &individual: population) {
        if (individual.fitness > bestChromosome.fitness) {
            bestChromosome = individual;
        }
    }

    calculateFitness(bestChromosome, classes, rooms);
    timeGA += chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - startGA);

    cout << "Capacity violations: " << capacity << endl;
    cout << "Overlap violations:  " << overlap << endl;
    cout << "Hard violations: " << capacity+overlap << endl;

    cout<<"Difference between best and initial:"<< abs(bestChromosome.fitness - initialFitness)<<endl;

    cout<<"GA took: "<<timeGA.count()/1000.0<<" seconds"<<endl;
    cout<<"Fit took: "<<timeFitness.count()/1000.0<<" seconds"<<endl;

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

    //determine what is returned by tournament
    if (randomNumber < tournamentProbability) {
        return *tournamentSet[0];
    } else {
        const int randomIndex = (rand() % (tournamentSize));
        return *tournamentSet[randomIndex];
    }
}


void singlePointCrossOver(Individual &parent1, Individual &parent2, const size_t length, double crossoverRate) {
    const int crossOverPoint = rand() % static_cast<int>(length);

    //swap each value after crossoverpoint if random number <= crosoverRate
    if (static_cast<double>(rand() % 1000) / 1000.0 <= crossoverRate) {
        for (int i = crossOverPoint; i < length; i++) {
            swap(parent1.chromosome[i], parent2.chromosome[i]);
        }
    }
}

void randomResettingMutation(vector<int> &chromosome, const double mutationProbability, const size_t roomsSize) {
    for (int i = 0; i < chromosome.size(); i++) {
        const double randomNumber = rand() % 1000;

        //replace with a randome value
        if (randomNumber < mutationProbability) {
            const int randomValue = rand() % static_cast<int>(roomsSize);
            chromosome[i] = randomValue;
        }
    }
}
