#ifndef INI_HH
#define INI_HH

#include <functional>
#include <new>
#include <type_traits>
#include <utility>
#include <unordered_map>
#include <string>

using std::string;

enum setting_type {
    bool_type,
    number_type,
    string_type
};

class setting {
  void destroy() {
    if (type == setting_type::string_type) {
      s.~string();
    }
  }

  union {
    bool b;
    long n;
    std::string s;
  };

  enum setting_type type;

public:
  setting(const bool &other) : b(other), type(setting_type::bool_type) {}
  setting(const long &other) : n(other), type(setting_type::number_type) {}
  setting(const std::string &other) : s(other), type(setting_type::string_type) {}
  setting(std::string &&other) : s(std::move(other)), type(setting_type::string_type) {}

  setting(const setting &other) : type(other.type) {
    switch (type) {
    case bool_type:
      new(&b) bool(other.b);
      break;
    case number_type:
      new(&n) long(other.n);
      break;
    case string_type:
      new(&s) std::string(other.s);
      break;
    }
  }

  setting(setting &&other) noexcept : type(other.type) {
    switch (type) {
    case bool_type:
      new(&b) bool(std::move(other.b));
      break;
    case number_type:
      new(&n) long(std::move(other.n));
      break;
    case string_type:
      new(&s) std::string(std::move(other.s));
      break;
    }
  }

  setting &operator=(setting other) {
    destroy();
    type = other.type;
    switch (type) {
    case bool_type:
      new(&b) bool(std::move(other.b));
      break;
    case number_type:
      new(&n) long(std::move(other.n));
      break;
    case string_type:
      new(&s) std::string(std::move(other.s));
      break;
    }
    return *this;
  }

  ~setting() {
    destroy();
  }

  template<typename BoolF, typename LongF, typename StringF>
  auto match(BoolF boolf, LongF longf, StringF stringf) -> decltype(boolf(b)) {
    switch (type) {
    case bool_type:
      return boolf(b);
    case number_type:
      return longf(n);
    case string_type:
      return stringf(s);
    }
  }

  template<typename BoolF, typename LongF, typename StringF>
  auto match(BoolF boolf, LongF longf, StringF stringf) const -> decltype(boolf(b)) {
    switch (type) {
    case bool_type:
      return boolf(b);
    case number_type:
      return longf(n);
    case string_type:
      return stringf(s);
    }
  }

  template<typename ...Args>
  void emplace_string(Args &&...args) {
    static_assert(std::is_nothrow_constructible<std::string, Args...>::value,
                  "constructor must be noexcept");
    destroy();
    type = setting_type::string_type;
    new(&s) std::string(std::forward<Args>(args)...);
  }

  bool operator==(const setting &other) const {
    if (type != other.type)
      return false;

    switch (type) {
    case bool_type:
      return b == other.b;
    case number_type:
      return n == other.n;
    case string_type:
      return s == other.s;
    }
  }

  bool operator!=(const setting &other) const {
    if (type != other.type)
      return true;

    switch (type) {
    case bool_type:
      return b != other.b;
    case number_type:
      return n != other.n;
    case string_type:
      return s != other.s;
    }
  }
};

namespace std {
  template <>
  struct hash<setting> {
    hash() : hash_bool(), hash_long(), hash_string() {}

    size_t operator()(const setting &s) const {
      return s.match([this](const bool &v) { return ~hash_bool(v); },
                     [this](const long &v) { return ~hash_long(v); },
                     [this](const std::string &v) { return ~hash_string(v); });
    }

  private:
    const hash<bool> hash_bool;
    const hash<long> hash_long;
    const hash<std::string> hash_string;
  };
}

std::unordered_map<std::string, setting> read_conf(const std::string &filename);

#endif
