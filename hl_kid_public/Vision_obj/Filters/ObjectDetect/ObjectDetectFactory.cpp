#include "ObjectDetectFactory.hpp"

//#include "BallRadiusProvider.hpp"
//#include "Recorder.hpp"
//#include "ColorDensity.hpp"
//#include "ROIBasedGTP.hpp"
//#include "WhiteLines.hpp"
#include "ByDNN.hpp"
#include "../FilterFactory.hpp"

namespace Vision_obj
{
namespace Filters
{
void registerObjectDetectFilters(FilterFactory* ff)
{
  //ff->registerBuilder("BallRadiusProvider", []() { return std::unique_ptr<Filter>(new BallRadiusProvider); });
  //ff->registerBuilder("Recorder", []() { return std::unique_ptr<Filter>(new Recorder()); });
  //ff->registerBuilder("ColorDensity", []() { return std::unique_ptr<Filter>(new ColorDensity()); });
  //ff->registerBuilder("ROIBasedGTP", []() { return std::unique_ptr<Filter>(new ROIBasedGTP()); });
  //ff->registerBuilder("WhiteLines", []() { return std::unique_ptr<Filter>(new WhiteLines()); });
  ff->registerBuilder("ByDNN", []() { return std::unique_ptr<Filter>(new ByDNN()); });
}
}  // namespace Filters
}  // namespace Vision