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
         initFunc = [retValue](size_t /*row*/, size_t /*column*/)->ElementType { return retValue; }
      }
      init(rowCount, columnCount, initFunc);
   }
   explicit StandardMatrix(const Matrix<ElementType>& sourceMatrix)
   {
      initFunc = [&sourceMatrix](size_t row, size_t column)->ElementType { return sourceMatrix.Element(row, column); }
      init(sourceMatrix.RowCount(), sourceMatrix.ColumnCount(), );
   }

   // Matrix
   virtual size_t RowCount() const override { return body_.size(); }
   virtual size_t ColumnCount() const override { return body_.empty() ? 0 : body_[0].size(); }
   virtual ElementType Element(size_t row, size_t column) const override { return body_[row][column]; }
   virtual EfficiencyType CopyingEfficiency() const override { return 100; } // Quadric speed
   virtual OperationResult Copy() const override { return copy(); }
   virtual EfficiencyType AdditionEfficiency(const Matrix& otherMatrix) override { return 100; } // Quadric speed
   virtual OperationResult Add(const Matrix& otherMatrix) override { return add(otherMatrix); }
   virtual EfficiencyType MultiplyByNumberEfficiency() const override { return 100; } // Quadric speed
   virtual OperationResult MultiplyByNumber(const T& number) override { return multiplyByNumber(number); }
   virtual EfficiencyType MultiplyEfficiency(const Matrix& anotherMatrix, bool anotherMatrixIsOnTheLeft) { return UndefinedEfficiency; }
   virtual OperationResult Multiply(const Matrix& anotherMatrix, bool anotherMatrixIsOnTheLeft) { return OperationResult(); }
   virtual EfficiencyType TransposeEfficiency() const { return UndefinedEfficiency; }
   virtual OperationResult Transpose() { return OperationResult(); }

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
      result.Code_ = OperationResultCode::Ok;
      result.ResultMatrix_ = std::make_shared<StandardMatrix<ElementType>>(*this);
      return result;
   }

   OperationResult add(const Matrix& otherMatrix)
   {
      OperationResult result;
      CheckIfCanAddTogether(*this, otherMatrix, result.Code_, result.ResultMatrix_);
      if (result.Code_ == OperationResult::Error)
      {
         return result;
      }

      const Matrix& matrix1 = *this;
      const Matrix& matrix2 = otherMatrix;
      auto initFunc = [&matrix1, &matrix2](size_t row, size_t column)->ElementType { return matrix1.Element(row, column) + matrix2.Element(row, column); }
      result.ResultMatrix_ = std::make_shared<>(RowCount(), ColumnCount(), initFunc);
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