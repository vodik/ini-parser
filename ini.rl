#include <cstddef>
#include <cassert>
#include <cstring>
#include <string>
#include <memory>

#include "ini.hh"
#include "mmap.hh"

using std::unique_ptr;

namespace ini {

%%{
  machine parser;

  action mark { mark = fpc - data; }

  action set_section {
    string name(data + mark, (size_t)(fpc - data - mark));

    printf("section: %s\n", name.c_str());
    settings[name] = {};
    section = &settings.at(name);
  }

  action store_key {
    key = string(data + mark, (size_t)(fpc - data - mark));
  }

  action store_value {
    string value(data + mark, (size_t)(fpc - data - mark));
    if (section) {
      section->emplace(std::move(key), std::move(value));
    }
  }

  string = ( alpha | digit | '-' | '_' )+;
  key    = string >mark %store_key;
  value  = [^\n]* >mark %store_value;

  comment = ( '#' | ';' ) [^\n]*;
  section = '[' string >mark %set_section ']';
  setting = key space* '=' space* value >mark;
  line    = ( comment | section | setting ) {,1} '\n';

  main := line*;
}%%

%% write data;

class parser {
  int cs;
  ptrdiff_t mark;
  size_t nread;
  ini::section *section;

public:
  ini::settings settings;

  parser() : cs(0), mark(0), nread(0), section(nullptr) {
    %%write init;
  }

  size_t feed(const char *data, size_t len, size_t off) {
    assert(off <= len && "offset past end of buffer");

    string key;
    const char *p = data + off;
    const char *pe = data + len;

    assert(pe - p == len - off && "pointers aren't same distance");

    %%write exec;

    assert(p <= pe && "buffer overflow after parsing.");

    nread += p - data + off;

    assert(nread <= len && "nread longer than length");
    assert(mark < len && "mark is after buffer end");

    return nread;
  }

  bool error() const { return cs == parser_error; }
  bool finished() const { return cs >= parser_error; }
};

}

/* should be able to read multiple files at once */
ini::settings ini::read_conf(const string &filename) {
  ini::parser parser;
  memorymap map(filename);

  parser.feed(map.data(), map.size(), 0);

  printf("error? %s\n", parser.error() ? "yes" : "no");
  printf("finished? %s\n", parser.finished() ? "yes" : "no");

  return parser.settings;
}
