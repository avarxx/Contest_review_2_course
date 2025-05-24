#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

namespace GeoAnalysis {

template <typename T>
struct Point2D {
    T x;
    T y;

    Point2D<T> operator-(const Point2D<T>& other) const {
        return {x - other.x, y - other.y};
    }

    Point2D<T> operator+(const Point2D<T>& other) const {
        return {x + other.x, y + other.y};
    }
};

template <typename T>
using Polygon = std::vector<Point2D<T>>;

namespace MathUtils {
    template <typename T>
    T crossProduct(const Point2D<T>& a, const Point2D<T>& b) {
        return a.x * b.y - b.x * a.y;
    }

    template <typename T>
    bool isPointOnSegment(const Point2D<T>& segmentStart, const Point2D<T>& segmentEnd, const Point2D<T>& point) {
        if (crossProduct(segmentEnd - segmentStart, point - segmentStart) != 0) 
            return false;

        return (point.x >= std::min(segmentStart.x, segmentEnd.x) && 
                point.x <= std::max(segmentStart.x, segmentEnd.x) && 
                point.y >= std::min(segmentStart.y, segmentEnd.y) && 
                point.y <= std::max(segmentStart.y, segmentEnd.y));
    }
} // namespace MathUtils

class ConvexHullBuilder {
public:
    template <typename T>
    static Polygon<T> computeConvexHull(Polygon<T> points) {
        if (points.size() < 3) return points;

        auto pivotIter = std::min_element(points.begin(), points.end(), 
            [](const Point2D<T>& a, const Point2D<T>& b) {
                return std::tie(a.y, a.x) < std::tie(b.y, b.x);
            });
        std::iter_swap(points.begin(), pivotIter);

        std::sort(points.begin() + 1, points.end(), 
            [base = points[0]](const Point2D<T>& a, const Point2D<T>& b) {
                T cross = MathUtils::crossProduct(a - base, b - base);
                if (cross != 0) return cross > 0;
                return (a.x - base.x)*(a.x - base.x) + (a.y - base.y)*(a.y - base.y) < 
                       (b.x - base.x)*(b.x - base.x) + (b.y - base.y)*(b.y - base.y);
            });

        Polygon<T> hull;
        for (const auto& p : points) {
            while (hull.size() >= 2) {
                Point2D<T> a = hull[hull.size() - 2];
                Point2D<T> b = hull.back();
                if (MathUtils::crossProduct(b - a, p - b) > 0) break;
                hull.pop_back();
            }
            hull.push_back(p);
        }
        return hull;
    }
};

class PointInPolygonTester {
public:
    template <typename T>
    static bool isInsidePolygon(const Point2D<T>& point, const Polygon<T>& polygon) {
        if (polygon.size() < 3) return false;

        for (size_t i = 0; i < polygon.size(); ++i) {
            const Point2D<T>& current = polygon[i];
            const Point2D<T>& next = polygon[(i + 1) % polygon.size()];
            if (MathUtils::isPointOnSegment(current, next, point)) 
                return true;
        }

        int crossings = 0;
        for (size_t i = 0; i < polygon.size(); ++i) {
            const Point2D<T>& current = polygon[i];
            const Point2D<T>& next = polygon[(i + 1) % polygon.size()];
            
            if ((current.y > point.y) != (next.y > point.y)) {
                double xIntersect = current.x + (double)(point.y - current.y) * 
                                    (next.x - current.x) / (next.y - current.y);
                if (point.x < xIntersect) crossings++;
            }
        }
        return (crossings % 2) == 1;
    }
};

class ZoneAnalyzer {
public:
    template <typename T>
    static std::vector<int> calculateZones(const Polygon<T>& landmarks, const std::vector<Point2D<T>>& queries) {
        std::vector<Point2D<T>> remainingPoints = landmarks;
        std::vector<int> zoneResults(queries.size(), 0);
        int currentZoneLevel = 0;

        while (remainingPoints.size() >= 3) {
            Polygon<T> currentHull = ConvexHullBuilder::computeConvexHull(remainingPoints);
            
            for (size_t i = 0; i < queries.size(); ++i) {
                if (zoneResults[i] == currentZoneLevel && 
                    PointInPolygonTester::isInsidePolygon(queries[i], currentHull)) {
                    zoneResults[i] = currentZoneLevel + 1;
                }
            }

            removeHullPoints(remainingPoints, currentHull);
            currentZoneLevel++;
        }

        for (auto& zone : zoneResults) {
            zone = std::max(zone - 1, 0);
        }

        return zoneResults;
    }

private:
    template <typename T>
    static void removeHullPoints(Polygon<T>& points, const Polygon<T>& hull) {
        auto isOnHull = [&hull](const Point2D<T>& p) {
            for (size_t i = 0; i < hull.size(); ++i) {
                const Point2D<T>& a = hull[i];
                const Point2D<T>& b = hull[(i + 1) % hull.size()];
                if (MathUtils::isPointOnSegment(a, b, p)) return true;
            }
            return false;
        };

        points.erase(std::remove_if(points.begin(), points.end(), isOnHull), points.end());
    }
};

} // namespace GeoAnalysis

int main() {
    using namespace GeoAnalysis;

    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int landmarkCount, queryCount;
    std::cin >> landmarkCount;

    Polygon<long long> landmarks(landmarkCount);
    for (auto& p : landmarks) {
        std::cin >> p.x >> p.y;
    }

    std::cin >> queryCount;
    std::vector<Point2D<long long>> queries(queryCount);
    for (auto& q : queries) {
        std::cin >> q.x >> q.y;
    }

    auto zones = ZoneAnalyzer::calculateZones(landmarks, queries);
    for (int zone : zones) {
        std::cout << zone << '\n';
    }

    return 0;
}