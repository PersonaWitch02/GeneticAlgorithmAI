
#include "GeneticAlgorithm.h"
#include <iostream>

#include <chrono>

using namespace  std;

int main() {

    const auto startTime = chrono::steady_clock::now();

    constexpr int populationSize = 100;
    constexpr int tournamentSize = 7;
    constexpr double crossoverRate = 0.8;
    constexpr double mutationRate = 0.001;
    constexpr int maximumGeneration = 50000; //actual maxGen used for report
    // constexpr int maximumGeneration = 6000; // use for demo since its faster
    // constexpr int maximumGeneration = 1000000;
    constexpr double tournamentRate = 0.95;

    randomBaselineGeneration(populationSize);

    cout<< "tournament size: "<<tournamentSize<<endl;
    cout<< "crossover rate: "<<crossoverRate<<endl;
    cout<< "mutation rate: "<<mutationRate<<endl;
    cout<< "maximum generation: "<<maximumGeneration<<endl;
    cout<< "probability: "<<tournamentRate<<endl;

    //return best Individual fom GA
    Individual best = geneticAlgo(populationSize, tournamentSize, crossoverRate, mutationRate, maximumGeneration, tournamentRate);

    cout<<"Best: "<< best.fitness<<endl;

    const auto endtime = chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - startTime);
    cout<<"Time elapsed: "<<endtime.count()<< " seconds"<<endl;
    return 0;
}