#include "ShaderVerifier.hpp"

sf::Shader ShaderVerifier::compileShader(const std::string& fragmentSource)
{
    std::string vertexSource = R"(
            #version 330 core
            layout(location = 0) in vec2 position;
            layout(location = 1) in vec2 texCoord;
            out vec2 fragTexCoord;
            void main() {
                gl_Position = vec4(position, 0.0, 1.0);
                fragTexCoord = texCoord;
            }
        )";
    sf::Shader shader;
    if (!shader.loadFromMemory(vertexSource, sf::Shader::Type::Vertex))
        throw std::runtime_error("Failed to compile vertex shader");
    if (!shader.loadFromMemory(fragmentSource, sf::Shader::Type::Fragment))
        throw std::runtime_error("Failed to compile fragment shader");

    return shader;
}

sf::Image ShaderVerifier::renderShader(sf::Shader& shader, float testTime)
{
    sf::RenderTexture renderTarget{{TEST_WIDTH, TEST_HEIGHT}};

    sf::VertexArray vertices(sf::PrimitiveType::TriangleStrip, 4);
    vertices[0].position = sf::Vector2f(-1.f, -1.f);
    vertices[1].position = sf::Vector2f(1.f, -1.f);
    vertices[2].position = sf::Vector2f(-1.f, 1.f);
    vertices[3].position = sf::Vector2f(1.f, 1.f);

    vertices[0].texCoords = sf::Vector2f(0.f, 0.f);
    vertices[1].texCoords = sf::Vector2f(1.f, 0.f);
    vertices[2].texCoords = sf::Vector2f(0.f, 1.f);
    vertices[3].texCoords = sf::Vector2f(1.f, 1.f);

    shader.setUniform("time", testTime);
    shader.setUniform("resolution", sf::Vector2f(TEST_WIDTH, TEST_HEIGHT));
    shader.setUniform("iTime", testTime);
    shader.setUniform("iResolution", sf::Vector3f(TEST_WIDTH, TEST_HEIGHT, 0.0f));

    renderTarget.clear(sf::Color::Black);
    renderTarget.draw(vertices, &shader);
    renderTarget.display();

    return renderTarget.getTexture().copyToImage();
}

double ShaderVerifier::compareImages(const sf::Image& img1, const sf::Image& img2)
{
    if (img1.getSize() != img2.getSize())
        return 100.0;

    unsigned long long totalDiff{0};
    unsigned long long pixelCount{img1.getSize().x * img1.getSize().y};

    for (unsigned int y{0}; y < img1.getSize().y; ++y)
    {
        for (unsigned int x{0}; x < img1.getSize().x; ++x)
        {
            sf::Color c1{img1.getPixel({x, y})};
            sf::Color c2{img2.getPixel({x, y})};

            totalDiff += std::abs(c1.r - c2.r);
            totalDiff += std::abs(c1.g - c2.g);
            totalDiff += std::abs(c1.b - c2.b);
            totalDiff += std::abs(c1.a - c2.a);
        }
    }

    double maxDiff{pixelCount * 4 * 255.0}; // 4 channels, 255 per channel
    return (totalDiff / maxDiff) * 100.0;
}

ShaderVerifier::VerificationResult ShaderVerifier::verify(const std::string& originalSource,
                                                          const std::string& minifiedSource)
{
    VerificationResult result;
    result.originalCompiled = false;
    result.minifiedCompiled = false;
    result.imagesMatch = false;
    result.pixelDifference = 100.0;

    try
    {
        sf::Shader originalShader{compileShader(originalSource)};
        result.originalCompiled = true;

        sf::Shader minifiedShader{compileShader(minifiedSource)};
        result.minifiedCompiled = true;

        float testTime{1.0f};
        sf::Image originalImage{renderShader(originalShader, testTime)};
        sf::Image minifiedImage{renderShader(minifiedShader, testTime)};

        result.pixelDifference = compareImages(originalImage, minifiedImage);

        result.imagesMatch = (result.pixelDifference < 0.1);
    }
    catch (const std::exception& e)
    {
        result.errorMessage = e.what();
    }

    return result;
}

void ShaderVerifier::printResult(const VerificationResult& result)
{
    std::cout << "\nOriginal shader: " << (result.originalCompiled ? "PASS" : "FAIL") << '\n';
    std::cout << "\nMinified shader: " << (result.minifiedCompiled ? "PASS" : "FAIL") << '\n';

    if (result.originalCompiled && result.minifiedCompiled)
    {
        std::cout << "Pixel difference: " << result.pixelDifference << "%\n";
        std::cout << "Images match: " << (result.imagesMatch ? "YES" : "NO") << '\n';
    }
    if (!result.errorMessage.empty())
        std::cout << "Error: " << result.errorMessage << '\n';
    if (result.passed())
        std::cout << "Minification passes tests\n";
    else
        std::cout << "Minification doesnt pass tests, changes behaviour\n";
}
