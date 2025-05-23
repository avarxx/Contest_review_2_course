#include <array>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <vector>

struct Point3D {
    double x;
    double y;
    double z;
};

struct Face3D {
    std::array<int, 3> vid;
    double nx;
    double ny;
    double nz;
    bool alive;
};

struct Plane3D {
    double a;
    double b;
    double c;
    double d;
};

static constexpr double kTol = 1e-12;

Point3D Sub(const Point3D& u, const Point3D& v) { return {u.x - v.x, u.y - v.y, u.z - v.z}; }

double Dot(const Point3D& u, const Point3D& v) { return u.x * v.x + u.y * v.y + u.z * v.z; }

Point3D Cross(const Point3D& u, const Point3D& v) {
    return {u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x};
}

std::vector<Point3D> ReadPoints3D(int count) {
    std::vector<Point3D> pts;
    pts.reserve(count);
    for (int i = 0; i < count; ++i) {
        Point3D p;
        std::cin >> p.x >> p.y >> p.z;
        pts.push_back(p);
    }
    return pts;
}

std::vector<Face3D> BuildConvexHull(const std::vector<Point3D>& pts) {
    int n = pts.size();
    int i0 = 0;
    int i1 = 1;
    while (i1 < n && Dot(Sub(pts[i1], pts[i0]), Sub(pts[i1], pts[i0])) < kTol) {
        ++i1;
    }
    int i2 = i1 + 1;
    while (i2 < n && Dot(Cross(Sub(pts[i1], pts[i0]), Sub(pts[i2], pts[i0])),
                         Cross(Sub(pts[i1], pts[i0]), Sub(pts[i2], pts[i0]))) < kTol) {
        ++i2;
    }
    int i3 = i2 + 1;
    while (i3 < n && std::abs(Dot(Cross(Sub(pts[i1], pts[i0]), Sub(pts[i2], pts[i0])),
                                  Sub(pts[i3], pts[i0]))) < kTol) {
        ++i3;
    }
    std::array<std::array<int, 3>, 4> fidx{
        {{{i0, i1, i2}}, {{i0, i2, i3}}, {{i1, i3, i2}}, {{i1, i0, i3}}}};
    std::array<int, 4> opp{{i3, i1, i0, i2}};
    std::vector<Face3D> hull;
    hull.reserve(n * 2);
    for (int fi = 0; fi < 4; ++fi) {
        auto v = fidx[fi];
        Point3D norm = Cross(Sub(pts[v[1]], pts[v[0]]), Sub(pts[v[2]], pts[v[0]]));
        if (Dot(norm, Sub(pts[opp[fi]], pts[v[0]])) > 0) {
            std::swap(v[1], v[2]);
            norm.x = -norm.x;
            norm.y = -norm.y;
            norm.z = -norm.z;
        }
        hull.push_back({v, norm.x, norm.y, norm.z, true});
    }
    for (int pi = 0; pi < n; ++pi) {
        if (pi == i0 || pi == i1 || pi == i2 || pi == i3) {
            continue;
        }
        std::vector<bool> vis(hull.size());
        bool any = false;
        for (size_t hi = 0; hi < hull.size(); ++hi) {
            if (!hull[hi].alive)
                continue;
            Point3D base = pts[hull[hi].vid[0]];
            double dist = hull[hi].nx * (pts[pi].x - base.x) + hull[hi].ny * (pts[pi].y - base.y) +
                          hull[hi].nz * (pts[pi].z - base.z);
            if (dist > kTol) {
                vis[hi] = true;
                any = true;
            }
        }
        if (!any)
            continue;
        std::vector<std::pair<int, int>> boundary;
        for (size_t hi = 0; hi < hull.size(); ++hi) {
            if (!vis[hi])
                continue;
            auto face = hull[hi];
            for (int e = 0; e < 3; ++e) {
                int u = face.vid[e];
                int v = face.vid[(e + 1) % 3];
                bool removed = false;
                for (auto it = boundary.begin(); it != boundary.end(); ++it) {
                    if (it->first == v && it->second == u) {
                        boundary.erase(it);
                        removed = true;
                        break;
                    }
                }
                if (!removed) {
                    boundary.emplace_back(u, v);
                }
            }
            hull[hi].alive = false;
        }
        for (auto edge : boundary) {
            Point3D norm =
                Cross(Sub(pts[edge.second], pts[edge.first]), Sub(pts[pi], pts[edge.first]));
            hull.push_back({{edge.first, edge.second, pi}, norm.x, norm.y, norm.z, true});
        }
    }
    return hull;
}

std::vector<Plane3D> ExtractPlanes(const std::vector<Face3D>& faces,
                                   const std::vector<Point3D>& pts) {
    std::vector<Plane3D> planes;
    planes.reserve(faces.size());
    for (auto const& f : faces) {
        if (!f.alive)
            continue;
        double normLen = std::sqrt(f.nx * f.nx + f.ny * f.ny + f.nz * f.nz);
        double ia = 1.0 / normLen;
        double a = f.nx * ia;
        double b = f.ny * ia;
        double c = f.nz * ia;
        Point3D base = pts[f.vid[0]];
        double d = -(a * base.x + b * base.y + c * base.z);
        planes.push_back({a, b, c, d});
    }
    return planes;
}

std::vector<double> QueryDistances(const std::vector<Plane3D>& planes, int queries) {
    std::vector<double> result;
    result.reserve(queries);
    for (int qi = 0; qi < queries; ++qi) {
        Point3D q;
        std::cin >> q.x >> q.y >> q.z;
        double best = std::numeric_limits<double>::infinity();
        for (auto const& pl : planes) {
            double v = pl.a * q.x + pl.b * q.y + pl.c * q.z + pl.d;
            double d = v < 0 ? -v : v;
            if (d < best)
                best = d;
        }
        result.push_back(best);
    }
    return result;
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n;
    std::cin >> n;
    auto pts = ReadPoints3D(n);
    auto hull = BuildConvexHull(pts);
    auto planes = ExtractPlanes(hull, pts);
    int q;
    std::cin >> q;
    auto distances = QueryDistances(planes, q);
    std::cout << std::fixed << std::setprecision(10);
    for (double d : distances) {
        std::cout << d << "\n";
    }
    return 0;
}
