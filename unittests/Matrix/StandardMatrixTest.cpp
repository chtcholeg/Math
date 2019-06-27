#include "../stdafx.h"

#include "gtest/gtest.h"

#include "../../SMT/Matrix/MatrixOperations.h"
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
         const double diff = matrix->Element(i, j) - (i == j ? SMT::MatrixSettings::One<double>() : SMT::MatrixSettings::Zero<double>());
         ASSERT_TRUE(SMT::MatrixSettings::CanAssumeItIsZero<double>(diff));
      }
   }
}

TEST_F(StandardMatrixTest, ZeroMatrix)
{
   auto initFunc = [](size_t /*row*/, size_t /*column*/)->double { return 0.0; };
   const size_t columnCount = 10;
   const size_t rowCount = 10;
   auto matrix = createStandardMatrix(rowCount, columnCount, initFunc);
   for (size_t i = 0; i < rowCount; ++i)
   {
      for (size_t j = 0; j < columnCount; ++j)
      {
         ASSERT_TRUE(SMT::MatrixSettings::CanAssumeItIsZero<double>(matrix->Element(i, j)));
      }
   }
}

TEST_F(StandardMatrixTest, CopyMatrix)
{
   auto initFunc = [](size_t row, size_t column)->double { return static_cast<double>(row)* 100.0 + static_cast<double>(column); };
   const size_t columnCount = 50;
   const size_t rowCount = 40;
   auto originalMatrix = createStandardMatrix(rowCount, columnCount, initFunc);
   auto copyMatrixResult = SMT::Copy(*originalMatrix);
   EXPECT_EQ(copyMatrixResult.Code_, SMT::OperationResultCode::Ok);
   auto copyMatrix = copyMatrixResult.Matrix_;
   ASSERT_TRUE(copyMatrix != nullptr);
   EXPECT_EQ(copyMatrix->TypeName(), originalMatrix->TypeName());
   EXPECT_EQ(copyMatrix->ColumnCount(), originalMatrix->ColumnCount());
   EXPECT_EQ(copyMatrix->RowCount(), originalMatrix->RowCount());

   for (size_t i = 0; i < rowCount; ++i)
   {
      for (size_t j = 0; j < columnCount; ++j)
      {
         EXPECT_EQ(copyMatrix->Element(i, j), originalMatrix->Element(i, j));
      }
   }
}

TEST_F(StandardMatrixTest, CopyMatrix2)
{
   auto initFunc = [](size_t row, size_t column)->double { return static_cast<double>(row)* 99.0 + static_cast<double>(column); };
   const size_t columnCount = 30;
   const size_t rowCount = 40;
   auto originalMatrix = createStandardMatrix(rowCount, columnCount, initFunc);
   SMT::StandardMatrix<double> copyMatrix = SMT::StandardMatrix<double>(*originalMatrix);
   EXPECT_EQ(copyMatrix.TypeName(), originalMatrix->TypeName());
   EXPECT_EQ(copyMatrix.ColumnCount(), originalMatrix->ColumnCount());
   EXPECT_EQ(copyMatrix.RowCount(), originalMatrix->RowCount());

   for (size_t i = 0; i < rowCount; ++i)
   {
      for (size_t j = 0; j < columnCount; ++j)
      {
         EXPECT_EQ(copyMatrix.Element(i, j), originalMatrix->Element(i, j));
      }
   }
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

   for (size_t i = 0; i < rowCount; ++i)
   {
      for (size_t j = 0; j < columnCount; ++j)
      {
         ASSERT_TRUE(SMT::MatrixSettings::CanAssumeItIsZero<double>(resultMatrix->Element(i, j) - initFunc1(i, j) - initFunc2(i, j)));
      }
   }
}