#ifndef COMPOSER_H_
#define COMPOSER_H_

#include <string>
#include <vector>

using namespace std;

void handlePixelCommand(std::string const &input_line);
void handleModeCommand(std::string const &input_line);
void handleTextCommand(std::string const &input_line);
void updateScreen(bool);
int get_params(int*, std::string const &, int, int);
void fifoProcessingWorkflow(bool is_script, string const &filename);
void parseCommandLine(string cmdline);


#endif /*COMPOSER_H_*/
