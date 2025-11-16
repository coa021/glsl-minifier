#include "Application.hpp"
#include "Scanner.hpp"
#include "Minifier.hpp"
#include "ShaderVerifier.hpp"
#include "ErrorReporter.hpp"

#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

void Application::runMinifier()
{
    std::string source{readFile(m_Config.inputPath)};
    ErrorReporter errorReporter;

    Scanner scanner{source, &errorReporter};
    std::vector<Token> tokens{scanner.scanTokens()};
    std::cout << "Tokens generated: " << tokens.size() << '\n';

    if (errorReporter.hasFatalErrors())
    {
        std::cerr << "\nFatal errors during scanning\n";
        errorReporter.print();
        return;
    }

    std::cout << "\nminifying..\n";
    Minifier minifier{tokens};
    minifier.setOriginalSize(source.length());
    std::string minified{minifier.minify()};

    minifier.printStats();

    if (m_Config.showDeadCode)
        minifier.printDeadCode();

    minifier.printRenamings();

    if (!m_Config.outputPath.empty())
    {
        writeFile(m_Config.outputPath, minified);
        std::cout << "Minified version written to: " << m_Config.outputPath << '\n';
    }

    if (m_Config.verify)
    {
        std::cout << "\nVerifying minified shader correctness\n";
        ShaderVerifier verifier;
        auto result{verifier.verify(source, minified)};
        verifier.printResult(result);

        if (!result.passed())
            std::cerr << "Minification has changed the default behaviour!\n";
    }
    errorReporter.print();
}

void Application::runRenderer()
{
    const unsigned int WIDTH{1024};
    const unsigned int HEIGHT{1024};
    sf::RenderWindow window{sf::VideoMode({WIDTH, HEIGHT}), "GLSL Shader Viewer"};
    window.setFramerateLimit(60);

    std::string fragmentSource{readFile(m_Config.inputPath)};

    std::string vertexSource{
        R"(
        #version 330 core

        layout(location = 0) in vec2 position;
        layout(location = 1) in vec2 texCoord;

        out vec2 fragTexCoord;

        void main() {
            gl_Position = vec4(position, 0.0, 1.0);
            fragTexCoord = texCoord;
        }
    )"
    };

    sf::Shader shader;
    if (!shader.loadFromMemory(vertexSource, fragmentSource))
    {
        std::cerr << "Error: Failed to compile shader" << std::endl;
        return;
    }

    std::cout << "Shader compiled successfully!" << std::endl;
    std::cout << "Controls: ESC to exit, R to reload shader" << std::endl;

    sf::VertexArray vertices{sf::PrimitiveType::TriangleStrip, 4};
    vertices[0].position = sf::Vector2f(-1.f, -1.f);
    vertices[1].position = sf::Vector2f(1.f, -1.f);
    vertices[2].position = sf::Vector2f(-1.f, 1.f);
    vertices[3].position = sf::Vector2f(1.f, 1.f);

    vertices[0].texCoords = sf::Vector2f(0.f, 0.f);
    vertices[1].texCoords = sf::Vector2f(1.f, 0.f);
    vertices[2].texCoords = sf::Vector2f(0.f, 1.f);
    vertices[3].texCoords = sf::Vector2f(1.f, 1.f);

    sf::Clock clock;
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Escape))
                window.close();

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::R))
            {
                std::cout << "Reloading shader..." << std::endl;
                fragmentSource = readFile(m_Config.inputPath);
                if (shader.loadFromMemory(vertexSource, fragmentSource))
                {
                    std::cout << "Shader reloaded successfully!" << std::endl;
                    clock.restart();
                }
                else
                    std::cerr << "Failed to reload shader!" << std::endl;
            }
        }

        float time{clock.getElapsedTime().asSeconds()};

        shader.setUniform("time", time);
        shader.setUniform("resolution", sf::Vector2f(WIDTH, HEIGHT));
        shader.setUniform("iTime", time);
        shader.setUniform("iResolution", sf::Vector3f(WIDTH, HEIGHT, 0.0f));

        window.clear(sf::Color::Black);
        window.draw(vertices, &shader);
        window.display();
    }
}

void Application::showHelp() const
{
    std::cout << "Usage:\n";
    std::cout << "  Minify:   glsl_minifier minify <input.glsl> [output.glsl] [options]\n";
    std::cout << "  Render:   glsl_minifier render <shader.glsl>\n";
    std::cout << "  Help:     glsl_minifier --help\n\n";
    std::cout << "Options:\n";
    std::cout << "  --verify        Run correctness verification (compile and compare)\n";
    std::cout << "  --dead-code     Show dead code analysis\n\n";
    std::cout << "Examples:\n";
    std::cout << "  glsl_minifier minify shader.glsl out.glsl\n";
    std::cout << "  glsl_minifier minify shader.glsl out.glsl --verify --dead-code\n";
    std::cout << "  glsl_minifier render shader.glsl\n";
}

bool Application::parseCommandLine(int argc, char* argv[])
{
    if (argc < 2)
        return false;

    std::string modeStr{argv[1]};
    if (modeStr == "--help" || modeStr == "-h")
    {
        m_Config.mode = Config::Mode::HELP;
        return true;
    }

    if (modeStr == "minify")
    {
        m_Config.mode = Config::Mode::MINIFY;

        if (argc < 3)
        {
            std::cerr << "Error: minify requires an input file\n";
            return false;
        }

        m_Config.inputPath = argv[2];

        if (argc >= 4 && argv[3][0] != '-')
            m_Config.outputPath = argv[3];

        for (int i{3}; i < argc; ++i)
        {
            std::string arg{argv[i]};
            if (arg == "--verify")
                m_Config.verify = true;
            else if (arg == "--dead-code")
                m_Config.showDeadCode = true;
        }

        return true;
    }
    else if (modeStr == "render")
    {
        m_Config.mode = Config::Mode::RENDER;

        if (argc < 3)
        {
            std::cerr << "Error: Shader file required\n";
            return false;
        }

        m_Config.inputPath = argv[2];
        return true;
    }
    std::cerr << "Error: unknown mode " << modeStr << '\n';
    return false;
}

std::string Application::readFile(const std::string& path)
{
    std::ifstream file{path};
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file " << path << std::endl;
        exit(1);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void Application::writeFile(const std::string& path, const std::string& content)
{
    std::ofstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not write to file " << path << std::endl;
        exit(1);
    }
    file << content;
}

Application::Application(int argc, char* argv[])
{
    if (!parseCommandLine(argc, argv))
        m_Config.mode = Config::Mode::HELP;
}

int Application::run()
{
    switch (m_Config.mode)
    {
    case Config::Mode::MINIFY:
        runMinifier();
        return 0;
    case Config::Mode::RENDER:
        runRenderer();
        return 0;
    case Config::Mode::HELP:
        showHelp();
        return 0;
    case Config::Mode::NONE:
        std::cerr << "Error: Invalid arguments, use --help\n";
        return 1;
    }

    return 1; // somehow got here
}
