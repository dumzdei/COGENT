#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <filesystem>

struct Port
{
    std::string portName;
    bool direction;  //TRUE == IN   FALSE == OUT
    int width;
};

struct Module
{
    std::string name;
    std::vector<Port> ports;
};

std::string FindNextWord(const std::string& text, const std::string& targetWord)
{
    std::stringstream ss(text);
    std::string current, next;

    ss >> current;

    while (ss >> next)
    {
        if (current == targetWord)
        {
            return next;
        }
        current = next;
    }

    return "";
}

class Parser 
{
private:
    std::vector<std::string> lines;
    std::string line;
public:
    bool loadFile(const std::string& filename)
    {
        lines.clear();
        std::ifstream file(filename);
        if (!file.is_open()) return false;

        std::string line;
        while (std::getline(file, line))
        {
            lines.push_back(line);
        }
        return true;
    }


    void parse()
    {
		Module module;
		Port port;

        for (size_t i = 0; i < lines.size(); ++i)
        {
            const std::string& line = lines[i];

            if (line.find("module") != std::string::npos && line.find("endmodule") == std::string::npos)
            {
				module.name = FindNextWord(line, "module");
                std::cout << "module name: " << module.name << "\n";
            }
            else if (line.find("input") != std::string::npos || line.find("output") != std::string::npos)
            {
                port.direction = line.find("input") != std::string::npos;

                std::vector<std::string> stopWords = { "reg", "wire" };
                for (auto& sw : stopWords)
                {
                    if (line.find(sw) != std::string::npos)
                    {
                        port.portName = FindNextWord(line, sw);
                        break;
                    }
                }

                port.width = 1;
                size_t bracketPos = line.find('[');
                if (bracketPos != std::string::npos)
                {
                    size_t colonPos = line.find(':', bracketPos);
                    size_t endBracketPos = line.find(']', colonPos);
                    if (colonPos != std::string::npos && endBracketPos != std::string::npos)
                    {
                        int msb = std::stoi(line.substr(bracketPos + 1, colonPos - bracketPos - 1));
                        int lsb = std::stoi(line.substr(colonPos + 1, endBracketPos - colonPos - 1));
                        port.width = msb - lsb + 1;

                        std::string after = line.substr(endBracketPos + 1);
                        std::istringstream iss(after);
                        std::string name;
                        if (iss >> name) {

                            while (!name.empty() && (name.back() == ';' || name.back() == ',')) {
                                name.pop_back();
                            }
                            port.portName = name;
                        }
                    }
                }
                
                std::cout << "Port found: " << port.portName << ", Direction: " << (port.direction ? "IN" : "OUT") << ", Width: " << port.width << "\n";
            }
            if (line.find("//*") != std::string::npos)
            {
                size_t pos = line.find("//*");
                std::cout << "Comment found: " << line.substr(pos) << "\n";
            }
            else if (line.find("/**") != std::string::npos)
            {
                std::cout << "Multi-line comment start found: " << line << "\n";

                while (i < lines.size() && lines[i].find("*/") == std::string::npos)
                {
                    std::cout << lines[i] << "\n";
                    ++i;
                }
                if (i < lines.size() && lines[i].find("*/") != std::string::npos)
                {
                    std::cout << "Multi-line comment end found: " << lines[i] << "\n";
                }
            }
        }
    }
};


int main()
{
    Parser parser;

	std::cout << "Enter directory path: ";
    std::string directory_path;
    std::cin >> directory_path;

    std::ofstream html("../reports/report.html");
    if (!html.is_open()) return 1;

    std::streambuf* oldCout = std::cout.rdbuf(html.rdbuf());

    std::cout << "<html><head><meta charset='UTF-8'><title>Report</title></head><body><pre>\n";

    for (const auto& entry : std::filesystem::directory_iterator(directory_path))
    {
        if (!entry.is_regular_file()) continue;

        std::cout << "Processing file: " << entry.path() << "\n";

        if (parser.loadFile(entry.path().string()))
        {
            parser.parse();
        }
        else
        {
            std::cerr << "Failed to load file.\n";
        }
    }

    std::cout << "</pre></body></html>\n";

    std::cout.rdbuf(oldCout);

    return 0;
}