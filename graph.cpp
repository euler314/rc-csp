// graph.cpp
#include "graph.hpp"

#include "common.hpp"
#include <cassert>

// DEBUG!
#include <iostream>
#include <bitset>

void graph::add_edge(index_t u, index_t v)
{
	assert(u >= 0 &&
		v >= 0 &&
		u != v &&
		u < adj_.size() &&
		v < adj_.size());

	adj_[u] |= (1 << v);
	adj_[v] |= (1 << u);
	++edges_;
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
	for (index_t i = 0; i < g.num_vertices(); ++i)
	{
		for (index_t j = i; j < g.num_vertices(); ++j)
		{
			if (is_adjacent(g, i, j))
			{
				std::cout << "(" << i << "," << j << ")\n";
			}
		}
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

void bfs(const graph& g, std::vector<index_t>& dist, index_t source)
{
	assert(g.num_vertices() <= 64);
	assert(source < g.num_vertices());

	const index_t n = g.num_vertices();
	index_t visited = (1ULL << source);
	index_t q = (1ULL << source);

	assert(dist.size() == n && std::count(dist.cbegin(), dist.cend(), 0) == n);

	while (q != 0)
	{
		int v = ctz64(q);

		for (index_t i = 0; i < n; ++i)
		{
			if (bittest64(g.adj_[v], i) && !bittest64(visited, i))
			{
				dist[i] = 1 + dist[v];
				q |= (1ULL << i);
				visited |= (1ULL << i);
			}
		}

		q &= ~(1ULL << v);
	}
}