// model.hpp
#ifndef MODEL_HPP
#define MODEL_HPP

#include "common.hpp"
#include <iostream>
class graph;

// TODO: Rework a nice interface?

void build_rc_model(const graph& g, index_t k, std::ostream& os = std::cout);

#endif