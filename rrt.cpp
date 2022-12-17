#include "rrt.h"
#include <math.h>
const int N = 10000000;
vector<pair<Point, float>> obs;

int extract_min(vector<int>& Q, vector<float>& distance)
{
	float min_dist = N;
	auto u = Q.end();
	int result = N;

	for (auto i = Q.begin(); i < Q.end(); ++i)
	{
		if (*i < distance.size()) {
			if (distance[*i] < min_dist) {
				min_dist = distance[*i];
				u = i;
				result = *i;
			}
		}
	}

	if (u != Q.end()) {
		Q.erase(u);
	}

	return result;
}

double length(const Graph& g, const vector<int>& path)
{
	if (path.size() <= 1) {
		return 0;
	}
	double result = 0;
	for (auto i = 0; i < path.size() - 1; ++i)
	{
		result += g.edge_weight(path[i], path[i + 1]);
	}
	return result;
}

vector<int> build_path(vector<int>& parent, int start_vertex, int finish_vertex)
{
	vector<int> path;
	if (finish_vertex >= parent.size()) {
		return path;
	}
	int i = finish_vertex;
	path.push_back(finish_vertex);
	while (i != start_vertex)
	{
		i = parent[i];
		if (i == N or i == finish_vertex) {
			return {};
		}
		path.insert(path.begin(), i);
	}
	return path;
}

vector<int> shortest_path(const Graph& g, int start_vertex, int finish_vertex)
{
	vector<int> Q = g.get_vertices();
	if (Q.size() == 0) {
		return {};
	}

	vector<float> distance(Q.size());
	vector<int> parent(Q.size());
	for (int i = 0; i < distance.size(); ++i) {
		distance[i] = N;
		parent[i] = N;
	}
	distance[start_vertex] = 0;

	while (!Q.empty())
	{
		int u = extract_min(Q, distance);
		if (u == N) {
			return build_path(parent, start_vertex, finish_vertex);
		}
		auto v = g.get_adjacent_vertices(u);
		for (int i = 0; i < v.size(); ++i)
		{
			if (distance[v[i]] > (distance[u] + g.edge_weight(u, v[i]))) {
				distance[v[i]] = distance[u] + g.edge_weight(u, v[i]);
				parent[v[i]] = u;
			}
		}
	}
	return build_path(parent, start_vertex, finish_vertex);
}

////////////////////////////////////////////////////////////////////////////////////////

float Dot(const Point& a, const Point& b)
{
	return (a.x * b.x + a.y * b.y);
}

bool intersection_pc(pair<Point, float> c, const Point& p)
{
	Point a(p.x - c.first.x, p.y - c.first.y);
	return (Dot(a, a) <= c.second * c.second);
}

Point Closest(const Point& a, const Point& b, const Point& p)
{
	Point ap(p.x - a.x, p.y - a.y);
	Point ab(b.x - a.x, b.y - a.y);
	float d = Dot(ap, ab) / Dot(ab, ab);
	if (d > 1) {
		d = 1;
	}
	else if (d < 0) {
		d = 0;
	}

	Point result(a.x + (b.x - a.x) * d, a.y + (b.y - a.y) * d);
	return result;
}

bool intersection_lc(const Point& a, const Point& b, pair<Point, double> c)
{
	return intersection_pc(c, Closest(a, b, c.first));
}
//////////////////////////////////////////////////////////////////////////////////////

Point RandomSample()
{
	Point p(rand() % 800, rand() % 600);
	return p;
}

bool CollisionFree(const Point& a, const Point& b)
{
	for (int i = 0; i < obs.size(); ++i)
	{
		if (intersection_lc(a, b, obs[i])) {
			return false;
		}
	}
	return true;
}

Point Nearest(Graph& g, const Point& p)
{
	float min_dist = N;
	Point nearest_point(N, N);
	int point_a = N, point_b = N;

	for (int i = 0; i < g.points.size(); ++i)
	{
		float dis_to_point = p.distance_p(g.points[i]);
		auto u = g.get_adjacent_vertices(i);
		for (int j = 0; j < u.size(); ++j)
		{
			float dis_to_line = p.distance_p(Closest(g.points[i], g.points[u[j]], p));
			if (p.distance_p(Closest(g.points[i], g.points[u[j]], p)) < min_dist) {
				min_dist = p.distance_p(Closest(g.points[i], g.points[u[j]], p));
				nearest_point = Closest(g.points[i], g.points[u[j]], p);
				point_a = i;
				point_b = u[j];
			}
		}
		if (dis_to_point < min_dist) {
			min_dist = dis_to_point;
			nearest_point = g.points[i];
		}
	}

	if (!g.has_vertex(nearest_point)) {
		g.remove_edge(point_a, point_b);
		g.add_vertex(nearest_point);
		g.add_edge(g.points[point_a], nearest_point, nearest_point.distance_p(g.points[point_a]));
		g.add_edge(nearest_point, g.points[point_b], nearest_point.distance_p(g.points[point_b]));
	}

	return nearest_point;
}

Point Steer(const Point& a, const Point& b)
{
	float dx = ((b.x - a.x) / b.distance_p(a)) * 2;
	float dy = ((b.y - a.y) / b.distance_p(a)) * 2;

	Point z(b);

	while (!CollisionFree(a, z) and z.x < 800 and z.x > 0 and z.y < 600 and z.y > 0)
	{
		z.x -= dx;
		z.y -= dy;
	}
	if(z.x > 0 and z.x < 800 and z.y < 600 and z.y > 0){
		return z; 
	}
	else { 
		return a; 
	}
}

vector<Point> RRT(int S, Graph& g, const Point& Qinit, const Point& Qgoal, const vector<pair<Point, float>>& v)
{
	obs = v;
	g.add_vertex(Qinit);

	while (g.points.size() < S)
	{
		auto Qrand = RandomSample();
		auto On = Nearest(g, Qrand);
		auto Os = Steer(On, Qrand);
		g.add_edge(On, Os, On.distance_p(Os));
	}

	auto On = Nearest(g, Qgoal);
	if (CollisionFree(On, Qgoal)) {
		g.add_vertex(Qgoal);
		g.add_edge(On, Qgoal, Qgoal.distance_p(On));
		auto result = shortest_path(g, 0, g.points.size() - 1);

		vector<Point> answer;
		for (auto i = 0; i < result.size(); ++i)
		{
			answer.push_back(g.points[result[i]]);
		}
		return answer;
	}
	else {
		return {};
	}
}