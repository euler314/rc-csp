// vertex_model_writer.cpp
#include "vertex_model_writer.hpp"
#include "model_writer.hpp"

namespace
{
	const std::string VAR_PREFIX = "x";
	const std::string VAR_DECL = "var 1..k: " + VAR_PREFIX;

	void add_vertex_variables(const graph& g, std::ostream& os)
	{
		const index_t n = g.num_vertices();

		for (index_t i = 0; i < n; ++i)
		{
			os << VAR_DECL << i << ";\n";
		}
	}
}

void vertex_model_writer::impl_preprocess()
{
	prepare_model(get_solution_size(), get_output_stream());
	add_vertex_variables(get_graph(), get_output_stream());
}

void vertex_model_writer::impl_process_vertex_pair(index_t u, index_t v)
{
	get_output_stream() << get_comment() << " Vertex pair " << u << " " << v << "\n";
	std::vector<vertex_path> paths;
	list_paths(get_graph(), u, v, paths, get_solution_size());

	get_output_stream() << "constraint ( ";

	for (index_t j = 0; j < paths.size(); ++j)
	{
		add_alldiff(paths[j], get_output_stream());

		if (j != paths.size() - 1)
			get_output_stream() << "\\/ ";
	}

	get_output_stream() << ");\n";
}

void vertex_model_writer::impl_process()
{
	index_t u = 0;
	index_t v = 1;
	const index_t n = get_graph().num_vertices();
	const index_t pairs = nchoosek(n, 2);

	get_output_stream() << get_comment() << " Paths between vertex pairs\n";

	for (index_t i = 0; i < pairs; ++i)
	{
		std::vector<index_t> dist(get_graph().num_vertices());
		bfs(get_graph(), dist, u);

		// Skip vertices that are at a distance 1 or 2.
		if (dist[v] >= 3)
		{
			impl_process_vertex_pair(u, v);
		}

		next_pair(u, v, n);
	}
}

void vertex_model_writer::impl_postprocess()
{
	get_output_stream() << "solve satisfy;\n";

	// TODO: cut vertices must receive a distinct color
}
