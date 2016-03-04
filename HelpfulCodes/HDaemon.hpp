#ifndef HDAEMON_HPP
#define HDAEMON_HPP
#include <string>

namespace hdaemon {


////////////////////////////////////////////////////////////////
/// Utils for daemon fork, sys sigs, pids, opts.
////////////////////////////////////////////////////////////////

// function to read pin ffrom file by filename
unsigned pid_fread(const std::string &fname);

// write pid to file
bool pid_fwrite(const std::string &fname, unsigned pid);

} //namespace

#endif // HDAEMON_HPP
