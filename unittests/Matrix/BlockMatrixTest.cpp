#include "../stdafx.h"

#include "MatrixTest.h"
#include "../../SMT/Matrix/BlockMatrix.h"

class BlockMatrixTest : public MatrixTest
{
protected:
   static SMT::Matrix<double>::SharedPtr CreateBlockMatrix(size_t rowCount, size_t columnCount, size_t blockSize, const std::function<double(size_t /*row*/, size_t /*column*/)>& func)
   {
      SMT::Matrix<double>::SharedPtr result = std::make_shared<SMT::BlockMatrix<double>>(rowCount, columnCount, blockSize, func);

      EXPECT_EQ(rowCount, result->RowCount());
      EXPECT_EQ(columnCount, result->ColumnCount());

      return result;
   }
};

TEST_F(BlockMatrixTest, IdentityMatrix)
{
   auto initFunc = [](size_t row, size_t column)->double { return row == column ? 1.0 : 0.0; };
   const size_t columnCount = 10;
   const size_t rowCount = 10;
   const size_t blockSize = 3;
   auto matrix = CreateBlockMatrix(rowCount, columnCount, blockSize, initFunc);
   ASSERT_TRUE(matrix != nullptr);
   CheckForEachElement<double>(*matrix, initFunc, true, SMT::MatrixSettings::One<double>());
}

TEST_F(BlockMatrixTest, ZeroMatrix)
{
   auto initFunc = [](size_t /*row*/, size_t /*column*/)->double { return 0.0; };
   const size_t columnCount = 10;
   const size_t rowCount = 10;
   const size_t blockSize = 3;
   auto matrix = CreateBlockMatrix(rowCount, columnCount, blockSize, initFunc);
   ASSERT_TRUE(matrix != nullptr);
   CheckForEachElement<double>(*matrix, initFunc, true, SMT::MatrixSettings::One<double>());
}

TEST_F(BlockMatrixTest, CopyMatrix)
{
   auto initFunc = [](size_t row, size_t column)->double { return static_cast<double>(row)* 100.0 + static_cast<double>(column); };
   const size_t columnCount = 50;
   const size_t rowCount = 40;
   const size_t blockSize = 13;
   auto originalMatrix = CreateBlockMatrix(rowCount, columnCount, blockSize, initFunc);
   ASSERT_TRUE(originalMatrix != nullptr);
   auto copyMatrixResult = originalMatrix->Copy();
   EXPECT_EQ(copyMatrixResult.Code_, SMT::OperationResultCode::Ok);
   auto copyMatrix = copyMatrixResult.Matrix_;
   ASSERT_TRUE(copyMatrix != nullptr);
   CheckEquality(*originalMatrix, *copyMatrix, true, SMT::MatrixSettings::One<double>());
}

TEST_F(BlockMatrixTest, CopyMatrix2)
{
   auto initFunc = [](size_t row, size_t column)->double { return static_cast<double>(row)* 99.0 + static_cast<double>(column); };
   const size_t columnCount = 30;
   const size_t rowCount = 40;
   const size_t blockSize = 11;
   auto originalMatrix = CreateBlockMatrix(rowCount, columnCount, blockSize, initFunc);
   ASSERT_TRUE(originalMatrix != nullptr);
   SMT::StandardMatrix<double> copyMatrix(*originalMatrix);
   CheckEquality(*originalMatrix, copyMatrix, true, SMT::MatrixSettings::One<double>());
}

TEST_F(BlockMatrixTest, AddingMatricesTogether)
{
   auto initFunc1 = [](size_t row, size_t column)->double { return static_cast<double>(row)* 100.0 + static_cast<double>(column); };
   auto initFunc2 = [](size_t row, size_t column)->double { return static_cast<double>(column)* 1000.0 + static_cast<double>(row); };
   const size_t columnCount = 50;
   const size_t rowCount = 40;
   const size_t blockSize = 7;
   auto matrix1 = CreateBlockMatrix(rowCount, columnCount, blockSize, initFunc1);
   auto matrix2 = CreateBlockMatrix(rowCount, columnCount, blockSize, initFunc2);

   auto result = matrix1->Add(*matrix2);
   EXPECT_EQ(result.Code_, SMT::OperationResultCode::Ok);
   auto resultMatrix = result.Matrix_;
   ASSERT_TRUE(resultMatrix != nullptr);
   EXPECT_EQ(resultMatrix->ColumnCount(), columnCount);
   EXPECT_EQ(resultMatrix->RowCount(), rowCount);

   auto resultFunc = [initFunc1, initFunc2](size_t row, size_t column) -> double { return initFunc1(row, column) + initFunc2(row, column); };
   CheckForEachElement<double>(*resultMatrix, resultFunc, false, SMT::MatrixSettings::One<double>());
}

TEST_F(BlockMatrixTest, MultiplicationByNumber)
{
   auto initFunc = [](size_t row, size_t column)->double { return static_cast<double>(row)* 10.0 + static_cast<double>(column); };
   const size_t columnCount = 51;
   const size_t rowCount = 41;
   const double number = 2.1;
   const size_t blockSize = 19;
   auto matrix = CreateBlockMatrix(rowCount, columnCount, blockSize, initFunc);

   auto result = matrix->MultiplyByNumber(number);
   EXPECT_EQ(result.Code_, SMT::OperationResultCode::Ok);
   auto resultMatrix = result.Matrix_;
   
   ASSERT_TRUE(resultMatrix != nullptr);
   EXPECT_EQ(resultMatrix->ColumnCount(), columnCount);
   EXPECT_EQ(resultMatrix->RowCount(), rowCount);

   auto resultFunc = [number, initFunc](size_t row, size_t column) -> double { return initFunc(row, column) * number; };
   CheckForEachElement<double>(*resultMatrix, resultFunc, false, SMT::MatrixSettings::One<double>());
}

TEST_F(BlockMatrixTest, Multiplication)
{
   // |1  2  3  4  5|     |1  0  0  0  0|     |1   4   9  16  25|
   // |0  1  2  3  4|     |0  2  0  0  0|     |0   2   6  12  20|
   // |0  0  1  2  3|  X  |0  0  3  0  0|  =  |0   0   3   8  15|
   // |0  0  0  1  2|     |0  0  0  4  0|     |0   0   0   4  10|
   // |0  0  0  0  1|     |0  0  0  0  5|     |0   0   0   0   5|
   auto initFunc1 = [](size_t row, size_t column)->double 
   { 
      return (column < row) ? 0.0 : (static_cast<double>(column - row) + 1.0);
   };
   auto initFunc2 = [](size_t row, size_t column)->double
   {
      return (row == column) ? (static_cast<double>(row) + 1.0) : 0.0;
   };
   const size_t columnCount = 5;
   const size_t rowCount = 5;
   const size_t blockSize = 2;
   auto matrix1 = CreateBlockMatrix(rowCount, columnCount, blockSize, initFunc1);
   auto matrix2 = CreateBlockMatrix(rowCount, columnCount, blockSize, initFunc2);

   auto result = matrix1->Multiply(*matrix2, false);
   EXPECT_EQ(result.Code_, SMT::OperationResultCode::Ok);
   auto resultMatrix = result.Matrix_;

   ASSERT_TRUE(resultMatrix != nullptr);
   EXPECT_EQ(resultMatrix->ColumnCount(), columnCount);
   EXPECT_EQ(resultMatrix->RowCount(), rowCount);

   auto resultFunc = [](size_t row, size_t column) -> double 
   { 
      if (column < row) {
         return 0.0;
      }
      const size_t diagonalNum = (column - row);
      return static_cast<double>((diagonalNum + 1) * (diagonalNum + row + 1));
   };
   CheckForEachElement<double>(*resultMatrix, resultFunc, false, SMT::MatrixSettings::One<double>());
}

TEST_F(BlockMatrixTest, Inversion1)
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
   const size_t columnCount = 3;
   const size_t rowCount = 3;
   const size_t blockSize = 2;
   auto matrix = CreateBlockMatrix(rowCount, columnCount, blockSize, initFunc);
   auto result = matrix->Invert();
   EXPECT_EQ(result.Code_, SMT::OperationResultCode::Ok);
   auto resultMatrix = result.Matrix_;

   ASSERT_TRUE(resultMatrix != nullptr);
   EXPECT_EQ(resultMatrix->ColumnCount(), columnCount);
   EXPECT_EQ(resultMatrix->RowCount(), rowCount);

   auto resultFunc = [](size_t row, size_t column) -> double
   {
      if (row == 0 && column == 0) return 1.0;  if (row == 0 && column == 1) return -2.0; if (row == 0 && column == 2) return  2.0;
      if (row == 1 && column == 0) return 0.0;  if (row == 1 && column == 1) return  0.0; if (row == 1 && column == 2) return  1.0;
      if (row == 2 && column == 0) return -0.5; if (row == 2 && column == 1) return  1.5; if (row == 2 && column == 2) return -1.0;
      return 100000.0;
   };
   CheckForEachElement<double>(*resultMatrix, resultFunc, false, SMT::MatrixSettings::One<double>());
}

TEST_F(BlockMatrixTest, Inversion2)
{
   //     |  1   0   0  ... 0  |
   //     |  2   1   0  ... 0  |
   // A = |  3   2   1  ... 0  |
   //     |  ................  |
   //     | 10   9   8  ... 1  |
   auto initFunc = [](size_t row, size_t column)->double
   {
      return (column > row) ? 0.0 : (static_cast<double>(row - column) + 1.0);
   };
   const size_t columnCount = 10;
   const size_t rowCount = 10;
   const size_t blockSize = 3;
   auto matrix = CreateBlockMatrix(rowCount, columnCount, blockSize, initFunc);
   auto result = matrix->Invert();
   EXPECT_EQ(result.Code_, SMT::OperationResultCode::Ok);
   auto resultMatrix = result.Matrix_;
   ASSERT_TRUE(resultMatrix != nullptr);
   EXPECT_EQ(resultMatrix->ColumnCount(), columnCount);
   EXPECT_EQ(resultMatrix->RowCount(), rowCount);

   auto identityFunc = [](size_t row, size_t column)->double 
   {
      return (column == row) ? 1.0 : 0.0;
   };

   auto identityMatrixResult1 = resultMatrix->Multiply(*matrix, true);
   EXPECT_EQ(identityMatrixResult1.Code_, SMT::OperationResultCode::Ok);
   auto identityMatrix1 = identityMatrixResult1.Matrix_;
   ASSERT_TRUE(identityMatrix1 != nullptr);
   EXPECT_EQ(identityMatrix1->ColumnCount(), columnCount);
   EXPECT_EQ(identityMatrix1->RowCount(), rowCount);
   CheckForEachElement<double>(*identityMatrix1, identityFunc, false, SMT::MatrixSettings::One<double>());
}

TEST_F(BlockMatrixTest, Transposition)
{
   auto initFunc = [](size_t row, size_t column)->double { return static_cast<double>(row)* 10.0 + static_cast<double>(column); };
   const size_t columnCount = 51;
   const size_t rowCount = 41;
   const size_t blockSize = 3;
   auto matrix = CreateBlockMatrix(rowCount, columnCount, blockSize, initFunc);

   auto result = matrix->Transpose();
   EXPECT_EQ(result.Code_, SMT::OperationResultCode::Ok);
   auto resultMatrix = result.Matrix_;

   ASSERT_TRUE(resultMatrix != nullptr);
   EXPECT_EQ(resultMatrix->ColumnCount(), rowCount);
   EXPECT_EQ(resultMatrix->RowCount(), columnCount);

   auto resultFunc = [initFunc](size_t row, size_t column) -> double { return initFunc(column, row); };
   CheckForEachElement<double>(*resultMatrix, resultFunc, true, SMT::MatrixSettings::One<double>());
}

TEST_F(BlockMatrixTest, Determination1)
{
   //     |  1   0   0  ... 0  |
   //     |  2   1   0  ... 0  |
   // A = |  3   2   1  ... 0  |
   //     |  ................  |
   //     | 50  49  48  ... 1  |
   auto initFunc = [](size_t row, size_t column)->double { return (column > row) ? 0.0 : (static_cast<double>(row - column) + 1.0); };
   const size_t columnCount = 50;
   const size_t rowCount = 50;
   const size_t blockSize = 13;
   auto matrix = CreateBlockMatrix(rowCount, columnCount, blockSize, initFunc);

   auto result = matrix->Determinant();
   EXPECT_EQ(result.Code_, SMT::OperationResultCode::Ok);
   auto value = result.Value_;

   ASSERT_TRUE(value != nullptr);
   CheckEquality<double>(*value, 1.0, false, 1.0);
}

TEST_F(BlockMatrixTest, Determination2)
{
   //     |  1   2   4   3   0  |
   //     |  2   1  -1   1   3  |
   // A = |  4  -1  -2   5   1  |
   //     |  7   3   6   2   1  |
   //     |  1   0  -1   1   1  |
   auto initFunc = [](size_t row, size_t column) -> double
   {
      if (row == 0 && column == 0) return 1.0;  if (row == 0 && column == 1) return  2.0; if (row == 0 && column == 2) return  4.0; if (row == 0 && column == 3) return  3.0; if (row == 0 && column == 4) return  0.0;
      if (row == 1 && column == 0) return 2.0;  if (row == 1 && column == 1) return  1.0; if (row == 1 && column == 2) return -1.0; if (row == 1 && column == 3) return  1.0; if (row == 1 && column == 4) return  3.0;
      if (row == 2 && column == 0) return 4.0;  if (row == 2 && column == 1) return -1.0; if (row == 2 && column == 2) return -2.0; if (row == 2 && column == 3) return  5.0; if (row == 2 && column == 4) return  1.0;
      if (row == 3 && column == 0) return 7.0;  if (row == 3 && column == 1) return  3.0; if (row == 3 && column == 2) return  6.0; if (row == 3 && column == 3) return  2.0; if (row == 3 && column == 4) return  1.0;
      if (row == 4 && column == 0) return 1.0;  if (row == 4 && column == 1) return  0.0; if (row == 4 && column == 2) return -1.0; if (row == 4 && column == 3) return  1.0; if (row == 4 && column == 4) return  1.0;
      return 100000.0;
   };
   const size_t columnCount = 5;
   const size_t rowCount = 5;
   const size_t blockSize = 3;
   auto matrix = CreateBlockMatrix(rowCount, columnCount, blockSize, initFunc);

   auto result = matrix->Determinant();
   EXPECT_EQ(result.Code_, SMT::OperationResultCode::Ok);
   auto value = result.Value_;

   ASSERT_TRUE(value != nullptr);
   CheckEquality<double>(*value, -34.0, false, 1.0);
}

TEST_F(BlockMatrixTest, InversionAndDetermination)
{
   //     |  1   2   4   3   0  |
   //     |  2   1  -1   1   3  |
   // A = |  4  -1  -2   5   1  |
   //     |  7   3   6   2   1  |
   //     |  1   0  -1   1   1  |
   // We'll check that |A * A^(-1)| = |A|*|A^(-1)| = |I| = 1
   auto initFunc = [](size_t row, size_t column) -> double
   {
      if (row == 0 && column == 0) return 1.0;  if (row == 0 && column == 1) return  2.0; if (row == 0 && column == 2) return  4.0; if (row == 0 && column == 3) return  3.0; if (row == 0 && column == 4) return  0.0;
      if (row == 1 && column == 0) return 2.0;  if (row == 1 && column == 1) return  1.0; if (row == 1 && column == 2) return -1.0; if (row == 1 && column == 3) return  1.0; if (row == 1 && column == 4) return  3.0;
      if (row == 2 && column == 0) return 4.0;  if (row == 2 && column == 1) return -1.0; if (row == 2 && column == 2) return -2.0; if (row == 2 && column == 3) return  5.0; if (row == 2 && column == 4) return  1.0;
      if (row == 3 && column == 0) return 7.0;  if (row == 3 && column == 1) return  3.0; if (row == 3 && column == 2) return  6.0; if (row == 3 && column == 3) return  2.0; if (row == 3 && column == 4) return  1.0;
      if (row == 4 && column == 0) return 1.0;  if (row == 4 && column == 1) return  0.0; if (row == 4 && column == 2) return -1.0; if (row == 4 && column == 3) return  1.0; if (row == 4 && column == 4) return  1.0;
      return 100000.0;
   };
   const size_t columnCount = 5;
   const size_t rowCount = 5;
   const size_t blockSize = 2;
   auto matrix = CreateBlockMatrix(rowCount, columnCount, blockSize, initFunc);
   auto matrixDeterminant = matrix->Determinant();
   EXPECT_EQ(matrixDeterminant.Code_, SMT::OperationResultCode::Ok);
   ASSERT_TRUE(matrixDeterminant.Value_ != nullptr);

   auto invertedMatrixResult = matrix->Invert();
   EXPECT_EQ(invertedMatrixResult.Code_, SMT::OperationResultCode::Ok);
   auto invertedMatrix = invertedMatrixResult.Matrix_;
   auto invertedMatrixDeterminant = invertedMatrix->Determinant();
   EXPECT_EQ(invertedMatrixDeterminant.Code_, SMT::OperationResultCode::Ok);
   ASSERT_TRUE(invertedMatrixDeterminant.Value_ != nullptr);

   auto result = matrix->Multiply(*invertedMatrix, false);
   EXPECT_EQ(result.Code_, SMT::OperationResultCode::Ok);
   auto resultMatrix = result.Matrix_;
   ASSERT_TRUE(resultMatrix != nullptr);
   auto identityFunc = [](size_t row, size_t column)->double { return (column == row) ? 1.0 : 0.0; };
   CheckForEachElement<double>(*resultMatrix, SMT::MatrixSettings::IdentityMatrixFunction<double>(), false, 100.0);

   auto resultDeterminent = resultMatrix->Determinant();
   EXPECT_EQ(resultDeterminent.Code_, SMT::OperationResultCode::Ok);
   ASSERT_TRUE(resultDeterminent.Value_ != nullptr);

   CheckEquality<double>(*resultDeterminent.Value_, (*matrixDeterminant.Value_) * (*invertedMatrixDeterminant.Value_), false, 100.0);
   CheckEquality<double>(1.0, (*matrixDeterminant.Value_) * (*invertedMatrixDeterminant.Value_), false, 10.0);
   CheckEquality<double>(*resultDeterminent.Value_, 1.0, false, 10.0);
}