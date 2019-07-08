#ifndef __STANDARD_MATRIX_H__
#define __STANDARD_MATRIX_H__

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Standard matrix
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MatrixDefs.h"
#include "MatrixOperations.h"
#include "MatrixAlgorithms.h"

namespace SMT
{
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Standard matrix.
// It represents as a vector of vectors.
// Very simple but not optimized.
template <typename ElementType>
class StandardMatrix 
   : public Matrix<ElementType>
   , public Matrix<ElementType>::IElementaryOperations
{
public:
   using InitFunc = std::function<ElementType (size_t /*column*/, size_t /*row*/)>; // A function which initializes all matrix elements
   StandardMatrix(size_t rowCount, size_t columnCount, InitFunc initFunc)
   {
      assert(initFunc);
      if (!initFunc)
      {
         const ElementType retValue = MatrixSettings::Zero<ElementType>();
         initFunc = [retValue](size_t /*row*/, size_t /*column*/)->ElementType { return retValue; };
      }
      init(rowCount, columnCount, initFunc);
   }
   explicit StandardMatrix(const Matrix<ElementType>& sourceMatrix)
   {
      auto initFunc = [&sourceMatrix](size_t row, size_t column)->ElementType { return sourceMatrix.Element(row, column); };
      init(sourceMatrix.RowCount(), sourceMatrix.ColumnCount(), initFunc);
   }

   static OperationResult Add(const Matrix<ElementType>& matrix1, const Matrix<ElementType>& matrix2)
   {
	  OperationResult result;
      CheckIfCanAddTogether<ElementType>(matrix1, matrix2, result.Code_, result.Description_);
      if (result.Code_ == OperationResultCode::Error)
      {
         return result;
      }
	  
	  auto initFunc = [&matrix1, &matrix2](size_t row, size_t column)->ElementType { return matrix1.Element(row, column) + matrix2.Element(row, column); };
      result.Matrix_ = std::make_shared<StandardMatrix<ElementType>>(matrix1.RowCount(), matrix1.ColumnCount(), initFunc);
      result.Code_ = OperationResultCode::Ok;
      return result;
   }
   
   static OperationResult Multiply(const Matrix<ElementType>& leftMatrix, const Matrix<ElementType>& rightMatrix)
   {
      OperationResult result;
      CheckIfCanMultiplyTogether(leftMatrix, rightMatrix, result.Code_, result.Description_);
      if (result.Code_ == OperationResultCode::Error)
      {
         return result;
      }
      const size_t numberOfItems = leftMatrix.ColumnCount();
      auto initFunc = [&leftMatrix, &rightMatrix, numberOfItems](size_t row, size_t column)-> ElementType 
      { 
         ElementType result = MatrixSettings::Zero<ElementType>();
         for (size_t i = 0; i < numberOfItems; ++i)
         {
            result += leftMatrix.Element(row, i) * rightMatrix.Element(i, column);
         }
         return result;
      };
      result.Matrix_ = std::make_shared<StandardMatrix<ElementType>>(leftMatrix.RowCount(), rightMatrix.ColumnCount(), initFunc);
      result.Code_ = OperationResultCode::Ok;
      return result;
   }

   // Matrix
   virtual size_t RowCount() const override { return body_.size(); }
   virtual size_t ColumnCount() const override { return body_.empty() ? 0 : body_[0].size(); }
   virtual ElementType Element(size_t row, size_t column) const override { return body_[row][column]; }
   virtual std::string TypeName() const { return "StandardMatrix"; }
   virtual Complexity::Type CopyingComplexity() const override { return Complexity::Quadratic; }
   virtual OperationResult Copy() const override { return copy(); }
   virtual Complexity::Type AdditionComplexity(const Matrix<ElementType>& otherMatrix) const override{ return Complexity::Quadratic; }
   virtual OperationResult Add(const Matrix<ElementType>& otherMatrix) const override{ return Add(*this, otherMatrix); }
   virtual Complexity::Type MultiplyByNumberComplexity() const override { return Complexity::Quadratic; }
   virtual OperationResult MultiplyByNumber(const ElementType& number) const override{ return multiplyByNumber(number); }
   virtual Complexity::Type MultiplyComplexity(const Matrix<ElementType>& anotherMatrix, bool anotherMatrixIsOnTheLeft) const override { return Complexity::Cubic; }
   virtual OperationResult Multiply(const Matrix<ElementType>& anotherMatrix, bool anotherMatrixIsOnTheLeft) const override{ return anotherMatrixIsOnTheLeft ? Multiply(anotherMatrix, *this) : Multiply(*this, anotherMatrix); }
   virtual Complexity::Type InversionComplexity() const override { return Complexity::Cubic; }
   virtual OperationResult Invert() const override { return Algorithms::GaussJordanElimination<ElementType>(*this, createIdentityMatrix); }
   virtual Complexity::Type TransposeComplexity() const override { return Complexity::Quadratic; }
   virtual OperationResult Transpose() const override { return transpose(); }
   virtual Complexity::Type DeterminantEvaluationComplexity() const override { return Complexity::Cubic; }
   virtual ScalarOperationResult Determinant() const override { return Algorithms::CalcDeterminant_GaussJordanElimination<ElementType>(*this); }
   virtual IElementaryOperations* ElementaryOperations() { return this; }
   // Matrix::IElementaryOperations
   virtual bool SwapRows(size_t rowIndex1, size_t rowIndex2) { return swap(rowIndex1, rowIndex2); }
   virtual bool MultiplyRowByNumber(size_t rowIndex, ElementType number) { return multiplyByNumber(rowIndex, number); }
   virtual bool MultiplyAndSubtract(size_t rowIndex1, size_t rowIndex2, ElementType number) { return multiplyAndSubtract(rowIndex1, rowIndex2, number); }

private:
   using Row = std::vector<ElementType>;
   using Table = std::vector<Row>;

private:
   Table body_;

   void init(size_t rowCount, size_t columnCount, InitFunc initFunc)
   {
      body_.resize(rowCount);
      for (size_t rowIndex = 0; rowIndex < rowCount; ++rowIndex)
      {
         auto& row = body_[rowIndex];
         row.resize(columnCount);
         for (size_t columnIndex = 0; columnIndex < columnCount; ++columnIndex)
         {
            row[columnIndex] = initFunc(rowIndex, columnIndex);
         }
      }
   }

   OperationResult copy() const
   {
      OperationResult result;
      result.Matrix_ = std::make_shared<StandardMatrix<ElementType>>(*this);
      result.Code_ = OperationResultCode::Ok;
      return result;
   }

   OperationResult multiplyByNumber(const ElementType& number) const
   { 
      OperationResult result;
      const Matrix& matrix = *this;
      auto initFunc = [&matrix, number](size_t row, size_t column)->ElementType { return matrix.Element(row, column) * number; };
      result.Matrix_ = std::make_shared<StandardMatrix<ElementType>>(RowCount(), ColumnCount(), initFunc);
      result.Code_ = OperationResultCode::Ok;
      return result;
   }
   
   OperationResult transpose() const
   {
      OperationResult result;
      auto initFunc = [this](size_t row, size_t column)->ElementType { return Element(column, row); };
      result.Matrix_ = std::make_shared<StandardMatrix<ElementType>>(ColumnCount(), RowCount(), initFunc);
      result.Code_ = OperationResultCode::Ok;
      return result;
   }

   bool swap(size_t rowIndex1, size_t rowIndex2)
   {
      if (rowIndex1 >= RowCount() || rowIndex2 >= RowCount())
      {
         return false;
      }
      if (rowIndex1 == rowIndex2)
      {
         return true;
      }
      const size_t columnCount = ColumnCount();
      auto& row1 = body_[rowIndex1];
      auto& row2 = body_[rowIndex2];
      for (size_t column = 0; column < columnCount; ++column)
      {
         const auto buffer = row1[column];
         row1[column] = row2[column];
         row2[column] = buffer;
      }
      return true;
   }

   bool multiplyByNumber(size_t rowIndex, ElementType number)
   {
      if (rowIndex >= RowCount())
      {
         return false;
      }
      const size_t columnCount = ColumnCount();
      auto& row = body_[rowIndex];
      for (size_t column = 0; column < columnCount; ++column)
      {
         row[column] *= number;
      }
      return true;
   }

   bool multiplyAndSubtract(size_t rowIndex1, size_t rowIndex2, ElementType number)
   {
      if (rowIndex1 >= RowCount() || rowIndex2 >= RowCount())
      {
         return false;
      }
      const size_t columnCount = ColumnCount();
      auto& row1 = body_[rowIndex1];
      auto& row2 = body_[rowIndex2];
      for (size_t column = 0; column < columnCount; ++column)
      {
         row1[column] -= row2[column] * number;
      }
      return true;
   }

   static Matrix<ElementType>::SharedPtr createIdentityMatrix(size_t size)
   {
      auto initFunc = [](size_t row, size_t column) -> ElementType
      {
         return (row == column) ? MatrixSettings::One<ElementType>() : MatrixSettings::Zero<ElementType>();
      };
      return std::make_shared<StandardMatrix<ElementType>>(size, size, initFunc);
   }
};

} // namespace SMT

#endif // __STANDARD_MATRIX_H__