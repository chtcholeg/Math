// SMT.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iomanip>
#include <iostream>

#include "Matrix/StandardMatrix.h"

//static double init(size_t column, size_t row)
//{
//   return 1.0 / (1.0 + static_cast<double>(column)+static_cast<double>(row));
//}

static double init1(size_t row, size_t column)
{
   if (column == 0 && row == 0)
      return 1.0;
   if (column == 1 && row == 0)
      return 2.0;
   if (column == 2 && row == 0)
      return 3.0;
   if (column == 0 && row == 1)
      return 1.0;
   if (column == 1 && row == 1)
      return 2.0;
   if (column == 2 && row == 1)
      return 4.0;

   return 0.0;
}

static double init2(size_t row, size_t column)
{
   if (column == 0 && row == 0)
      return 1.0;
   if (column == 1 && row == 0)
      return 0.0;
   if (column == 0 && row == 1)
      return 0.0;
   if (column == 1 && row == 1)
      return 1.0;
   if (column == 0 && row == 2)
      return 1.0;
   if (column == 1 && row == 2)
      return 0.0;

   return 0.0;
}

template<typename ElementType>
void print(const SMT::Matrix<ElementType>& matrix)
{
   std::cout << std::endl;
   for (size_t rowIndex = 0; rowIndex < matrix.RowCount(); ++rowIndex)
   {
      for (size_t columnIndex = 0; columnIndex < matrix.ColumnCount(); ++columnIndex)
      {
         std::cout << std::fixed << std::setprecision(4) << matrix.Element(rowIndex, columnIndex) << " ";
      }
      std::cout << std::endl;
   }
}

int _tmain(int argc, _TCHAR* argv[])
{
   SMT::StandardMatrix<double> matr1(2, 3, init1);
   print(matr1);
   SMT::StandardMatrix<double> matr2(3, 2, init2);
   print(matr2);
   
   auto result = SMT::Multiply(matr1, matr2);
   if (result.Matrix_ != nullptr)
   {
      print(*result.Matrix_);
   }

	return 0;
}

