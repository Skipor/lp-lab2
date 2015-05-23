//
// Created by  Vladimir Skipor on 5/23/15.
//
#include "node.h"
#include "parser.hpp"


const std::string NDefinition::intend("  ");
const std::unordered_map<int, std::string> NBinaryOperator::token_to_operator({
        {TCEQ  ,"==" },
        {TCNE  ,"!=" },
        {TCLT  ,"<" },
        {TCLE  ,"<=" },
        {TCGT  ,">" },
        {TCGE  ,">=" },
        {TDIV  ,"/" },
        {TMUL  ,"*" },
        {TMINUS,"-" },
        {TPLUS ,"+" }
    });

