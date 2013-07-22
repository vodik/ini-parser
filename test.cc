#include <cstdio>
#include <string>
#include <functional>

#include "maybe.hh"
#include "ini.hh"

using std::string;

template <class T>
T convert(string __attribute__((unused)) str) { }

template <> string convert(string str) { return str; }
template <> int convert(string str) { return atoi(str.c_str()); }
template <> double convert(string str) { return atof(str.c_str()); }

template <class T>
void apply(std::function<void(T)> app, ini::settings &settings, maybe<string> def, string section, string key)
{
    string val = def.get_value_or([&] {
        return settings.at(section).at(key);
    });
    app(convert<T>(val));
}

int main()
{
    auto settings = ini::read_conf("/home/simon/.config/termite/config");

    std::function<void(string)> apply_str = [](string str){ printf("STR: %s\n", str.c_str()); };
    std::function<void(double)> apply_dbl = [](double dbl){ printf("DBL: %f\n", dbl); };

    apply(apply_str, settings, {},    "options", "font");
    apply(apply_dbl, settings, "5.5", "options", "transparency");

    return 0;
}
