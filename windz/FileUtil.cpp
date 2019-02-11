//
// Created by crystalwind on 19-2-10.
//

#include "FileUtil.h"
#include <assert.h>
#include <stdio.h>
#include <string>

namespace windz {

AppendFile::AppendFile(const std::string &filename)
        : fp_(fopen(filename.c_str(), "ae")) {
    setbuffer(fp_, buf_, sizeof(buf_));
}

AppendFile::~AppendFile() {
    fclose(fp_);
}

void AppendFile::Append(const char *logline, size_t len) {
    size_t nwriten = fwrite_unlocked(logline, 1, len, fp_);
    size_t remain = len - nwriten;
    while (remain > 0) {
        size_t n = fwrite_unlocked(logline + nwriten, 1, remain, fp_);
        if (n == 0) {
            if (ferror(fp_)) {
                fprintf(stderr, "Append::append() failed !\n");
            }
            break;
        }
        nwriten += n;
        remain -= n;
    }

    written_bytes_ += len;
}

void AppendFile::Flush() {
    fflush(fp_);
}

}  // namespace windz
