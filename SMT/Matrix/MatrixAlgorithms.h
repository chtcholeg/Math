#ifndef __MATRIX_ALGORITHMS_H__
#define __MATRIX_ALGORITHMS_H__

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Matrix algorithms
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MatrixDefs.h"
#include "MatrixOperations.h"

namespace SMT
{
namespace Algorithms
{
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Gauss–Jordan elimination
template <typename ElementType>
typename Matrix<ElementType>::OperationResult GaussJordanElimination(const Matrix<ElementType>& matrix, const Matrix<ElementType>& identityMatrix)
{
  Matrix<ElementType>::OperationResult result;
  if (matrix.RowCount() != matrix.ColumnCount())
  {
	  result.Code_ = OperationResultCode::Error;
	  result.Description_ = "Matrix can't be inverted: the number of rows (=" + std::to_string(RowCount()) + ") doesn't equal the number of columns (=" + std::to_string(ColumnCount()) + ")";
	  return result;
  }
  assert(matrix.RowCount() == identityMatrix.RowCount() && matrix.ColumnCount() == identityMatrix.RowCount());
  if (matrix.RowCount() != identityMatrix.RowCount() || matrix.ColumnCount() != identityMatrix.RowCount())
  {
	  result.Code_ = OperationResultCode::Error;
	  result.Description_ = "Matrix can't be inverted: an identity matrix is the wrong size)";
	  return result;
  }
  
  auto initFunc = [](size_t row, size_t column)->ElementType { return row == column ? MatrixSettings::One<ElementType>();  : MatrixSettings::Zero<ElementType>(); };
  result.Matrix_ = std::make_shared<StandardMatrix<ElementType>>(RowCount(), ColumnCount(), initFunc);
  for
}

} namespace Algorithms
} // namespace SMT

#endif // __MATRIX_ALGORITHMS_H__