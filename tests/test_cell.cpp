//
// Created by kedar on 7/2/2024.
//
#include <gtest/gtest.h>
#include "cell.h"

TEST(CellTest, XOROperator) {
cell cell1(false);


cell cell2(false, "B12");
cell result = cell1 ^ cell2;

//EXPECT_EQ(result.pixel["B11"], );
EXPECT_EQ(result.pixel["B12"], 1);
//EXPECT_EQ(result.pixel["B13"], 5);
EXPECT_EQ(result.state, false);
}

TEST(CellTest, ToString) {
    cell cell1(false);
    cell cell2(true, "B11");
    cell cell3(true, "B12");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
