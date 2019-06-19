#ifndef __MATRIX_DEFS_H__
#define __MATRIX_DEFS_H__

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
// A type of result for each matrix operation
enum class OperationResultCode
{
   Ok,                                       // Operation is successfully done
   NotImplemented,                           // Operation is not implemented in the inherited class
   Error,                                    // Operation can't be done. Matrix_ is not valid! OperationResult::ErrorDescription_ contains a description.
   Warning,                                  // Operation is done. Matrix_ contains a valid result but there is some specific information which can be logged
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
   using EfficiencyType = long long int;        // Type of algorithm efficiency evaluation. The more value is, the more efficient an algorithm is
   // A result for each matrix operation
   struct OperationResult
   {
      OperationResultCode Code_ =               // Type of the result
          OperationResultCode::NotImplemented;   
      std::string Description_;            		// Description error/warning/action. It is used on logging.
      SharedPtr Matrix_;                        // The result of the operation. It equals nullptr if the operation is failed
   };

   // -- Constants
   enum : long long
   {
      ConstEfficiency = -1,
      LogEfficiency = -7,
      LinearEfficiency = -100,
      QuadraticEfficiency = -10000,
      CubicEfficiency = -1000000,
      UndefinedEfficiency = LLONG_MIN,
   };

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
   virtual ElementType Element(size_t /*row*/, size_t /*column*/) const = 0;
   // Name of matrix type
   virtual std::string TypeName() const = 0;

   // -- Operations
   //    All *****Effeciency methods return special coefficient which means: how many times does the computation time increase by if the matrices sizes are increased by 100 times?
   //    (!) The computation time includes the creation of the result.
   //    Examples:
   //       1. Increasing of copying/addition time is quadric (depending on the size of matrix). So if we increase matrix size by 100 times the copying/addition time increases by 10000 times (usually). So CopyingEfficiency()/AdditionEfficiency() returns 10000 (usually).
   //       2. Diagonal matrix represents only by diagonal elements. So if we increase matrix size by 100 times the copying time of diagonal matrix increases by 100 times only. So CopyingEfficiency() returns 100.
   //       3. Function matrix contains an only function which calculates every element and doesn't contain any matrix data (except size, of course). So if we increase matrix size by 100 times the copying time of diagonal doesn't increase. So CopyingEfficiency() returns 1!
   //       4. Zero matrix "knows" that it doesn't change another matrix (another matrix which is involved in the operation with Zero matrix) on addition. So we need to create only a copy of another matrix. So AdditionEfficiency() of zero matrix is equal CopyingEfficiency() of another matrix.
   //    All operations return OperationResult, a structure which contains a result code and a result matrix (if the operation has been done successfully)
   // Copying (deeply)
   virtual EfficiencyType CopyingEfficiency() const { return UndefinedEfficiency; }
   virtual OperationResult Copy() const { return OperationResult(); }
   // Addition
   virtual EfficiencyType AdditionEfficiency(const Matrix<ElementType>& /*otherMatrix*/) const { return UndefinedEfficiency; }
   virtual OperationResult Add(const Matrix<ElementType>& /*otherMatrix*/) const { return OperationResult(); }
   // Multiplication
   virtual EfficiencyType MultiplyByNumberEfficiency() const { return UndefinedEfficiency; }
   virtual OperationResult MultiplyByNumber(const ElementType& /*number*/) const { return OperationResult(); }
   virtual EfficiencyType MultiplyEfficiency(const Matrix<ElementType>& anotherMatrix, bool anotherMatrixIsOnTheLeft) const { return UndefinedEfficiency; }
   virtual OperationResult Multiply(const Matrix<ElementType>& anotherMatrix, bool anotherMatrixIsOnTheLeft) const { return OperationResult(); }
   // Transposition
   virtual EfficiencyType TransposeEfficiency() const { return UndefinedEfficiency; }
   virtual OperationResult Transpose() const { return OperationResult(); }
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Main matrix settings. 
namespace MatrixSettings
{
template <typename ElementType>
   ElementType Zero() { return static_cast<ElementType>(0); }
template <typename ElementType>
   ElementType One() { return static_cast<ElementType>(1); }
template <typename ElementType>
   typename std::enable_if<std::is_integral<ElementType>::value, ElementType>::type Epsilon() { return Zero<ElementType>(); }
template <typename ElementType>
   typename std::enable_if<std::is_floating_point<ElementType>::value, ElementType>::type Epsilon() { return std::numeric_limits<ElementType>::epsilon(); }
template <typename ElementType>
   bool CanAssumeItIsZero(const ElementType& element) { return std::abs<ElementType>(element - Zero<ElementType>()) <= Epsilon<ElementType>(); }
} // namespace MatrixSettings

} // namespace SMT

#endif // __MATRIX_DEFS_H__