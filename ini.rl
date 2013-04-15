#include <cstddef>
#include <cassert>
#include <cstring>
#include <string>

#include "ini.hh"
#include "mmap.hh"

%%{
  machine ini_parser;

  action ini_mark { mark = fpc - data; }

  action ini_section {
    section = std::string(data + mark, fpc - data - mark);
    printf("section: %s\n", section.c_str());
  }

  action ini_key {
    key = std::string(data + mark, fpc - data - mark);
  }

  action ini_bool {
    if (strncasecmp("true", data + mark, fpc - data - mark) == 0) {
      settings.emplace(std::move(key), true);
    } else {
      settings.emplace(std::move(key), false);
    }
  }

  action ini_number {
    long value = strtoul(data + mark, NULL, 0);
    settings.emplace(std::move(key), value);
  }

  action ini_string {
    std::string value(data + mark, fpc - data - mark);
    settings.emplace(std::move(key), std::move(value));
  }

  # comment = '#' | ';';

  string = ( alpha | digit | '-' | '_' )+;
  bool   = /true/i | /false/i;
  number = '0x' xdigit+ | digit+; #| alpha alnum*;

  key   = string >ini_mark %ini_key;
  # value = string >ini_mark %ini_string;
  # value = ( bool   %ini_bool
          # | number %ini_number
          # | string %ini_string
          # ) >ini_mark;
  value = bool %ini_bool
        | number %ini_number
        | (string - number - bool) %ini_string;

  section = '[' string >ini_mark %ini_section ']';
  setting = key space* '=' space* value >ini_mark;

  line = ( section | setting ) . '\n';

  main := line*;
}%%

%% write data;

class ini_parser {
  int cs;
  ptrdiff_t mark;
  size_t nread;

public:
  std::unordered_map<std::string, setting> settings;

  ini_parser() : cs(0), mark(0), nread(0) {
    %%write init;
  }

  size_t feed(const char *data, size_t len, size_t off)
  {
    const char *p, *pe;
    std::string section, key;

    assert(off <= len && "offset past end of buffer");

    p  = data + off;
    pe = data + len;

    assert(pe - p == len - off && "pointers aren't same distance");

    %% write exec;

    assert(p <= pe && "buffer overflow after parsing.");

    nread += p - data + off;

    assert(nread <= len && "nread longer than length");
    assert(mark < len && "mark is after buffer end");

    return nread;
  }

  bool error() const { return cs == ini_parser_error; }
  bool finished() const { return cs >= ini_parser_error; }
};

/* should be able to read multiple files at once */
std::unordered_map<std::string, setting> read_conf(const std::string &filename)
{
  ini_parser parser;
  memorymap map(filename);

  parser.feed(map.data(), map.size(), 0);

  printf("error? %s\n", parser.error() ? "yes" : "no");
  printf("finished? %s\n", parser.finished() ? "yes" : "no");

  return parser.settings;
}
