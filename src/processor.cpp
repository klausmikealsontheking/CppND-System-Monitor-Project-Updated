#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
  long active_jiffies = LinuxParser::ActiveJiffies() - prev_active_jiffies_;
  long idle_jiffies = LinuxParser::IdleJiffies() - prev_idle_jiffies_;
  long total_jiffies = active_jiffies + idle_jiffies;

  float utilization = float(active_jiffies) / float(total_jiffies);

  prev_active_jiffies_ = LinuxParser::ActiveJiffies();
  prev_idle_jiffies_ = LinuxParser::IdleJiffies();

  return utilization; 
}