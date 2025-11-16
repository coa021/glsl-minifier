#ifndef MINIFICATIONSTATS_HPP
#define MINIFICATIONSTATS_HPP

#include <chrono>
#include <iostream>

class MinificationStats
{
private:
    std::size_t m_OriginalSize{0};
    std::size_t m_MinifiedSize{0};
    int m_VariablesRenamed{0};
    int m_FunctionsFound{0};
    int m_UniformsFound{0};
    int m_DeadCodeRemoved{0};
    double m_ProcessingTimeMs{0.0};
    std::chrono::high_resolution_clock::time_point m_StartTime;

public:
    void startTiming();
    void stopTiming();
    inline void setOriginalSize(std::size_t size) { m_OriginalSize = size; }
    inline void setMinifiedSize(std::size_t size) { m_MinifiedSize = size; }
    inline void setVariablesRenamed(int count) { m_VariablesRenamed = count; }
    inline void setFunctionsFound(int count) { m_FunctionsFound = count; }
    inline void setUniformsFound(int count) { m_UniformsFound = count; }
    inline void setDeadCodeRemoved(int count) { m_DeadCodeRemoved = count; }

    inline int getUniformsFound() const { return m_UniformsFound; }
    inline int getFunctionsFound() const { return m_FunctionsFound; }

    inline double getCompressionRatio() const
    {
        return m_OriginalSize == 0
                   ? 0.0
                   : (1.0 - (double)m_MinifiedSize / (double)m_OriginalSize) * 100.0;
    }

    inline std::size_t getBytesReduced() const { return m_OriginalSize - m_MinifiedSize; }

    void print();
};


#endif //MINIFICATIONSTATS_HPP
