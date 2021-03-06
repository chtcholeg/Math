///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Logging classes
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <memory>
#include <string>

namespace SMT
{
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Logger
struct Logger
{
   using WeakPtr = std::weak_ptr<Logger>;

   virtual ~Logger() {};

   enum class Level
   {
      Error,      // Very important information. Better log it
      Warning,    // Important information (it is not error)
      Verbose,    // Details
   };

   virtual void SMTLog(Level level, const std::string& text) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sets a global logger
void SetLogger(const Logger::WeakPtr& logger);
// Adds text into global logger
void Log(Logger::Level level, const std::string& text);
} // namespace SMT