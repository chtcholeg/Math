#include <functional>
#include <memory>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The main interfaces that are related to matrices
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace SMT
{

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Interface of matrix classes.
// All specific matrices have to be inherited from this class.
struct Matrix
{
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
   // If there is no implementation a Standard Matrix is returned (see a declaration StandardMatrix).
   virtual SharedPtr Copy() const;

   // -- Operations
   //    The methods return FALSE if they are not implemented (or can't be done)
   // Addition
   virtual bool Add(const Matrix& /*otherMatrix*/) { return false; }
   // Multiplication
   virtual bool Multiply(const T& /*number*/) { return false; }
   virtual bool LMultiply(const Matrix& /*leftMatrix*/) { return false; }    // leftMatrix * this
   virtual bool RMultiply(const Matrix& /*rightMatrix*/) { return false; }   // this * rightMatrix
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