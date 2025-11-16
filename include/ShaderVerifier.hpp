#ifndef SHADERVERIFIER_HPP
#define SHADERVERIFIER_HPP

#include <string>
#include <iostream>
#include <cmath>

#include <SFML/Graphics.hpp>


class ShaderVerifier
{
private:
    const unsigned int TEST_WIDTH{1024};
    const unsigned int TEST_HEIGHT{1024};

    sf::Shader compileShader(const std::string& fragmentSource);
    sf::Image renderShader(sf::Shader& shader, float testTime);

    double compareImages(const sf::Image& img1, const sf::Image& img2);

public:
    struct VerificationResult
    {
        bool originalCompiled;
        bool minifiedCompiled;
        bool imagesMatch;
        double pixelDifference;
        std::string errorMessage;

        bool passed() const { return originalCompiled && minifiedCompiled && imagesMatch; }
    };

    VerificationResult verify(const std::string& originalSource, const std::string& minifiedSource);
    void printResult(const VerificationResult& result);

};


#endif //SHADERVERIFIER_HPP
