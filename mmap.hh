#include <string>
#include <system_error>
#include <cerrno>

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

class memorymap
{
  int fd;
  char *memblock;
  struct stat st;
  size_t st_size;

  char *const map_failed = (char *)MAP_FAILED;

public:
  memorymap(const std::string &filename) : fd(0), memblock(map_failed) {
    fd = open(filename.c_str(), O_RDONLY);
    if (fd < 0) {
      throw std::system_error(std::error_code(errno, std::generic_category()));
    }

    fstat(fd, &st);
    st_size = (size_t)st.st_size;
    memblock = (char *)mmap(nullptr, st_size, PROT_READ, MAP_SHARED | MAP_POPULATE, fd, 0);
    if (memblock == map_failed) {
      close(fd);
      throw std::system_error(std::error_code(errno, std::generic_category()));
    }
  }

  ~memorymap() {
    if (fd >= 0)
      close(fd);
    if (memblock != map_failed)
      munmap(memblock, st_size);
  }

  size_t size() const { return st_size; }
  const char *data() const { return memblock; }
};
