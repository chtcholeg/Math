// SMT.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iomanip>
#include <iostream>

#include "Matrix/StandardMatrix.h"

static double init(size_t column, size_t row)
{
   return 1.0 / (1.0 + static_cast<double>(column)+static_cast<double>(row));
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
   SMT::StandardMatrix<double> standardMatrix(10, 12, init);
   print(standardMatrix);
	return 0;
}

