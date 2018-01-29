#include "multiquotient.h"
#include <ompl/util/Time.h>
#include <queue>

using namespace og;

template <class T, class Tlast>
MultiQuotient<T,Tlast>::MultiQuotient(std::vector<ob::SpaceInformationPtr> &si_vec_, std::string type):
  ob::Planner(si_vec_.back(),"QMP"+type), si_vec(si_vec_)
{
  T::resetCounter();
  for(uint k = 0; k < si_vec.size(); k++){
    og::Quotient* previous = nullptr;
    if(k>0) previous = quotientSpaces.back();

    if(k>0 && k>=si_vec.size()-1){
      Tlast* ss = new Tlast(si_vec.at(k), previous);
      quotientSpaces.push_back(ss);
    }else{
      T* ss = new T(si_vec.at(k), previous);
      quotientSpaces.push_back(ss);
    }

  }

  std::cout << "Created hierarchy with " << si_vec.size() << " levels." << std::endl;
}

template <class T, class Tlast>
MultiQuotient<T,Tlast>::~MultiQuotient(){
}

template <class T, class Tlast>
void MultiQuotient<T,Tlast>::setup(){

  Planner::setup();
  for(uint k = 0; k < quotientSpaces.size(); k++){
    quotientSpaces.at(k)->setup();
  }
}

template <class T, class Tlast>
void MultiQuotient<T,Tlast>::clear(){
  Planner::clear();
  solutions.clear();
  for(uint k = 0; k < quotientSpaces.size(); k++){
    quotientSpaces.at(k)->clear();
  }
  foundKLevelSolution = false;
}

template <class T, class Tlast>
ob::PlannerStatus MultiQuotient<T,Tlast>::solve(const base::PlannerTerminationCondition &ptc){
  
  static const double T_GROW = 0.001; //time to grow before Checking if solution exists

  auto cmp = [](og::Quotient* left, og::Quotient* right) 
              { 
                return left->GetSamplingDensity() > right->GetSamplingDensity();
              };

  std::priority_queue<og::Quotient*, std::vector<og::Quotient*>, decltype(cmp)> Q(cmp);

  for(uint k = 0; k < quotientSpaces.size(); k++){
    base::PathPtr sol_k;
    foundKLevelSolution = false;

    quotientSpaces.at(k)->Init();
    Q.push(quotientSpaces.at(k));

    base::PlannerTerminationCondition ptcOrSolutionFound([this, &ptc]
                                   { return ptc || foundKLevelSolution; });

    ompl::time::point t_k_start = ompl::time::now();
    while (!ptcOrSolutionFound())
    {
      og::Quotient* jQuotient = Q.top();
      Q.pop();
      jQuotient->Grow(T_GROW);

      quotientSpaces.at(k)->CheckForSolution(sol_k);

      if(quotientSpaces.at(k)->HasSolution()){
        solutions.push_back(sol_k);
        double t_k_end = ompl::time::seconds(ompl::time::now() - t_k_start);
        std::cout << "Found Solution on Level " << k << " after " << t_k_end << " seconds." << std::endl;
        for(uint i = 0; i <= k; i++){
          og::Quotient *Qi = quotientSpaces.at(i);
          std::cout << ">> level " << i << " vertices " << Qi->GetNumberOfVertices() << " edges " << Qi->GetNumberOfEdges() << std::endl;
        }
        foundKLevelSolution = true;
      }
      Q.push(jQuotient);
    }

    if(!foundKLevelSolution){
      std::cout << "could not find a solution on level " << k << std::endl;
      std::cout << "aborting" << std::endl;
      for(uint i = 0; i <= k; i++){
        og::Quotient *Qi = quotientSpaces.at(i);
        std::cout << ">> level " << i << " vertices " << Qi->GetNumberOfVertices() << " edges " << Qi->GetNumberOfEdges() << std::endl;
      }
      return ob::PlannerStatus::TIMEOUT;
    }
  }
  std::cout << "Found exact solution" << std::endl;
  for(uint k = 0; k < quotientSpaces.size(); k++){
    og::Quotient *Qi = quotientSpaces.at(k);
    std::cout << ">> level " << k << " vertices " << Qi->GetNumberOfVertices() << " edges " << Qi->GetNumberOfEdges() << std::endl;
  }

  base::PathPtr sol;
  quotientSpaces.back()->CheckForSolution(sol);
  if (sol)
  {
    base::PlannerSolution psol(sol);
    psol.setPlannerName(getName());
    pdef_->addSolutionPath(psol);
  }

  return ob::PlannerStatus::EXACT_SOLUTION;
}


template <class T, class Tlast>
void MultiQuotient<T,Tlast>::setProblemDefinition(std::vector<ob::ProblemDefinitionPtr> &pdef_){
  pdef_vec = pdef_;
  ob::Planner::setProblemDefinition(pdef_vec.back());
  for(uint k = 0; k < pdef_vec.size(); k++){
    quotientSpaces.at(k)->setProblemDefinition(pdef_vec.at(k));
  }
}

template <class T, class Tlast>
void MultiQuotient<T,Tlast>::setProblemDefinition(const ob::ProblemDefinitionPtr &pdef){

  //ob::ProblemDefinitionPtr pp = pdef.back();
  this->Planner::setProblemDefinition(pdef);
}

template <class T, class Tlast>
void MultiQuotient<T,Tlast>::getPlannerData(ob::PlannerData &data) const{
  quotientSpaces.back()->getPlannerData(data);
}
