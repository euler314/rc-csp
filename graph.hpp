// graph.hpp
#ifndef GRAPH_HPP
#define GRAPH_HPP

#include "common.hpp"
#include <vector>
#include <iostream>
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

	std::vector<index_t> get_edges() const;

	std::vector<index_t> adj_;
	std::vector<index_t> edges_;
};

bool is_adjacent(const graph& g, index_t u, index_t v);

void print_graph(const graph& g);

graph build_clique(index_t n);

graph build_path(index_t n);

graph build_biclique(index_t p, index_t q);

graph build_star(index_t l);

graph build_cycle(index_t n);

graph build_wheel(index_t spokes);

graph build_corona(index_t n);

graph build_random_graph(index_t n, double p);

graph read_dimacs(const std::string& file);

void bfs(const graph& g, std::vector<index_t>& dist, index_t source = 0);

bool is_connected(const graph& g);

std::vector<index_t> get_bridges(const graph& g);

index_t get_diameter(const graph& g);

std::pair<index_t, index_t> get_diametral_pair(const graph& g);

void write_dot(const graph& g, const std::vector<index_t>& cols, std::ostream& os = std::cout);

namespace detail
{
	template <typename Path>
	void recursive_list_paths(const graph& g, index_t current, index_t t, Path current_path, std::vector<Path>& paths, index_t length)
	{
		// This sets the bit "current", so we discover it
		// p |= (1 << current);

		current_path.discover_vertex(current);

		if (current == t)
		{
			//if (current_path.size() <= length)
				paths.emplace_back(current_path);
		}
		else
		{
			index_t adj = g.adj_[current];

			for (int iter; adj != 0; adj &= ~(1 << iter))
			{
				if (current_path.size() >= length)
					break;

				iter = ctz64(adj);

				//if (!bittest64(p, iter))
				if (!current_path.contains_vertex(iter))
				{
					recursive_list_paths<Path>(g, iter, t, current_path, paths, length);
				}
			}
		}

		// This clears the bit "current", so we backtrack
		// p &= ~(1 << current);
		current_path.backtrack_vertex(current);
	}
}

template <typename Path>
void list_paths(const graph& g, index_t s, index_t t, std::vector<Path>& paths, index_t length)
{
	//index_t stack = (1 << s);
	Path current_path;
	detail::recursive_list_paths(g, s, t, current_path, paths, length);
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

template <typename Path>
Path get_shortest_path(const graph& g, index_t s, index_t t)
{
	assert(g.num_vertices() <= 64);
	assert(s < g.num_vertices());

	const index_t n = g.num_vertices();
	index_t visited = (1ULL << s);
	index_t q = (1ULL << s);

	std::vector<index_t> dist(n);
	assert(dist.size() == n && std::count(dist.cbegin(), dist.cend(), 0) == n);

	const int NO_PREDECESSOR = -1;
	std::vector<int> pred(n, NO_PREDECESSOR);

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
				pred[i] = v;
			}
		}

		q &= ~(1ULL << v);
	}

	Path st_path;

	index_t w = t;
	while (pred[w] != NO_PREDECESSOR)
	{
		st_path.discover_vertex(w);
		w = pred[w];
	}
	
	st_path.discover_vertex(s);
	return st_path;
}

#endif