#include "HFiles.hpp"
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

namespace hfile {

bool readtostr(const std::string &filename, std::string &file_data)
{
    int fd=-1;
    // size of body content file (file descriptor from exim)
    struct stat stat_buff{0};

    fd = open(filename.c_str(), O_RDONLY);

    if (fd == -1)
        return false;

    fstat(fd, &stat_buff);
    long bsize = stat_buff.st_size;

    // resize string to add body from file (trim first 19 bites of file)
    file_data.resize(bsize);
    // set position offset to 19 byte
    lseek(fd, SEEK_SET, 0);
    // read body from fd to string
    read(fd ,&file_data[0], bsize);

    close(fd);

    return true;
}

} // namespace
