#include "../stdafx.h"

#include "gtest/gtest.h"

#include "../../SMT/Matrix/StandardMatrix.h"

class StandardMatrixTest : public ::testing::Test 
{
protected:
   static SMT::Matrix<double>::SharedPtr createStandardMatrix(size_t rowCount, size_t columnCount, const std::function<double(size_t, size_t)>& func)
   {
      SMT::Matrix<double>::SharedPtr result = std::make_shared<SMT::StandardMatrix<double>>(rowCount, columnCount, func);

      EXPECT_EQ(rowCount, result->RowCount());
      EXPECT_EQ(columnCount, result->ColumnCount());

      return result;
   }
};

TEST_F(StandardMatrixTest, IdentityMatrix)
{
   auto initFunc = [](size_t row, size_t column)->double { return row == column ? 1.0 : 0.0; };
   const size_t columnCount = 10;
   const size_t rowCount = 10;
   auto matrix = createStandardMatrix(rowCount, columnCount, initFunc);
   for (size_t i = 0; i < rowCount; ++i)
   {
      for (size_t j = 0; j < columnCount; ++j)
      {
         const double diff = matrix->Element(i, j) - (i == j ? 1.0 : 0.0);
         ASSERT_TRUE(std::abs<double>(diff - SMT::MatrixSettings::Zero<double>()) <= SMT::MatrixSettings::Epsilon<double>());
      }
   }
}