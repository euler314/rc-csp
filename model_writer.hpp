// model_writer.hpp
#ifndef MODEL_WRITER_HPP
#define MODEL_WRITER_HPP

#include "common.hpp"
#include "graph.hpp"
#include "path.hpp"
#include <ostream>
#include <string>

class model_writer
{
public:
	model_writer(const graph& g, index_t k, std::ostream& os, const std::string& comment = "%")
		: g_(g), k_(k), os_(os), comment_(comment)
	{

	}

	virtual ~model_writer() { }

	void write();

	void add_line(const std::string& line);

protected:
	const graph& get_graph() const { return g_; }
	index_t get_solution_size() const { return k_; }
	std::ostream& get_output_stream() const { return os_; }
	const std::string& get_comment() const { return comment_; }

private:
	virtual void impl_preprocess();
	virtual void impl_process();
	virtual void impl_postprocess();

	virtual void impl_process_vertex_pair(index_t u, index_t v);

	const graph& g_;
	index_t k_;
	std::ostream& os_;
	const std::string comment_;
};

void prepare_model(index_t k, std::ostream& os);

void add_alldiff(const std::vector<index_t>& edges, std::ostream& os);
void add_alldiff(const edge_path& p, std::ostream& os);
void add_alldiff(const vertex_path& p, std::ostream& os);

void enforce_path(model_writer& w, const edge_path& p);

#endif
