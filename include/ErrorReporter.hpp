#ifndef ERRORREPORTER_HPP
#define ERRORREPORTER_HPP

#include <iostream>
#include <vector>

enum class ErrorSeverity
{
    WARNING, ERROR, FATAL
};

struct CompilerError
{
    ErrorSeverity severity;
    std::string message;
    int line;
    int column;
    std::string context;

    void print() const;
};

class ErrorReporter
{
private:
    std::vector<CompilerError> m_Errors;
    bool m_HasFatalError{false};

public:
    void reportError(ErrorSeverity severity, const std::string& message, int line, int column,
                     const std::string& context = "");
    inline bool hasErrors() const { return !m_Errors.empty(); }
    inline bool hasFatalErrors() const { return m_HasFatalError; }
    inline std::size_t getErrorCount() const { return m_Errors.size(); }

    void print() const;
    void clear();
};


#endif //ERRORREPORTER_HPP
