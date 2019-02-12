#ifndef WINDZ_FILEUTIL_H
#define WINDZ_FILEUTIL_H

#include "Noncopyable.h"

#include <stdio.h>

#include <string>

namespace windz {

class AppendFile : Noncopyable {
  public:
    explicit AppendFile(const std::string &filename);
    ~AppendFile();

    void Append(const char *logline, size_t len);
    void Flush();

    off_t written_byte() const { return written_bytes_; }

  private:
    FILE *fp_;
    char buf_[64*1024];
    off_t written_bytes_;
};

}  // namespace windz

#endif //WINDZ_FILEUTIL_H
