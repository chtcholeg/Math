#include "../stdafx.h"

#include "MatrixTest.h"

class StandardMatrixTest : public MatrixTest
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

TEST_F(StandardMatrixTest, IdentityMatrix)
{
   auto initFunc = [](size_t row, size_t column)->double { return row == column ? 1.0 : 0.0; };
   const size_t columnCount = 10;
   const size_t rowCount = 10;
   auto matrix = CreateStandardMatrix(rowCount, columnCount, initFunc);
   ASSERT_TRUE(matrix != nullptr);
   CheckForEachElement(*matrix, initFunc, true);
}

TEST_F(StandardMatrixTest, ZeroMatrix)
{
   auto initFunc = [](size_t /*row*/, size_t /*column*/)->double { return 0.0; };
   const size_t columnCount = 10;
   const size_t rowCount = 10;
   auto matrix = CreateStandardMatrix(rowCount, columnCount, initFunc);
   ASSERT_TRUE(matrix != nullptr);
   CheckForEachElement(*matrix, initFunc, true);
}

TEST_F(StandardMatrixTest, CopyMatrix)
{
   auto initFunc = [](size_t row, size_t column)->double { return static_cast<double>(row)* 100.0 + static_cast<double>(column); };
   const size_t columnCount = 50;
   const size_t rowCount = 40;
   auto originalMatrix = CreateStandardMatrix(rowCount, columnCount, initFunc);
   ASSERT_TRUE(originalMatrix != nullptr);
   auto copyMatrixResult = SMT::Copy(*originalMatrix);
   EXPECT_EQ(copyMatrixResult.Code_, SMT::OperationResultCode::Ok);
   auto copyMatrix = copyMatrixResult.Matrix_;
   ASSERT_TRUE(copyMatrix != nullptr);
   CheckEquality(*originalMatrix, *copyMatrix, true);
}

TEST_F(StandardMatrixTest, CopyMatrix2)
{
   auto initFunc = [](size_t row, size_t column)->double { return static_cast<double>(row)* 99.0 + static_cast<double>(column); };
   const size_t columnCount = 30;
   const size_t rowCount = 40;
   auto originalMatrix = CreateStandardMatrix(rowCount, columnCount, initFunc);
   ASSERT_TRUE(originalMatrix != nullptr);
   SMT::StandardMatrix<double> copyMatrix(*originalMatrix);
   CheckEquality(*originalMatrix, copyMatrix, true);
}

TEST_F(StandardMatrixTest, AddingMatricesTogether)
{
   auto initFunc1 = [](size_t row, size_t column)->double { return static_cast<double>(row)* 100.0 + static_cast<double>(column); };
   auto initFunc2 = [](size_t row, size_t column)->double { return static_cast<double>(column)* 1000.0 + static_cast<double>(row); };
   const size_t columnCount = 50;
   const size_t rowCount = 40;
   auto matrix1 = CreateStandardMatrix(rowCount, columnCount, initFunc1);
   auto matrix2 = CreateStandardMatrix(rowCount, columnCount, initFunc2);

   auto result = SMT::Add(*matrix1, *matrix2);
   EXPECT_EQ(result.Code_, SMT::OperationResultCode::Ok);
   auto resultMatrix = result.Matrix_;
   ASSERT_TRUE(resultMatrix != nullptr);
   EXPECT_EQ(resultMatrix->ColumnCount(), columnCount);
   EXPECT_EQ(resultMatrix->RowCount(), rowCount);

   auto resultFunc = [initFunc1, initFunc2](size_t row, size_t column) -> double { return initFunc1(row, column) + initFunc2(row, column); };
   CheckForEachElement(*resultMatrix, resultFunc, false);
}

TEST_F(StandardMatrixTest, MultiplicationByNumber)
{
   auto initFunc = [](size_t row, size_t column)->double { return static_cast<double>(row)* 10.0 + static_cast<double>(column); };
   const size_t columnCount = 51;
   const size_t rowCount = 41;
   const double number = 2.1;
   auto matrix = CreateStandardMatrix(rowCount, columnCount, initFunc);

   auto result = SMT::MultiplyByNumber(*matrix, number);
   EXPECT_EQ(result.Code_, SMT::OperationResultCode::Ok);
   auto resultMatrix = result.Matrix_;
   
   ASSERT_TRUE(resultMatrix != nullptr);
   EXPECT_EQ(resultMatrix->ColumnCount(), columnCount);
   EXPECT_EQ(resultMatrix->RowCount(), rowCount);

   auto resultFunc = [number, initFunc](size_t row, size_t column) -> double { return initFunc(row, column) * number; };
   CheckForEachElement(*resultMatrix, resultFunc, false);   
}

TEST_F(StandardMatrixTest, Multiply)
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
   auto matrix1 = CreateStandardMatrix(rowCount, columnCount, initFunc1);
   auto matrix2 = CreateStandardMatrix(rowCount, columnCount, initFunc2);

   auto result = SMT::Multiply(*matrix1, *matrix2);
   EXPECT_EQ(result.Code_, SMT::OperationResultCode::Ok);
   auto resultMatrix = result.Matrix_;

   ASSERT_TRUE(resultMatrix != nullptr);
   EXPECT_EQ(resultMatrix->ColumnCount(), 2);
   EXPECT_EQ(resultMatrix->RowCount(), 2);

   auto resultFunc = [](size_t row, size_t column) -> double 
   { 
      if (row == 0 && column == 0) return 11.0; if (row == 0 && column == 1) return 8.0;
      if (row == 1 && column == 0) return 23.0; if (row == 1 && column == 1) return 18.0;
      return 100000.0;
   };
   CheckForEachElement(*resultMatrix, resultFunc, false);
}
