// strong_model_writer.hpp
#ifndef STRONG_MODEL_WRITER_HPP
#define STRONG_MODEL_WRITER_HPP

#include "model_writer.hpp"

class strong_model_writer : public model_writer
{
public:
	strong_model_writer(const graph& g, index_t k, std::ostream& os, const std::string& comment = "%")
		: model_writer(g, k, os, comment)
	{

	}

private:
	virtual void impl_process_vertex_pair(index_t u, index_t v);
};

#endif