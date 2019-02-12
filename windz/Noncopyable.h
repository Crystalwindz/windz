#ifndef WINDZ_NONCOPYABLE_H
#define WINDZ_NONCOPYABLE_H

namespace windz {

class Noncopyable {
  protected:
    Noncopyable() = default;
    ~Noncopyable() = default;

    Noncopyable(const Noncopyable &) = delete;
    Noncopyable &operator=(const Noncopyable &) = delete;
};

}  // namespace windz

#endif //WINDZ_NONCOPYABLE_H
