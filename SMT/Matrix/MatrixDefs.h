#ifndef __MATRIX_DEFS_H__
#define __MATRIX_DEFS_H__

#include <cmath>
#include <ctgmath>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The main objects that are related to matrices
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <functional>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

namespace SMT
{
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Complexity (Big O notation)
struct Complexity
{
   using Type = int;
   enum : Type {
      Constant,                                 // O(1)
      DoubleLogarithmic,                        // O(log(logN))
      Logarithmic,                              // O(logN)
      Polylogarithmic,                          // O((logN)^C), C > 1
      FractionalPower,                          // O(N^C), 0 < C < 1
      Linear,                                   // O(N)
      Loglinear,                                // O(N * logN)
      Quadratic,                                // O(N^2)
      Cubic,                                    // O(N^3)
      Polynomial,                               // O(N^C), C > 3
      Exponential,                              // O(C^N)
      Factorial,                                // O(N!)
      
      Max,
      Undefined = Max,
   };
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// A type of result for each matrix operation
enum class OperationResultCode
{
   Ok,                                          // Operation is successfully done
   NotImplemented,                              // Operation is not implemented in the inherited class
   Error,                                       // Operation can't be done. Matrix_ is not valid! OperationResult::ErrorDescription_ contains a description.
   Warning,                                     // Operation is done. Matrix_ contains a valid result but there is some specific information which can be logged
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Base class for matrices.
// All specific matrices have to be inherited from this class.
template <typename ElementType>
class Matrix
{
public:
   // -- Type definitions
   using SharedPtr = std::shared_ptr<Matrix>;   // Pointer to matrix
   // A result for each matrix operation
   struct OperationResult
   {
      OperationResultCode Code_ =               // Type of the result
          OperationResultCode::NotImplemented;   
      std::string Description_;            		// Description error/warning/action. It is used on logging.
      SharedPtr Matrix_;                        // The result of the operation. It equals nullptr if the operation is failed
   };

   // -- Class-specific methods
   // Constantructor
   Matrix() {}
   // Required virtual destructor
   virtual ~Matrix() {}

   // -- Matrix data
   // Matrix parameters
   virtual size_t RowCount() const = 0;
   virtual size_t ColumnCount() const = 0;
   // Access to matrix element.
   // If row >= RowCount() or column >= ColumnCount() behavior is undefined
   virtual ElementType Element(size_t /*row*/, size_t /*column*/) const = 0;
   // Name of matrix type
   virtual std::string TypeName() const = 0;

   // -- Operations
   //    (!) The complexity includes the creation of the result.
   //    Examples:
   //       1. Increasing of copying/addition time is quadratic (depending on the size of matrix). So if we increase matrix size by 100 times the copying/addition time increases by 10000 times (usually). So CopyingComplexity()/AdditionComplexity() returns Complexity::Quadratic (usually).
   //       2. Diagonal matrix represents only by diagonal elements. So if we increase matrix size by 100 times the copying time of diagonal matrix increases by 100 times only. So CopyingComplexity() returns Complexity::Linear.
   //       3. Function matrix contains an only function which calculates every element and doesn't contain any matrix data (except size, of course). So if we increase matrix size by 100 times the copying time of diagonal doesn't increase. So CopyingComplexity() returns Complexity::Constantant!
   //       4. Zero matrix "knows" that it doesn't change another matrix (another matrix which is involved in the operation with Zero matrix) on addition. So we need to create only a copy of another matrix. So AdditionComplexity() of zero matrix is equal CopyingComplexity() of another matrix.
   //    All operations return OperationResult, a structure which contains a result code and a result matrix (if the operation has been done successfully)
   // Copying (deeply)
   virtual Complexity::Type CopyingComplexity() const { return Complexity::Undefined; }
   virtual OperationResult Copy() const { return OperationResult(); }
   // Addition
   virtual Complexity::Type AdditionComplexity(const Matrix<ElementType>& /*otherMatrix*/) const { return Complexity::Undefined; }
   virtual OperationResult Add(const Matrix<ElementType>& /*otherMatrix*/) const { return OperationResult(); }
   // Multiplication
   virtual Complexity::Type MultiplyByNumberComplexity() const { return Complexity::Undefined; }
   virtual OperationResult MultiplyByNumber(const ElementType& /*number*/) const { return OperationResult(); }
   virtual Complexity::Type MultiplyComplexity(const Matrix<ElementType>& anotherMatrix, bool anotherMatrixIsOnTheLeft) const { return Complexity::Undefined; }
   virtual OperationResult Multiply(const Matrix<ElementType>& anotherMatrix, bool anotherMatrixIsOnTheLeft) const { return OperationResult(); }
   // Invert
   virtual Complexity::Type InversionComplexity() const { return Complexity::Undefined; }
   virtual OperationResult Invert() const { return OperationResult(); }
   // Transposition
   virtual Complexity::Type TransposeComplexity() const { return Complexity::Undefined; }
   virtual OperationResult Transpose() const { return OperationResult(); }

   // Interface for elementary operations
   struct IElementaryOperations
   {
      // Swaps the positions of two rows
      virtual bool SwapRows(size_t rowIndex1, size_t rowIndex2) = 0;
      // Multiplies a row by a number
      virtual bool MultiplyRowByNumber(size_t rowIndex, ElementType number) = 0;
      // Row#1 = Row#1 - Row#2 * Number 
      virtual bool MultiplyAndSubtract(size_t rowIndex1, size_t rowIndex2, ElementType number) = 0;
   };
   virtual IElementaryOperations* ElementaryOperations() { return nullptr; }
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Main matrix settings. 
namespace MatrixSettings
{
template <typename ElementType>
   ElementType Zero() { return static_cast<ElementType>(0); }
template <typename ElementType>
   ElementType One() { return static_cast<ElementType>(1); }
template <typename ElementType>
   ElementType Max() { return std::numeric_limits<ElementType>::max(); }
template <typename ElementType>
   typename std::enable_if<std::is_integral<ElementType>::value, ElementType>::type Epsilon() { return Zero<ElementType>(); }
template <typename ElementType>
   typename std::enable_if<std::is_floating_point<ElementType>::value, ElementType>::type Epsilon() { return std::numeric_limits<ElementType>::epsilon(); }
template <typename ElementType>
   bool CanAssumeItIsZero(const ElementType& element) { return std::abs<ElementType>(element - Zero<ElementType>()) <= Epsilon<ElementType>(); }
} // namespace MatrixSettings

} // namespace SMT

#endif // __MATRIX_DEFS_H__