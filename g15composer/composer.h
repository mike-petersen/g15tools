#ifndef COMPOSER_H_
#define COMPOSER_H_

#include <string>
#include <vector>

using namespace std;


int get_params(int*, std::string const &, int, int);
//void fifoProcessingWorkflow(bool is_script, string const &filename);
int doOpen(string const &filename);

#endif /*COMPOSER_H_*/
