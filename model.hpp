// model.hpp
#ifndef MODEL_HPP
#define MODEL_HPP

#include "common.hpp"
#include <iostream>
class graph;

// TODO: Rework a nice interface?

//void build_rc_model(const graph& g, index_t k, std::ostream& os = std::cout);

//void build_src_model(const graph& g, index_t k, std::ostream& os = std::cout);

void build_src_model_forest(const graph& g, index_t k, index_t n, std::ostream& os = std::cout);

void build_minion_rc_model(const graph& g, index_t k, std::ostream& os = std::cout);

#endif