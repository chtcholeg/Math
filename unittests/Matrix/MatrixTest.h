#ifndef __MATRIX_TEST_H__
#define __MATRIX_TEST_H__

#include "../stdafx.h"

#include "gtest/gtest.h"

#include "../../SMT/Matrix/MatrixOperations.h"
#include "../../SMT/Matrix/StandardMatrix.h"

class MatrixTest : public ::testing::Test 
{
protected:
   static void CheckForEachElement(const SMT::Matrix<double>& matrix, const std::function<double(size_t /*row*/, size_t /*column*/)>& func, bool epsilonIsZero)
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

   static void CheckEquality(const SMT::Matrix<double>& matrix1, const SMT::Matrix<double>& matrix2, bool epsilonIsZero)
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
   }
};

#endif __MATRIX_TEST_H__
