#include "ErrorReporter.hpp"

void CompilerError::print() const
{
    std::string severityStr;
    switch (severity)
    {
    case ErrorSeverity::WARNING:
        severityStr = "Warning";
        break;
    case ErrorSeverity::FATAL:
        severityStr = "Fatal Error";
        break;
    case ErrorSeverity::ERROR:
        severityStr = "Error";
        break;
    }

    std::cerr << severityStr << " at line " << line << ", column " << column << ": " << message << '\n';
    if (!context.empty())
    {
        std::cerr << "\t" << context << '\n';
        std::cerr << "\t" << std::string(column - 1, ' ') << "^\n";
    }
}

void ErrorReporter::reportError(ErrorSeverity severity, const std::string& message, int line, int column,
                                const std::string& context)
{
    CompilerError error{severity, message, line, column, context};
    m_Errors.push_back(error);

    if (severity == ErrorSeverity::FATAL)
        m_HasFatalError = true;

    error.print();
}

void ErrorReporter::print() const
{
    if (m_Errors.empty())
    {
        std::cout << "No errors found\n";
        return;
    }

    int warnings{0}, errors{0}, fatal{0};
    for (const auto& err : m_Errors)
    {
        switch (err.severity)
        {
        case ErrorSeverity::WARNING:
            ++warnings;
            break;
        case ErrorSeverity::ERROR:
            ++errors;
            break;
        case ErrorSeverity::FATAL:
            ++fatal;
            break;
        }
    }

    std::cout << "\n";
    if (fatal > 0)
        std::cout << fatal << " fatal errors\n";
    if (errors > 0)
        std::cout << errors << " errors\n";
    if (warnings > 0)
        std::cout << warnings << " warnings\n";
}

void ErrorReporter::clear()
{
    m_Errors.clear();
    m_HasFatalError = false;
}
