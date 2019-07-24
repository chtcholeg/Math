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
   FunctionMatrix(size_t rowCount, size_t columnCount, const ElementFunc& elementFunc = ElementFunc())
      : rowCount_(rowCount)
      , columnCount_(columnCount)
      , func_(elementFunc)
   {
      setZeroFuncIfEmpy();
   }
   FunctionMatrix(const FunctionMatrix<ElementType>& sourceMatrix)
      : rowCount_(sourceMatrix.rowCount_)
      , columnCount_(sourceMatrix.columnCount_)
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
   virtual Complexity::Type CopyingComplexity() const override { return Complexity::Constant; }
   virtual OperationResult Copy() const override { return copy(); }
   virtual Complexity::Type AdditionComplexity(const Matrix<ElementType>& otherMatrix) const override { return additionComplexity(otherMatrix); }
   virtual OperationResult Add(const Matrix<ElementType>& otherMatrix) const override{ return add(otherMatrix); }
   virtual Complexity::Type MultiplyByNumberComplexity() const override { return Complexity::Constant; }
   virtual OperationResult MultiplyByNumber(const ElementType& number) const override{ return multiplyByNumber(number); }
   virtual Complexity::Type TransposeComplexity() const override { return Complexity::Constant; }
   virtual OperationResult Transpose() const override{ return transpose(); }

private:
   const size_t rowCount_;
   const size_t columnCount_;
   ElementFunc func_;

   OperationResult copy() const
   {
      OperationResult result;
      result.Matrix_ = std::make_shared<FunctionMatrix<ElementType>>(*this);
      result.Code_ = OperationResultCode::Ok;
      return result;
   }

   Complexity::Type additionComplexity(const Matrix<ElementType>& otherMatrix) const
   {
      return (dynamic_cast<const FunctionMatrix<ElementType>*>(&otherMatrix) != nullptr) ? Complexity::Constant : Complexity::Quadratic;
   }

   OperationResult add(const Matrix<ElementType>& otherMatrix) const
   {
      const FunctionMatrix<ElementType>* otherFuncMatrix = dynamic_cast<const FunctionMatrix<ElementType>*>(&otherMatrix);
      if (otherFuncMatrix != nullptr)
      {
         OperationResult result;
         CheckIfCanAddTogether<ElementType>(*this, otherMatrix, result.Code_, result.Description_);
         if (result.Code_ == OperationResultCode::Error)
         {
            return result;
         }
         auto func1 = func_;
         auto func2 = otherFuncMatrix->func_;
         auto newFunc = [func1, func2](size_t column, size_t row)->ElementType
         {
            return func1(column, row) + func2(column, row);
         };
         result.Matrix_ = std::make_shared<FunctionMatrix<ElementType>>(rowCount_, columnCount_, newFunc);
         return result;
      }
      return StandardMatrix<ElementType>::Add(*this, otherMatrix);
   }
   
   OperationResult multiplyByNumber(const ElementType& number) const
   { 
      OperationResult result;
      auto func = [this, number](size_t row, size_t column)->ElementType { return func_(row, column) * number; };
      result.Matrix_ = std::make_shared<FunctionMatrix<ElementType>>(RowCount(), ColumnCount(), func);
      result.Code_ = OperationResultCode::Ok;
      return result;
   }
   
   OperationResult multiply(const Matrix<ElementType>& anotherMatrix, bool anotherMatrixIsOnTheLeft) const
   {
      return anotherMatrixIsOnTheLeft ? StandardMatrix<ElementType>::Multiply(anotherMatrix, *this) : StandardMatrix<ElementType>::Multiply(*this, anotherMatrix);
   }
   
   OperationResult transpose() const
   {
      OperationResult result;
      auto func = [this](size_t row, size_t column)->ElementType { return func_(column, row); };
      result.Matrix_ = std::make_shared<FunctionMatrix<ElementType>>(ColumnCount(), RowCount(), func);
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