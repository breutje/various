//
// stdxl copyright 20013 Bert Reuling
//
#include <algorithm>
#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <ctime>


#if !defined(__STDXL_INCLUDED)
#define __STDXL_INCLUDED

namespace stdxl
{

//
// convert (int) value to hexadecimal with leading zeroes and width (default based on type)
//
template <typename T>
inline std::string to_hexstring(T value, size_t width = sizeof(T) * 2)
{
   std::stringstream ss;
   ss << std::setfill('0') << std::setw(width) << std::hex << (value|0);
   return ss.str();
}

//
// convert pointer to (possibly meaningless) string
//
template <typename T>
inline std::string pointless(T value)
{
   if (value == nullptr)
      return "nullptr";
   else if (sizeof(T) == 4)
      return "@x" + to_hexstring(reinterpret_cast<__int32>(value));
   else if (sizeof(T) == 8)
      return "@x" + to_hexstring(reinterpret_cast<__int64>(value));
   else
      return "<pointless>";
}

//
// convert std::string to uppercase (latin only)
//
inline std::string to_uppercase(std::string strValue)
{
   std::transform(strValue.begin(), strValue.end(), strValue.begin(), ::toupper);
   return strValue;
}

//
// convert std::string to lowercase (latin only)
//
inline std::string to_lowercase(std::string strValue)
{
   std::transform(strValue.begin(), strValue.end(), strValue.begin(), ::tolower);
   return strValue;
}

//
// simple log to file with date & time stamp
//
inline void log(std::string logfilename, std::string logmessage)
{
   std::time_t now = std::time(nullptr);
   std::tm tm = *std::gmtime(&now);
   std::ostringstream oss;
   oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S - ");
   std::ofstream ofs(logfilename, std::ios_base::out | std::ios_base::app);
   ofs << oss.str() << logmessage << std::endl;
}

//
// force coredump
//
inline void coredump()
{
   int (* crash)(void){0};
   crash();
}

} // namespace stdxl

#endif // __STDXL_INCLUDED
