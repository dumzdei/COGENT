#include "CmdLine.hpp"

#include <cstring>
#include <iostream>
#include <iomanip>

#include "Colors.hpp"

CmdLine::CmdLine(int _argc, char *_argv[]) {
	// Проверяем, что аргументы вообще есть
	if (1 == _argc) {
		std::cerr << FORMAT_ERROR "no arguments. Use options '-h' or '--help' for help." << std::endl;
		argsOk		= true;
		shouldStop	= true;
		return;
	}

	for (int i = 1; i < _argc; i++) {
		if (strcmp(_argv[i], "--path") == 0 || (strcmp(_argv[i], "-p") == 0)) {
			if (i < _argc - 1) {
				if (!path.empty())
					std::cerr << FORMAT_WARNING "the path to the source files will be overwritten from '" <<
					path << "' to '" << _argv[i + 1] << "'" << std::endl;
				path = _argv[++i];
				continue;
			}
			else {
				std::cerr << FORMAT_ERROR "the '" << _argv[i] <<
					"' should be followed by a path to the input files." << std::endl;
				argsOk		= false;
				shouldStop	= true;
				return;
			}
		}
		if (strcmp(_argv[i], "--style") == 0 || (strcmp(_argv[i], "-s") == 0)) {
			if (i < _argc - 1) {
				themeName = _argv[++i];
				if (themeName != "dark" && themeName != "lite") {
					std::cerr << FORMAT_WARNING "the style '" << _argv[i] <<
						"' is not supported. The default value of 'dark' will be used." << std::endl;
					themeName = "dark";
				}
				continue;
			}
			else {
				std::cerr << FORMAT_ERROR "the '" << _argv[i] << "' should be followed by thestyle name." << std::endl;
				argsOk		= false;
				shouldStop	= true;
				return;
			}
		}
		if (strcmp(_argv[i], "--portlist") == 0 || (strcmp(_argv[i], "-pl") == 0)) {
			portlist = true;
			continue;
		}
		if (strcmp(_argv[i], "--format") == 0 || (strcmp(_argv[i], "-f") == 0)) {
			if (i < _argc - 1) {
				++i;
				if (!strcmp(_argv[i], "html")) {
					std::cerr << FORMAT_WARNING "HTML is the default output format, no need to specify it." <<
						std::endl;
					continue;
				}
				if (!strcmp(_argv[i], "markdown") || !strcmp(_argv[i], "md")) {
					std::cout << FORMAT_INFO "output format has been set to Markdown" << std::endl;
					format = OutputFormat::markdown;
					continue;
				}
				if (!strcmp(_argv[i], "asciidoc") || !strcmp(_argv[i], "adoc")) {
					std::cout << FORMAT_INFO "output format has been set to Asciidoc" << std::endl;
					format = OutputFormat::asciidoc;
					continue;
				}
				continue;
			}
			else {
				std::cerr << FORMAT_ERROR "the '" << _argv[i] << "' should be followed by the output format name." <<
					std::endl;
				argsOk		= false;
				shouldStop	= true;
				return;
			}
		}
		if (strcmp(_argv[i], "--help") == 0 || (strcmp(_argv[i], "-h") == 0)) {
			printHelpMessage();
			argsOk		= true;
			shouldStop	= true;
			return;
		}

		std::cerr << FORMAT_ERROR "unknown option: " << _argv[i] << ". Use '-h' or '--help'.\n";
		return;
	}
	if (portlist) {
		if (format != OutputFormat::asciidoc) {
			std::cout << FORMAT_WARNING "portlist export is only supported for Asciidoc. Ignoring --portlist.\n";
			portlist = false;
		}
		else {
			std::cout << FORMAT_INFO "Separate port list export enabled\n";
		}
	}
	// Проверяем, что все необходимые аргументы были заданы
	// По факту он такой один - путь к файлам для обработки
	if (!checkArgs()) {
		argsOk = false;
		shouldStop = true;
		return;
	}
}

bool CmdLine::checkArgs() {
	if (path.empty()) {
		std::cerr << FORMAT_ERROR "no input files were given! Use '-h' or '--help' to learn about .\n";
		return false;
	}
	return true;
}

void CmdLine::printHelpMessage() {
	std::cout << "\033[35m";
	std::cout << R"(
=========================================
    __________  _____________   ________
   / ____/ __ \/ ____/ ____/ | / /_  __/
  / /   / / / / / __/ __/ /  |/ / / /   
 / /___/ /_/ / /_/ / /___/ /|  / / /    
 \____/\____/\____/_____/_/ |_/ /_/     
=========================================
		)";
	std::cout << "\033[0m\n";
	std::cout << "Options:\n";
	std::cout << std::left << std::setw(32) << "  -p,   --path <path>"	    << "[required] the path to the input files\n";
	std::cout << std::left << std::setw(32) << "  -s,   --style <style>"	<< "[optional] the style of the theme, possible values are: 'light' and 'dark'\n";
	std::cout << std::left << std::setw(32) << ""						    << "           the 'dark' theme is used by default\n";
	std::cout << std::left << std::setw(32) << "  -f,   --format <format>"  << "[optional] the format of the result document\n";
	std::cout << std::left << std::setw(32) << "  -pl,  --portlist"         << "[optional] the output of a separate portlist\n";
	std::cout << std::left << std::setw(32) << "  -h,   --help"			    << "[optional] show this message\n";
}
