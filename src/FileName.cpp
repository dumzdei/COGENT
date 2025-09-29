#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <filesystem>
#include <regex>

struct Port
{
    std::string name;
	std::string type;
	std::string description;
    int width = 0;
};

struct Module
{
    std::string name;
    std::vector<Port> ports;
};

struct Comment_block
{
    std::vector<std::string> comment_block;
    std::string tag;
};

struct FileInfo
{
    std::vector<Module> modules;
	std::vector<Comment_block> comments;
};


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

    std::string trim(const std::string& str) 
    {
        size_t first = str.find_first_not_of(" \t");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t");
        return str.substr(first, last - first + 1);
    }

    std::vector<Port> PortParcer(const std::string& source_line)
    {
        std::vector<Port> ports;

        std::string line = source_line;
        line.erase(0, line.find_first_not_of(" \t"));

        std::regex baseRegex(R"((input wire|output wire|output reg)\s*(\[[0-9]+:[0-9]+\])?)");
        std::smatch match;

        if (std::regex_search(line, match, baseRegex))
        {
            Port p;
            p.type = match[1];

            std::string widthStr = match[2];
            if (!widthStr.empty())
            {
                int msb, lsb;
                sscanf_s(widthStr.c_str(), "[%d:%d]", &msb, &lsb);
                p.width = abs(msb - lsb) + 1;
            }
            else
                p.width = 1;

            std::string rest = line.substr(match[0].length());

            std::vector<std::string> commentVariants = { "//*", "/**", "//", "/*" };
            size_t firstPos = std::string::npos;

            for (const auto& cmt : commentVariants)
            {
                size_t pos = rest.find(cmt);
                if (pos != std::string::npos)
                {
                    if (firstPos == std::string::npos || pos < firstPos)
                    {
                        firstPos = pos;
                    }
                }
            }

            if (firstPos != std::string::npos)
            {
                rest = rest.substr(0, firstPos);
            }
            std::stringstream ss(rest);
            std::string name;
            while (std::getline(ss, name, ','))
            {
                name.erase(0, name.find_first_not_of(" \t"));
                name.erase(name.find_last_not_of(" \t;") + 1);

                if (!name.empty())
                {
                    Port np = p;
                    np.name = name;
                    ports.push_back(np);
                }
            }
        }
        return ports;
    }

    FileInfo parse()
    {
		FileInfo fileinfo;
        Module module;
		Comment_block comment_block;
        std::vector<std::string> tags = { "@brief" , "@port"};

        for (size_t i = 0; i < lines.size(); ++i)
        {
            const std::string& line = lines[i];
            if (line.find("input wire") != std::string::npos || line.find("output wire") != std::string::npos || line.find("output reg") != std::string::npos)
            {
				std::vector<Port> parsed_ports = PortParcer(line);
                for (const auto& p : parsed_ports)
                    module.ports.push_back(p);
            }
            else if (line.find("module") != std::string::npos && line.find("endmodule") == std::string::npos)
            {
                size_t pos = line.find("module");
                if (pos != std::string::npos)
                {
					pos += 6;  //length of "module"
                    size_t end = line.find_first_of("(", pos);
                    module.name = trim(line.substr(pos, end - pos));
                }
                else
					module.name = "No name ";
            }
            if (line.find("//*") != std::string::npos)
            {
                size_t pos = line.find("//*");
                std::string cleaned_line = trim(line.substr(pos + 3));

                for (const auto& tag : tags)
                {
                    if (line.find(tag) != std::string::npos)
                    {
                        comment_block.tag = tag;
                        
                        size_t tag_pos = cleaned_line.find(tag);
                        if (tag == "@port")
                        {
                            module.ports.back().description = trim(cleaned_line.substr(tag_pos + tag.length()));
                        }
                        if (tag_pos != std::string::npos)
                        {
                            cleaned_line.erase(tag_pos, tag.length());
                        }
                        break;
                    }
                }
                comment_block.comment_block.push_back(cleaned_line);
                fileinfo.comments.push_back(comment_block);
                comment_block = Comment_block();
            }
            else if (line.find("/**") != std::string::npos)
            {
                while (i < lines.size())
                {
                    std::string comment = trim(lines[i]);
                    if (lines[i].find("/**") != std::string::npos)
                    {
						size_t start_pos = lines[i].find("/**");
                        comment = trim(lines[i].substr(start_pos + 3));
                    }
                    else if (lines[i].find("**/") != std::string::npos)
                    {
                        size_t end_pos = lines[i].find("**/");
                        if (end_pos != std::string::npos && end_pos <= lines[i].size())
                            comment = trim(lines[i].substr(0, end_pos));
                        else
                            comment = "";
                        break;
                    }
                    for (const auto& tag : tags)
                    {
                        if (comment.find(tag) != std::string::npos)
                        {
                            comment_block.tag = tag;
                            
                            size_t tag_pos = comment.find(tag);
                            if (tag_pos != std::string::npos)
                            {
                                comment.erase(tag_pos, tag.length());
                            }
                            break;
                        }
                    }
                    if (comment.empty())
                    {
                        ++i;
                        continue;
                    }
                    ++i;
                    comment_block.comment_block.push_back(comment);
                    
                }
                fileinfo.comments.push_back(comment_block);
                comment_block = Comment_block();
            }
        }
        fileinfo.modules.push_back(module);
		return fileinfo;
    }

    void ExportHTML(FileInfo& fileinfo, std::ofstream& html)
    {
        // Комментарии
        html << "<h2>Comments</h2><ul>\n";
        for (const auto& cmt : fileinfo.comments)
        {
            if (cmt.tag == "@brief" && !cmt.comment_block.empty())
            {
                html << "<li><b>Brief:</b><ul>\n";
                for (const auto& line : cmt.comment_block)
                {
                    html << "<li class='brief'>" << line << "</li>\n";
                }
                html << "</ul></li>\n";
            }
        }
        html << "</ul>\n";

        // Модули и их порты
        html << "<h2>Modules</h2>\n";
        for (const auto& module : fileinfo.modules)
        {
            html << "<h3>Module: " << module.name << "</h3>\n";
            if (!module.ports.empty())
            {
                html << "<table border='1' cellspacing='0' cellpadding='4'>\n";
                html << "<tr><th>Port name</th><th>Type</th><th>Width</th><th>Description</th></tr>\n";
                for (const auto& port : module.ports)
                    html << "<tr><td>" << port.name << "</td><td>" << port.type << "</td><td>" << port.width << "</td><td>" << "<li class='param'>" << port.description << "</td></tr>\n";
                html << "</table>\n";
            }
            else
            {
                html << "<p>No ports</p>\n";
            }
        }
    }
};


int main()
{
    Parser parser;
	std::vector<FileInfo> files_info;


	std::cout << "Enter directory path: ";
    std::string directory_path;
    std::cin >> directory_path;

    std::ofstream html("report.html");
    if (!html.is_open()) return 1;

    html << "<html><head><meta charset='UTF-8'><title>Report</title></head><body>\n";

    html << "<style>"
        "body { font-family: Arial, sans-serif; margin:20px; background:#f9f9f9; color:#222; }"
        "h1 { font-size:22px; color:#2c3e50; border-bottom:2px solid #ccc; padding-bottom:5px; }"
        "h2 { font-size:20px; color:#34495e; margin-top:20px; }"
        "h3 { font-size:18px; color:#16a085; }"
        ".brief { font-size:18px; font-weight:bold; color:#333; margin:10px 0; list-style-type: none; padding - left: 0;}"
        ".author { color:#555; margin:5px 0; }"
        ".param { font-style:italic; margin-left:15px; list-style-type: none; padding - left: 0;}"
        "table { border-collapse:collapse; width:80%; margin-top:10px; background:white; }"
        "table, th, td { border:1px solid #ccc; }"
        "th { background:#f0f0f0; padding:6px; text-align:left; }"
        "td { padding:6px; }"
        "</style>";
    html << "</head><body>\n";
        
    for (const auto& entry : std::filesystem::directory_iterator(directory_path))
    {
        if (!entry.is_regular_file()) continue;

        std::cout << "Processing file: " << entry.path() << "\n";

        if (parser.loadFile(entry.path().string()))
        {
            files_info.push_back(parser.parse());
        }
        else
        {
            std::cerr << "Failed to load file.\n";
        }
    }
    for (auto& fileinfo : files_info)
    {
        parser.ExportHTML(fileinfo, html);
	}
    html << "</body></html>\n";

    return 0;
}