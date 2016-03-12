// strong_model_writer.cpp
#include "strong_model_writer.hpp"

#include <iostream>
#include "path.hpp"

void strong_model_writer::impl_process_vertex_pair(index_t u, index_t v)
{
	auto& os = get_output_stream();
	os << get_comment() << " Vertex pair " << u << " " << v << "\n";
	std::vector<edge_path> paths;
	list_shortest_paths<edge_path>(get_graph(), u, v, paths);

	os << "constraint ( ";

	for (index_t j = 0; j < paths.size(); ++j)
	{
		add_alldiff(paths[j], os);

		if (j != paths.size() - 1)
			os << "\\/ ";
	}

	os << ");\n";
}