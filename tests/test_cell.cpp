//
// Created by kedar on 7/2/2024.
//
#include <gtest/gtest.h>
#include "cell.h"
#include "cellular_automata.h"

TEST(CATest, object_creation) {
    cell A(1, 2, "B11");
    cell B(1, 2, "B12");
    cell C(1, 2, "B21");
    A = B ^ C;
    EXPECT_EQ(A.to_string(), "1B12+1B21");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
