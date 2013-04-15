#include <cstdio>
#include <string>

#include "ini.hh"

static void print_arg(setting &s)
{
  s.match([](const bool &v) { printf("bool: %s\n", v ? "true" : "false"); },
          [](const long &v) { printf("unsigned: %ld\n", v); },
          [](const std::string &v) { printf("string: %s\n", v.c_str()); });
}

int main(void)
{
  /* auto settings = read_conf("/home/simon/.config/termite/config"); */
  auto settings = read_conf("test.ini");

  printf(">> DUMPING\n");
  for (auto& x: settings) {
      printf("key[%s] ", x.first.c_str());
      print_arg(x.second);
  }

  return 0;
}
