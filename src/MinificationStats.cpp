#include "MinificationStats.hpp"

void MinificationStats::startTiming()
{
    m_StartTime = std::chrono::high_resolution_clock::now();
}

void MinificationStats::stopTiming()
{
    auto endTime{std::chrono::high_resolution_clock::now()};
    m_ProcessingTimeMs = std::chrono::duration<double, std::milli>{endTime - m_StartTime}.count();
}

void MinificationStats::print()
{
    std::cout << "\nOriginal size:\t\t" << m_OriginalSize << " bytes\n";
    std::cout << "\nMinified size:\t\t" << m_MinifiedSize << " bytes\n";
    std::cout << "\nBytes reduced:\t\t" << getBytesReduced() << " bytes\n";
    std::cout << "\nCompression ratio:\t\t" << getCompressionRatio() << " %\n";

    std::cout << "\nVariables renamed:\t\t" << m_VariablesRenamed << " bytes\n";
    std::cout << "\nFunctions found:\t\t" << m_FunctionsFound << " bytes\n";
    std::cout << "\nUniforms found:\t\t" << m_UniformsFound << " bytes\n";
    std::cout << "\nDead code remove:\t\t" << m_DeadCodeRemoved << " bytes\n";

    std::cout << "\nProcessing time:\t\t" << m_ProcessingTimeMs << " ms\n";

    if (m_ProcessingTimeMs > 0)
    {
        double throughput{(m_OriginalSize / 1024.0) / (m_ProcessingTimeMs / 1000.0)};
        std::cout << "Throughput:\t\t" << throughput << " KB/s\n";
    }
    std::cout << "\n\n";
}
