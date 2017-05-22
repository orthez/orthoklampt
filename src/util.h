#pragma once

#include <ctime>
#include <iostream>
#include <iomanip>
#include <Simulation/ODERobot.h>
#include <Simulation/WorldSimulation.h>

namespace util {
  inline void SetSimulatedRobot( Robot *robot, WorldSimulation &sim, Config &q)
  {
    robot->UpdateConfig(q);
    int robotidx = 0;
    ODERobot *simrobot = sim.odesim.robot(robotidx);
    simrobot->SetConfig(q);
  }

  inline std::string GetCurrentTimeString()
  {
    time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );
    ostringstream os;

    int yyyy = now->tm_year + 1900;
    int mm = now->tm_mon + 1;
    int dd = now->tm_mday;

    os << yyyy << "_" << setfill('0') << setw(2) << mm << "_" << setfill('0') << setw(2) << dd;
    return os.str();
  }
  inline void PrintCurrentTime()
  {
    time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );

    printf ( "The current date/time is: %s\n", asctime (now) );

  }
  inline std::string GetApplicationFolder()
  {
    std::string name = getenv("USER");

    return "/home/"+name+"/git/orthoklampt/";
  }
  inline std::string GetDataFolder()
  {
    std::string name = getenv("USER");

    return "/home/"+name+"/git/orthoklampt/data";
  }
}// namespace util

