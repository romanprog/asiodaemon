#ifndef DNSUTILS_HPP
#define DNSUTILS_HPP

#include "DnsTypes.hpp"

#include <stdint.h>
#include <map>
#include <vector>
#include <string>

namespace dns {

// Utils. Read/write/generate/parse/... DNS packages.
namespace utils {
// Convert sizeof(ResT) bytes from bufer to unsigned integral type ResT.
// Pointer remains at the same position.
template <typename T>
T buff_read(const char * pt)
{
    T result = 0;
    for (int i = 0; i < sizeof(T); ++i)
    {
        result += static_cast<T>(static_cast<unsigned char>(pt[i])) * pow(256, sizeof(T) - i - 1);
    }
    return result;
}
// Convert sizeof(ResT) bytes from bufer to unsigned integral type ResT.
// Moves cursor pointer to sizeof(ResT) bytes forward.
template <typename T>
T buff_step_read(const char *&cursor)
{
    T result = buff_read<T>(cursor);
    cursor += sizeof(T);
    return result;
}
// Write sizeof(ResT) bytes from unsigned integral type ResT to buffer in dns format.
// Pointer remains at the same position.
template <typename T>
static void buff_write(char * pt, T val)
{
    for (int i = 0; i < sizeof(T) - 1; ++i)
    {
        pt[i] = static_cast<char>(val / pow(256, sizeof(T) - i - 1));
    }
    pt[sizeof(T) - 1] = static_cast<char>(val % 256);
}
// Write sizeof(ResT) bytes from unsigned integral type ResT to buffer in dns format.
// Moves cursor pointer to sizeof(ResT) bytes forward.
template <typename T>
void buff_step_write(char *&cursor, T val)
{
    buff_write(cursor, val);
    cursor += sizeof(T);
}

// Return DNS pkg bool flag.
bool get_flag(const char * ch, const DnsFlag flag_num);

// Set DNS pkg bool flag.
void set_flag(char * ch, const DnsFlag flag_num);

// Get DNS answer error from flags.
uint16_t get_error(const char * flag);

// Get string domain from DNS data set. Return offset first byte after reading name.
void buff_step_read_qdn(const char *dns_pkg, const char *&cursor, std::string & result);

// Convert domain to package format and write result directly in buffer.
// Return size of the data, written to buffer.
void write_step_qdn_to_buff(const std::string &dname, char *&_res);

// Convert string IP to .in-addr.arpa QDN.
std::string ip_to_arpa(const std::string &ip);

// Generate random ID for DNS pkg ident.
uint16_t rand_qid();

// Read 4 bytes ip to string. Move cursor.
std::string ip_step_read(const char *&cursor);

// Validate IP v4 address.
bool is_ip_v4(const std::string & ip);

// Validate IP v4 address.
bool is_fqdn(const std::string & name);

// Write DNS request package to buffer (direct write).
// Buffer must have enough free space (512 bytes recomended).
size_t request_buff_write(void * buffer, DnsRequest req);

// Parse DNS respond in buffer. Size is determined automatically.
// req_id used to compare respont and request. 0 - don't check (not recomended).
DnsError respond_buff_parse(const void *buffer, DnsRespond & res, uint16_t req_id = 0);

// Create DnsRequest obj.
DnsError create_request(const std::string & req_str, DnsQType t, DnsRequest & req);

// Utils for represent various record data, as sorted MX pref list, random IP etc.
std::map<uint16_t, std::string> get_mx_sorted(const DnsRespond & resp);

std::vector<std::string> get_rec_list(const DnsRespond & resp);

std::string get_rand_rec_value(const DnsRespond & resp);


} // namespace utils

} // namespace dns

#endif // DNSUTILS_HPP
