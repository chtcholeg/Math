///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The main objects that are related to matrices
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "../LogDefs.h"

namespace SMT
{
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Base class for matrices.
// All specific matrices have to be inherited from this class.
template <typename ElementType>
class Matrix
{
public:
   // -- Type definitions
   using SharedPtr = std::shared_ptr<Matrix>;   // Pointer to matrix
   using EfficiencyType = long long int;        // Type of algorithm efficiency evaluation
   // A result for each matrix operation
   enum class OperationResultCode
   {
      Ok,                                       // Operation is successfully done
      NotImplemented,                           // Operation is not implemented in the inherited class
      Error,                                    // Operation can't be done. ResultMatrix_ is not valid! OperationResult::ErrorDescription_ contains a description.
      Warning,                                  // Operation is done. ResultMatrix_ contains a valid result but there is some specific information which can be logged
   };
   struct OperationResult
   {
      OperationResultCode Code_ =               // Type of the result
         OperationResultCode::NotImplemented;   
      std::string ErrorDescription_;            // Description error. It is used on logging.
      SharedPtr ResultMatrix_;                  // The result of the operation. It equals nullptr if the operation is failed
   };

   // -- Constants
   enum
   {
      UndefinedEfficiency = 0,
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

   // -- Operations
   //    All *****Effeciency methods return special coefficient which means: how many times does the computation time increase by if the matrices sizes are increased by 10 times?
   //    (!) The computation time includes the creation of the result.
   //    Examples:
   //       1. Increasing of copying/addition time is quadric (depending on the size of matrix). So if we increase matrix size by 10 times the copying/addition time increases by 100 times. So CopyingEfficiency()/AdditionEfficiency() returns 100 by default.
   //       2. Diagonal matrix represents only by diagonal elements. So if we increase matrix size by 10 times the copying time of diagonal matrix increases by 10 times only. So CopyingEfficiency() returns 10.
   //       3. Function matrix contains an only function which calculates every element and doesn't contain any matrix data (except size, of course). So if we increase matrix size by 10 times the copying time of diagonal doesn't increase. So CopyingEfficiency() returns 1!
   //       4. Zero matrix "knows" that it doesn't change another matrix (another matrix which is involved in the operation with Zero matrix) on addition. So we need to create only a copy of another matrix. So AdditionEfficiency() of zero matrix is equal CopyingEfficiency() of another matrix.
   //    All operations return OperationResult, a structure which contains a result code and a result matrix (if the operation has been done successfully)
   // Copying (deeply)
   virtual EfficiencyType CopyingEfficiency() const { return OperationResult(); }
   virtual OperationResult Copy() const { return OperationResult(); }
   // Addition
   virtual EfficiencyType AdditionEfficiency(const Matrix& /*otherMatrix*/) { return UndefinedEfficiency; }
   virtual OperationResult Add(const Matrix& /*otherMatrix*/) { return OperationResult(); }
   // Multiplication
   virtual EfficiencyType MultiplyByNumberEfficiency() const { return UndefinedEfficiency; }
   virtual OperationResult MultiplyByNumber(const T& /*number*/) { return OperationResult(); }
   virtual EfficiencyType MultiplyEfficiency(const Matrix& anotherMatrix, bool anotherMatrixIsOnTheLeft) { return UndefinedEfficiency; }
   virtual OperationResult Multiply(const Matrix& anotherMatrix, bool anotherMatrixIsOnTheLeft) { return OperationResult(); }
   // Transposition
   virtual EfficiencyType TransposeEfficiency() const { return UndefinedEfficiency; }
   virtual OperationResult Transpose() { return OperationResult(); }
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Main matrix settings. 
// Class has to be specialized if you want another behavior.
template <typename ElementType>
struct MatrixSettings
{
   static ElementType Zero() { return static_cast<ElementType>(0); }
   static ElementType One() { return static_cast<ElementType>(1); };
   template<class T> static typename std::enable_if<std::is_floating_point<ElementType>::value, ElementType>::type Epsilon() { return std::numeric_limits<ElementType>::epsilon(); }
   template<class T> static typename std::enable_if<std::is_integral<ElementType>::value, ElementType>::type Epsilon() { return Zero(); }
   static bool CanAssumeItIsZero(const ElementType& element) { return std::abs<ElementType>(element - Zero()) < Epsilon(); }
};
} // namespace SMT