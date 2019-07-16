#include "../stdafx.h"
#include "gtest/gtest.h"

#include "../../SMT/Matrix/MatrixOperations.h"

class OperationsTest : public ::testing::Test
{
protected:
   static SMT::Matrix<double>::SharedPtr CreateStandardMatrix(size_t rowCount, size_t columnCount, const std::function<double(size_t /*row*/, size_t /*column*/)>& func)
   {
      SMT::Matrix<double>::SharedPtr result = std::make_shared<SMT::StandardMatrix<double>>(rowCount, columnCount, func);

      EXPECT_EQ(rowCount, result->RowCount());
      EXPECT_EQ(columnCount, result->ColumnCount());

      return result;
   }
   template<typename ElementType>
   static void CheckForEachElement(const SMT::Matrix<ElementType>& matrix, const std::function<ElementType(size_t /*row*/, size_t /*column*/)>& func, bool epsilonIsZero, ElementType factor)
   {
      const size_t columnCount = matrix.ColumnCount();
      const size_t rowCount = matrix.RowCount();
      for (size_t i = 0; i < rowCount; ++i)
      {
         for (size_t j = 0; j < columnCount; ++j)
         {
            if (epsilonIsZero)
            {
               EXPECT_EQ(matrix.Element(i, j), func(i, j));
            }
            else
            {
               ASSERT_TRUE(SMT::MatrixSettings::CanAssumeItIsZero<double>(matrix.Element(i, j) - func(i, j), factor));
            }
         }
      }
   }
};

TEST_F(OperationsTest, Addition)
{
   auto func1 = [](size_t row, size_t column)->double
   {
      return static_cast<double>(row) * static_cast<double>(column) + 1;
   };
   const auto matrix_4x4_1 = CreateStandardMatrix(4, 4, func1);
   auto func2 = [](size_t row, size_t column)->double
   {
      return static_cast<double>(row) + static_cast<double>(column)+1;
   };
   const auto matrix_4x4_2 = CreateStandardMatrix(4, 4, func2);
   auto func3 = [](size_t row, size_t column)->double
   {
      return static_cast<double>(row) / (static_cast<double>(column) + 1.0);
   };
   const auto matrix_5x5_3 = CreateStandardMatrix(5, 5, func3);

   SMT::OperationResultCode code;
   std::string description;
   SMT::CheckIfCanAddTogether<double>(*matrix_4x4_1, *matrix_4x4_2, code, description);
   EXPECT_EQ(code, SMT::OperationResultCode::Ok);

   SMT::CheckIfCanAddTogether<double>(*matrix_4x4_1, *matrix_5x5_3, code, description);
   EXPECT_EQ(code, SMT::OperationResultCode::Error);

   const auto matrix_4x4_1plus2 = SMT::Add(*matrix_4x4_1, *matrix_4x4_2);
   EXPECT_EQ(matrix_4x4_1plus2.Code_, SMT::OperationResultCode::Ok);
   auto func1plus2 = [func1, func2](size_t row, size_t column)->double { return func1(row, column) + func2(row, column); };
   CheckForEachElement<double>(*matrix_4x4_1plus2.Matrix_, func1plus2, false, 1.0);
}