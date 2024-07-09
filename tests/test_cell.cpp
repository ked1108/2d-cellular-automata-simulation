//
// Created by kedar on 7/2/2024.
//
#include <gtest/gtest.h>
#include "cell.h"
#include "cellular_automata.h"

TEST(CATest, object_creation) {
    cellular_automata* CA = new cellular_automata(5, 4, 4);
    EXPECT_EQ(CA->get_pos(0,0), 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
