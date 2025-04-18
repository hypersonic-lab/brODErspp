#ifndef SETUPLARSEN1T_H
#define SETUPLARSEN1T_H

#include "SetupProblem.h"

class SetupLarsen1T: public SetupProblem {
public:
    SetupLarsen1T(); 
    ~SetupLarsen1T(){}

    Data* getData(Mutation::Mixture& l_mix, 
                  const std::vector< std::string > l_input_file);

    Problem* getProblem(Mutation::Mixture& l_mix, Data& l_data);
};

#endif /* SETUPLARSEN1T_H */
