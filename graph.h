#pragma once
#include <map>
#include <vector>

class Point {
public:
	Point() {}
	Point(float xx, float yy) : x(xx), y(yy) {}

	float distance_p(const Point& p) const;

	bool operator==(const Point& p) const;
	bool operator!=(const Point& p) const;
	bool operator<(const Point& p) const;
	bool operator<=(const Point& p) const;

public:
	float x{ 0.0 };
	float y{ 0.0 };
};

class Graph {
public:
	Graph() {};

	int add_vertex(const Point& p);
	void add_edge(const Point& x, const Point& y, float weight);
	bool has_vertex(const Point& x) const;
	bool has_edge(const Point& x, const Point& y) const;
	std::vector<int> get_vertices() const;
	std::vector<int> get_adjacent_vertices(int vertex) const;
	float edge_weight(int x, int y) const;
	void remove_vertex(const Point& p);
	void remove_edge(int start_vertex, int end_vertex);
	std::vector<std::pair<int, float>> get_adjacent_edges(int src_vertex) const;
	void destroy();
	std::vector<Point> points;

private:
	std::map<int, std::map<int, float>> vertices;
};