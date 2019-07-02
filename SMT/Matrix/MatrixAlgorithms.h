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
// Distance from 1 to number
template <typename ElementType>
ElementType DistanceToOne(ElementType value)
{
   const ElementType absValue = std::abs<ElementType>(value);
   const ElementType one = MatrixSettings::One<ElementType>();
   if (MatrixSettings::CanAssumeItIsZero<ElementType>(absValue - one)
   {
      return MatrixSettings::Zero<ElementType>();
   }
   if (MatrixSettings::CanAssumeItIsZero<ElementType>(absValue))
   {
      return MatrixSettings::Max<ElementType>();
   }
   return (absValue > one) ? ((absValue - one) / one) : ((one - absValue) / one);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Gauss–Jordan elimination
template <typename ElementType>
typename Matrix<ElementType>::OperationResult GaussJordanElimination(const Matrix<ElementType>& matrixConst, const Matrix<ElementType>& identityMatrixConst)
{
   auto matrix = matrixConst.Copy();
   Matrix<ElementType>::OperationResult result;
   if (matrix == nullptr || matrix->RowCount() != matrix->ColumnCount())
   {
      result.Code_ = OperationResultCode::Error;
      result.Description_ = matrix == nullptr 
            ? "Matrix is not copyable"
            : "Matrix can't be inverted: the number of rows (=" + std::to_string(matrix->RowCount()) + ") doesn't equal the number of columns (=" + std::to_string(matrix->ColumnCount()) + ")";
      return result;
   }
   auto result.Matrix_ = identityMatrixConst.Copy();
   if (result.Matrix_ == nullptr)
   {
      result.Code_ = OperationResultCode::Error;
      result.Description_ = "Identity matrix is not copyable";
      return result;
   }
   assert(matrix->RowCount() == result.Matrix_->RowCount() && matrix->ColumnCount() == result.Matrix_->RowCount());
   if (matrix->RowCount() != result.Matrix_->RowCount() || matrix->ColumnCount() != result.Matrix_->RowCount())
   {
      result.Code_ = OperationResultCode::Error;
      result.Description_ = "Matrix can't be inverted: an identity matrix is the wrong size";
      return result;
   }
  
   auto matrixElementaryOperations = matrix->ElementaryOperations();
   auto resultElementaryOperations = result.Matrix_->ElementaryOperations();
   if (matrixElementaryOperations == nullptr || resultElementaryOperations == nullptr)
   {
      result.Code_ = OperationResultCode::Error;
      result.Description_ = "Matrix can't be inverted: elementary operations are not implemented";
      return result;
   }

   const size_t size = matrix->RowCount();
   for (size_t i = 0; i < size; ++i)
   {
      size_t bestRow = i;
      ElementType bestDistance = DistanceToOne(matrix->Element(i, i));
      for (size_t j = i + 1; j < size; ++j)
      {
         const ElementType curDistance = DistanceToOne(matrix->Element(j, i));
         if (curDistance < bestDistance)
         {
            bestRow = j;
            bestDistance = curDistance;
         }
      }
      if (MatrixSettings::CanAssumeItIsZero<ElementType>(matrix->Element(bestRow, i))
      {
         result.Code_ = OperationResultCode::Error;
         result.Description_ = "Matrix can't be inverted: it is not invertible";
         return result;
      }

      matrixElementaryOperations->SwapRows(i, bestRow);
      resultElementaryOperations->SwapRows(i, bestRow);

      const ElementType value = matrix->Element(i, i);
      const ElementType koef = MatrixSettings::One<ElementType>() / value;
      matrixElementaryOperations->MultiplyRowByNumber(i, koef);
      resultElementaryOperations->MultiplyRowByNumber(i, koef);

      for (size_t j = 0; j < size; ++j)
      {
         if (i == j)
         {
            continue;
         }
         const ElementType curValue = matrix->Element(j, i);
         if (MatrixSettings::CanAssumeItIsZero<ElementType>(curValue))
         {
            continue;
         }
         const ElementType curKoef = matrix->Element(j, i);
         matrixElementaryOperations->MultiplyAndAdd(j, i, curKoef);
         resultElementaryOperations->MultiplyAndAdd(j, i, curKoef);
      }
   }

   return result;
}

} namespace Algorithms
} // namespace SMT

#endif // __MATRIX_ALGORITHMS_H__