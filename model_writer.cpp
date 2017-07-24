// model_writer.cpp
#include "model_writer.hpp"

#include "graph.hpp"
#include "path.hpp"

#include <string>
#include <vector>
#include <ostream>

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

	void add_edge_variables(const graph& g, std::ostream& os)
	{
		const index_t n = g.num_vertices();

		for (index_t i = 0; i < n; ++i)
		{
			index_t adj = g.adj_[i];
			for (index_t j = i; j < n; ++j)
			{
				if (adj & (1 << j))
					os << VAR_DECL << i << "_" << j << ";\n";
			}
		}
	}

	void add_path_constraints(const graph& g, index_t k, std::ostream& os)
	{
		index_t u = 0;
		index_t v = 1;
		const index_t n = g.num_vertices();
		const index_t pairs = nchoosek(n, 2);

		os << "% Paths between vertex pairs\n";

		for (index_t i = 0; i < pairs; ++i)
		{
			if (!is_adjacent(g, u, v))
			{
				os << "% Vertex pair " << u << " " << v << "\n";
				std::vector<edge_path> paths;
				list_paths(g, u, v, paths, k);

				os << "constraint ( ";

				for (index_t j = 0; j < paths.size(); ++j)
				{
					add_alldiff(paths[j], os);

					if (j != paths.size() - 1)
						os << "\\/ ";
				}

				os << ");\n";
			}

			next_pair(u, v, n);
		}
	}
}

void prepare_model(index_t k, std::ostream& os)
{
	os << "include \"alldifferent.mzn\";\n";
	os << "int: k=" << k << ";\n";
}

void model_writer::write()
{
	impl_preprocess();
	impl_process();
	impl_postprocess();
}

void model_writer::add_line(const std::string& line)
{
	os_ << line << "\n";
}

void model_writer::impl_process()
{
	index_t u = 0;
	index_t v = 1;
	const index_t n = g_.num_vertices();
	const index_t pairs = nchoosek(n, 2);

	os_ << comment_ << " Paths between vertex pairs\n";

	for (index_t i = 0; i < pairs; ++i)
	{
		if (!is_adjacent(g_, u, v))
		{
			impl_process_vertex_pair(u, v);
		}

		next_pair(u, v, n);
	}
}

void model_writer::impl_preprocess()
{
	prepare_model(k_, os_);
	add_edge_variables(g_, os_);
}

void model_writer::impl_postprocess()
{
	// Bridges must get distinct colors.
	auto bridges = get_bridges(g_);

	// At least 2 bridges?
	if (bridges.size() >= 4)
	{
		os_ << "% Bridges\n";
		os_ << "constraint ( ";
		add_alldiff(bridges, os_);
		os_ << ");\n";
	}

	os_ << "solve satisfy;";
}

void model_writer::impl_process_vertex_pair(index_t u, index_t v)
{
	os_ << comment_ << " Vertex pair " << u << " " << v << "\n";
	std::vector<edge_path> paths;
	list_paths(g_, u, v, paths, k_);

	os_ << "constraint ( ";

	for (index_t j = 0; j < paths.size(); ++j)
	{
		add_alldiff(paths[j], os_);

		if (j != paths.size() - 1)
			os_ << "\\/ ";
	}

	os_ << ");\n";
}

void add_alldiff(const std::vector<index_t>& edges, std::ostream& os)
{
	os << "alldifferent([";

	for (auto it = edges.cbegin(); it != edges.cend(); it += 2)
	{
		os << VAR_PREFIX << *(it) << "_" << *(it + 1);

		if (it != (edges.cend() - 2))
			os << ",";
	}

	os << "]) ";
}

void add_alldiff(const edge_path& p, std::ostream& os)
{
	add_alldiff(to_edge_list(p), os);
}

void add_alldiff(const vertex_path& p, std::ostream& os)
{
	std::vector<index_t> ints = to_internal_list(p);

	os << "alldifferent([";

	for (auto it = ints.cbegin(); it != ints.cend(); ++it)
	{
		os << VAR_PREFIX << *(it);

		if (it != (ints.cend() - 1))
			os << ",";
	}

	os << "]) ";
}

void enforce_path(model_writer& w, const edge_path& p)
{
	auto edges = to_edge_list(p);

	index_t color = 1;
	for (index_t i = 0; i < edges.size(); i += 2)
	{
		std::string line = 
			"constraint " + VAR_PREFIX + 
			std::to_string(edges[i]) + "_" + std::to_string(edges[i + 1]) + 
			" = " + std::to_string(color) + ";\n";

		++color;
		w.add_line(line);
	}
}
