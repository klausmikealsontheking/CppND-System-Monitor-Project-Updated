#include "format.h"

#include <iomanip>  // Include the iomanip header
#include <sstream>
#include <string>

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
  int const hours = seconds / 3600;
  int const minutes = (seconds % 3600) / 60;
  int const secs = seconds % 60;

  std::ostringstream oss;
  oss << std::setw(2) << std::setfill('0') << hours << ":" << std::setw(2)
      << std::setfill('0') << minutes << ":" << std::setw(2) << std::setfill('0')
      << secs;

  return oss.str();
}