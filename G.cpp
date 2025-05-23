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

static long double g_field_width;
static long double g_field_height;
static const long double kEps = 1e-12L;

static Point IntersectSegmentLine(const Point& p1, const Point& p2,
                                  long double A, long double B,
                                  long double C) {
  long double x1 = p1.x;
  long double y1 = p1.y;
  long double x2 = p2.x;
  long double y2 = p2.y;
  long double dx = x2 - x1;
  long double dy = y2 - y1;
  long double denom = A * dx + B * dy;
  Point result;
  if (fabsl(denom) < kEps) {
    result = p1;
  } else {
    long double t = (C - (A * x1 + B * y1)) / denom;
    if (t < 0) t = 0;
    if (t > 1) t = 1;
    result.x = x1 + t * dx;
    result.y = y1 + t * dy;
  }
  return result;
}

static Polygon ClipPolygon(const Polygon& poly,
                           long double A, long double B, long double C) {
  Polygon new_poly;
  const std::vector<Point>& verts = poly.vertices;
  if (verts.empty()) return new_poly;
  size_t m = verts.size();
  for (size_t i = 0; i < m; ++i) {
    const Point& P = verts[i];
    const Point& Q = verts[(i + 1) % m];
    long double valP = A * P.x + B * P.y - C;
    long double valQ = A * Q.x + B * Q.y - C;
    bool insideP = valP <= kEps;
    bool insideQ = valQ <= kEps;
    if (insideP) new_poly.vertices.push_back(P);
    if (insideP ^ insideQ) {
      Point inter = IntersectSegmentLine(P, Q, A, B, C);
      new_poly.vertices.push_back(inter);
    }
  }
  std::vector<Point>& nv = new_poly.vertices;
  std::vector<Point> unique;
  unique.reserve(nv.size());
  for (const Point& pt : nv) {
    if (unique.empty()) {
      unique.push_back(pt);
    } else {
      Point last = unique.back();
      if (fabsl(last.x - pt.x) > kEps || fabsl(last.y - pt.y) > kEps)
        unique.push_back(pt);
    }
  }
  if (unique.size() > 1) {
    Point first = unique.front();
    Point last = unique.back();
    if (fabsl(first.x - last.x) < kEps && fabsl(first.y - last.y) < kEps)
      unique.pop_back();
  }
  new_poly.vertices.swap(unique);
  return new_poly;
}

static Polygon ComputeZoneForPlayer(int i, const std::vector<Point>& players) {
  Polygon poly;
  poly.vertices.reserve(4);
  poly.vertices.push_back({0.0L, 0.0L});
  poly.vertices.push_back({g_field_width, 0.0L});
  poly.vertices.push_back({g_field_width, g_field_height});
  poly.vertices.push_back({0.0L, g_field_height});
  long double xi = players[i].x;
  long double yi = players[i].y;
  for (size_t j = 0; j < players.size(); ++j) {
    if ((int)j == i) continue;
    long double xj = players[j].x;
    long double yj = players[j].y;
    long double A = xj - xi;
    long double B = yj - yi;
    long double C = (xj * xj - xi * xi + yj * yj - yi * yi) * 0.5L;
    poly = ClipPolygon(poly, A, B, C);
    if (poly.vertices.empty()) break;
  }
  return poly;
}

static void PreparePolygonOutput(Polygon& poly) {
  if (poly.vertices.empty()) return;
  long double area2 = 0.0L;
  size_t m = poly.vertices.size();
  for (size_t i = 0; i < m; ++i) {
    size_t j = (i + 1) % m;
    area2 += poly.vertices[i].x * poly.vertices[j].y -
             poly.vertices[j].x * poly.vertices[i].y;
  }
  if (area2 < 0) std::reverse(poly.vertices.begin(), poly.vertices.end());
  long double min_x = poly.vertices[0].x;
  for (size_t i = 1; i < m; ++i)
    if (poly.vertices[i].x < min_x - kEps) min_x = poly.vertices[i].x;
  long double min_y = std::numeric_limits<long double>::max();
  size_t start_index = 0;
  for (size_t i = 0; i < m; ++i)
    if (fabsl(poly.vertices[i].x - min_x) < kEps)
      if (poly.vertices[i].y < min_y - kEps) {
        min_y = poly.vertices[i].y;
        start_index = i;
      }
  std::rotate(poly.vertices.begin(),
              poly.vertices.begin() + start_index, poly.vertices.end());
}

static void ReadInput(std::vector<Point>& players) {
  int n;
  long double field_x, field_y;
  if (!(std::cin >> field_x >> field_y >> n)) return;
  g_field_width = field_x;
  g_field_height = field_y;
  players.resize(n);
  for (int i = 0; i < n; ++i) {
    long double px, py;
    std::cin >> px >> py;
    players[i].x = px;
    players[i].y = py;
  }
}

static void OutputZones(const std::vector<Polygon>& zones) {
  std::cout.setf(std::ios::fixed);
  std::cout << std::setprecision(7);
  for (const Polygon& poly : zones) {
    size_t m = poly.vertices.size();
    std::cout << m;
    for (size_t i = 0; i < m; ++i) {
      long double X = poly.vertices[i].x;
      long double Y = poly.vertices[i].y;
      if (X < 0 && X > -kEps) X = 0;
      if (Y < 0 && Y > -kEps) Y = 0;
      if (X > g_field_width && X < g_field_width + kEps) X = g_field_width;
      if (Y > g_field_height && Y < g_field_height + kEps) Y = g_field_height;
      std::cout << " " << static_cast<double>(X) << " "
                << static_cast<double>(Y);
    }
    std::cout << '\n';
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
