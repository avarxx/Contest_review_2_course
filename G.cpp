#include <iostream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <limits>

struct Point {
    long double x;
    long double y;
};

struct Polygon {
    std::vector<Point> vertices;
};

static long double global_field_width;
static long double global_field_height;
static const long double kEpsilon = 1e-12L;

static Point IntersectSegmentLine(const Point& segment_start, const Point& segment_end,
                                  long double line_a, long double line_b, long double line_c) {
    long double x1 = segment_start.x;
    long double y1 = segment_start.y;
    long double x2 = segment_end.x;
    long double y2 = segment_end.y;
    long double dx = x2 - x1;
    long double dy = y2 - y1;
    long double denominator = line_a * dx + line_b * dy;
    Point intersection_point;

    if (fabsl(denominator) < kEpsilon) {
        intersection_point = segment_start;
    } else {
        long double parameter = (line_c - (line_a * x1 + line_b * y1)) / denominator;
        parameter = std::clamp(parameter, 0.0L, 1.0L);
        intersection_point.x = x1 + parameter * dx;
        intersection_point.y = y1 + parameter * dy;
    }
    return intersection_point;
}

static Polygon ClipPolygon(const Polygon& polygon,
                           long double line_a, long double line_b, long double line_c) {
    Polygon clipped_polygon;
    const std::vector<Point>& vertices = polygon.vertices;
    if (vertices.empty()) return clipped_polygon;

    for (size_t i = 0; i < vertices.size(); ++i) {
        const Point& current_point = vertices[i];
        const Point& next_point = vertices[(i + 1) % vertices.size()];
        long double current_value = line_a * current_point.x + line_b * current_point.y - line_c;
        long double next_value = line_a * next_point.x + line_b * next_point.y - line_c;
        bool is_current_inside = current_value <= kEpsilon;
        bool is_next_inside = next_value <= kEpsilon;

        if (is_current_inside) {
            clipped_polygon.vertices.push_back(current_point);
        }
        if (is_current_inside != is_next_inside) {
            Point intersection = IntersectSegmentLine(current_point, next_point, line_a, line_b, line_c);
            clipped_polygon.vertices.push_back(intersection);
        }
    }

    std::vector<Point>& new_vertices = clipped_polygon.vertices;
    std::vector<Point> unique_vertices;
    unique_vertices.reserve(new_vertices.size());
    for (const Point& vertex : new_vertices) {
        if (unique_vertices.empty() || 
            fabsl(unique_vertices.back().x - vertex.x) > kEpsilon || 
            fabsl(unique_vertices.back().y - vertex.y) > kEpsilon) {
            unique_vertices.push_back(vertex);
        }
    }
    if (unique_vertices.size() > 1 && 
        fabsl(unique_vertices.front().x - unique_vertices.back().x) < kEpsilon && 
        fabsl(unique_vertices.front().y - unique_vertices.back().y) < kEpsilon) {
        unique_vertices.pop_back();
    }
    clipped_polygon.vertices.swap(unique_vertices);
    return clipped_polygon;
}

static Polygon ComputeZoneForPlayer(int player_index, const std::vector<Point>& all_players) {
    Polygon zone;
    zone.vertices.reserve(4);
    zone.vertices.push_back({0.0L, 0.0L});
    zone.vertices.push_back({global_field_width, 0.0L});
    zone.vertices.push_back({global_field_width, global_field_height});
    zone.vertices.push_back({0.0L, global_field_height});

    long double current_player_x = all_players[player_index].x;
    long double current_player_y = all_players[player_index].y;

    for (size_t other_player_index = 0; other_player_index < all_players.size(); ++other_player_index) {
        if (static_cast<int>(other_player_index) == player_index) continue;
        long double other_player_x = all_players[other_player_index].x;
        long double other_player_y = all_players[other_player_index].y;

        long double line_a = other_player_x - current_player_x;
        long double line_b = other_player_y - current_player_y;
        long double line_c = (other_player_x * other_player_x - current_player_x * current_player_x +
                             other_player_y * other_player_y - current_player_y * current_player_y) * 0.5L;

        zone = ClipPolygon(zone, line_a, line_b, line_c);
        if (zone.vertices.empty()) break;
    }
    return zone;
}

static void PreparePolygonOutput(Polygon& polygon) {
    if (polygon.vertices.empty()) return;

    long double area = 0.0L;
    for (size_t i = 0; i < polygon.vertices.size(); ++i) {
        size_t j = (i + 1) % polygon.vertices.size();
        area += polygon.vertices[i].x * polygon.vertices[j].y - polygon.vertices[j].x * polygon.vertices[i].y;
    }
    if (area < 0) {
        std::reverse(polygon.vertices.begin(), polygon.vertices.end());
    }

    long double min_x = polygon.vertices[0].x;
    for (const Point& vertex : polygon.vertices) {
        if (vertex.x < min_x - kEpsilon) min_x = vertex.x;
    }

    long double min_y = std::numeric_limits<long double>::max();
    size_t start_index = 0;
    for (size_t i = 0; i < polygon.vertices.size(); ++i) {
        if (fabsl(polygon.vertices[i].x - min_x) < kEpsilon && 
            polygon.vertices[i].y < min_y - kEpsilon) {
            min_y = polygon.vertices[i].y;
            start_index = i;
        }
    }

    std::rotate(polygon.vertices.begin(), polygon.vertices.begin() + start_index, polygon.vertices.end());
}

static void ReadInput(std::vector<Point>& players) {
    int num_players;
    std::cin >> global_field_width >> global_field_height >> num_players;
    players.resize(num_players);
    for (int i = 0; i < num_players; ++i) {
        std::cin >> players[i].x >> players[i].y;
    }
}

static void OutputZones(const std::vector<Polygon>& zones) {
    std::cout << std::fixed << std::setprecision(8);
    for (const Polygon& poly : zones) {
        std::cout << poly.vertices.size();
        for (const Point& vertex : poly.vertices) {
            long double x = vertex.x;
            long double y = vertex.y;
            if (x < 0 && x > -kEpsilon) x = 0;
            if (y < 0 && y > -kEpsilon) y = 0;
            if (x > global_field_width && x < global_field_width + kEpsilon) x = global_field_width;
            if (y > global_field_height && y < global_field_height + kEpsilon) y = global_field_height;
            std::cout << " " << x << " " << y;
        }
        std::cout << "\n";
    }
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::vector<Point> players;
    ReadInput(players);
    std::vector<Polygon> zones;
    zones.reserve(players.size());
    for (int i = 0; i < static_cast<int>(players.size()); ++i) {
        Polygon zone = ComputeZoneForPlayer(i, players);
        PreparePolygonOutput(zone);
        zones.push_back(zone);
    }
    OutputZones(zones);
    return 0;
}