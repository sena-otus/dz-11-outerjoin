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

TEST(simpledb, inputAB) {
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
              "3,violation,proposal\n"
              "4,quality,example\n"
              "5,precision,lake\n"
              "OK\n");
    EXPECT_EQ(sdb.exec("SYMMETRIC_DIFFERENCE"),
              "0,lean,\n"
              "1,sweater,\n"
              "2,frank,\n"
              "6,,flour\n"
              "7,,wonder\n"
              "8,,selection\n"
              "OK\n");
    EXPECT_EQ(sdb.exec("TRUNCATE A"), "OK\n");
    EXPECT_EQ(sdb.exec("INTERSECTION"), "OK\n");
    EXPECT_EQ(sdb.exec("SYMMETRIC_DIFFERENCE"),
              "3,,proposal\n"
              "4,,example\n"
              "5,,lake\n"
              "6,,flour\n"
              "7,,wonder\n"
              "8,,selection\n"
              "OK\n");
  }
}

TEST(simpledb, inputBA) {
  {
    SimpleDB sdb;
    EXPECT_EQ(sdb.exec("INSERT B 0 lean"      ), "OK\n");
    EXPECT_EQ(sdb.exec("INSERT B 0 understand"), "ERR дупликат 0\n");
    EXPECT_EQ(sdb.exec("INSERT B 1 sweater"   ), "OK\n");
    EXPECT_EQ(sdb.exec("INSERT B 2 frank"     ), "OK\n");
    EXPECT_EQ(sdb.exec("INSERT B 3 violation" ), "OK\n");
    EXPECT_EQ(sdb.exec("INSERT B 4 quality"   ), "OK\n");
    EXPECT_EQ(sdb.exec("INSERT B 5 precision" ), "OK\n");
    EXPECT_EQ(sdb.exec("INSERT A 3 proposal"), "OK\n");
    EXPECT_EQ(sdb.exec("INSERT A 4 example"), "OK\n");
    EXPECT_EQ(sdb.exec("INSERT A 5 lake"), "OK\n");
    EXPECT_EQ(sdb.exec("INSERT A 6 flour"     ), "OK\n");
    EXPECT_EQ(sdb.exec("INSERT A 7 wonder"), "OK\n");
    EXPECT_EQ(sdb.exec("INSERT A 8 selection"), "OK\n");
    EXPECT_EQ(sdb.exec("INTERSECTION"),
              "3,proposal,violation\n"
              "4,example,quality\n"
              "5,lake,precision\n"
              "OK\n");
    EXPECT_EQ(sdb.exec("SYMMETRIC_DIFFERENCE"),
              "0,,lean\n"
              "1,,sweater\n"
              "2,,frank\n"
              "6,flour,\n"
              "7,wonder,\n"
              "8,selection,\n"
              "OK\n");
  }
}

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
} // namespace
