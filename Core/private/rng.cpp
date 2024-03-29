#include "rng.hpp"
#include <boost/random.hpp>
#include <random>

std::random_device rd;
thread_local boost::random::mt19937 rng (rd());
thread_local boost::uniform_real<> doubleGen(-1.0, 1.0);
thread_local boost::uniform_real<> perturbationGen(-0.05, 0.05);
thread_local boost::uniform_real<> realGen(0, 1.0);

bool Rng::genProbability(const double chance)
{
   return realGen(rng) < chance;
}

double Rng::genWeight()
{
   return doubleGen(rng);
}

double Rng::genReal()
{
   return realGen(rng);
}

double Rng::genPerturbation()
{
   return perturbationGen(rng);
}

unsigned int Rng::genChoise(const unsigned int numOptions)
{
   return rng() % numOptions;
}

unsigned int Rng::gen32()
{
    return rng();
}
