#ifndef VHPOP_CALLABLE_H
#define VHPOP_CALLABLE_H

#include "parameters.h"
#include <iostream>
#include <limits.h>
#include <cstdio>
#include <cerrno>
#include <cstring>


typedef struct Vhpop_callable{
public:
    Vhpop_callable();
    int getOptions(int argc, char* argv[], bool no_flaw_order, bool no_search_limit);
    bool read_file(const char* name);
    int parseFile(int argc, char* argv[]);
    const Plan* solve(bool, const std::string);

    ~Vhpop_callable();

private:

} Vhpop_callable;

#endif // VHPOP_CALLABLE_H
