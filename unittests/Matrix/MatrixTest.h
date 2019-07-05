#ifndef __MATRIX_TEST_H__
#define __MATRIX_TEST_H__

#include "../stdafx.h"

#include "gtest/gtest.h"

#include "../../SMT/Matrix/MatrixOperations.h"
#include "../../SMT/Matrix/StandardMatrix.h"

class MatrixTest : public ::testing::Test 
{
protected:
   template<typename ElementType>
   static void CheckForEachElement(const SMT::Matrix<ElementType>& matrix, const std::function<ElementType(size_t /*row*/, size_t /*column*/)>& func, bool epsilonIsZero)
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

   template<typename ElementType>
   static void CheckEquality(const SMT::Matrix<ElementType>& matrix1, const SMT::Matrix<ElementType>& matrix2, bool epsilonIsZero)
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
   template<typename ElementType>
   static void CheckEquality(ElementType scalar1, ElementType scala2, bool epsilonIsZero)
   {
      if (epsilonIsZero)
      {
         EXPECT_EQ(matrix1.Element(i, j), matrix2.Element(i, j));
      }
      else
      {
         ASSERT_TRUE(SMT::MatrixSettings::CanAssumeItIsZero<double>(scalar1 - scalar1));
      }
   }
};

#endif __MATRIX_TEST_H__
