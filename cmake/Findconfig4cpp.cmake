find_library(CONFIG4CPP_LIBRARY config4cpp
  PATH ${PROJECT_SOURCE_DIR}/libraries/config4cpp/lib
)

find_path(CONFIG4CPP_INCLUDE_DIR "config4cpp/Configuration.h"
  PATH ${PROJECT_SOURCE_DIR}/libraries/config4cpp/include
)