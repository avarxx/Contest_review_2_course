#include <algorithm>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <limits>
#include <vector>

struct Point {
  long double x;
  long double y;
};

constexpr long double kSnapshotSeconds = 60.0L;

std::vector<Point> ReadVertices(std::size_t count) {
  std::vector<Point> vertices;
  vertices.reserve(count);
  for (std::size_t i = 0; i < count; ++i) {
    long double x, y;
    std::cin >> x >> y;
    vertices.push_back({x, y});
  }
  return vertices;
}

std::size_t FindLowestVertexIndex(const std::vector<Point>& hull) {
  std::size_t best = 0;
  for (std::size_t i = 1; i < hull.size(); ++i) {
    if (hull[i].y < hull[best].y ||
        (hull[i].y == hull[best].y && hull[i].x < hull[best].x)) {
      best = i;
    }
  }
  return best;
}

std::vector<Point> MinkowskiSum(const std::vector<Point>& p,
                                const std::vector<Point>& q) {
  if (p.empty() || q.empty()) {
    return {};
  }
  std::size_t n = p.size();
  std::size_t m = q.size();
  std::size_t i = FindLowestVertexIndex(p);
  std::size_t j = FindLowestVertexIndex(q);
  std::vector<Point> sum;
  sum.reserve(n + m);

  auto NextIndex = [](std::size_t idx, std::size_t sz) {
    return idx + 1 < sz ? idx + 1 : 0;
  };

  std::size_t start_i = i;
  std::size_t start_j = j;
  do {
    const Point& pi = p[i];
    const Point& qj = q[j];
    sum.push_back({pi.x + qj.x, pi.y + qj.y});

    Point edge_p = {
      p[NextIndex(i, n)].x - pi.x,
      p[NextIndex(i, n)].y - pi.y
    };
    Point edge_q = {
      q[NextIndex(j, m)].x - qj.x,
      q[NextIndex(j, m)].y - qj.y
    };
    long double cross = edge_p.x * edge_q.y - edge_p.y * edge_q.x;
    if (cross > 0) {
      i = NextIndex(i, n);
    } else if (cross < 0) {
      j = NextIndex(j, m);
    } else {
      i = NextIndex(i, n);
      j = NextIndex(j, m);
    }
  } while (i != start_i || j != start_j);

  return sum;
}

long double ComputeMinimalTranslationDistance(
    const std::vector<Point>& difference) {
  long double minimal_distance =
      std::numeric_limits<long double>::infinity();
  std::size_t k = difference.size();
  for (std::size_t idx = 0; idx < k; ++idx) {
    const Point& current = difference[idx];
    const Point& next = difference[(idx + 1) % k];
    long double ex = next.x - current.x;
    long double ey = next.y - current.y;
    long double edge_length = std::hypotl(ex, ey);
    if (edge_length == 0.0L) {
      continue;
    }
    long double cross = ex * current.y - ey * current.x;
    long double distance = std::abs(cross) / edge_length;
    minimal_distance = std::min(minimal_distance, distance);
  }
  return minimal_distance;
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  std::size_t airport_count, cloud_count;
  std::cin >> airport_count >> cloud_count;
  std::vector<Point> airport_vertices = ReadVertices(airport_count);
  std::vector<Point> cloud_vertices = ReadVertices(cloud_count);

  std::vector<Point> negated_cloud;
  negated_cloud.reserve(cloud_count);
  for (const Point& pt : cloud_vertices) {
    negated_cloud.push_back({-pt.x, -pt.y});
  }

  std::vector<Point> minkowski_diff =
      MinkowskiSum(airport_vertices, negated_cloud);
  long double minimal_shift =
      ComputeMinimalTranslationDistance(minkowski_diff);
  long double remaining_time = minimal_shift - kSnapshotSeconds;
  if (remaining_time < 0.0L) {
    remaining_time = 0.0L;
  }

  std::cout << std::fixed << std::setprecision(9) << remaining_time << "\n";
  return 0;
}
