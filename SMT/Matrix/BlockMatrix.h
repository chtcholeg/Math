#ifndef __STANDARD_MATRIX_H__
#define __STANDARD_MATRIX_H__

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Block matrix
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MatrixDefs.h"
#include "MatrixOperations.h"
#include "MatrixAlgorithms.h"
#include "StandardMatrix.h"

namespace SMT
{
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Block matrix.
// It represents as a matrix of standard matrices.
template <typename ElementType>
class BlockMatrix 
   : public Matrix<ElementType>
   , public Matrix<ElementType>::IElementaryOperations
{
public:
   using InitFunc = std::function<ElementType (size_t /*column*/, size_t /*row*/)>; // A function which initializes all matrix elements
   BlockMatrix(size_t rowCount, size_t columnCount, std::uint32_t blockSize, InitFunc initFunc)
      : ownData_(rowCount * columnCount)
      , rowCount_(rowCount)
      , columnCount_(columnCount)
      , blockSize_(blockSize == 0 ? 1 : blockSize)
   {
      assert(initFunc);
      assert(blockSize != 0);
      if (!initFunc)
      {
         const ElementType retValue = MatrixSettings::Zero<ElementType>();
         initFunc = [retValue](size_t /*row*/, size_t /*column*/)->ElementType { return retValue; };
      }
      init(rowCount_, columnCount_, initFunc);
   }
   BlockMatrix(const Matrix<ElementType>& sourceMatrix, std::uint32_t blockSize)
      : ownData_(sourceMatrix.RowCount() * sourceMatrix.ColumnCount())
      , rowCount_(sourceMatrix.RowCount())
      , columnCount_(sourceMatrix.ColumnCount())
      , blockSize_(blockSize == 0 ? 1 : blockSize)
   {
      assert(blockSize != 0);
      auto initFunc = [&sourceMatrix](size_t row, size_t column)->ElementType { return sourceMatrix.Element(row, column); };
      init(rowCount_, columnCount_, initFunc);
   }
   explicit BlockMatrix(const BlockMatrix<ElementType>& sourceMatrix)
      : ownData_(sourceMatrix.rowCount_ * sourceMatrix.columnCount_)
      , rowCount_(sourceMatrix.rowCount_)
      , columnCount_(sourceMatrix.columnCount_)
      , blockSize_(sourceMatrix.blockSize_ == 0 ? 1 : sourceMatrix.blockSize_)
      , blocks_(sourceMatrix.blocks_)
   {
      assert(sourceMatrix.blockSize_ != 0);
      const size_t totalCount = rowCount_ * columnCount_;
      for (size_t i = 0; i < totalCount; ++i)
      {
         data_[i] = sourceMatrix.data_[i];
      }
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
      result.Matrix_ = std::make_shared<BlockMatrix<ElementType>>(matrix1.RowCount(), matrix1.ColumnCount(), blockSize_, initFunc);
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
   virtual size_t RowCount() const override { return rowCount_; }
   virtual size_t ColumnCount() const override { return columnCount_; }
   virtual ElementType Element(size_t row, size_t column) const override { return data_[row * columnCount_ + column]; }
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
   using Block = std::shared_ptr<StandardMatrix<ElementType>>;

private:
   std::vector<ElementType> ownData_;
   std::vector<std::vector<Block>> blocks_;
   const size_t rowCount_;
   const size_t columnCount_;
   const std::uint64_t blockSize_;   

   void init(size_t rowCount, size_t columnCount, InitFunc initFunc)
   {
      const size_t blockRowCount = (rowCount + blockSize_ - 1) / blockSize_;
      const size_t blockColumnCount = (columnCount_ + blockSize_ - 1) / blockSize_;
      blocks_.resize(blockRowCount);

      for (size_t blockRowIndex = 0; blockRowIndex < blockRowCount; ++blockRowIndex)
      {
         auto& blockRow = blocks_[blockRowIndex];
         blockRow.resize(blockColumnCount);
         for (size_t blockColumnIndex = 0; blockColumnIndex < blockColumnCount; ++blockColumnIndex)
         {
            const size_t blockWidth = ((blockRowIndex < blockRowCount - 1) || (rowCount_ % blockSize_ == 0)) ? blockSize_ : (rowCount_ % blockSize_);
            const size_t blockHeight = ((blockColumnIndex < blockColumnCount - 1) || (columnCount_ % blockSize_ == 0)) ? blockSize_ : (columnCount_ % blockSize_);
            auto blockInit = [=](size_t blockColumn, size_t blockRow) -> ElementType
            {
               size_t matrixColumn = 0;
               size_t matrixRow = 0;
               convertBlockCoordinatesToMatrixCoordinates(blockColumnIndex, blockRowIndex, blockColumn, blockRow, matrixColumn, matrixRow);
               return initFunc(matrixRow, matrixColumn);
            };
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
      if (rowIndex1 >= rowCount_ || rowIndex2 >= rowCount_)
      {
         return false;
      }
      if (rowIndex1 == rowIndex2)
      {
         return true;
      }
      auto row1 = &data_[rowIndex1 * columnCount_];
      auto row2 = &data_[rowIndex2 * columnCount_];
      for (size_t column = 0; column < columnCount_; ++column)
      {
         const auto buffer = row1[column];
         row1[column] = row2[column];
         row2[column] = buffer;
      }
      return true;
   }

   bool multiplyByNumber(size_t rowIndex, ElementType number)
   {
      if (rowIndex >= rowCount_)
      {
         return false;
      }
      auto row = &data_[rowIndex * columnCount_];
      for (size_t column = 0; column < columnCount_; ++column)
      {
         row[column] *= number;
      }
      return true;
   }

   bool multiplyAndSubtract(size_t rowIndex1, size_t rowIndex2, ElementType number)
   {
      if (rowIndex1 >= rowCount_ || rowIndex2 >= rowCount_)
      {
         return false;
      }
      auto row1 = &data_[rowIndex1 * columnCount_];
      auto row2 = &data_[rowIndex2 * columnCount_];
      for (size_t column = 0; column < columnCount_; ++column)
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

   static void convertBlockCoordinatesToMatrixCoordinates(size_t blockColumnIndex, size_t blockRowIndex, size_t blockColumn, size_t blockRow, size_t& matrixColumn, size_t& matrixRow)
   {
      matrixColumn = blockColumnIndex * blockSize_ + blockColumn;
      matrixRow = blockRowIndex * blockSize_ + blockRow;
   }
};

} // namespace SMT

#endif // __STANDARD_MATRIX_H__