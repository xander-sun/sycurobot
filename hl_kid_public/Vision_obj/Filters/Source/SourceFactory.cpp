#include "SourceFactory.hpp"

#include "SourceLogs.hpp"
#include "SourcePtGrey.hpp"

#include "../FilterFactory.hpp"

namespace Vision_obj {
namespace Filters {

void registerSourceFilters(FilterFactory* ff) 
{
  ff->registerBuilder("SourceLogs"  , [](){return std::unique_ptr<Filter>(new SourceLogs  );});
  ff->registerBuilder("SourcePtGrey", [](){return std::unique_ptr<Filter>(new SourcePtGrey);});
}

}
}
