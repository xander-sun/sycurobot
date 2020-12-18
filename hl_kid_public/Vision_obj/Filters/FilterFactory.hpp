#pragma once

#include "rhoban_utils/serialization/factory.h"

#include "Filters/Filter.hpp"

namespace Vision_obj {
namespace Filters {
class FilterFactory : public rhoban_utils::Factory<Filter> {
public:
  FilterFactory();
};
}
}
