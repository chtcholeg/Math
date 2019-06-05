#include <functional>
#include <memory>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The main interfaces that are related to matrices
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace SMT
{

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Base class for matrices.
// All specific matrices have to be inherited from this class.
class Matrix
{
public:
   // -- Type definitions
   using T = double;                            // Type of a matrix element
   using SharedPtr = std::shared_ptr<Matrix>;   // Pointer to matrix
   // -- Constants
   static const T Epsilon;                      // A very small number which is considered like zero

   // -- Class-specific methods
   // Constructor
   Matrix() {}
   // Required virtual destructor
   virtual ~Matrix() {}

   // -- Matrix data
   // Matrix parameters
   virtual size_t RowCount() const = 0;
   virtual size_t ColumnCount() const = 0;
   // Access to matrix element.
   // If row >= RowCount() or column >= ColumnCount() behavior is undefined
   virtual T Element(size_t row, size_t column) const = 0;
   // Copies itself deeply.
   virtual SharedPtr Copy() const = 0;

protected:
   // -- Operations
   //    All *****Effeciency methods return special coefficient which means: how many times would the computation time increase by, if the matrices sizes were increased by 10 times?
   //    (!) The computation time includes the creation of the result.
   //    Examples:
   //       1. Increasing of copying/addition time is quadric. So if we increase matrix size by 10 times the copying/addition time increases by 100 times. So CopyingEfficiency()/AdditionEfficiency() returns 100 by default.
   //       2. Diagonal matrix represents only by diagonal elements. So if we increase matrix size by 10 times the copying time of diagonal matrix increases by 10 times. So CopyingEfficiency() returns 10.
   //       3. Functional matrix represents by a function. So if we increase matrix size by 10 times the copying time of diagonal doesn't increase. So CopyingEfficiency() returns 1!
   //       4. Zero matrix "knows" that it doesn't change other matrix on addition. So we need to create only a copy of other matrix. So AdditionEfficiency() of zero matrix is equal CopyingEfficiency() of other matrix.
   // Copying
   virtual size_t CopyingEfficiency() const { return 100; }
   // Addition
   virtual size_t AdditionEfficiency(const Matrix& /*otherMatrix*/) { return 100; }
   virtual bool Add(const Matrix& /*otherMatrix*/) { return false; }
   // Multiplication
   virtual SharedPtr Multiply(const T& /*number*/) = 0;
   virtual bool LMultiply(const Matrix& /*leftMatrix*/) { return false; }    // this = leftMatrix * this
   virtual bool RMultiply(const Matrix& /*rightMatrix*/) { return false; }   // this = this * rightMatrix
   // Transposition
   virtual bool Transpose() { return false; }

   // -- Properties checking
   //    Implementations of these methods are simple: they run throw ALL elements and check them (used functions: RowCount(), ColumnCount() and Element(...)) 
   virtual bool CheckIfIdentityMatrix() const;
   virtual bool CheckIfZeroMatrix() const;
   virtual bool CheckIfDiagonalMatrix() const;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Standard matrix.
// It represents as a vector of vectors.
// Very simple but not optimized.
class StandardMatrix : public Matrix
{
public:
   using InitFunc = std::function<Matrix::T (size_t /*column*/, size_t /*row*/)>; // A function which initializes all matrix elements
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