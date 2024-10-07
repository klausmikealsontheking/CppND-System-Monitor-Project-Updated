#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::stoi;
using std::stol;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() {
  string line;
  string key;
  float value;
  float mem_total;
  float mem_free;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal:") {
          mem_total = value;
        }
        if (key == "MemFree:") {
          mem_free = value;
          return (mem_total - mem_free) / mem_total;
        }
      }
    }
  }
  return 0.0;
}

long LinuxParser::UpTime() {
  string line;
  long uptime;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return uptime;
}

long LinuxParser::Jiffies() {
  return ActiveJiffies() + IdleJiffies();
}

long LinuxParser::ActiveJiffies(int pid) {
  string line, value;
  vector<string> proc_stat;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      proc_stat.push_back(value);
    }
  }

  long active_jiffies = 0;
  if (proc_stat.size() > 21) {
    active_jiffies = stol(proc_stat[13]) + stol(proc_stat[14]) +
                     stol(proc_stat[15]) + stol(proc_stat[16]);
  }

  return active_jiffies;
}

long LinuxParser::ActiveJiffies() {
  vector<string> cpu_values = CpuUtilization();
  return stol(cpu_values[kUser_]) + stol(cpu_values[kNice_]) +
         stol(cpu_values[kSystem_]) + stol(cpu_values[kIRQ_]) +
         stol(cpu_values[kSoftIRQ_]) + stol(cpu_values[kSteal_]);
}

long LinuxParser::IdleJiffies() {
  vector<string> cpu_values = CpuUtilization();
  return stol(cpu_values[kIdle_]) + stol(cpu_values[kIOwait_]);
}

vector<string> LinuxParser::CpuUtilization() {
  vector<string> cpu_values;
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> key;
    while (linestream >> value) {
      cpu_values.push_back(value);
    }
  }
  return cpu_values;
}

int LinuxParser::TotalProcesses() {
  string line;
  string key;
  int value;
  std::ifstream filestream(kProcDirectory + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          return value;
        }
      }
    }
  }
  return 0;
}

int LinuxParser::RunningProcesses() {
  string line;
  string key;
  int value;
  std::ifstream filestream(kProcDirectory + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          return value;
        }
      }
    }
  }
  return 0;
}

string LinuxParser::Command(int pid) {
  string line;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    return line;
  }
  return string();
}

string LinuxParser::Ram(int pid) {
  string line;
  string key;
  int value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize:") {
          return to_string(value / 1024);
        }
      }
    }
  }
  return string();
}

string LinuxParser::Uid(int pid) {
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:") {
          return value;
        }
      }
    }
  }
  return string();
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string uid = Uid(pid);
  string line;
  string user, x, value;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> user >> x >> value) {
        if (value == uid) {
          return user;
        }
      }
    }
  }
  return string();
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  string line, 
 value;
  vector<string> proc_stat;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line); 

    while (linestream >> value) {
      proc_stat.push_back(value); 

    }
  }
  long uptime = 0;
  if (!proc_stat.empty()) {
    uptime = stol(proc_stat[21]) / sysconf(_SC_CLK_TCK);
  }
  return uptime;
}