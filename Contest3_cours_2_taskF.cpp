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
    std::array<int, 3> vertex_indices;  
    double normal_x;
    double normal_y;
    double normal_z;
    bool is_active;                     
};

struct Plane3D {
    double a;  
    double b;
    double c;
    double d;
};

static constexpr double kTolerance = 1e-12;

Point3D Subtract(const Point3D& u, const Point3D& v) { 
    return {u.x - v.x, u.y - v.y, u.z - v.z}; 
}

double DotProduct(const Point3D& u, const Point3D& v) { 
    return u.x * v.x + u.y * v.y + u.z * v.z; 
}

Point3D CrossProduct(const Point3D& u, const Point3D& v) {
    return {
        u.y * v.z - u.z * v.y,
        u.z * v.x - u.x * v.z,
        u.x * v.y - u.y * v.x
    };
}

std::vector<Point3D> ReadPoints3D(int count) {
    std::vector<Point3D> points;
    points.reserve(count);
    for (int i = 0; i < count; ++i) {
        Point3D p;
        std::cin >> p.x >> p.y >> p.z;
        points.push_back(p);
    }
    return points;
}

std::vector<Face3D> BuildConvexHull(const std::vector<Point3D>& points) {
    const int num_points = points.size();
    
    int initial_point = 0;
    int second_distinct_point = 1;
    while (second_distinct_point < num_points && 
           DotProduct(Subtract(points[second_distinct_point], points[initial_point]),
                      Subtract(points[second_distinct_point], points[initial_point])) < kTolerance) {
        ++second_distinct_point;
    }
    
    int third_noncolinear_point = second_distinct_point + 1;
    while (third_noncolinear_point < num_points && 
           DotProduct(CrossProduct(Subtract(points[second_distinct_point], points[initial_point]),
                       CrossProduct(Subtract(points[third_noncolinear_point], points[initial_point])) < kTolerance) {
        ++third_noncolinear_point;
    }
    
    int fourth_noncoplanar_point = third_noncolinear_point + 1;
    while (fourth_noncoplanar_point < num_points && 
           std::abs(DotProduct(CrossProduct(Subtract(points[second_distinct_point], points[initial_point]),
                                           Subtract(points[third_noncolinear_point], points[initial_point])),
                                  Subtract(points[fourth_noncoplanar_point], points[initial_point]))) < kTolerance) {
        ++fourth_noncoplanar_point;
    }
    
    std::array<std::array<int, 3>, 4> face_vertices = {{
        {{initial_point, second_distinct_point, third_noncolinear_point}},
        {{initial_point, third_noncolinear_point, fourth_noncoplanar_point}},
        {{second_distinct_point, fourth_noncoplanar_point, third_noncolinear_point}},
        {{second_distinct_point, initial_point, fourth_noncoplanar_point}}
    }};
    
    std::vector<Face3D> convex_hull;
    convex_hull.reserve(num_points * 2);
    for (const auto& face : face_vertices) {
        Point3D normal = CrossProduct(
            Subtract(points[face[1]], points[face[0]]),
            Subtract(points[face[2]], points[face[0]])
        );
        convex_hull.push_back({
            face,
            normal.x,
            normal.y,
            normal.z,
            true
        });
    }
    
    for (int point_index = 0; point_index < num_points; ++point_index) {
        if (point_index == initial_point || 
            point_index == second_distinct_point || 
            point_index == third_noncolinear_point || 
            point_index == fourth_noncoplanar_point) {
            continue;
        }
        
        std::vector<bool> is_visible(convex_hull.size(), false);
        bool has_visible = false;
        for (size_t face_index = 0; face_index < convex_hull.size(); ++face_index) {
            const Face3D& face = convex_hull[face_index];
            if (!face.is_active) continue;
            
            const Point3D base = points[face.vertex_indices[0]];
            const double distance = face.normal_x * (points[point_index].x - base.x) +
                                   face.normal_y * (points[point_index].y - base.y) +
                                   face.normal_z * (points[point_index].z - base.z);
            
            if (distance > kTolerance) {
                is_visible[face_index] = true;
                has_visible = true;
            }
        }
        
        if (!has_visible) continue;
        
        std::vector<std::pair<int, int>> boundary_edges;
        for (size_t face_index = 0; face_index < convex_hull.size(); ++face_index) {
            if (!is_visible[face_index]) continue;
            
            const Face3D& face = convex_hull[face_index];
            for (int edge = 0; edge < 3; ++edge) {
                int u = face.vertex_indices[edge];
                int v = face.vertex_indices[(edge + 1) % 3];
                
                bool is_removed = false;
                for (auto it = boundary_edges.begin(); it != boundary_edges.end(); ++it) {
                    if (it->first == v && it->second == u) {
                        boundary_edges.erase(it);
                        is_removed = true;
                        break;
                    }
                }
                if (!is_removed) {
                    boundary_edges.emplace_back(u, v);
                }
            }
            convex_hull[face_index].is_active = false;
        }
        
        for (const auto& edge : boundary_edges) {
            Point3D normal = CrossProduct(
                Subtract(points[edge.second], points[edge.first]),
                Subtract(points[point_index], points[edge.first])
            );
            convex_hull.push_back({
                {edge.first, edge.second, point_index},
                normal.x,
                normal.y,
                normal.z,
                true
            });
        }
    }
    
    return convex_hull;
}

std::vector<Plane3D> ExtractPlanes(const std::vector<Face3D>& faces, const std::vector<Point3D>& points) {
    std::vector<Plane3D> planes;
    planes.reserve(faces.size());
    for (const Face3D& face : faces) {
        if (!face.is_active) continue;
        
        const double norm_length = std::sqrt(face.normal_x * face.normal_x + 
                                           face.normal_y * face.normal_y + 
                                           face.normal_z * face.normal_z);
        const double inv_norm = 1.0 / norm_length;
        const double a = face.normal_x * inv_norm;
        const double b = face.normal_y * inv_norm;
        const double c = face.normal_z * inv_norm;
        const Point3D base = points[face.vertex_indices[0]];
        const double d = -(a * base.x + b * base.y + c * base.z);
        
        planes.push_back({a, b, c, d});
    }
    return planes;
}

std::vector<double> CalculateDistances(const std::vector<Plane3D>& planes, int query_count) {
    std::vector<double> distances;
    distances.reserve(query_count);
    for (int query_index = 0; query_index < query_count; ++query_index) {
        Point3D query_point;
        std::cin >> query_point.x >> query_point.y >> query_point.z;
        
        double min_distance = std::numeric_limits<double>::max();
        for (const Plane3D& plane : planes) {
            const double value = plane.a * query_point.x + plane.b * query_point.y + 
                                plane.c * query_point.z + plane.d;
            const double distance = std::abs(value);
            if (distance < min_distance) {
                min_distance = distance;
            }
        }
        distances.push_back(min_distance);
    }
    return distances;
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n;
    std::cin >> n;
    const auto points = ReadPoints3D(n);
    const auto convex_hull = BuildConvexHull(points);
    const auto planes = ExtractPlanes(convex_hull, points);
    
    int q;
    std::cin >> q;
    const auto distances = CalculateDistances(planes, q);
    
    std::cout << std::fixed << std::setprecision(4);
    for (const double distance : distances) {
        std::cout << distance << "\n";
    }
    
    return 0;
}