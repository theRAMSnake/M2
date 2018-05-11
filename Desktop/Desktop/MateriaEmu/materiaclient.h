#ifndef MATERIACLIENT_H
#define MATERIACLIENT_H

#include <string>
#include <vector>

namespace materia
{

struct Id
{
    std::string guid;
};

struct Affinity
{
   Id id;
   std::string name;
   Id iconId;
   std::string colorName;

   bool operator == (const Affinity& other) const;
   bool operator != (const Affinity& other) const;
};

struct StrategyItem
{
   Id id;
   Id parentGoalId;

   std::string name;
   std::string notes;
   Id iconId;
};

struct Goal : public StrategyItem
{
   bool focused;
   bool achieved;
   Id affinityId;

   bool operator == (const Goal& other) const;
   bool operator != (const Goal& other) const;
};

class IStrategy
{
public:
    virtual void configureAffinities(const std::vector<Affinity>& affinites) = 0;
    virtual std::vector<Affinity> getAffinities() = 0;
    virtual std::vector<Goal> getGoals() = 0;

    virtual ~IStrategy() {}
};

class MateriaClient
{
public:
    MateriaClient(const std::string& clientName, const std::string& ip);

    IStrategy& getStrategy();
};

}

#endif // MATERIACLIENT_H
