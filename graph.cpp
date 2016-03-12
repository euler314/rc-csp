// graph.cpp
#include "graph.hpp"

#include "common.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <cassert>
#include <random>
#include <queue>

// DEBUG!
#include <iostream>
#include <bitset>

namespace
{
	index_t read_graph_size(const std::string& filename)
	{
		std::ifstream file(filename);
		std::string line = "";

		index_t n = 0;
		index_t m = 0;

		while (std::getline(file, line))
		{
			std::istringstream iss(line);
			char ch;

			if (iss >> ch)
			{
				if (ch == 'p')
				{
					std::string format = "";
					if (iss >> format >> n >> m)
					{
						// Format can be whatever.
					}
					else
					{
						const std::string error = "Badly formatted problem line in " + filename;
						throw std::exception(error.c_str());
					}
				}
			}
		}

		return n;
	}
}

void graph::add_edge(index_t u, index_t v)
{
	assert(u >= 0 &&
		v >= 0 &&
		u != v &&
		u < adj_.size() &&
		v < adj_.size());

	adj_[u] |= (1ULL << v);
	adj_[v] |= (1ULL << u);
	
	edges_.emplace_back(u);
	edges_.emplace_back(v);
}

index_t graph::get_degree(index_t u) const
{
	return popcount64(adj_[u]);
}

index_t graph::num_vertices() const
{
	return adj_.size();
}

index_t graph::num_edges() const
{
	return edges_.size() / 2;
}

std::vector<index_t> graph::get_edges() const
{
	return edges_;
}

bool is_adjacent(const graph& g, index_t u, index_t v)
{
	assert(u >= 0 && v >= 0 && u < g.adj_.size() && v < g.adj_.size());

	return bittest64(g.adj_[u], v);
}

void print_graph(const graph& g)
{
	std::cout << "Matrix:\n";
	for (index_t i = 0; i < g.num_vertices(); ++i)
	{
		std::cout << std::bitset<4>(g.adj_[i]).to_string() << "\n";
	}

	std::cout << "\nEdges:\n";
	auto e = g.get_edges();

	for (int i = 0; i < e.size(); i += 2)
	{
		std::cout << "(" << e[i] << "," << e[i + 1] << ")\n";
	}
}

graph build_clique(index_t n)
{
	graph g(n);

	for (index_t i = 0; i < n; ++i)
	{
		for (index_t j = i + 1; j < n; ++j)
		{
			g.add_edge(i, j);
		}
	}

	return g;
}

graph build_biclique(index_t p, index_t q)
{
	graph g(p + q);

	for (index_t i = 0; i < p; ++i)
	{
		for (index_t j = p; j < (p + q); ++j)
		{
			g.add_edge(i, j);
		}
	}

	return g;
}

graph build_star(index_t l)
{
	graph g(l + 1);

	for (index_t i = 1; i <= l; ++i)
	{
		g.add_edge(0, i);
	}

	return g;
}

graph build_cycle(index_t n)
{
	graph g(n);

	for (int i = 0; i < (n - 1); ++i)
	{
		g.add_edge(i, i + 1);
	}

	g.add_edge(0, n - 1);

	return g;
}

graph build_random_graph(index_t n, double p)
{
	assert(n >= 2 && n <= 64);

	std::random_device rd;
	std::mt19937 gen(rd());
	gen.seed(123);
	std::bernoulli_distribution d(p);

	graph g(n);

	for (index_t i = 0; i < n; ++i)
	{
		for (index_t j = i + 1; j < n; ++j)
		{
			if (d(gen))
			{
				g.add_edge(i, j);
			}
		}
	}

	return g;
}

graph read_dimacs(const std::string& filename)
{
	index_t n = 0;
	try
	{
		n = read_graph_size(filename);
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << "\n";
	}

	std::ifstream file(filename);
	std::string line = "";

	graph g(n);

	while (std::getline(file, line))
	{
		std::istringstream iss(line);
		char ch;
		
		if (iss >> ch)
		{
			if (ch == 'e')
			{
				index_t u = 0;
				index_t v = 0;
				if (iss >> u >> v)
				{
					// In DIMACS, vertices start from 1 (not from 0)
					g.add_edge(u - 1, v - 1);
				}
				else
				{
					const std::string error = "Badly formatted edge line in " + filename;
					throw std::exception(error.c_str());
				}
			}
		}
	}

	return g;
}

void bfs(const graph& g, std::vector<index_t>& dist, index_t source)
{
	assert(g.num_vertices() <= 64);
	assert(source < g.num_vertices());

	const index_t n = g.num_vertices();
	index_t visited = (1ULL << source);
	std::queue<index_t> q;
	q.push(source);

	assert(dist.size() == n && std::count(dist.cbegin(), dist.cend(), 0) == n);

	while (!q.empty())
	{
		index_t v = q.front();

		for (index_t i = 0; i < n; ++i)
		{
			if (bittest64(g.adj_[v], i) && !bittest64(visited, i))
			{
				dist[i] = 1 + dist[v];
				q.push(i);
				visited |= (1ULL << i);
			}
		}

		q.pop();
	}
}

bool is_connected(const graph& g)
{
	const index_t n = g.num_vertices();
	index_t visited = 1ULL;
	index_t q = 1ULL;

	// The vertex 0 is visited and added to the queue.
	assert(visited == q);
	assert(popcount64(visited) <= g.num_vertices());

	while (q != 0)
	{
		assert(popcount64(visited) <= g.num_vertices());
		index_t v = ctz64(q);

		for (index_t i = 0; i < n; ++i)
		{
			if (bittest64(g.adj_[v], i) && !bittest64(visited, i))
			{
				q |= (1ULL << i);
				visited |= (1ULL << i);
			}
		}

		q &= ~(1ULL << v);
	}

	return popcount64(visited) == g.num_vertices();
}

std::vector<index_t> get_bridges(const graph& g)
{
	// Naive algorithm: remove each edge, and check if the graph is connected.

	std::vector<index_t> bridges;
	auto edges = g.get_edges();
	const index_t m = edges.size();

	for (index_t i = 0; i < (m - 1); i += 2)
	{
		auto edges_ref = edges;
		auto candidate = std::make_pair(edges[i], edges[i + 1]);

		assert(candidate.first == *(edges_ref.begin() + i));
		assert(candidate.second == *(edges_ref.begin() + i + 1));

		edges_ref.erase(edges_ref.begin() + i, edges_ref.begin() + i + 2);

		//std::cout << "Testing edge: " << candidate.first << "," << candidate.second << "\n";

		graph h(g.num_vertices());

		for (index_t j = 0; j < edges_ref.size(); j += 2)
		{
			h.add_edge(edges_ref[j], edges_ref[j + 1]);
		}

		if (!is_connected(h))
		{
			assert(candidate.first < candidate.second);
			bridges.emplace_back(candidate.first);
			bridges.emplace_back(candidate.second);
		}
	}

	return bridges;
}

index_t get_diameter(const graph& g)
{
	const index_t n = g.num_vertices();
	index_t diam = 0;
	for (index_t i = 0; i < n; ++i)
	{
		std::vector<index_t> dist(n);
		bfs(g, dist, i);
		
		const index_t current_max = *std::max_element(dist.cbegin(), dist.cend());
		if (current_max > diam)
			diam = current_max;
	}

	return diam;
}

std::pair<index_t, index_t> get_diametral_pair(const graph& g)
{
	const index_t diam = get_diameter(g);
	const index_t n = g.num_vertices();

	for (index_t i = 0; i < n; ++i)
	{
		std::vector<index_t> dist(n);
		bfs(g, dist, i);
		auto distant_elem = std::max_element(dist.cbegin(), dist.cend());

		if (diam == *distant_elem)
		{
			return std::minmax(i, std::distance(dist.cbegin(), distant_elem));
		}
	}

	assert(false && "no diametral pair found");
	return std::make_pair(0, 0);
}

// neato -Tpng foo.dot -o foo.png :-)
void write_dot(const graph& g, const std::vector<index_t>& cols, std::ostream& os)
{
	const std::string SPACE = "  ";
	os << "graph G {\n";
	os << SPACE;

	os << "node [fixedsize=true shape=circle width=0.325];\n";

	assert(false && "finish the impl");
}