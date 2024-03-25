/**
 * @file gtest_outerjoin
 *
 * @brief test outerjoin
 *  */

#include "../simpledb.h"
#include <gtest/gtest.h>

namespace {
using std::string;

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

TEST(simpledb, input1) {
  {
    SimpleDB sdb;

    EXPECT_EQ(sdb.exec("INSERT A 0 lean"      ), "OK\n");
    EXPECT_EQ(sdb.exec("INSERT A 0 understand"), "ERR дупликат 0\n");
    EXPECT_EQ(sdb.exec("INSERT A 1 sweater"   ), "OK\n");
    EXPECT_EQ(sdb.exec("INSERT A 2 frank"     ), "OK\n");
    EXPECT_EQ(sdb.exec("INSERT A 3 violation" ), "OK\n");
    EXPECT_EQ(sdb.exec("INSERT A 4 quality"   ), "OK\n");
    EXPECT_EQ(sdb.exec("INSERT A 5 precision" ), "OK\n");
    EXPECT_EQ(sdb.exec("INSERT B 3 proposal"), "OK\n");
    EXPECT_EQ(sdb.exec("INSERT B 4 example"), "OK\n");
    EXPECT_EQ(sdb.exec("INSERT B 5 lake"), "OK\n");
    EXPECT_EQ(sdb.exec("INSERT B 6 flour"     ), "OK\n");
    EXPECT_EQ(sdb.exec("INSERT B 7 wonder"), "OK\n");
    EXPECT_EQ(sdb.exec("INSERT B 8 selection"), "OK\n");
    EXPECT_EQ(sdb.exec("INTERSECTION"),
              "5,precision,lake\n"
              "4,quality,example\n"
              "3,violation,proposal\n"
              "OK\n");
    EXPECT_EQ(sdb.exec("SYMMETRIC_DIFFERENCE"),
              "2,frank,\n"
              "1,sweater,\n"
              "0,lean,\n"
              "8,,selection\n"
              "7,,wonder\n"
              "6,,flour\n"
              "OK\n");
    EXPECT_EQ(sdb.exec("TRUNCATE A"), "OK\n");
    EXPECT_EQ(sdb.exec("INTERSECTION"), "OK\n");
    EXPECT_EQ(sdb.exec("SYMMETRIC_DIFFERENCE"),
              "8,,selection\n"
              "7,,wonder\n"
              "6,,flour\n"
              "5,,lake\n"
              "4,,example\n"
              "3,,proposal\n"
              "OK\n");
  }
}

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
} // namespace
