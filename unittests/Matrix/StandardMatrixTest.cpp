#include "../stdafx.h"

#include "gtest/gtest.h"

#include "../../SMT/Matrix/MatrixOperations.h"
#include "../../SMT/Matrix/StandardMatrix.h"

class StandardMatrixTest : public ::testing::Test 
{
protected:
   static SMT::Matrix<double>::SharedPtr createStandardMatrix(size_t rowCount, size_t columnCount, const std::function<double(size_t /*row*/, size_t /*column*/)>& func)
   {
      SMT::Matrix<double>::SharedPtr result = std::make_shared<SMT::StandardMatrix<double>>(rowCount, columnCount, func);

      EXPECT_EQ(rowCount, result->RowCount());
      EXPECT_EQ(columnCount, result->ColumnCount());

      return result;
   }
   static void checkForEachElement(const SMT::Matrix<double>& matrix, const std::function<double(size_t /*row*/, size_t /*column*/)>& func, bool epsilonIsZero)
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
			     ASSERT_TRUE(SMT::MatrixSettings::CanAssumeItIsZero<double>(matrix.Element(i, j) - func(i, j)));
			  }
		  }
	   }
   }
   static void checkEquality(const SMT::Matrix<double>& matrix1, const SMT::Matrix<double>& matrix2, bool epsilonIsZero)
   {
       EXPECT_EQ(matrix1.TypeName(), matrix2.TypeName());
       EXPECT_EQ(matrix1.ColumnCount(), matrix2.ColumnCount());
       EXPECT_EQ(matrix1.RowCount(), matrix2.RowCount());

	   const size_t columnCount = matrix1.ColumnCount();
	   const size_t rowCount = matrix1.RowCount();

	   for (size_t i = 0; i < rowCount; ++i)
	   {
		  for (size_t j = 0; j < columnCount; ++j)
		  {
			  if (epsilonIsZero)
			  {
	              EXPECT_EQ(matrix1.Element(i, j), matrix2.Element(i, j));
			  }
			  else
			  {
				  ASSERT_TRUE(SMT::MatrixSettings::CanAssumeItIsZero<double>(matrix1.Element(i, j) - matrix2.Element(i, j)));
			  }  
		  }
	   }
   {  
};

TEST_F(StandardMatrixTest, IdentityMatrix)
{
   auto initFunc = [](size_t row, size_t column)->double { return row == column ? 1.0 : 0.0; };
   const size_t columnCount = 10;
   const size_t rowCount = 10;
   auto matrix = createStandardMatrix(rowCount, columnCount, initFunc);
   ASSERT_TRUE(matrix != nullptr);
   checkForEachElement(*matrix, initFunc, true);
}

TEST_F(StandardMatrixTest, ZeroMatrix)
{
   auto initFunc = [](size_t /*row*/, size_t /*column*/)->double { return 0.0; };
   const size_t columnCount = 10;
   const size_t rowCount = 10;
   auto matrix = createStandardMatrix(rowCount, columnCount, initFunc);
   ASSERT_TRUE(matrix != nullptr);
   checkForEachElement(*matrix, initFunc, true);
}

TEST_F(StandardMatrixTest, CopyMatrix)
{
   auto initFunc = [](size_t row, size_t column)->double { return static_cast<double>(row)* 100.0 + static_cast<double>(column); };
   const size_t columnCount = 50;
   const size_t rowCount = 40;
   auto originalMatrix = createStandardMatrix(rowCount, columnCount, initFunc);
   ASSERT_TRUE(originalMatrix != nullptr);
   auto copyMatrixResult = SMT::Copy(*originalMatrix);
   EXPECT_EQ(copyMatrixResult.Code_, SMT::OperationResultCode::Ok);
   auto copyMatrix = copyMatrixResult.Matrix_;
   ASSERT_TRUE(copyMatrix != nullptr);
   checkEquality(*originalMatrix, *copyMatrix, true);
}

TEST_F(StandardMatrixTest, CopyMatrix2)
{
   auto initFunc = [](size_t row, size_t column)->double { return static_cast<double>(row)* 99.0 + static_cast<double>(column); };
   const size_t columnCount = 30;
   const size_t rowCount = 40;
   auto originalMatrix = createStandardMatrix(rowCount, columnCount, initFunc);
   ASSERT_TRUE(originalMatrix != nullptr);
   SMT::StandardMatrix<double> copyMatrix(*originalMatrix);
   checkEquality(*originalMatrix, copyMatrix, true);
}

TEST_F(StandardMatrixTest, AddingMatricesTogether)
{
   auto initFunc1 = [](size_t row, size_t column)->double { return static_cast<double>(row)* 100.0 + static_cast<double>(column); };
   auto initFunc2 = [](size_t row, size_t column)->double { return static_cast<double>(column)* 1000.0 + static_cast<double>(row); };
   const size_t columnCount = 50;
   const size_t rowCount = 40;
   auto matrix1 = createStandardMatrix(rowCount, columnCount, initFunc1);
   auto matrix2 = createStandardMatrix(rowCount, columnCount, initFunc2);

   auto result = SMT::Add(*matrix1, *matrix2);
   EXPECT_EQ(result.Code_, SMT::OperationResultCode::Ok);
   auto resultMatrix = result.Matrix_;
   ASSERT_TRUE(resultMatrix != nullptr);
   EXPECT_EQ(resultMatrix->ColumnCount(), columnCount);
   EXPECT_EQ(resultMatrix->RowCount(), rowCount);

   auto resultFunc = [initFunc1, initFunc2](size_t row, size_t column) -> double { return initFunc1(row, column) + initFunc2(row, column); };
   checkForEachElement(*resultMatrix, resultFunc, false);
}

TEST_F(StandardMatrixTest, MultiplicationByNumber)
{
   auto initFunc = [](size_t row, size_t column)->double { return static_cast<double>(row)* 10.0 + static_cast<double>(column); };
   const size_t columnCount = 51;
   const size_t rowCount = 41;
   const double number = 2.1;
   auto matrix = createStandardMatrix(rowCount, columnCount, initFunc);

   auto result = SMT::MultiplyByNumber(*matrix, number);
   EXPECT_EQ(result.Code_, SMT::OperationResultCode::Ok);
   auto resultMatrix = result.Matrix_;
   
   ASSERT_TRUE(resultMatrix != nullptr);
   EXPECT_EQ(resultMatrix->ColumnCount(), columnCount);
   EXPECT_EQ(resultMatrix->RowCount(), rowCount);

   auto resultFunc = [number, initFunc](size_t row, size_t column) -> double { return initFunc(row, column) * number; };
   checkForEachElement(*resultMatrix, resultFunc, false);   
}
