#include "graph.h"
#include <cmath>
#include <stdexcept>

using namespace std;

float Point::distance_p(const Point& p) const {
    return sqrt((x - p.x) * (x - p.x) + (y - p.y) * (y - p.y));
}

namespace {
    static const double EPS = 1e-8;

    bool equal(float a, float b) {
        return abs(a - b) < EPS;
    }
}

bool Point::operator==(const Point& p) const {
    return equal(x, p.x) && equal(y, p.y);
}

bool Point::operator!=(const Point& p) const {
    return !equal(x, p.x) || !equal(y, p.y);
}

bool Point::operator<(const Point& p) const {
    return (x < p.x) || (equal(x, p.x) && y < p.y);
}

bool Point::operator<=(const Point& p) const {
    return (x < p.x) || (equal(x, p.x) && ((y < p.y) || equal(y, p.y)));
}

////////////////////////////////////////////////////////////////////////////

bool Graph::has_vertex(const Point& p) const {
    return (find(points.begin(), points.end(), p) != points.end());
}

bool Graph::has_edge(const Point& x, const Point& y) const {

    int start = points.size();
    int finish = points.size();

    for (int i = 0; i < points.size(); ++i)
    {
        if (points[i] == x) {
            start = i;
        }
        else if (points[i] == y) {
            finish = i;
        }
    }

    const auto it = vertices.find(start);
    if (it == vertices.end()) {
        return false;
    }
    return (it->second.find(finish) != it->second.end());
}

std::vector<int> Graph::get_adjacent_vertices(int p) const {
    const auto it = vertices.find(p);
    if (it == vertices.end()) {
        return std::vector<int> {};
    }
    vector<int> result;
    for (const auto& p : it->second) {
        result.push_back(p.first);
    }
    return result;
}

std::vector<int> Graph::get_vertices() const {
    std::vector<int> result;
    for (const auto& p : vertices) {
        result.push_back(p.first);
    }
    return result;
}

float Graph::edge_weight(int x, int y) const {
    const auto it_s = vertices.find(x);
    if (it_s == vertices.end()) {
        throw invalid_argument("Edge doesn't exist");
    }
    const auto it_e = it_s->second.find(y);
    if (it_e == it_s->second.end()) {
        throw invalid_argument("Edge doesn't exist");
    }
    return it_e->second;
}

void Graph::add_edge(const Point& x, const Point& y, float weight) {
    int start = add_vertex(x);
    int finish = add_vertex(y);
    vertices[start][finish] = weight;
    vertices[finish][start] = weight;
}

int Graph::add_vertex(const Point& p) {

    int ind = points.size();
    if (!has_vertex(p)) {
        vertices[ind] = std::map<int, float>();
        points.push_back(p);
        return ind;
    }
    else {
        for (int i = 0; i < points.size(); ++i)
        {
            if (points[i] == p) {
                ind = i;
            }
        }
        return ind;
    }
}

void Graph::remove_vertex(const Point& p) {
    auto d = find(points.begin(), points.end(), p);
    if (d != points.end())
    {
        int vertex = points.size();
        for (int i = 0; i < points.size(); ++i)
        {
            if (points[i] == p) {
                vertex = i;
            }
        }

        auto adjacent_vertices = get_adjacent_vertices(vertex);
        for (const auto& adj_v : adjacent_vertices) {
            remove_edge(adj_v, vertex);
        }
        vertices.erase(vertex);
        points.erase(d);
    }
}

void Graph::destroy()
{
    vertices.clear();
    points.clear();
}


void Graph::remove_edge(int start_vertex, int end_vertex) {
    auto it_s = vertices.find(start_vertex);
    if (it_s != vertices.end()) {
        it_s->second.erase(end_vertex);
    }
    auto it_e = vertices.find(end_vertex);
    if (it_e != vertices.end()) {
        it_e->second.erase(start_vertex);
    }
}

vector<pair<int, float>> Graph::get_adjacent_edges(int src_vertex) const {
    const auto it = vertices.find(src_vertex);
    if (it == vertices.end()) {
        return vector<pair<int, float>> {};
    }
    vector<pair<int, float>> result;
    for (const auto& p : it->second) {
        result.push_back(make_pair(p.first, p.second));
    }
    return result;
}