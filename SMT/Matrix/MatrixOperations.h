///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The main operations that are related to matrices
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace SMT
{

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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Standard matrix.
// It represents as a vector of vectors.
// Very simple but not optimized.
template <typename ElementType = double>
class StandardMatrix : public Matrix<ElementType>
{
public:
   using InitFunc = std::function<ElementType (size_t /*column*/, size_t /*row*/)>; // A function which initializes all matrix elements
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