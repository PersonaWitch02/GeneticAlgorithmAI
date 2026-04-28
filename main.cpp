
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
    constexpr int maximumGeneration = 5000;
    constexpr double probability = 0.95;
    

    cout<< "tournament size: "<<tournamentSize<<endl;
    cout<< "crossover rate: "<<crossoverRate<<endl;
    cout<< "mutation rate: "<<mutationRate<<endl;
    cout<< "maximum generation: "<<maximumGeneration<<endl;
    cout<< "probability: "<<probability<<endl;

    Individual best = geneticAlgo(populationSize, tournamentSize, crossoverRate, mutationRate, maximumGeneration, probability);

    cout<<"Best: "<< best.fitness<<endl;

    // for (int i=0; i<best.chromosome.size(); i++) {
    //     cout<<best.chromosome[i]<<"  ";
    // }
    // cout<<endl;

    randomBaselineGeneration(populationSize);

    const auto endtime = chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - startTime);
    cout<<"Time elapsed: "<<endtime.count()<< " seconds"<<endl;
    return 0;
}