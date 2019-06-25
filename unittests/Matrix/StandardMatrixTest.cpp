#include "../stdafx.h"

#include "gtest/gtest.h"

#include "../../SMT/Matrix/StandardMatrix.h"

class StandardMatrixTest : public ::testing::Test {
};

TEST_F(StandardMatrixTest, IdentityMatrix)
{
   auto initFunc = [](size_t row, size_t column)->double { return row == column ? 1.0 : 0.0; };
   const size_t columnCount = 10;
   const size_t rowCount = 10;
   SMT::StandardMatrix<double> identityMatrix(columnCount, rowCount, initFunc);
   for (size_t i = 0; i < 10; ++i)
}