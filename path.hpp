// path.hpp
#ifndef PATH_HPP
#define PATH_HPP

#include "common.hpp"
#include <algorithm>
#include <vector>

// DEBUG!
/*
#include <bitset>
#include <iostream>
*/

template <typename T, typename Iterator>
class path
{
public:
	void discover_vertex(index_t v)
	{
		static_cast<T*>(this)->discover_vertex(v);
	}

	void backtrack_vertex(index_t v)
	{
		static_cast<T*>(this)->backtrack_vertex(v);
	}

	bool contains_vertex(index_t v) const
	{
		return static_cast<const T* const>(this)->contains_vertex(v);
	}

	Iterator cbegin() const
	{
		static_cast<const T* const>(this)->cbegin();
	}

	Iterator cend() const
	{
		static_cast<const T* const>(this)->cend();
	}
};

class vertex_path : public path<vertex_path, std::vector<bool>::const_iterator>
{
public:
	vertex_path() : p_(64, false) { }

	vertex_path(index_t s)
		: //path_(1 << s),
		p_(64, false)
	{
		p_[s] = true;
	}

	void discover_vertex(index_t v)
	{
		//path_ |= (1 << v);
		p_[v] = true;
	}

	void backtrack_vertex(index_t v)
	{
		//path_ &= ~(1 << v);
		p_[v] = false;
	}

	bool contains_vertex(index_t v) const
	{
		//assert(bittest64(path_, v) == p_[v]);
		//return bittest64(path_, v);
		return p_[v];
	}

	typedef std::vector<bool>::const_iterator Iterator;

	Iterator cbegin() const 
	{
		return p_.cbegin(); 
	}

	Iterator cend() const
	{
		return p_.cend();
	}

private:
	//index_t path_;
	std::vector<bool> p_;
};

void drop_endpoints(vertex_path& p);

class edge_path : public path<edge_path, std::vector<index_t>::const_iterator>
{
public:
	edge_path() { }

	void discover_vertex(index_t v)
	{
		edges_.emplace_back(v);
	}

	void backtrack_vertex(index_t v)
	{
		edges_.pop_back();
	}

	bool contains_vertex(index_t v) const
	{
		return std::find(edges_.cbegin(), edges_.cend(), v) != edges_.cend();
	}

	typedef std::vector<index_t>::const_iterator Iterator;

	Iterator cbegin() const 
	{
		return edges_.cbegin();
	}

	Iterator cend() const
	{
		return edges_.cend();
	}

private:
	std::vector<index_t> edges_;
};

std::vector<index_t> to_edge_list(const edge_path& p);

#endif