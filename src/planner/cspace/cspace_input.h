#pragma once
#include <KrisLibrary/robotics/RobotKinematics3D.h> //Config

struct CSpaceInput{
  double timestep_min;
  double timestep_max;
  Config uMin;
  Config uMax;
  Config dqMin;
  Config dqMax;
  bool fixedBase{false};
  bool kinodynamic{false};
  bool multiAgent{false};
};
