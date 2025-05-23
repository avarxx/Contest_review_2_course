#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

namespace GeoAnalysis {

template <typename T>
struct Point2D {
  T x_val;
  T y_val;

  Point2D<T> operator-(const Point2D<T>& other) const {
    return {x_val - other.x_val, y_val - other.y_val};
  }

  Point2D<T> operator+(const Point2D<T>& other) const {
    return {x_val + other.x_val, y_val + other.y_val};
  }
};

template <typename T>
using Polygon = std::vector<Point2D<T>>;

namespace MathUtils {
template <typename T>
T crossProduct(const Point2D<T>& a, const Point2D<T>& b) {
  return a.x_val * b.y_val - b.x_val * a.y_val;
}

template <typename T>
bool areCollinear(const Point2D<T>& a, const Point2D<T>& b,
                  const Point2D<T>& c) {
  return crossProduct(b - a, c - a) == 0;
}

template <typename T>
bool onSegment(const Point2D<T>& segStart, const Point2D<T>& segEnd,
               const Point2D<T>& point) {
  if (!areCollinear(segStart, segEnd, point)) return false;

  T minX = std::min(segStart.x_val, segEnd.x_val);
  T maxX = std::max(segStart.x_val, segEnd.x_val);
  T minY = std::min(segStart.y_val, segEnd.y_val);
  T maxY = std::max(segStart.y_val, segEnd.y_val);

  return point.x_val >= minX && point.x_val <= maxX && point.y_val >= minY &&
         point.y_val <= maxY;
}
}  // namespace MathUtils

class ConvexHullProcessor {
 public:
  template <typename T>
  static Polygon<T> compute(Polygon<T> points) {
    if (points.size() < 3) return points;

    auto pivotIt = findPivotPoint(points);
    std::iter_swap(points.begin(), pivotIt);

    auto comp = [base = points[0]](const Point2D<T>& a, const Point2D<T>& b) {
      auto orient = MathUtils::crossProduct(a - base, b - base);
      if (orient != 0) return orient > 0;
      auto distA = (a - base).x_val * (a - base).x_val +
                   (a - base).y_val * (a - base).y_val;
      auto distB = (b - base).x_val * (b - base).x_val +
                   (b - base).y_val * (b - base).y_val;
      return distA < distB;
    };

    std::sort(points.begin() + 1, points.end(), comp);

    Polygon<T> hull;
    for (const auto& p : points) {
      while (hull.size() >= 2) {
        auto a = hull[hull.size() - 2];
        auto b = hull.back();
        if (MathUtils::crossProduct(b - a, p - b) > 0) break;
        hull.pop_back();
      }
      hull.push_back(p);
    }

    return hull;
  }

 private:
  template <typename T>
  static typename Polygon<T>::iterator findPivotPoint(Polygon<T>& points) {
    auto pivot = points.begin();
    for (auto it = points.begin(); it != points.end(); ++it) {
      if (it->y_val < pivot->y_val ||
          (it->y_val == pivot->y_val && it->x_val < pivot->x_val)) {
        pivot = it;
      }
    }
    return pivot;
  }
};

class PointInPolygonChecker {
 public:
  template <typename T>
  static bool isInside(const Point2D<T>& point, const Polygon<T>& polygon) {
    if (polygon.size() < 3) return false;

    bool onBoundary = false;
    for (size_t i = 0; i < polygon.size(); ++i) {
      const auto& a = polygon[i];
      const auto& b = polygon[(i + 1) % polygon.size()];

      if (MathUtils::onSegment(a, b, point)) {
        onBoundary = true;
        break;
      }
    }
    if (onBoundary) return true;

    int crossings = 0;
    for (size_t i = 0; i < polygon.size(); ++i) {
      const auto& a = polygon[i];
      const auto& b = polygon[(i + 1) % polygon.size()];

      if ((a.y_val > point.y_val) != (b.y_val > point.y_val)) {
        double x_intersect = a.x_val + (double)(point.y_val - a.y_val) *
                                           (b.x_val - a.x_val) /
                                           (b.y_val - a.y_val);

        if (point.x_val < x_intersect) {
          crossings++;
        }
      }
    }

    return crossings % 2 == 1;
  }
};

class ZoneCalculator {
 public:
  template <typename T>
  static std::vector<int> determineZones(
      const Polygon<T>& landmarks, const std::vector<Point2D<T>>& queries) {
    auto remainingPoints = landmarks;
    std::vector<int> zones(queries.size(), 0);
    int currentZone = 0;

    while (remainingPoints.size() >= 3) {
      auto hull = ConvexHullProcessor::compute(remainingPoints);

      for (size_t i = 0; i < queries.size(); ++i) {
        if (zones[i] == currentZone &&
            PointInPolygonChecker::isInside(queries[i], hull)) {
          zones[i] = currentZone + 1;
        }
      }

      removePointsOnHull(remainingPoints, hull);
      currentZone++;
    }

    for (auto& zone : zones) {
      zone = std::max(zone - 1, 0);
    }

    return zones;
  }

 private:
  template <typename T>
  static void removePointsOnHull(Polygon<T>& points, const Polygon<T>& hull) {
    points.erase(std::remove_if(points.begin(), points.end(),
                                [&hull](const Point2D<T>& p) {
                                  for (size_t i = 0; i < hull.size(); ++i) {
                                    const auto& a = hull[i];
                                    const auto& b = hull[(i + 1) % hull.size()];
                                    if (MathUtils::onSegment(a, b, p)) {
                                      return true;
                                    }
                                  }
                                  return false;
                                }),
                 points.end());
  }
};

}  // namespace GeoAnalysis

int main() {
  using namespace GeoAnalysis;

  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int N, K;
  std::cin >> N;

  Polygon<long long> landmarks(N);
  for (auto& p : landmarks) {
    std::cin >> p.x_val >> p.y_val;
  }

  std::cin >> K;
  std::vector<Point2D<long long>> queries(K);
  for (auto& q : queries) {
    std::cin >> q.x_val >> q.y_val;
  }

  auto zones = ZoneCalculator::determineZones(landmarks, queries);
  for (auto zone : zones) {
    std::cout << zone << '\n';
  }

  return 0;
}