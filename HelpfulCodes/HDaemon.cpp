#include "HDaemon.hpp"
#include "HStrings.hpp"
#include "HFiles.hpp"

namespace hdaemon {

unsigned pid_fread(const std::string & fname)
{
    std::string spid;
    if (!hfile::readtostr(fname, spid) || !hstrings::is_digit_only(spid))
        return 0;
    return std::stoi(spid);
}

bool pid_fwrite(const std::string &fname, unsigned pid)
{

    auto f = fopen(fname.c_str(), "w+");
    if (!f)
        return false;

    fprintf(f, "%u", pid);
    fclose(f);
    return true;

}

} //namespace
