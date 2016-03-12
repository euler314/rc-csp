// minion_model_writer.hpp
#ifndef MINION_MODEL_WRITER_HPP
#define MINION_MODEL_WRITER_HPP

#include "model_writer.hpp"

class minion_model_writer : public model_writer
{
public:
	minion_model_writer(const graph& g, index_t k, std::ostream& os)
		: model_writer(g, k, os, "#")
	{

	}

private:
	virtual void impl_preprocess();
	//virtual void impl_process();
	virtual void impl_postprocess();

	virtual void impl_process_vertex_pair(index_t u, index_t v);
};

#endif