#include <cstdio>
#include <string>

#include "ini.hh"

static void print_arg(const setting &s)
{
  s.match([](const bool v) { printf("%s [bool]\n", v ? "true" : "false"); },
          [](const long v) { printf("%ld [long]\n", v); },
          [](const std::string &v) { printf("%s [string]\n", v.c_str()); });
}

int main(void)
{
  /* auto settings = read_conf("/home/simon/.config/termite/config"); */
  const auto settings = read_conf("test.ini");

  printf(">> DUMPING\n");
  for (auto &x: settings) {
      printf("%s = ", x.first.c_str());
      print_arg(x.second);
  }

  return 0;
}
