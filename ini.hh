#ifndef INI_HH
#define INI_HH

#include <functional>
#include <new>
#include <type_traits>
#include <utility>
#include <unordered_map>
#include <string>

using std::string;

namespace ini {

typedef typename std::unordered_map<string, string> section;
typedef typename std::unordered_map<string, section> settings;

settings read_conf(const string &filename);

}

#endif
