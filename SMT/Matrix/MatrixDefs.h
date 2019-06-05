///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The main objects that are related to matrices
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <functional>
#include <memory>
#include <vector>

namespace SMT
{
namespace Matrix
{
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Base class for matrices.
// All specific matrices have to be inherited from this class.
template <typename ElementType>
class MatrixBase
{
public:
   // -- Type definitions
   using SharedPtr = std::shared_ptr<MatrixBase>;   // Pointer to matrix
   using EfficiencyType = unsigned long long;       // Type of algorithm efficiency evaluation

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
   virtual ElementType Element(size_t row, size_t column) const = 0;
   // Copies itself deeply.
   virtual SharedPtr Copy() const = 0;

   // -- Operations
   //    All *****Effeciency methods return special coefficient which means: how many times does the computation time increase by if the matrices sizes are increased by 10 times?
   //    (!) The computation time includes the creation of the result.
   //    Examples:
   //       1. Increasing of copying/addition time is quadric (depending on the size of matrix). So if we increase matrix size by 10 times the copying/addition time increases by 100 times. So CopyingEfficiency()/AdditionEfficiency() returns 100 by default.
   //       2. Diagonal matrix represents only by diagonal elements. So if we increase matrix size by 10 times the copying time of diagonal matrix increases by 10 times only. So CopyingEfficiency() returns 10.
   //       3. Function matrix contains an only function which calculates every element and doesn't contain any matrix data (except size, of course). So if we increase matrix size by 10 times the copying time of diagonal doesn't increase. So CopyingEfficiency() returns 1!
   //       4. Zero matrix "knows" that it doesn't change another matrix (another matrix which is involved in the operation with Zero matrix) on addition. So we need to create only a copy of another matrix. So AdditionEfficiency() of zero matrix is equal CopyingEfficiency() of another matrix.
   //    
   // Copying
   virtual EfficiencyType CopyingEfficiency() const { return 100; }
   // Addition
   virtual EfficiencyType AdditionEfficiency(const Matrix& /*otherMatrix*/) { return 100; }
   virtual SharedPtr Add(const Matrix& /*otherMatrix*/) { return nullptr; }
   // Multiplication
   virtual EfficiencyType MultiplyByNumberEfficiency() const { return 100; }
   virtual SharedPtr MultiplyByNumber(const T& /*number*/) { return nullptr; }
   virtual EfficiencyType MultiplyEfficiency(const Matrix& anotherMatrix, bool anotherMatrixIsOnTheLeft) { return 1000; }
   virtual SharedPtr Multiply(const Matrix& anotherMatrix, bool anotherMatrixIsOnTheLeft) { return nullptr; }
   // Transposition
   virtual EfficiencyType TransposeEfficiency() const { return 100; }
   virtual SharedPtr Transpose() { return nullptr; }
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Main matrix settings. 
// Class has to be specialized if you want another behavior.
template <typename ElementType>
struct Settings
{
	static ElementType Zero() { return static_cast<ElementType>(0); }
	static ElementType One() { return static_cast<ElementType>(1); };
	template<class T> static typename std::enable_if<std::is_floating_point<ElementType>::value, ElementType>::type Epsilon() { return std::numeric_limits<ElementType>::epsilon(); }
	template<class T> static typename std::enable_if<std::is_integral<ElementType>::value, ElementType>::type Epsilon() { return Zero(); }
	static bool CanAssumeItIsZero(const ElementType& element) { return std::abs<ElementType>(element - Zero()) < Epsilon(); }	
};
} // namespace Matrix
} // namespace SMT