// graph.hpp
#ifndef GRAPH_HPP
#define GRAPH_HPP

#include "common.hpp"
#include <vector>
#include <cassert>

struct graph
{
	graph(index_t n) : adj_(n), edges_(0)
	{
		assert(n <= 64);
	}

	graph& operator=(const graph&) = delete;

	~graph() { }

	void add_edge(index_t u, index_t v);

	index_t get_degree(index_t u) const;

	index_t num_vertices() const;

	index_t num_edges() const;

	std::vector<index_t> adj_;
	index_t edges_;
};

bool is_adjacent(const graph& g, index_t u, index_t v);

void print_graph(const graph& g);

graph build_clique(index_t n);

graph build_biclique(index_t p, index_t q);

graph build_star(index_t l);

graph build_cycle(index_t n);

void bfs(const graph& g, std::vector<index_t>& dist, index_t source = 0);

namespace detail
{
	template <typename Path>
	void recursive_list_paths(const graph& g, index_t current, index_t t, Path current_path, std::vector<Path>& paths)
	{
		// This sets the bit "current", so we discover it
		// p |= (1 << current);

		current_path.discover_vertex(current);

		if (current == t)
		{
			paths.emplace_back(current_path);
		}
		else
		{
			index_t adj = g.adj_[current];

			for (int iter; adj != 0; adj &= ~(1 << iter))
			{
				iter = ctz64(adj);

				//if (!bittest64(p, iter))
				if (!current_path.contains_vertex(iter))
				{
					recursive_list_paths<Path>(g, iter, t, current_path, paths);
				}
			}
		}

		// This clears the bit "current", so we backtrack
		// p &= ~(1 << current);
		current_path.backtrack_vertex(current);
	}
}

template <typename Path>
void list_paths(const graph& g, index_t s, index_t t, std::vector<Path>& paths)
{
	//index_t stack = (1 << s);
	Path current_path;
	detail::recursive_list_paths(g, s, t, current_path, paths);
}

namespace detail
{
	template <typename Path>
	void recursive_list_shortest_paths(
		const graph& g,
		index_t current,
		index_t t,
		const std::vector<index_t>& dist,
		Path& current_path,
		std::vector<Path>& paths)
	{
		if (current == t)
		{
			paths.emplace_back(current_path);
		}

		index_t adj = g.adj_[current];

		for (int iter; adj != 0; adj &= ~(1 << iter))
		{
			iter = ctz64(adj);

			if (dist[iter] < dist[current])
			{
				current_path.discover_vertex(iter);

				recursive_list_shortest_paths<Path>(g, iter, t, dist, current_path, paths);

				current_path.backtrack_vertex(iter);
			}
		}
	}
}

template <typename Path>
void list_shortest_paths(const graph& g, index_t s, index_t t, std::vector<Path>& paths)
{
	std::vector<index_t> dist(g.num_vertices(), 0);
	bfs(g, dist, t);

	Path current_path(s);
	current_path.discover_vertex(s);
	detail::recursive_list_shortest_paths(g, s, t, dist, current_path, paths);
}

#endif