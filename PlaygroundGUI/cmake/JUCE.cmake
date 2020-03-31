include(FetchContent)

FetchContent_Declare(JUCE
  GIT_REPOSITORY "https://github.com/WeAreROLI/JUCE.git"
  GIT_TAG "dddeb1ad68f8539f5ea2fbac36c07532f12bf9cf"
  UPDATE_COMMAND ""
  SOURCE_DIR ${automaton_PATH}/JUCE
#  URL https://github.com/WeAreROLI/JUCE/archive/5.4.7.tar.gz
#  URL_HASH SHA256=0f446cf09177e559d2f2e9a77a78faed611cc869e219a7dc859a6e9b72eca64d 
)

FetchContent_GetProperties(JUCE)

if (NOT JUCE_POPULATED)
  FetchContent_Populate(JUCE)
endif()