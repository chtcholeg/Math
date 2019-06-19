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

   // Matrix
   virtual size_t RowCount() const override { return body_.size(); }
   virtual size_t ColumnCount() const override { return body_.empty() ? 0 : body_[0].size(); }
   virtual ElementType Element(size_t row, size_t column) const override { return body_[row][column]; }
   virtual std::string TypeName() const { return "StandardMatrix"; }
   virtual EfficiencyType CopyingEfficiency() const override { return QuadraticEfficiency; }
   virtual OperationResult Copy() const override { return copy(); }
   virtual EfficiencyType AdditionEfficiency(const Matrix<ElementType>& otherMatrix) const override{ return QuadraticEfficiency; }
   virtual OperationResult Add(const Matrix<ElementType>& otherMatrix) const override{ return add(otherMatrix); }
   virtual EfficiencyType MultiplyByNumberEfficiency() const override { return QuadraticEfficiency; }
   virtual OperationResult MultiplyByNumber(const ElementType& number) const override{ return multiplyByNumber(number); }
   virtual EfficiencyType MultiplyEfficiency(const Matrix<ElementType>& anotherMatrix, bool anotherMatrixIsOnTheLeft) const override { return CubicEfficiency; }
   virtual OperationResult Multiply(const Matrix<ElementType>& anotherMatrix, bool anotherMatrixIsOnTheLeft) const override{ return multiply(anotherMatrix, anotherMatrixIsOnTheLeft); }
   virtual EfficiencyType TransposeEfficiency() const override { return QuadraticEfficiency; }
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

   OperationResult add(const Matrix<ElementType>& otherMatrix) const
   {
      OperationResult result;
      CheckIfCanAddTogether<ElementType>(*this, otherMatrix, result.Code_, result.Description_);
      if (result.Code_ == OperationResultCode::Error)
      {
         return result;
      }

      const Matrix<ElementType>& matrix1 = *this;
      const Matrix<ElementType>& matrix2 = otherMatrix;
      auto initFunc = [&matrix1, &matrix2](size_t row, size_t column)->ElementType { return matrix1.Element(row, column) + matrix2.Element(row, column); };
      result.Matrix_ = std::make_shared<StandardMatrix<ElementType>>(RowCount(), ColumnCount(), initFunc);
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