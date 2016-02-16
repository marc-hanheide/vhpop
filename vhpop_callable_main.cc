#include <iostream>
#include "vhpop_callable.h"
#include "plans.h"

int main(int argc, char* argv[])
{

    Vhpop_callable v_callable = Vhpop_callable();


    bool no_flaw_order = true;
    bool no_search_limit = true;
    /*
     * Get command line options.
     */

    if (v_callable.getOptions(argc,argv, no_flaw_order, no_search_limit) == -1)
         return -1;

    if (v_callable.parseFile(argc,argv) == -1)
         return -1;


    const Plan * plan_p= new Plan(*v_callable.solve(false,""));
    std::cout << *plan_p;
}
