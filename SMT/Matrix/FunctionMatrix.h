#ifndef __FUNCTION_MATRIX_H__
#define __FUNCTION_MATRIX_H__

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function matrix
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MatrixDefs.h"
#include "MatrixOperations.h"

namespace SMT
{
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function matrix.
// It represents as a function.
template <typename ElementType>
class FunctionMatrix : public Matrix<ElementType>
{
public:
   using ElementFunc = std::function<ElementType (size_t /*column*/, size_t /*row*/)>; // A function which defines this matrix
   FunctionMatrix(size_t rowCount, size_t columnCount, const ElementFunc& elementFunc = ElementFunc(), EfficiencyType efficiency = Efficiency::Const)
      : rowCount_(rowCount)
      , columnCount_(columnCount)
      , efficiency_(efficiency)
      , func_(elementFunc)
   {
      setZeroFuncIfEmpy();
   }
   FunctionMatrix(const FunctionMatrix<ElementType>& sourceMatrix)
      : rowCount_(sourceMatrix.rowCount_)
      , columnCount_(sourceMatrix.columnCount_)
      , efficiency_(sourceMatrix.efficiency_)
      , func_(sourceMatrix.func_)
   {
      assert(func_);
      setZeroFuncIfEmpy();
   }

   // Matrix
   virtual size_t RowCount() const override { return rowCount_; }
   virtual size_t ColumnCount() const override { return columnCount_; }
   virtual ElementType Element(size_t row, size_t column) const override { return func_(row, column); }
   virtual std::string TypeName() const { return "FunctionMatrix"; }
   virtual EfficiencyType CopyingEfficiency() const override { return Efficiency::Const; }
   virtual OperationResult Copy() const override { return copy(); }
   virtual EfficiencyType AdditionEfficiency(const Matrix<ElementType>& otherMatrix) const override { return additionEfficiency(otherMatrix); }
   virtual OperationResult Add(const Matrix<ElementType>& otherMatrix) const override{ return add(otherMatrix); }
   virtual EfficiencyType MultiplyByNumberEfficiency() const override { return Efficiency::Const; }
   virtual OperationResult MultiplyByNumber(const ElementType& number) const override{ return multiplyByNumber(number); }
   virtual EfficiencyType MultiplyEfficiency(const Matrix<ElementType>& anotherMatrix, bool anotherMatrixIsOnTheLeft) const override { return multiplicationEfficiency(anotherMatrix, anotherMatrixIsOnTheLeft); }
   virtual OperationResult Multiply(const Matrix<ElementType>& anotherMatrix, bool anotherMatrixIsOnTheLeft) const override{ return multiply(anotherMatrix, anotherMatrixIsOnTheLeft); }
   virtual EfficiencyType TransposeEfficiency() const override { return Efficiency::Const; }
   virtual OperationResult Transpose() const override{ return transpose(); }

private:
   size_t rowCount_;
   size_t columnCount_;
   EfficiencyType efficiency_;			// Efficiency of this function (it can be combination of function)
   ElementFunc func_;

   OperationResult copy() const
   {
      OperationResult result;
      result.Matrix_ = std::make_shared<FunctionMatrix<ElementType>>(*this);
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
      result.Matrix_ = std::make_shared<FunctionMatrix<ElementType>>(RowCount(), ColumnCount(), initFunc);
      result.Code_ = OperationResultCode::Ok;
      return result;
   }
   
   OperationResult multiplyByNumber(const ElementType& number) const
   { 
      OperationResult result;
      const Matrix& matrix = *this;
      auto initFunc = [&matrix, number](size_t row, size_t column)->ElementType { return matrix.Element(row, column) * number; };
      result.Matrix_ = std::make_shared<FunctionMatrix<ElementType>>(RowCount(), ColumnCount(), initFunc);
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
      result.Matrix_ = std::make_shared<FunctionMatrix<ElementType>>(leftMatrix.RowCount(), rightMatrix.ColumnCount(), initFunc);
      result.Code_ = OperationResultCode::Ok;
      return result;
   }
   
   OperationResult transpose() const
   {
      OperationResult result;
      auto initFunc = [this](size_t row, size_t column)->ElementType { return Element(column, row); };
      result.Matrix_ = std::make_shared<FunctionMatrix<ElementType>>(ColumnCount(), RowCount(), initFunc);
      result.Code_ = OperationResultCode::Ok;
      return result;
   }

   void setZeroFuncIfEmpy()
   {
      if (!func_) {
         const ElementType retValue = MatrixSettings::Zero<ElementType>();
         func_ = [retValue](size_t /*row*/, size_t /*column*/)->ElementType { return retValue; };
      }
   }
};

} // namespace SMT

#endif // __FUNCTION_MATRIX_H__