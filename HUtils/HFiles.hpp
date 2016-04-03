#ifndef HFILES_HPP
#define HFILES_HPP

#include <string>

namespace hfile {

// fast read all file to string
bool readtostr(const std::string & filename, std::string & file_data);

} // namespace
#endif // HFILES_HPP
