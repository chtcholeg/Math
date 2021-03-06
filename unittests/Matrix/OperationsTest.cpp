#include "../stdafx.h"
#include "gtest/gtest.h"

#include "../../SMT/Matrix/MatrixOperations.h"
#include "MatrixTest.h"

class OperationsTest : public MatrixTest
{
protected:
   static SMT::Matrix<double>::SharedPtr CreateStandardMatrix(size_t rowCount, size_t columnCount, const std::function<double(size_t /*row*/, size_t /*column*/)>& func)
   {
      SMT::Matrix<double>::SharedPtr result = std::make_shared<SMT::StandardMatrix<double>>(rowCount, columnCount, func);

      EXPECT_EQ(rowCount, result->RowCount());
      EXPECT_EQ(columnCount, result->ColumnCount());

      return result;
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

TEST_F(OperationsTest, Multiplication)
{
   auto func1 = [](size_t row, size_t column)->double
   {
      return static_cast<double>(row) * static_cast<double>(column) + 1;
   };
   const auto matrix_4x4_1 = CreateStandardMatrix(4, 4, func1);
   auto func2 = [](size_t row, size_t column)->double
   {
      return static_cast<double>(row)+static_cast<double>(column)+1;
   };
   const auto matrix_4x4_2 = CreateStandardMatrix(4, 4, func2);
   auto func3 = [](size_t row, size_t column)->double
   {
      return static_cast<double>(row) / (static_cast<double>(column)+1.0);
   };
   const auto matrix_5x5_3 = CreateStandardMatrix(5, 5, func3);

   SMT::OperationResultCode code;
   std::string description;
   SMT::CheckIfCanMultiplyTogether<double>(*matrix_4x4_1, *matrix_4x4_2, code, description);
   EXPECT_EQ(code, SMT::OperationResultCode::Ok);

   SMT::CheckIfCanMultiplyTogether<double>(*matrix_4x4_1, *matrix_5x5_3, code, description);
   EXPECT_EQ(code, SMT::OperationResultCode::Error);

   const auto matrix_4x4_1mult2 = SMT::Multiply(*matrix_4x4_1, *matrix_4x4_2);
   EXPECT_EQ(matrix_4x4_1mult2.Code_, SMT::OperationResultCode::Ok);
   auto func1mult2 = [func1, func2](size_t row, size_t column)->double 
   { 
      double result = 0.0;
      for (size_t i = 0; i < 4; ++i)
      {
         result += func1(row, i) * func2(i, column);
      }
      return result;
   };
   CheckForEachElement<double>(*matrix_4x4_1mult2.Matrix_, func1mult2, false, 1.0);
}

TEST_F(OperationsTest, MultiplicationByNumber)
{
   auto func = [](size_t row, size_t column)->double
   {
      return static_cast<double>(row) * static_cast<double>(column) + 1;
   };
   const auto matrix_4x4 = CreateStandardMatrix(4, 4, func);
   const double number = 1.9;

   std::string description;
   const auto result = SMT::MultiplyByNumber(*matrix_4x4, number);
   EXPECT_EQ(result.Code_, SMT::OperationResultCode::Ok);
   auto funcMult = [func, number](size_t row, size_t column)->double
   {
      return func(row, column) * number;
   };
   CheckForEachElement<double>(*result.Matrix_, funcMult, false, 1.0);
}

TEST_F(OperationsTest, Invert)
{
   auto func = [](size_t row, size_t column)->double
   {
      return 1.0 / (static_cast<double>(row) * static_cast<double>(column) + 1.0);
   };
   const auto matrix = CreateStandardMatrix(4, 4, func);

   const auto result = SMT::Invert(*matrix);
   EXPECT_EQ(result.Code_, SMT::OperationResultCode::Ok);
   const auto multMatrixResult = SMT::Multiply(*matrix, *result.Matrix_);
   EXPECT_EQ(multMatrixResult.Code_, SMT::OperationResultCode::Ok);
   auto identityFunc = [](size_t row, size_t column)->double
   {
      return (column == row) ? 1.0 : 0.0;
   };
   const auto idenityMatrix = CreateStandardMatrix(4, 4, identityFunc);
   CheckEquality<double>(*multMatrixResult.Matrix_, *idenityMatrix, false, 1000.0);
}

TEST_F(OperationsTest, Transpose)
{
   auto func = [](size_t row, size_t column)->double
   {
      return static_cast<double>(column) + 1.0 / (static_cast<double>(row) + 1.0);
   };
   const auto matrix = CreateStandardMatrix(4, 4, func);

   const auto result = SMT::Transpose(*matrix);
   EXPECT_EQ(result.Code_, SMT::OperationResultCode::Ok);

   auto requiredFunc = [func](size_t row, size_t column)->double
   {
      return func(column, row);
   };
   const auto requiredMatrix = CreateStandardMatrix(4, 4, requiredFunc);
   CheckEquality<double>(*result.Matrix_, *requiredMatrix, true, 1.0);
}

TEST_F(OperationsTest, Determinant)
{
   const size_t size = 3;
   auto func = [size](size_t row, size_t column)->double
   {
      return (row + column < size - 1) ? 0.0 : (row + column - static_cast<double>(size - 2));
   };
   const auto matrix = CreateStandardMatrix(size, size, func);
   const auto result = SMT::Determinant(*matrix);
   EXPECT_EQ(result.Code_, SMT::OperationResultCode::Ok);
   ASSERT_TRUE(result.Value_ != nullptr);
   ASSERT_TRUE(SMT::MatrixSettings::CanAssumeItIsZero<double>(*result.Value_ + 1.0, 1.0));
}