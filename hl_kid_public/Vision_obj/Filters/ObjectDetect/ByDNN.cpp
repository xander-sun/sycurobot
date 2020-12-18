#include "Filters/ObjectDetect/ByDNN.hpp"
//#include "detect.h"
#include "CameraState/CameraState.hpp"
//#include "Filters/Patches/PatchProvider.hpp"
//#include "Utils/RotatedRectUtils.hpp"
#include "Utils/Interface.h"
#include "Utils/OpencvUtils.h"
#include "Utils/ROITools.hpp"
#include "rhoban_utils/timing/benchmark.h"

#include "rhoban_geometry/circle.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <utility>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <rhoban_utils/logging/logger.h>

#include <ctime>
#include <jsoncpp/json/json.h>
#include <curl/curl.h>
#include "../../Utils/base64.h"


//#include <hl_monitoring/field.h>


static rhoban_utils::Logger logger("ByDNN");

using namespace std;
using namespace rhoban_geometry;
using ::rhoban_utils::Benchmark;


// call back function in curl post
size_t OnWriteData(void* buffer, size_t size, size_t nmemb, void* lpVoid)
{
    std::string* str = dynamic_cast<std::string*>((std::string *)lpVoid);
    if( NULL == str || NULL == buffer )
    {
        return -1;
    }
 
    char* pData = (char*)buffer;
    str->append(pData, size * nmemb);
    return nmemb;
}

//Detector _detector;

namespace Vision_obj
{
namespace Filters
{


/*  
std::map<std::string, hl_monitoring::Field::POIType> EverythingByDNN::stringToPOIEnum = {
  { "ArenaCorner", hl_monitoring::Field::POIType::ArenaCorner },
  { "LineCorner", hl_monitoring::Field::POIType::LineCorner },
  { "T", hl_monitoring::Field::POIType::T },
  { "X", hl_monitoring::Field::POIType::X },
  { "Center", hl_monitoring::Field::POIType::Center },
  { "PenaltyMark", hl_monitoring::Field::POIType::PenaltyMark },
  { "PostBase", hl_monitoring::Field::POIType::PostBase }
};
*/

ByDNN::ByDNN() : BallProvider("ByDNN")
{
  // TODO load classes from json config file

  // WARNING order is important (alphabetical order)
  //   if classes are removed, just comment the corresponding line, make sure to keep alphabetical order
  classNames.push_back("Empty");
  classNames.push_back("Ball");
  classNames.push_back("Goal");
  classNames.push_back("Robot");
  classNames.push_back("X");
  //classNames.push_back("Robot");
}

void ByDNN::setParameters()
{
  debugLevel = ParamInt(0, 0, 1);
  scoreThreshold = ParamFloat(0.5, 0.0, 1.0);
  thresholdIOU = ParamFloat(0.0, 0.0, 1.0); 
  imSize = ParamInt(32, 1, 64);
  input_width = ParamInt(299, 32, 512);
  input_height = ParamInt(299, 32, 512);
  input_mean_R = ParamFloat(0.406, 0.0, 1.0);
  input_mean_G = ParamFloat(0.456, 0.0, 1.0);
  input_mean_B = ParamFloat(0.485, 0.0, 1.0);
  input_std_R = ParamFloat(0.225, 0.0, 1.0);
  input_std_G = ParamFloat(0.224, 0.0, 1.0);
  input_std_B = ParamFloat(0.299, 0.0, 1.0);

  params()->define<ParamInt>("debugLevel", &debugLevel);
  params()->define<ParamFloat>("scoreThreshold", &scoreThreshold);
  params()->define<ParamFloat>("thresholdIOU", &thresholdIOU);
  params()->define<ParamInt>("imSize", &imSize);
  params()->define<ParamInt>("input_width", &input_width);
  params()->define<ParamInt>("input_height", &input_height);
  params()->define<ParamFloat>("input_mean_R", &input_mean_R);
  params()->define<ParamFloat>("input_mean_G", &input_mean_G);
  params()->define<ParamFloat>("input_mean_B", &input_mean_B);
  params()->define<ParamFloat>("input_std_R", &input_std_R);
  params()->define<ParamFloat>("input_std_G", &input_std_G);
  params()->define<ParamFloat>("input_std_B", &input_std_B);
  for (const std::string& className : classNames)
  {
    isUsingFeature[className] = ParamInt(0, 0, 1);
    params()->define<ParamInt>("uses" + className, &(isUsingFeature[className]));
  }
}

void ByDNN::updateUsedClasses()
{
  usedClassNames.clear();
  for (size_t idx = 0; idx < classNames.size(); idx++)
  {
    const std::string& className = classNames[idx];
    //if (className == "Empty" || isUsingFeature[className] != 0)
    //{
    usedClassNames.push_back(className);
    //}
  }
}

std::string ByDNN::getClassName() const
{
  return "ByDNN";
}

Json::Value ByDNN::toJson() const
{
  Json::Value v = Filter::toJson();
  v["model_path"] = model_path;
  return v;
}

void ByDNN::fromJson(const Json::Value& v, const std::string& dir_name)
{
  Filter::fromJson(v, dir_name);
  rhoban_utils::tryRead(v, "model_path", &model_path);
  //model_path = "/home/ys/tensorflow_test/test/tensorflow-object-detection-cpp/demo/ssd_mobilenet_v1_egohands/frozen_inference_graph.pb";
  model_path = "/home/ys/tensorflow_test/test/object_detection_tutorial/ssd_mobilenet_v1_coco_2017_11_17/frozen_inference_graph.pb";
  //loadModel(model_path);
  //int loadModelStatus = _detector.loadModel(model_path);
  //  if (loadModelStatus < 0) {
  //      std::ostringstream oss;
  //      oss << "........Failed load DNN model.......: "<<std::endl;
        //LOG(ERROR) << "Load model failed!";
        //return -1;
  //  }
}

int ByDNN::expectedDependencies() const
{
  return 1;
}

void ByDNN::process()
{
  clearBallsData();
  updateUsedClasses();

  std::cout << (*getDependency(_dependencies[0]).getImg()).size() <<endl;

  //const std::vector<cv::Mat> image = getDependency(_dependencies[0]).getImg()->clone();
  cv::Mat image = getDependency(_dependencies[0]).getImg()->clone();
  std::cout << "ByDNN...load image:"<< image.size()<< endl;

  Benchmark::open("Cloning src"); 
  //cv::Mat image_new = cv::Mat(image, true);
  cv::Mat image_new = image;
  //cv::cvtColor(image_new, image_new, cv::COLOR_BGR2RGB);
  cv::cvtColor(image_new, image_new, CV_YCrCb2BGR);
  cv::cvtColor(image_new, image_new, cv::COLOR_BGR2RGB);
  Benchmark::close("Cloning src");

  //std::cout << "ByDNN...load image"<< endl;
  if (debugLevel > 0) {
      std::cout << "Filter: " << name << std::endl;
    }
    
  try 
  {
    //std::vector<vector<float> > outBoxes;
    //std::vector<float> outScores;
    //std::vector<size_t> outLabels;
    //int detectStatus = _detector.detect(image_new, scoreThreshold, outBoxes, outScores, outLabels);

    std::string url = "http://localhost:8181/test";
    clock_t t1, t2;
    t1 = clock();
    std::vector<unsigned char> data_encode;
    int res = imencode(".jpg", image_new, data_encode);
    std::string str_encode(data_encode.begin(), data_encode.end());
    const char* c = str_encode.c_str();
    unsigned char* cc = (unsigned char*)c;
    Json::Value j;
    j["detect_img"] = Json::Value(base64_encode(cc, str_encode.size()));
    std::string jsonout = j.toStyledString();
    //j["detect_img"] = Json::Value(base64_encode(str_encode, str_encode.size()));
    //Json::FastWriter writer;  
    //std::string strWrite = writer.write(j);
    //curl_global_init(CURL_GLOBAL_ALL);

    //std::cout << "ByDNN..image.size().:"<< image_new.rows<<image_new.cols<<image_new.channels()<< endl;
    string favor;
    CURL *curl;
    curl = curl_easy_init();
    //curl_slist *http_headers = NULL;
    //http_headers = curl_slist_append(http_headers, "Accept: application/json");
    //http_headers = curl_slist_append(http_headers, "Content-Type: application/json");//text/html
    //http_headers = curl_slist_append(http_headers, "charsets: utf-8");
    //curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    //curl_easy_setopt(curl, CURLOPT_HTTPHEADER, http_headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonout.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &favor);
    CURLcode resp = curl_easy_perform(curl);
    int detectStatus;
    vector<double> box;
    vector<vector<double> >outBoxes;
    vector<int> outLabels;
    vector<double>outScores;
    
    outBoxes.clear();
    outLabels.clear();
    outScores.clear();

    Json::Reader reader;
    Json::Value value;

    if (reader.parse(favor, value))            // json字符串转为json对象
    {   
			detectStatus = value["flag"].asInt();
			for (int i =0; i < value["boxes"].size(); i++)
			{ 
        box.clear();
        for (int j = 0; j < 4; j++)
        {
          box.push_back(value["boxes"][i][j].asDouble()); 
        }
			    outBoxes.push_back(box);
			}
      for (int i = 0; i < value["labels"].size(); i++)
      {
          outLabels.push_back(value["labels"][i].asInt());
      }
      for (int i = 0; i < value["scores"].size(); i++)
      {
          outScores.push_back(value["scores"][i].asDouble());
      }
    }  
            //int     age = value["array"][i]["age"].asInt();
            //string sex  = value["array"][i]["sex"].asString();
  
    t2 = clock();
    std::cout << "ByDNN...predict time cost----" << (double)(t2 - t1)/CLOCKS_PER_SEC << endl;

    std::ostringstream oss;
    oss << ".......the run time is :  "<< (double)(t2 - t1)/CLOCKS_PER_SEC << "s" <<std::endl;


    if (detectStatus == 0) {
      //std::ostringstream oss;
      oss << "........Failed load DNN model.......: "<<std::endl; 
    }
    else {
      int boxes_num = outLabels.size();
      std::cout << boxes_num << endl;
      for(size_t class_id = 0; class_id < boxes_num && class_id < 20; class_id++)
      {
         // over threshold 
          bool isValid = outScores[class_id] >= scoreThreshold;
          std::cout << "isValid" << isValid <<"scores"<< outScores[class_id] <<"threshold" <<scoreThreshold << isValid<<endl;
         // 
          std::string s_class;
          //s_class = std::to_string(outLabels[class_id]);
          s_class = usedClassNames.at(outLabels[class_id]);
          std::cout << "label is:" << outLabels[class_id];

          double l_x = outBoxes[class_id][0];
          double l_y = outBoxes[class_id][1];
          double r_x = outBoxes[class_id][2];
          double r_y = outBoxes[class_id][3];

          cv::Rect box(l_y, l_x, r_y - l_y, r_x - l_x);
        
        // get center, radius of ball from box
          double  center_x, center_y, ball_radius;
          center_x = (l_x + r_x) / 2.0;
          center_y = (l_x + r_y) / 2.0;;
          ball_radius = box.width / 2.0;  // inner circle in rectangele 
          std::cout << Circle(center_x, center_y, ball_radius)<<endl;


          //if (s_class != "Empty")
          if (isValid)
          {                         // not Empty
              //pushBall(box);
              if (s_class == "Ball")  // Ball
                 //pushBall(Circle(center_x, center_y, ball_radius), image_new);
                pushBall(box); 
              else if (s_class == "Robot")
                pushRobot(box);
              else if (s_class == "Goal")
                pushGoal(box);
              else if (s_class == "X")
                pushX(box);
              //else if (stringToPOIEnum.count(s_class) > 0)
             //    pushPOI(stringToPOIEnum.at(s_class), box);
              else
                logger.error("Unknown label: '%s'", s_class.c_str());
        }
    
        if (s_class == "Empty")  // Empty
        {
        //drawRotatedRectangle(output, roi, cv::Scalar(0, 0, 255), 2);
        }
        else
        {
           double font_scale = 1.2;
        if (isValid)
        {
          cv::rectangle(image_new, box, cv::Scalar(0, 255, 0), 2);
          cv::putText(image_new, s_class, box.tl(), cv::FONT_HERSHEY_COMPLEX_SMALL,
                      font_scale, cv::Scalar(0, 255, 0), 1.5, CV_AA);
        }
        else
        {
          cv::rectangle(image_new, box, cv::Scalar(0, 0, 255), 2);
          cv::putText(image_new, s_class, box.tl(), cv::FONT_HERSHEY_COMPLEX_SMALL,
                      font_scale, cv::Scalar(0, 0, 255), 1.5, CV_AA);
        }
      }
  }
      }
  }
  catch (const std::bad_alloc& exc)
  {
    std::ostringstream oss;
    oss << "Pipeline structure is invalid: "
        << "filter " << name << " dependency is not a PatchProvider" << std::endl;
    throw std::runtime_error(oss.str());
  }
  img() = image_new;

} 

/*
  try
  {
    const PatchProvider& dep = dynamic_cast<const PatchProvider&>(getDependency());
    output = dep.getImg()->clone();
    //const std::vector<cv::Mat>& patches = dep.getPatches();
    //const std::vector<std::pair<float, cv::RotatedRect>>& rois = dep.getRois();

    //if (rois.size() != patches.size())
    //  throw std::runtime_error("EverythingByDNN:: number of rois does not match number of patches");

    std::pair<int, double> res = getClass(output)
    
    for (size_t patch_id = 0; patch_id < rois.size(); patch_id++)
    {
      const cv::Mat& patch = patches[patch_id];
      const cv::RotatedRect& roi = rois[patch_id].second;

      std::pair<int, double> res = getClass(patch);

      bool isValid = res.second >= scoreThreshold;

      std::string s_class;
      s_class = usedClassNames.at(res.first);

      cv::Point2f roi_center(roi.center.x, roi.center.y);
      if (s_class != "Empty")
      {                         // not Empty
        if (s_class == "Ball")  // Ball
          pushBall(roi_center);
        else if (s_class == "Robot")
          pushRobot(roi_center);
        else if (stringToPOIEnum.count(s_class) > 0)
          pushPOI(stringToPOIEnum.at(s_class), cv::Point2f(roi.center.x, roi.center.y));
        else
          logger.error("Unknown label: '%s'", s_class.c_str());
      }

      if (s_class == "Empty")  // Empty
      {
        drawRotatedRectangle(output, roi, cv::Scalar(0, 0, 255), 2);
      }
      else
      {
        double font_scale = 1.2;
        if (isValid)
        {
          drawRotatedRectangle(output, roi, cv::Scalar(0, 255, 0), 2);
          cv::putText(output, s_class, cv::Point(roi.center.x, roi.center.y), cv::FONT_HERSHEY_COMPLEX_SMALL,
                      font_scale, cv::Scalar(0, 255, 0), 1.5, CV_AA);
        }
        else
        {
          drawRotatedRectangle(output, roi, cv::Scalar(0, 0, 255), 2);
          cv::putText(output, s_class, cv::Point(roi.center.x, roi.center.y), cv::FONT_HERSHEY_COMPLEX_SMALL,
                      font_scale, cv::Scalar(0, 0, 255), 1.5, CV_AA);
        }
      }
    }
  }
  catch (const std::bad_alloc& exc)
  {
    std::ostringstream oss;
    oss << "Pipeline structure is invalid: "
        << "filter " << name << " dependency is not a PatchProvider" << std::endl;
    throw std::runtime_error(oss.str());
  }
  img() = output;
}
*/
}  // namespace Filters
}  // namespace Vision