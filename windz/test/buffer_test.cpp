//
// Created by crystalwind on 19-1-22.
//

#include "windz/Buffer.h"
#include <string>

#define TEST(a, b) assert((a) == (b))

using namespace windz;
using namespace std;

void test1() {
    Buffer buf;

    TEST(buf.ReadableBytes(), 0);
    TEST(buf.WritableBytes(), Buffer::kInitSize);
    TEST(buf.PrependableBytes(), Buffer::kPrependSize);

    string s1(100, 'a');
    buf.Write(s1);
    TEST(buf.ReadableBytes(), s1.size());
    TEST(buf.WritableBytes(), Buffer::kInitSize - s1.size());
    TEST(buf.PrependableBytes(), Buffer::kPrependSize);

    string s2 = buf.Read(50);
    TEST(s2.size(), 50);
    TEST(buf.ReadableBytes(), s1.size() - s2.size());
    TEST(buf.WritableBytes(), Buffer::kInitSize - s1.size());
    TEST(buf.PrependableBytes(), Buffer::kPrependSize + s2.size());
    TEST(s2, string(50, 'a'));

    buf.Write(s1);
    TEST(buf.ReadableBytes(), 2*s1.size() - s2.size());
    TEST(buf.WritableBytes(), Buffer::kInitSize - 2*s1.size());
    TEST(buf.PrependableBytes(), Buffer::kPrependSize + s2.size());

    string s3 = buf.ReadAll();
    TEST(s3.size(), 150);
    TEST(buf.ReadableBytes(), 0);
    TEST(buf.WritableBytes(), Buffer::kInitSize);
    TEST(buf.PrependableBytes(), Buffer::kPrependSize);
    TEST(s3, string(150, 'a'));
}

void test2() {
    Buffer buf;

    buf.Write(string(500, 'b'));
    TEST(buf.ReadableBytes(), 500);
    TEST(buf.WritableBytes(), Buffer::kInitSize - 500);

    buf.Release(100);
    TEST(buf.ReadableBytes(), 400);
    TEST(buf.WritableBytes(), Buffer::kInitSize - 500);
    TEST(buf.PrependableBytes(), Buffer::kPrependSize + 100);

    buf.Write(string(1000, 'b'));
    TEST(buf.ReadableBytes(), 1400);
    TEST(buf.WritableBytes(), 0);
    TEST(buf.PrependableBytes(), Buffer::kPrependSize);

    buf.ReleaseAll();
    TEST(buf.ReadableBytes(),0);
    TEST(buf.WritableBytes(), 1400);
    TEST(buf.PrependableBytes(), Buffer::kPrependSize);
}

void test3() {
    Buffer buf;

    buf.Write(string(900, 'a'));
    TEST(buf.ReadableBytes(), 900);
    TEST(buf.WritableBytes(), Buffer::kInitSize - 900);

    buf.Release(600);
    TEST(buf.ReadableBytes(), 300);
    TEST(buf.WritableBytes(), Buffer::kInitSize - 900);
    TEST(buf.PrependableBytes(), Buffer::kPrependSize + 600);

    buf.Write(string(300, 'a'));
    TEST(buf.ReadableBytes(), 600);
    TEST(buf.WritableBytes(), Buffer::kInitSize - 600);
    TEST(buf.PrependableBytes(), Buffer::kPrependSize);
}

void test4() {
    Buffer buf;

    buf.Write(string(1800, 'a'));
    TEST(buf.ReadableBytes(), 1800);
    TEST(buf.WritableBytes(), 0);
    TEST(buf.PrependableBytes(), Buffer::kPrependSize);

    buf.Release(1500);
    TEST(buf.ReadableBytes(), 300);
    TEST(buf.WritableBytes(), 0);
    TEST(buf.PrependableBytes(), Buffer::kPrependSize + 1500);

    buf.Shrink(222);
    TEST(buf.ReadableBytes(), 300);
    TEST(buf.WritableBytes(), 222);
    TEST(buf.ReadAll(), string(300, 'a'));
    TEST(buf.PrependableBytes(), Buffer::kPrependSize);
}

void test5() {
    Buffer buf;

    buf.Write(string(100, 'a'));
    TEST(buf.ReadableBytes(), 100);
    TEST(buf.WritableBytes(), Buffer::kInitSize - 100);
    TEST(buf.PrependableBytes(), Buffer::kPrependSize);

    buf.Prepend("abcd");
    TEST(buf.ReadableBytes(), 104);
    TEST(buf.WritableBytes(), Buffer::kInitSize - 100);
    TEST(buf.PrependableBytes(), Buffer::kPrependSize - 4);

    int32_t x;
    buf.Prepend(&x, sizeof(x));
    TEST(buf.ReadableBytes(), 108);
    TEST(buf.WritableBytes(), Buffer::kInitSize - 100);
    TEST(buf.PrependableBytes(), Buffer::kPrependSize - 8);

    buf.Write(string(920, 'a'));
    TEST(buf.ReadableBytes(), 1028);
    TEST(buf.WritableBytes(), 4);
    TEST(buf.PrependableBytes(), 0);

    buf.Write(string(100, 'a'));
    TEST(buf.ReadableBytes(), 1128);
    TEST(buf.WritableBytes(), 0);
    TEST(buf.PrependableBytes(), Buffer::kPrependSize);
}

int main()
{
    test1();
    test2();
    test3();
    test4();
    test5();
}