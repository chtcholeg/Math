#ifndef __MATRIX_OPERATIONS_H__
#define __MATRIX_OPERATIONS_H__

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The main operations that are related to matrices
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MatrixDefs.h"
#include "StandardMatrix.h"

namespace SMT
{

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// -- Functions that check if we can do some actions
// Standard checks
template <typename ElementType>
void CheckIfCanAddTogether(const Matrix<ElementType>& matrix1, const Matrix<ElementType>& matrix2, 
   /*out*/ typename Matrix<ElementType>::OperationResultCode& code, /*out*/ std::string& description)
{
   if (matrix1.RowCount() != matrix2.RowCount())
   {
      code = Matrix<ElementType>::OperationResultCode::Error;
      description = "Matrices that are added together have a different number of rows.";
      return;
   }
   if (matrix1.ColumnCount() != matrix2.ColumnCount())
   {
      code = Matrix<ElementType>::OperationResultCode::Error;
      description = "Matrices that are added together have a different number of columns.";
      return;
   }
   code = Matrix<ElementType>::OperationResultCode::Ok;
   description.clear();
}

template <typename ElementType>
void CheckIfCanMultiplyTogether(const Matrix<ElementType>& leftMatrix, const Matrix<ElementType>& rightMatrix, 
   /*out*/ typename Matrix<ElementType>::OperationResultCode& code, /*out*/ std::string& description)
{
   if (leftMatrix.ColumnCount() != rightMatrix.RowCount())
   {
      code = Matrix<ElementType>::OperationResultCode::Error;
      description = "Matrices that are multiplyed together have a wrong number of column and rows: the left matrix has " + std::to_string(leftMatrix.ColumnCount()) + " column(s), the right matrix has " + std::to_string(rightMatrix.RowCount()) + " row(s)";
      return;
   }
   code = Matrix<ElementType>::OperationResultCode::Ok;
   description.clear();
}	   

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// -- Operations
template <typename ElementType>
typename Matrix<ElementType>::OperationResult Copy(const Matrix<ElementType>& matrix)
{
   Matrix<ElementType>::OperationResult result = matrix.Copy();
   if (result.Code_ == Matrix<ElementType>::OperationResultCode::NotImplemented)
   {
      result.Matrix_ = std::make_shared<StandardMatrix<ElementType>>(matrix);
      result.Code_ = Matrix<ElementType>::OperationResultCode::Warning;
      result.Description_ = "Matrix (type:" + matrix.TypeName() + ") has no Copy method. Standard matrix (type:" + result.Matrix_->TypeName() + ") is used instead";
   }
   else if ((result.Code_ == Matrix<ElementType>::OperationResultCode::Ok) && (result.Description_.empty()))
   {
	   result.Description_ = "Matrix (type:" + matrix.TypeName() + ") has been copied.";
   }   
   return result;
}

template <typename ElementType>
typename Matrix<ElementType>::OperationResult Add(const Matrix<ElementType>& matrix1, const Matrix<ElementType>& matrix2)
{
	const Matrix<ElementType>::EfficiencyType matrix1Efficiency = matrix1.AddEfficiency(matrix2);
   const Matrix<ElementType>::EfficiencyType matrix2Efficiency = matrix2.AddEfficiency(matrix1);
	const Matrix<ElementType>::EfficiencyType bestEfficiency = std::max<Matrix<ElementType>::EfficiencyType>(matrix1Efficiency, matrix2Efficiency);
	if (bestEfficiency != Matrix<ElementType>::UndefinedEfficiency)
	{	
      const Matrix<ElementType>& mainMatrix = (matrix1Efficiency >= matrix2Efficiency) ? matrix1 : matrix2;
      const Matrix<ElementType>& addedMatrix = (matrix1Efficiency >= matrix2Efficiency) ? matrix2 : matrix1;
      const Matrix<ElementType>::OperationResult result = mainMatrix.Add(addedMatrix);
      if ((result.Code_ == Matrix<ElementType>::Ok || result.Code_ == Matrix<ElementType>::Warning) && (result.Matrix_ != nullptr))
      {
         if (result.Description_.empty())
         {
            result.Description_ = "Matrices (1st matrix type :" + matrix1.TypeName() + ", 2nd matrix type:" + matrix2.TypeName() + ") are added together";
         }
         return result;
      }
   }
   StandardMatrix standardMatrixCopy(matrix1);
   Matrix<ElementType>::OperationResult result = standardMatrixCopy.Add(matrix2);
   if (result.Code_ == Matrix<ElementType>::OperationResultCode::Ok)
   {
      result.Code_ = Matrix<ElementType>::OperationResultCode::Warning;
      if (!result.Description_.empty())
         result.Description_ = "Matrices (1st matrix type :" + matrix1.TypeName() + ", 2nd matrix type:" + matrix2.TypeName() + ") can't be added together.  Standard matrix (type:" + standardMatrixCopy.TypeName() + ") is used instead";
   }
   return result;
}

template <typename ElementType>
typename Matrix<ElementType>::OperationResult MultiplyByNumber(const Matrix<ElementType>& matrix, const ElementType& number)
{
   Matrix<ElementType>::OperationResult result = matrix.MultiplyByNumber(number);
   if (result.Code_ == Matrix<ElementType>::OperationResultCode::NotImplemented)
   {
      const StandardMatrix<ElementType> standardMatrixCopy(matrix);
      result = standardMatrixCopy.MultiplyByNumber(number);
      if (result.Code_ == Matrix<ElementType>::OperationResultCode::Ok)
      {
         result.Code_ = Matrix<ElementType>::OperationResultCode::Warning;
         result.Description_ = "Matrix (type:" + matrix.TypeName() + ") has no MultiplyByNumber method. Standard matrix (type:" + result.Matrix_->TypeName() + ") is used instead";
         return result;
      }
   }
   
   if ((result.Code_ == Matrix<ElementType>::OperationResultCode::Ok) && (result.Description_.empty()))
   {
      result.Description_ = "Matrix (type:" + matrix.TypeName() + ") has been multiplied by number.";
   }
   return result;
}

template <typename ElementType>
typename Matrix<ElementType>::OperationResult Multiply(const Matrix<ElementType>& leftMatrix, const Matrix<ElementType>& rightMatrix)
{
   const Matrix<ElementType>::EfficiencyType leftMatrixEfficiency = leftMatrix.MultiplyEfficiency(rightMatrix, false);
   const Matrix<ElementType>::EfficiencyType rightMatrixEfficiency = rightMatrix.MultiplyEfficiency(leftMatrix, true);
   const Matrix<ElementType>::EfficiencyType bestEfficiency = std::max<Matrix<ElementType>::EfficiencyType>(leftMatrixEfficiency, rightMatrixEfficiency);
   if (bestEfficiency != Matrix<ElementType>::UndefinedEfficiency)
   {
      const Matrix<ElementType>& mainMatrix = (leftMatrixEfficiency >= rightMatrixEfficiency) ? leftMatrix : rightMatrix;
      const Matrix<ElementType>& anotherMatrix = (leftMatrixEfficiency >= rightMatrixEfficiency) ? rightMatrix : leftMatrix;
      const bool anotherMatrixIsOnTheLeft = (leftMatrixEfficiency < rightMatrixEfficiency);
      const Matrix<ElementType>::OperationResult result = mainMatrix.Multiply(anotherMatrix, anotherMatrixIsOnTheLeft);
      if ((result.Code_ == Matrix<ElementType>::Ok || result.Code_ == Matrix<ElementType>::Warning) && (result.Matrix_ != nullptr))
      {
         if (result.Description_.empty())
         {
            result.Description_ = "Matrices (1st matrix type :" + matrix1.TypeName() + ", 2nd matrix type:" + matrix2.TypeName() + ") are multiplied";
         }
         return result;
      }
   }
   StandardMatrix standardMatrixCopy(leftMatrix);
   Matrix<ElementType>::OperationResult result = standardMatrixCopy.Multiply(rightMatrix, false);
   if (result.Code_ == Matrix<ElementType>::OperationResultCode::Ok)
   {
      result.Code_ = Matrix<ElementType>::OperationResultCode::Warning;
      if (result.Description_.empty())
         result.Description_ = "Matrices (1st matrix type :" + leftMatrix.TypeName() + ", 2nd matrix type:" + rightMatrix.TypeName() + ") can't be multiplied together.  Standard matrix (type:" + standardMatrixCopy.TypeName() + ") is used instead";
   }
   return result;
}

template <typename ElementType>
typename Matrix<ElementType>::OperationResult Transpose(const Matrix<ElementType>& matrix)
{
   Matrix<ElementType>::OperationResult result = matrix.Transpose();
   if (result.Code_ == Matrix<ElementType>::OperationResultCode::NotImplemented)
   {
      result.Matrix_ = std::make_shared<StandardMatrix<ElementType>>(matrix);
      result.Code_ = Matrix<ElementType>::OperationResultCode::Warning;
      result.Description_ = "Matrix (type:" + matrix.TypeName() + ") has no Transpose method. Standard matrix (type:" + result.Matrix_->TypeName() + ") is used instead";
   }
   else if ((result.Code_ == Matrix<ElementType>::OperationResultCode::Ok) && (result.Description_.empty()))
   {
      result.Description_ = "Matrix (type:" + matrix.TypeName() + ") has been transposed.";
   }
   return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// -- Functions that check matrix settings
// Function moves through all elements and checks
template <typename ElementType>
bool Check(const Matrix<ElementType>& matrix, bool(*predicate)(size_t column, size_t row, ElementType element))
{
   if (!func)
   {
      return false;
   }
   const size_t rowCount = matrix.RowCount();
   const size_t columnCount = matrix.ColumnCount();
   for (size_t row = 0; row < rowCount; ++row)
   {
      for (size_t column = 0; column < columnCount; ++column)
      {
         if (!predicate(row, count, matrix.Element(row, count)))
         {
            return false;
         }
     }
   }
   return true;
}

// Standard checks
template <typename ElementType>
bool CheckIfIdentityMatrix(const Matrix<ElementType>& matrix)
{
   auto isIdentityMatrixElement = [](size_t column, size_t row, ElementType element)
   {
      const ElementType requiredElement = (column == row) ? MatrixSettings::One<ElementType>() : MatrixSettings::Zero<ElementType>();
      return MatrixSettings<ElementType>::CanAssumeItIsZero(requiredElement - element);
   };
   return Check(matrix, isIdentityMatrixElement);
}
template <typename ElementType>
bool CheckIfZeroMatrix()
{
   auto isZeroMatrixElement = [](size_t /*column*/, size_t /*row*/, ElementType element)
   {
      return MatrixSettings::CanAssumeItIsZero<ElementType>(element);
   };
   return Check(matrix, isZeroMatrixElement);
}
template <typename ElementType>
bool CheckIfDiagonalMatrix()
{
   auto isDiaginalMatrixElement = [](size_t column, size_t row, ElementType element)
   {
      return column == row ? true : MatrixSettings::CanAssumeItIsZero<ElementType>(element);
   };
   return Check(matrix, isDiaginalMatrixElement);
}

} // namespace SMT

#endif __MATRIX_OPERATIONS_H__