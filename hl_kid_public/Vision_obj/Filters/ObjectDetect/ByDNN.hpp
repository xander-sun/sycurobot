#pragma once

#include "Filters/Filter.hpp"
#include "Filters/Features/FeaturesProvider.hpp"
//#include <opencv2/dnn.hpp>

namespace Vision_obj
{
namespace Filters
{
class ByDNN : 
//public Filter,
public BallProvider
{
public:
  ByDNN();

  virtual std::string getClassName() const override;
  virtual int expectedDependencies() const override;
  virtual Json::Value toJson() const override;
  virtual void fromJson(const Json::Value& v, const std::string& dir_name) override;

protected:
  virtual void process() override;
  virtual void setParameters() override;
  void updateUsedClasses();

  //void loadModel(string model_path);

  /// Use the neural network to get the patch class and score
  // std::pair<int, double> getClass(cv::Mat patch);

private:
  ParamInt debugLevel;
  std::map<std::string,ParamInt> isUsingFeature;
  ParamInt imSize;
  ParamFloat scoreThreshold;
  ParamFloat thresholdIOU;
  ParamInt input_width; 
  ParamInt input_height; 
  ParamFloat input_mean_R; 
  ParamFloat input_mean_G; 
  ParamFloat input_mean_B; 
  ParamFloat input_std_R; 
  ParamFloat input_std_G; 
  ParamFloat input_std_B; 
  std::string model_path;
  //cv::Ptr<cv::dnn::Importer> importer;
  //cv::dnn::Net net;
  std::vector<std::string> classNames;
  std::vector<std::string> usedClassNames;

  //static std::map<std::string, hl_monitoring::Field::POIType> stringToPOIEnum;  // TODO find better name ...
};

}  // namespace Filters
}  // namespace Vision