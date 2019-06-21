#ifndef __STANDARD_MATRIX_H__
#define __STANDARD_MATRIX_H__

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Standard matrix
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MatrixDefs.h"
#include "MatrixOperations.h"

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
   virtual OperationResult Multiply(const Matrix<ElementType>& anotherMatrix, bool anotherMatrixIsOnTheLeft) const override{ return anotherMatrixIsOnTheLeft ? Multiple(anotherMatrix, *this) : Multiple(*this, anotherMatrix); }
   virtual Complexity::Type TransposeComplexity() const override { return Complexity::Quadratic; }
   virtual OperationResult Transpose() const override{ return transpose(); }

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
   
   OperationResult multiply(const Matrix<ElementType>& anotherMatrix, bool anotherMatrixIsOnTheLeft) const
   {
      OperationResult result;
      const Matrix<ElementType>& leftMatrix = anotherMatrixIsOnTheLeft ? anotherMatrix : *this;
      const Matrix<ElementType>& rightMatrix = anotherMatrixIsOnTheLeft ? *this : anotherMatrix;
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
   
   OperationResult transpose() const
   {
      OperationResult result;
      auto initFunc = [this](size_t row, size_t column)->ElementType { return Element(column, row); };
      result.Matrix_ = std::make_shared<StandardMatrix<ElementType>>(ColumnCount(), RowCount(), initFunc);
      result.Code_ = OperationResultCode::Ok;
      return result;
   }
};

} // namespace SMT

#endif // __STANDARD_MATRIX_H__