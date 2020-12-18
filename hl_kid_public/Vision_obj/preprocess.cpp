/*************************************************************************
	> File Name: preprocess.cpp
	> Author: Xander_sun
	> Mail: Xander_sun@163.com 
	> Created Time: 2020年09月09日 星期三 15时41分19秒
 ************************************************************************/

#include<iostream>
using namespace std;

#include <sstream>
#include <stdexcept>
#include <chrono>
#include <string>
#include <exception>
#include <locale>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "Filters/Filter.hpp"
#include "Filters/Pipeline.hpp"
#include <set>
#include <map>
#include "CameraState/CameraState.hpp"
#include "rhoban_utils/timing/benchmark.h"
#include "rhoban_utils/util.h"

#include "Utils/OpencvUtils.h"

#include "RhIO.hpp"

#include "rhoban_utils/logging/logger.h"

using namespace rhoban_geometry;
using namespace rhoban_utils;

static rhoban_utils::Logger out("Filter");

using namespace std;
class FlatBufferModel {
  // Build a model based on a file. Return a nullptr in case of failure.
  static std::unique_ptr<FlatBufferModel> BuildFromFile(
      const char* filename,
      ErrorReporter* error_reporter);

  // Build a model based on a pre-loaded flatbuffer. The caller retains
  // ownership of the buffer and should keep it alive until the returned object
  // is destroyed. Return a nullptr in case of failure.
  static std::unique_ptr<FlatBufferModel> BuildFromBuffer(
      const char* buffer,
      size_t buffer_size,
      ErrorReporter* error_reporter);
};

