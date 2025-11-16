#ifndef APPLICATION_HPP
#define APPLICATION_HPP
#include <string>


class Application
{
private:
    struct Config
    {
        enum class Mode
        {
            NONE, MINIFY, RENDER, HELP
        };

        Mode mode{Mode::NONE};
        std::string inputPath;
        std::string outputPath;
        bool verify{false};
        bool showDeadCode{false};
    };

    Config m_Config;

    void runMinifier();
    void runRenderer();
    void showHelp() const;

    bool parseCommandLine(int argc, char* argv[]);

    static std::string readFile(const std::string& path);
    static void writeFile(const std::string& path, const std::string& content);

public:
    Application(int argc, char* argv[]);

    int run();

    ~Application() = default;
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;
};


#endif //APPLICATION_HPP
