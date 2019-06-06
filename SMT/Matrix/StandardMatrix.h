///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Standard matrix
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MatrixDefs.h"

namespace SMT
{
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Standard matrix.
// It represents as a vector of vectors.
// Very simple but not optimized.
template <typename ElementType>
class StandardMatrix : public Matrix<ElementType>
{
public:
   using InitFunc = std::function<ElementType (size_t /*column*/, size_t /*row*/)>; // A function which initializes all matrix elements
   StandardMatrix(size_t rowCount, size_t columnCount, InitFunc initFunc) :
   {
      assert(initFunc);
      if (!initFunc)
      {
         const ElementType retValue = MatrixSettings<ElementType>::Zero();
         initFunc = [retValue](size_t /*row*/, size_t /*column*/)->ElementType { return retValue; };
      }
      init(rowCount, columnCount, initFunc);
   }
   explicit StandardMatrix(const Matrix<ElementType>& sourceMatrix)
   {
      initFunc = [&sourceMatrix](size_t row, size_t column)->ElementType { return sourceMatrix.Element(row, column); };
      init(sourceMatrix.RowCount(), sourceMatrix.ColumnCount(), initFunc);
   }

   // Matrix
   virtual size_t RowCount() const override { return body_.size(); }
   virtual size_t ColumnCount() const override { return body_.empty() ? 0 : body_[0].size(); }
   virtual ElementType Element(size_t row, size_t column) const override { return body_[row][column]; }
   virtual EfficiencyType CopyingEfficiency() const override { return QuadraticEfficiency; }
   virtual OperationResult Copy() const override { return copy(); }
   virtual EfficiencyType AdditionEfficiency(const Matrix<ElementType>& otherMatrix) override { return QuadraticEfficiency; }
   virtual OperationResult Add(const Matrix<ElementType>& otherMatrix) override { return add(otherMatrix); }
   virtual EfficiencyType MultiplyByNumberEfficiency() const override { return QuadraticEfficiency; }
   virtual OperationResult MultiplyByNumber(const T& number) override { return multiplyByNumber(number); }
   virtual EfficiencyType MultiplyEfficiency(const Matrix<ElementType>& anotherMatrix, bool anotherMatrixIsOnTheLeft) const override { return CubicEfficiency; }
   virtual OperationResult Multiply(const Matrix<ElementType>& anotherMatrix, bool anotherMatrixIsOnTheLeft) override { return multiply(number); }
   virtual EfficiencyType TransposeEfficiency() const override { return QuadraticEfficiency; }
   virtual OperationResult Transpose() override { return transpose(); }

private:
   using Row = std::vector<Matrix::T>;
   using Table = std::vector<Row>;

private:
   Table body_;

   void init(size_t rowCount, size_t columnCount, InitFunc initFunc)
   {
      body_.resize(rowCount);
      for (size_t row = 0; row < rowCount; ++row)
      {
         auto& row = body_[row];
         row.resize(columnCount);
         for (size_t column = 0; column < columnCount; ++columnCount)
         {
            row[column] = initFunc(column, row);
         }
      }
   }

   OperationResult copy()
   {
      OperationResult result;
      result.ResultMatrix_ = std::make_shared<StandardMatrix<ElementType>>(*this);
      result.Code_ = OperationResultCode::Ok;
      return result;
   }

   OperationResult add(const Matrix<ElementType>& otherMatrix)
   {
      OperationResult result;
      CheckIfCanAddTogether(*this, otherMatrix, result.Code_, result.Description_);
      if (result.Code_ == OperationResult::Error)
      {
         return result;
      }

      const Matrix<ElementType>& matrix1 = *this;
      const Matrix<ElementType>& matrix2 = otherMatrix;
      auto initFunc = [&matrix1, &matrix2](size_t row, size_t column)->ElementType { return matrix1.Element(row, column) + matrix2.Element(row, column); };
      result.ResultMatrix_ = std::make_shared<StandardMatrix<ElementType>>(RowCount(), ColumnCount(), initFunc);
	  result.Code_ = OperationResult::Ok;
      return result;
   }
   
   OperationResult multiplyByNumber(const T& number) 
   { 
       OperationResult result;
       const Matrix& matrix = *this;
       auto initFunc = [&matrix, number](size_t row, size_t column)->ElementType { return matrix.Element(row, column) * number; };
       result.ResultMatrix_ = std::make_shared<StandardMatrix<ElementType>>(RowCount(), ColumnCount(), initFunc);
	   result.Code_ = OperationResult::Ok;
       return result;
   }
   
   OperationResult multiply(const Matrix<ElementType>& anotherMatrix, bool anotherMatrixIsOnTheLeft) 
   {
	   OperationResult result;
	   const Matrix<ElementType>& leftMatrix = anotherMatrixIsOnTheLeft ? anotherMatrix : *this;
	   const Matrix<ElementType>& rightMatrix = anotherMatrixIsOnTheLeft ? *this : anotherMatrix;
       CheckIfCanMultiplyTogether(leftMatrix, rightMatrix, result.Code_, result.Description_);
       if (result.Code_ == OperationResult::Error)
       {
            return result;
       }
       const size_t numberOfItems = leftMatrix.ColumnCount();
       auto initFunc = [&leftMatrix, &rightMatrix, numberOfItems](size_t row, size_t column)-> ElementType 
	                  { 
					     ElementType result = MatrixSettings<ElementType>::Zero();
						 for (size_t i = 0; i < numberOfItems; ++i)
						 {
							 result += leftMatrix[row][i] * rightMatrix[i][column];
						 }
						 return result;
				      }
       result.ResultMatrix_ = std::make_shared<StandardMatrix<ElementType>>(RowCount(), ColumnCount(), initFunc);
	   result.Code_ = OperationResult::Ok;
       return result;
   }
   
   OperationResult transpose()
   {
	   OperationResult result;
	   auto initFunc = [this](size_t row, size_t column)->ElementType { return Element(column, row); };
       result.ResultMatrix_ = std::make_shared<StandardMatrix<ElementType>>(ColumnCount(), RowCount(), initFunc);
	   result.Code_ = OperationResult::Ok;
       return result;
   }
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