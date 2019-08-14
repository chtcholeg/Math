#ifndef __BLOCK_MATRIX_H__
#define __BLOCK_MATRIX_H__

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Block matrix
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <functional>

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
   BlockMatrix(size_t rowCount, size_t columnCount, std::uint32_t blockSize, InitFunc initFunc = InitFunc)
      : ownData_(rowCount * columnCount, MatrixSettings::Zero<ElementType>())
      , rowCount_(rowCount)
      , columnCount_(columnCount)
      , blockSize_(blockSize == 0 ? 1 : blockSize)
   {
      assert(blockSize != 0);
      if (initFunc)
      {
         init(initFunc);
      }
   }
   BlockMatrix(const Matrix<ElementType>& sourceMatrix, std::uint32_t blockSize)
      : ownData_(sourceMatrix.RowCount() * sourceMatrix.ColumnCount())
      , rowCount_(sourceMatrix.RowCount())
      , columnCount_(sourceMatrix.ColumnCount())
      , blockSize_(blockSize == 0 ? 1 : blockSize)
   {
      assert(blockSize != 0);
      auto initFunc = [&sourceMatrix](size_t row, size_t column)->ElementType { return sourceMatrix.Element(row, column); };
      init(initFunc);
   }
   explicit BlockMatrix(const BlockMatrix<ElementType>& sourceMatrix)
      : ownData_(sourceMatrix.rowCount_ * sourceMatrix.columnCount_)
      , rowCount_(sourceMatrix.rowCount_)
      , columnCount_(sourceMatrix.columnCount_)
      , blockSize_(sourceMatrix.blockSize_ == 0 ? 1 : sourceMatrix.blockSize_)
      , data_(sourceMatrix.data_)
   {
      assert(sourceMatrix.blockSize_ != 0);
   }

   static OperationResult Add(const Matrix<ElementType>& matrix1, const Matrix<ElementType>& matrix2)
   {
      OperationResult result;
      CheckIfCanAddTogether<ElementType>(matrix1, matrix2, result.Code_, result.Description_);
      if (result.Code_ == OperationResultCode::Error)
      {
         return result;
      }
      const BlockMatrix<ElementType>& blockMatrix1 = dynamic_cast<const BlockMatrix<ElementType>&>(matrix1);
      const BlockMatrix<ElementType>& blockMatrix2 = dynamic_cast<const BlockMatrix<ElementType>&>(matrix2);
      if ( (blockMatrix1 != nullptr) && (blockMatrix2 != nullptr) && (blockMatrix1.blockSize_ == blockMatrix2.blockSize_) )
      {
         result.Matrix_ = std::make_shared<BlockMatrix<ElementType>>(matrix1.RowCount(), matrix1.ColumnCount(), blockSize_);
         std::transform(blockMatrix1.ownData_.begin(), blockMatrix1.ownData_.end(), blockMatrix2.ownData_.begin(), result.Matrix_->ownData_.begin(), std::plus<ElementType>());
         result.Code_ = OperationResultCode::Ok;
      }
      else
      {
         result.Code_ = OperationResultCode::NotImplemented;
      }
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
      const BlockMatrix<ElementType>& blockMatrix1 = dynamic_cast<const BlockMatrix<ElementType>&>(matrix1);
      const BlockMatrix<ElementType>& blockMatrix2 = dynamic_cast<const BlockMatrix<ElementType>&>(matrix2);
      if ((blockMatrix1 != nullptr) && (blockMatrix2 != nullptr) && (blockMatrix1.blockSize_ == blockMatrix2.blockSize_))
      {
         result.Matrix_ = multiply(blockMatrix1, blockMatrix2);
         result.Code_ = (result.Matrix_ == nullptr) ? OperationResultCode::Error : OperationResultCode::Ok;
      }
      else
      {
         result.Code_ = OperationResultCode::NotImplemented;
      }
      return result;
   }

   // Matrix
   virtual size_t RowCount() const override { return rowCount_; }
   virtual size_t ColumnCount() const override { return columnCount_; }
   virtual ElementType Element(size_t row, size_t column) const override { return element(row, column); }
   virtual std::string TypeName() const { return "BlockMatrix"; }
   virtual Complexity::Type CopyingComplexity() const override { return Complexity::Quadratic; }
   virtual OperationResult Copy() const override { return copy(); }
   virtual Complexity::Type AdditionComplexity(const Matrix<ElementType>& otherMatrix) const override { return Complexity::Quadratic; }
   virtual OperationResult Add(const Matrix<ElementType>& otherMatrix) const override { return Add(*this, otherMatrix); }
   virtual Complexity::Type MultiplyByNumberComplexity() const override { return Complexity::Quadratic; }
   virtual OperationResult MultiplyByNumber(const ElementType& number) const override{ return multiplyByNumber(number); }
   virtual Complexity::Type MultiplyComplexity(const Matrix<ElementType>& anotherMatrix, bool anotherMatrixIsOnTheLeft) const override { return Complexity::Cubic; }
   virtual OperationResult Multiply(const Matrix<ElementType>& anotherMatrix, bool anotherMatrixIsOnTheLeft) const override{ return anotherMatrixIsOnTheLeft ? Multiply(anotherMatrix, *this) : Multiply(*this, anotherMatrix); }
   virtual Complexity::Type InversionComplexity() const override { return Complexity::Cubic; }
   virtual OperationResult Invert() const override { return Algorithms::GaussJordanEliminationBlock<ElementType>(*this, createIdentityMatrix); }
   virtual Complexity::Type TransposeComplexity() const override { return Complexity::Quadratic; }
   virtual OperationResult Transpose() const override { return transpose(); }
   virtual Complexity::Type DeterminantEvaluationComplexity() const override { return Complexity::Cubic; }
   virtual ScalarOperationResult Determinant() const override { return Algorithms::CalcDeterminant_GaussJordanEliminationBlock<ElementType>(*this); }
   virtual IElementaryOperations* ElementaryOperations() { return this; }

private:
   using Block = std::shared_ptr<StandardMatrix<ElementType>>;

private:
   std::vector<ElementType> ownData_;
   const size_t rowCount_;
   const size_t columnCount_;
   const std::uint64_t blockSize_;   

   void init(InitFunc initFunc)
   {
      const size_t blockRowCount = (rowCount_ + blockSize_ - 1) / blockSize_;
      const size_t blockColumnCount = (columnCount_ + blockSize_ - 1) / blockSize_;
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
            const size_t blockRowStart = blockRowIndex * blockSize_ * columnCount_;
            const size_t blockStart = blockRowStart + blockColumnIndex * blockSize_ * blockSize_;
            StandardMatrix<ElementItem>(blockHeight, blockWidth, ownData_[blockStart], blockInit);
         }
      }
   }

   ElementType element(size_t row, size_t column) const
   {
      const size_t blockRowCount = (rowCount_ + blockSize_ - 1) / blockSize_;
      const size_t blockRowIndex = row / blockSize_;
      const size_t blockColumnIndex = column / blockSize_;
      const size_t blockWidth = ((blockRowIndex < blockRowCount - 1) || (rowCount_ % blockSize_ == 0)) ? blockSize_ : (rowCount_ % blockSize_);
      const size_t blockRowStart = blockRowIndex * blockSize_ * columnCount_;
      const size_t blockStart = blockRowStart + blockColumnIndex * blockSize_ * blockSize_;
      return ownData_[blockStart + (row % blockSize_) * blockWidth + column % blockSize_];
   }

   OperationResult copy() const
   {
      OperationResult result;
      result.Matrix_ = std::make_shared<BlockMatrix<ElementType>>(*this);
      result.Code_ = OperationResultCode::Ok;
      return result;
   }

   OperationResult multiplyByNumber(const ElementType& number) const
   { 
      OperationResult result;
      result.Matrix_ = std::make_shared<BlockMatrix<ElementType>>(*this);
      auto& data = result.Matrix_->data_;
      std::transform(data.begin(), data.end(), data.begin(), [number](ElementType& value)->ElementType { return value * number; });
      result.Code_ = OperationResultCode::Ok;
      return result;
   }

   static typename BlockMatrix<ElementType>::SharedPtr multiply(const BlockMatrix<ElementType>& m1, const BlockMatrix<ElementType>& m2)
   {
      if (m1.blockSize_ != m2.blockSize_)
      {
         return nullptr;
      }
      BlockMatrix<ElementType>::SharedPtr result(std::make_shared<BlockMatrix<ElementType>>(m1.RowCount(), m2.ColumnCount(), m1.blockSize_));

      const size_t blockRowCount1 = (m1.RowCount() + blockSize_ - 1) / blockSize_;
      const size_t blockColumnCount1 = (m1.ColumnCount() + blockSize_ - 1) / blockSize_;
      const size_t blockRowCount2 = (m2.RowCount() + blockSize_ - 1) / blockSize_;
      const size_t blockColumnCount2 = (m2.ColumnCount() + blockSize_ - 1) / blockSize_;

      for (size_t blockColumnIndexResult = 0; blockColumnIndexResult < blockColumnCount2; ++blockColumnIndexResult) 
      {
         for (size_t blockRowIndexResult = 0; blockRowIndexResult < blockRowCount1; ++blockRowIndexResult)
         {
            const size_t blockWidth = ((blockRowIndexResult < blockRowCount1 - 1) || (m1.RowCount() % blockSize_ == 0)) ? blockSize_ : (m1.RowCount() % blockSize_);
            const size_t blockHeight = ((blockColumnIndexResult < blockColumnCount2 - 1) || (m2.ColumnCount() % blockSize_ == 0)) ? blockSize_ : (m2.ColumnCount() % blockSize_);
            auto& dataStart = ownData_[blockStart + (row % blockSize_) * blockWidth + column % blockSize_]
            StandardMatrix<ElementType> resultBlock(blockHeight, blockWidth, );


         }
      }

      const size_t blockRowIndex = row / blockSize_;
      const size_t blockColumnIndex = column / blockSize_;
      const size_t blockWidth = ((blockRowIndex < blockRowCount - 1) || (rowCount_ % blockSize_ == 0)) ? blockSize_ : (rowCount_ % blockSize_);
      const size_t blockRowStart = blockRowIndex * blockSize_ * columnCount_;
      const size_t blockStart = blockRowStart + blockColumnIndex * blockSize_ * blockSize_;

      return result;
   }
   
   OperationResult transpose() const
   {
      OperationResult result;
      auto initFunc = [this](size_t row, size_t column)->ElementType { return Element(column, row); };
      result.Matrix_ = std::make_shared<BlockMatrix<ElementType>>(ColumnCount(), RowCount(), initFunc);
      result.Code_ = OperationResultCode::Ok;
      return result;
   }

   static Matrix<ElementType>::SharedPtr createIdentityMatrix(size_t size)
   {
      auto initFunc = [](size_t row, size_t column) -> ElementType
      {
         return (row == column) ? MatrixSettings::One<ElementType>() : MatrixSettings::Zero<ElementType>();
      };
      return std::make_shared<BlockMatrix<ElementType>>(size, size, initFunc, blockSize_);
   }

   static void convertBlockCoordinatesToMatrixCoordinates(size_t blockColumnIndex, size_t blockRowIndex, size_t blockColumn, size_t blockRow, size_t& matrixColumn, size_t& matrixRow)
   {
      matrixColumn = blockColumnIndex * blockSize_ + blockColumn;
      matrixRow = blockRowIndex * blockSize_ + blockRow;
   }
};

} // namespace SMT

#endif // __BLOCK_MATRIX_H__