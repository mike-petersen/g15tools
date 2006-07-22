#ifndef COMPOSER_H_
#define COMPOSER_H_

#include <string>
#include <vector>

void handlePixelCommand(g15canvas *, std::string const &input_line);
void handleModeCommand(g15canvas *, std::string const &input_line);
void handleTextCommand(g15canvas *, std::string const &input_line);
void updateScreen(g15canvas *, bool);
int get_params(int*, std::string const &, int, int);


#endif /*COMPOSER_H_*/
