// minion_model_writer.hpp
#ifndef MINION_MODEL_WRITER_HPP
#define MINION_MODEL_WRITER_HPP

#include "model_writer.hpp"
#include <vector>
#include <string>
#include <sstream>
#include <iostream>

class minion_model_writer : public model_writer
{
public:
	minion_model_writer(const graph& g, index_t k, std::ostream& os)
		: model_writer(g, k, os, "#")
	{

	}

	virtual ~minion_model_writer() { }

private:
	virtual void impl_preprocess();
	virtual void impl_postprocess();

	virtual void impl_process_vertex_pair(index_t u, index_t v);
};

class strong_minion_model_writer : public minion_model_writer
{
public:
	strong_minion_model_writer(const graph& g, index_t k, std::ostream& os)
		: minion_model_writer(g, k, os)
	{

	}

private:
	virtual void impl_process_vertex_pair(index_t u, index_t v);
};

namespace
{
	const std::string VAR_PREFIX = "x";

	void add_minion_variables(const graph& g, index_t k, std::ostream& os)
	{
		const index_t n = g.num_vertices();

		for (index_t i = 0; i < n; ++i)
		{
			index_t adj = g.adj_[i];
			for (index_t j = i; j < n; ++j)
			{
				if (adj & (1ULL << j))
					os << "DISCRETE " << VAR_PREFIX << i << "_" << j << " {1.." << k << "}\n";
			}
		}
	}
}

template <typename ModelWriter>
std::vector<index_t> polynomial_points(const graph& g)
{
	const std::string filename = "temp_poly.txt";

	// Find paths only once, then modify the file in-place.
	std::ofstream ofs(filename);

	//auto start = std::chrono::high_resolution_clock::now();
	ModelWriter writer(g, 1, ofs);
	writer.write();
	//auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);

	ofs.close();
	const index_t m = g.num_edges();
	std::vector<index_t> sols;

	for (index_t i = 1; i <= m; ++i)
	{
		std::fstream ifs(filename);
		ifs << "MINION 3\n\n";
		ifs << "**VARIABLES**\n\n";

		add_minion_variables(g, i, ifs);
		ifs << "\n**CONSTRAINTS**\n#";
		ifs.close();

		const std::string outfile = "out_" + std::to_string(i) + ".txt";
		const std::string exec = "minion -findallsols -noprintsols " + filename + " > " + outfile;

		//auto start = std::chrono::high_resolution_clock::now();
		system(exec.c_str());
		//auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
		//std::cerr << "Point " << i << " evaluated in " << (duration.count() / 1000.0) << " s\n";

		// Parse solution count
		std::ifstream reader(outfile);
		std::string line = "";
		while (std::getline(reader, line))
		{
			if (line.find("Solutions Found:") != std::string::npos)
			{
				index_t sol_count = -1;
				std::istringstream ss(line);

				ss >> std::string() >> std::string() >> sol_count;
				sols.emplace_back(sol_count);
			}
		}

		reader.close();
		const std::string del = "del " + outfile;
		system(del.c_str());
	}

	const std::string del = "del " + filename;
	system(del.c_str());

	return sols;
}

#endif