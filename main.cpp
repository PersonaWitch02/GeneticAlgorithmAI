
#include "GeneticAlgorithm.h"

#include <vector>
#include <iostream>

#include <chrono>

int main() {

    const auto startTime = std::chrono::steady_clock::now();


    int populationSize = 100;
    int tournamentSize = 5;
    double crossoverRate = 0.95;
    double mutationRate = 0.001;
    int maximumGeneration = 5000;
    double probability = 0.95;

    using std::cout, std::endl;

    cout<< "tournament size: "<<tournamentSize<<endl;
    cout<< "crossover rate: "<<crossoverRate<<endl;
    cout<< "mutation rate: "<<mutationRate<<endl;
    cout<< "maximum generation: "<<maximumGeneration<<endl;
    cout<< "probability: "<<probability<<endl;

    Individual best = geneticAlgo(populationSize, tournamentSize, crossoverRate, mutationRate, maximumGeneration, probability);

    cout<<"Best: "<< best.fitness<<endl;

    // for (int i=0; i<best.chromosome.size(); i++) {
    //     cout<<best.chromosome[i]<<", ";
    // }
    // cout<<endl;

    const auto endtime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - startTime);
    cout<<"Time elapsed: "<<endtime.count()<< " seconds"<<endl;
    return 0;
}