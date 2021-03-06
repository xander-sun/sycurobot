#include "FilterFactory.hpp"

//#include "Ball/BallFactory.hpp"
//#include "Basics/BasicsFactory.hpp"
//#include "Colors/ColorsFactory.hpp"
#include "Custom/CustomFactory.hpp"
#include "Features/FeaturesFactory.hpp"
//#include "Goal/GoalFactory.hpp"
//#include "Obstacles/ObstacleFactory.hpp"
//#include "Patches/PatchFactory.hpp"
#include "Source/SourceFactory.hpp"
//#include "Templates/TemplateFactory.hpp"
//#include "RobotDetection/RobotDetectionFactory.hpp"

#include "ObjectDetect/ObjectDetectFactory.hpp"

#include <exception>
#include <string>
#include <vector>

namespace Vision_obj {
namespace Filters {

FilterFactory::FilterFactory() {
  
  //registerBallFilters(this);
  //registerBasicsFilters(this);
  //registerColorsFilters(this);
  registerCustomFilters(this);
  //registerFeaturesFilters(this);
  //registerGoalFilters(this);
  //registerObstacleFilters(this);
  //registerPatchFilters(this);
  
  registerSourceFilters(this);
  //registerTemplateFilters(this);
  //registerRobotDetectionFilters(this);

  registerObjectDetectFilters(this);
}

}
}
