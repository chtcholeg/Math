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
   /*out*/ Matrix<ElementType>::OperationResultCode& code, /*out*/ std::string& description) const
{
   if (matrix1.RowCount() != matrix2.RowCount())
   {
      code = OperationResultCode::Error;
      description = "Matrices that are added together have a different number of rows.";
      return;
   }
   if (matrix1.ColumnCount() != matrix2.ColumnCount())
   {
      code = OperationResultCode::Error;
      description = "Matrices that are added together have a different number of columns.";
      return;
   }
   code = OperationResultCode::Ok;
   description.clear();
}

template <typename ElementType>
void CheckIfCanMultiplyTogether(const Matrix<ElementType>& leftMatrix, const Matrix<ElementType>& rightMatrix, 
   /*out*/ Matrix<ElementType>::OperationResultCode& code, /*out*/ std::string& description) const
{
   if (leftMatrix.ColumnCount() != rightMatrix.RowCount())
   {
      code = OperationResultCode::Error;
      description = "Matrices that are multiplyed together have a wrong number of column and rows: the left matrix has " + std::to_string(leftMatrix.ColumnCount()) + " column(s), the right matrix has " + std::to_string(rightMatrix.RowCount()) + " row(s)";
      return;
   }
   code = OperationResultCode::Ok;
   description.clear();
}	   

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// -- Operations
template <typename ElementType>
Matrix<ElementType>::OperationResult Copy(const Matrix<ElementType>& matrix)
{
   Matrix<ElementType>::OperationResult result = matrix.Copy();
   if (result.Code_ == Matrix<ElementType>::OperationResultCode::NotImplemented)
   {
      result.ResultMatrix_ = std::make_shared<StandardMatrix<ElementType>>(matrix);
	  result.Code_ = Matrix<ElementType>::OperationResultCode::Warning;
	  result.Description_ = "Matrix (type:" + matrix.TypeName() + ") has no Copy method. Standard matrix (type:" + result.ResultMatrix_->TypeName() + ") is used instead";
   }
   else if ((result.Code_ == Matrix<ElementType>::OperationResultCode::Ok) && (result.Description_.empty()))
   {
	   result.Description_ = "Matrix (type:" + matrix.TypeName() + ") has been copied.";
   }   
   return result;
}
template <typename ElementType>
Matrix<ElementType>::OperationResult Add(const Matrix<ElementType>& matrix1, const Matrix<ElementType>& matrix2)
{
	const Matrix<ElementType>::EfficiencyType matrix1Efficiency = matrix1.Add(matrix2);
	const Matrix<ElementType>::EfficiencyType matrix2Efficiency = matrix2.Add(matrix1);
	const Matrix<ElementType>::EfficiencyType bestEfficiency = std::max<Matrix<ElementType>::EfficiencyType>(matrix1Efficiency, matrix2Efficiency);
	if (bestEfficiency == Matrix<ElementType>::UndefinedEfficiency)
	{	
		StandardMatrix standardMatrixCopy(matrix1);
		Matrix<ElementType>::OperationResult result = standardMatrixCopy.Add(matrix2);
		if (result.Code_ == Matrix<ElementType>::OperationResultCode::Ok)
		{
			result.Code_ = Matrix<ElementType>::OperationResultCode::Warning;
			result.Description_ = "Matrices (1st matrix type :" + matrix1.TypeName() + ", 2nd matrix type:" + matrix2.TypeName() + ") have undefined efficiency.  Standard matrix (type:" + standardMatrixCopy.TypeName() + ") is used instead";
		}
		return result;
	}
	const Matrix<ElementType>& mainMatrix = (matrix1Efficiency >= matrix2Efficiency) ? matrix1 : matrix2;
	const Matrix<ElementType>& addedMatrix = (matrix1Efficiency >= matrix2Efficiency) ? matrix2 : matrix1;
	Matrix<ElementType>::OperationResult result = mainMatrix.Add(addedMatrix);
	if 
    return result;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// -- Predefined matrices
// Square matrix with ones on the main diagonal and zeros elsewhere
Matrix::SharedPtr CreateIdentityMatrix(size_t size);
// Matrix in which the entries outside the main diagonal are all zero
Matrix::SharedPtr CreateDiagonalMatrix(const std::vector<Matrix::T>& diagonalElements);
Matrix::SharedPtr CreateDiagonalMatrix(std::vector<Matrix::T>&& diagonalElements);
template<typename InputIterator>
Matrix::SharedPtr CreateDiagonalMatrix(InputIterator begin, InputIterator end) { DiagonalMatrix(std::vector<Matrix::T>(begin, end)); }
// Matrix all of whose entries are zero
Matrix::SharedPtr CreateZeroMatrix(size_t rowCount, size_t columnCount);
// Matrix all of whose entries are one
Matrix::SharedPtr CreateOneMatrix(size_t rowCount, size_t columnCount);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Standard matrix.
// It represents as a vector of vectors.
// Very simple but not optimized.
template <typename ElementType = double>
class StandardMatrix : public Matrix<ElementType>
{
public:
   using InitFunc = std::function<ElementType (size_t /*column*/, size_t /*row*/)>; // A function which initializes all matrix elements
   StandardMatrix(size_t rowCount, size_t columnCount, InitFunc initFunc);

   // Matrix
   virtual size_t RowCount() const override { return rowCount_; }
   virtual size_t ColumnCount() const override { return columnCount_; }
   virtual T Element(size_t row, size_t column) const override;
   virtual bool Add(const Matrix& /*otherMatrix*/) override;
   virtual bool Multiply(const T& /*number*/) override;
   virtual bool LMultiply(const Matrix& /*leftMatrix*/) override;
   virtual bool RMultiply(const Matrix& /*rightMatrix*/) override;
   virtual bool Transpose() override;

private:
   using Row = std::vector<Matrix::T>;
   using Table = std::vector<Row>;

private:
   size_t rowCount_;
   size_t columnCount_;
   Table body_;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// -- Operations
virtual bool CheckIfIdentityMatrix() const;
virtual bool CheckIfZeroMatrix() const;
virtual bool CheckIfDiagonalMatrix() const;


template <typename T>
static Matrix<T>::SharedPtr Matrix<T>::Multiply(const Matrix<T>& matrix1, const Matrix<T>& matrix2)
{
   if (matrix1.ColumnCount() != matrix2.RowCount())
   {
      assert(false);
      return nullptr;
   }

   // Standard multiplication
   const size_t newRowCount = matrix1.RowCount();
   const size_t newColumnCount = matrix2.ColumnCount();
   result = std::make_shared<Matrix>(newRowCount, newColumnCount);
   for (size_t i = 0; i < newRowCount; ++i)
   {
      for (size_t j = 0; j < newColumnCount; ++j)
      {
         result->Item(i, j) = calcMultMatrixElement(matrix1, matrix2, i, j);
      }
   }

   return result;
}


} // namespace SMT