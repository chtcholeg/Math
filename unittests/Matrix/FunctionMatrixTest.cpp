#include "../stdafx.h"

#include "MatrixTest.h"

#include "../../SMT/Matrix/FunctionMatrix.h"

class FunctionMatrixTest : public MatrixTest
{
protected:
   static SMT::Matrix<double>::SharedPtr CreateFunctionMatrix(size_t rowCount, size_t columnCount, const std::function<double(size_t /*row*/, size_t /*column*/)>& func)
   {
      SMT::Matrix<double>::SharedPtr result = std::make_shared<SMT::FunctionMatrix<double>>(rowCount, columnCount, func);

      EXPECT_EQ(rowCount, result->RowCount());
      EXPECT_EQ(columnCount, result->ColumnCount());

      return result;
   }
};

TEST_F(FunctionMatrixTest, IdentityMatrix)
{
   auto initFunc = [](size_t row, size_t column)->double { return row == column ? 1.0 : 0.0; };
   const size_t columnCount = 1000;
   const size_t rowCount = 1000;
   auto matrix = CreateFunctionMatrix(rowCount, columnCount, initFunc);
   ASSERT_TRUE(matrix != nullptr);
   CheckForEachElement<double>(*matrix, initFunc, false, 0.0);
}

TEST_F(FunctionMatrixTest, ZeroMatrix)
{
   auto initFunc = [](size_t /*row*/, size_t /*column*/)->double { return 0.0; };
   const size_t columnCount = 1000;
   const size_t rowCount = 1000;
   auto matrix = CreateFunctionMatrix(rowCount, columnCount, initFunc);
   ASSERT_TRUE(matrix != nullptr);
   CheckForEachElement<double>(*matrix, initFunc, false, 0.0);
}

TEST_F(FunctionMatrixTest, CopyMatrix)
{
   auto initFunc = [](size_t row, size_t column)->double { return static_cast<double>(row) * 100.0 + static_cast<double>(column); };
   const size_t columnCount = 50;
   const size_t rowCount = 40;
   auto originalMatrix = CreateFunctionMatrix(rowCount, columnCount, initFunc);
   ASSERT_TRUE(originalMatrix != nullptr);
   auto copyMatrixResult = originalMatrix->Copy();
   EXPECT_EQ(copyMatrixResult.Code_, SMT::OperationResultCode::Ok);
   auto copyMatrix = copyMatrixResult.Matrix_;
   ASSERT_TRUE(copyMatrix != nullptr);
   CheckEquality(*originalMatrix, *copyMatrix, false, 0.0);
}

TEST_F(FunctionMatrixTest, AddingMatricesTogether)
{
   auto initFunc1 = [](size_t row, size_t column)->double { return static_cast<double>(row) * 100.0 + static_cast<double>(column); };
   auto initFunc2 = [](size_t row, size_t column)->double { return static_cast<double>(column) * 1000.0 + static_cast<double>(row); };
   const size_t columnCount = 50;
   const size_t rowCount = 40;
   auto matrix1 = CreateFunctionMatrix(rowCount, columnCount, initFunc1);
   auto matrix2 = CreateFunctionMatrix(rowCount, columnCount, initFunc2);

   auto result = matrix1->Add(*matrix2);
   EXPECT_EQ(result.Code_, SMT::OperationResultCode::Ok);
   auto resultMatrix = result.Matrix_;
   ASSERT_TRUE(resultMatrix != nullptr);
   EXPECT_EQ(resultMatrix->ColumnCount(), columnCount);
   EXPECT_EQ(resultMatrix->RowCount(), rowCount);

   auto resultFunc = [initFunc1, initFunc2](size_t row, size_t column) -> double { return initFunc1(row, column) + initFunc2(row, column); };
   CheckForEachElement<double>(*resultMatrix, resultFunc, false, SMT::MatrixSettings::One<double>());
}

TEST_F(FunctionMatrixTest, MultiplicationByNumber)
{
   auto initFunc = [](size_t row, size_t column)->double { return static_cast<double>(row)* 10.0 + static_cast<double>(column); };
   const size_t columnCount = 51;
   const size_t rowCount = 41;
   const double number = 2.1;
   auto matrix = CreateFunctionMatrix(rowCount, columnCount, initFunc);

   auto result = matrix->MultiplyByNumber(number);
   EXPECT_EQ(result.Code_, SMT::OperationResultCode::Ok);
   auto resultMatrix = result.Matrix_;
   
   ASSERT_TRUE(resultMatrix != nullptr);
   EXPECT_EQ(resultMatrix->ColumnCount(), columnCount);
   EXPECT_EQ(resultMatrix->RowCount(), rowCount);

   auto resultFunc = [number, initFunc](size_t row, size_t column) -> double { return initFunc(row, column) * number; };
   CheckForEachElement<double>(*resultMatrix, resultFunc, false, SMT::MatrixSettings::One<double>());
}

TEST_F(FunctionMatrixTest, Multiplication)
{
   // |1  2|     |1  2|     |11  8 |
   // |    |  X  |    |  =  |      |
   // |3  4|     |5  3|     |23  18|
   auto initFunc1 = [](size_t row, size_t column)->double 
   { 
      if (row == 0 && column == 0) return 1.0; if (row == 0 && column == 1) return 2.0;
      if (row == 1 && column == 0) return 3.0; if (row == 1 && column == 1) return 4.0;
      return 0.0;
   };
   auto initFunc2 = [](size_t row, size_t column)->double
   {
      if (row == 0 && column == 0) return 1.0; if (row == 0 && column == 1) return 2.0;
      if (row == 1 && column == 0) return 5.0; if (row == 1 && column == 1) return 3.0;
      return 0.0;
   };
   const size_t columnCount = 2;
   const size_t rowCount = 2;
   auto matrix1 = CreateFunctionMatrix(rowCount, columnCount, initFunc1);
   auto matrix2 = CreateFunctionMatrix(rowCount, columnCount, initFunc2);

   auto result = matrix1->Multiply(*matrix2, false);
   EXPECT_EQ(result.Code_, SMT::OperationResultCode::NotImplemented);
}

TEST_F(FunctionMatrixTest, Inversion)
{
   //     |  3  -2   4  |                 |   1   -2    2  |
   // A = |  1   0   2  |    ;   A^(-1) = |   0    0    1  |
   //     |  0   1   0  |                 | -0.5  1.5  -1  |
   auto initFunc = [](size_t row, size_t column)->double
   {
      if (row == 0 && column == 0) return 3.0; if (row == 0 && column == 1) return -2.0; if (row == 0 && column == 2) return 4.0;
      if (row == 1 && column == 0) return 1.0; if (row == 1 && column == 1) return  0.0; if (row == 1 && column == 2) return 2.0;
      if (row == 2 && column == 0) return 0.0; if (row == 2 && column == 1) return  1.0; if (row == 2 && column == 2) return 0.0;
      return 0.0;
   };
   auto matrix = CreateFunctionMatrix(3, 3, initFunc);
   auto result = matrix->Invert();
   EXPECT_EQ(result.Code_, SMT::OperationResultCode::NotImplemented);
}

TEST_F(FunctionMatrixTest, Transposition)
{
   auto initFunc = [](size_t row, size_t column)->double { return static_cast<double>(row)* 10.0 + static_cast<double>(column); };
   const size_t columnCount = 51;
   const size_t rowCount = 41;
   auto matrix = CreateFunctionMatrix(rowCount, columnCount, initFunc);

   auto result = matrix->Transpose();
   EXPECT_EQ(result.Code_, SMT::OperationResultCode::Ok);
   auto resultMatrix = result.Matrix_;

   ASSERT_TRUE(resultMatrix != nullptr);
   EXPECT_EQ(resultMatrix->ColumnCount(), rowCount);
   EXPECT_EQ(resultMatrix->RowCount(), columnCount);

   auto resultFunc = [initFunc](size_t row, size_t column) -> double { return initFunc(column, row); };
   CheckForEachElement<double>(*resultMatrix, resultFunc, true, SMT::MatrixSettings::One<double>());
}

TEST_F(FunctionMatrixTest, Determination)
{
   //     |  1   0   0  ... 0  |
   //     |  2   1   0  ... 0  |
   // A = |  3   2   1  ... 0  |
   //     |  ................  |
   //     | 50  49  48  ... 1  |
   auto initFunc = [](size_t row, size_t column)->double { return (column > row) ? 0.0 : (static_cast<double>(row - column) + 1.0); };
   const size_t columnCount = 50;
   const size_t rowCount = 50;
   auto matrix = CreateFunctionMatrix(rowCount, columnCount, initFunc);

   auto result = matrix->Determinant();
   EXPECT_EQ(result.Code_, SMT::OperationResultCode::NotImplemented);
}
