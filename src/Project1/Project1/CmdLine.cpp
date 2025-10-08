#include "CmdLine.hpp"

#include <iostream>
#include <iomanip>

CmdLine::CmdLine(int _argc, char *_argv[]) {
	// Проверяем, что аргументы вообще есть
	if (1 == _argc) {
		std::cerr << "__err__ : no arguments. Use options '-h' or '--help' for help." << std::endl;
		argsOk		= true;
		shouldStop	= true;
		return;
	}

	for (int i = 1; i < _argc; i++) {
		if (strcmp(_argv[i], "--path") == 0 || (strcmp(_argv[i], "-p") == 0)) {
			if (i < _argc - 1) {
				if (!path.empty())
					std::cerr << "__wrn__ : the path to the source files will be overwritten from '" << path << "' to '" << _argv[i + 1] << "'" << std::endl;
				path = _argv[++i];
				continue;
			}
			else {
				std::cerr << "__err__ : the '" << _argv[i] << "' should be followed by a path to the input files." << std::endl;
				argsOk		= false;
				shouldStop	= true;
				return;
			}
		}
		if (strcmp(_argv[i], "--style") == 0 || (strcmp(_argv[i], "-s") == 0)) {
			if (i < _argc - 1) {
				themeName = _argv[++i];
				if (themeName != "dark" && themeName != "lite") {
					std::cerr << "__wrn__ : the style '" << _argv[i] << "' is not supported. The default value of 'dark' will be used." << std::endl;
					themeName = "dark";
				}
				continue;
			}
			else {
				std::cerr << "__err__ : the '" << _argv[i] << "' should be followed by thestyle name." << std::endl;
				argsOk		= false;
				shouldStop	= true;
				return;
			}
		}
		if (strcmp(_argv[i], "--format") == 0 || (strcmp(_argv[i], "-f") == 0)) {
			if (i < _argc - 1) {
				std::cerr << "__wrn__ : the output format set option '" << _argv[i] << "' is not supported." << std::endl;
				++i;
				continue;
			}
			else {
				std::cerr << "__err__ : the '" << _argv[i] << "' should be followed by the output format name." << std::endl;
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

		std::cerr << "\033[31m__err__ : unknown option: " << _argv[i] << ". Use '-h' or '--help'.\033[0m\n";
		return;
	}
	// Проверяем, что все необходимые аргументы были заданы
	// По факту отн такой один - путь к файлам для обработки
	if (!checkArgs()) {
		argsOk = false;
		shouldStop = true;
		return;
	}
}

bool CmdLine::checkArgs() {
	if (path.empty()) {
		std::cerr << "\033[31m";
		std::cerr << "__err__ : no input files were given! Use '-h' or '--help' to learn about .\n";
		std::cerr << "\033[0m";
		return false;
	}
	return true;
}

void CmdLine::printHelpMessage() {
	std::cout << "\033[35m";
	//https://patorjk.com/software/taag/#p=display&f=Big&t=DOCUMENTATOR-9000&x=none&v=4&h=4&w=80&we=false
	//! Я сделал надпись покороче, чтобы она уместилась в стандартную консоль
	std::cout << R"(
================================================================================================================
 |  __ \ / __ \ / ____| |  | |  \/  |  ____| \ | |__   __|/\|__   __/ __ \|  __ \      / _ \ / _ \ / _ \ / _ \ 
 | |  | | |  | | |    | |  | | \  / | |__  |  \| |  | |  /  \  | | | |  | | |__) |____| (_) | | | | | | | | | |
 | |  | | |  | | |    | |  | | |\/| |  __| | . ` |  | | / /\ \ | | | |  | |  _  /______\__, | | | | | | | | | |
 | |__| | |__| | |____| |__| | |  | | |____| |\  |  | |/ ____ \| | | |__| | | \ \        / /| |_| | |_| | |_| |
 |_____/ \____/ \_____|\____/|_|  |_|______|_| \_|  |_/_/    \_\_|  \____/|_|  \_\      /_/  \___/ \___/ \___/ 
================================================================================================================
		)";
	std::cout << "\033[0m\n";
	std::cout << "Options:\n";
	std::cout << std::left << std::setw(32) << "  -p, --path <path>"	<< "[required] the path to the input files\n";
	std::cout << std::left << std::setw(32) << "  -s, --style <style>"	<< "[optional] the style of the theme, possible values are: 'light' and 'dark'\n";
	std::cout << std::left << std::setw(32) << ""						<< "           the 'dark' theme is used by default\n";
	std::cout << std::left << std::setw(32) << "  -f, --format <format>"<< "[optional] the format of the result document\n";
	std::cout << std::left << std::setw(32) << ""						<< "           by now only the 'html' format is supported\n";
	std::cout << std::left << std::setw(32) << "  -h, --help"			<< "[optional] show this message\n";
}