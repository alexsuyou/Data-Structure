#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <limits>

// Include GLM for vector math (Ensure GLM is in your include path)
#include <glm/glm.hpp>

// -----------------------------------------------------------------------------
// Constants & Configurations
// -----------------------------------------------------------------------------
const int NE = 0, NW = 1, SE = 2, SW = 3;

// -----------------------------------------------------------------------------
// Data Structures
// -----------------------------------------------------------------------------

// Represents a reflective line segment in the 2D environment
struct Object {
    glm::vec2 start;
    glm::vec2 end;
};

struct Ray {
    glm::vec2 origin;
    glm::vec2 direction;
};

struct Intersection {
    bool hit;               // true if the ray hits an object
    glm::vec2 hit_point;    // the coordinate of the intersection
    Object* hit_object;     // pointer to the object that was hit (for normal/reflection)
    
    Intersection(bool h) : hit(h), hit_point(0.0f, 0.0f), hit_object(nullptr) {}
};

struct QuadTreeNode {
    glm::vec2 min_bound;
    glm::vec2 max_bound;
    QuadTreeNode* children[4];
    std::vector<Object*> objects;

    QuadTreeNode(glm::vec2 min_b, glm::vec2 max_b) {
        min_bound = min_b;
        max_bound = max_b;
        for (int i = 0; i < 4; ++i) children[i] = nullptr;
    }
    
    // Destructor to clean up allocated children
    ~QuadTreeNode() {
        for (int i = 0; i < 4; ++i) {
            if (children[i] != nullptr) {
                delete children[i];
            }
        }
    }
};

// Used for sorting child nodes during traversal
struct HitNode {
    QuadTreeNode* node;
    float distance; // Entry distance of the ray into the node's bounding box
};

// -----------------------------------------------------------------------------
// Function Prototypes
// -----------------------------------------------------------------------------
float getIntersectParam(glm::vec2 A, glm::vec2 B, glm::vec2 C, glm::vec2 D) {
    glm::vec2 r = B - A;
    glm::vec2 s = D - C;
    
    // 2D Cross product of direction vectors
    float denom = r.x * s.y - r.y * s.x;
    
    // If the denominator is close to 0, the lines are parallel or collinear
    if (std::abs(denom) < 1e-6f) {
        return std::numeric_limits<float>::max();
    }
    glm::vec2 diff = C - A;
    
    // Calculate and return parameters t
    float t = (diff.x * s.y - diff.y * s.x) / denom;
    return t;
}

// Checks if an object intersects with an Axis-Aligned Bounding Box (AABB)
bool isIntersecting(Object* obj, const glm::vec2& min_b, const glm::vec2& max_b) {
    // TODO: Implement exact Box-Line intersection math (e.g., SAT or Line-AABB tests)
    return false; 
}

// Finds the closest intersection between a ray and a list of local objects
Intersection FindClosestIntersection(const Ray& ray, const std::vector<Object*>& objects) {
    Intersection closest(false);
    // TODO: Implement Ray-Line intersection math and find the closest hit
    return closest;
}

// Sorts the 4 child nodes based on how early the ray enters them
std::vector<HitNode> SortChildrenByDistance(const Ray& ray, QuadTreeNode* node) {
    std::vector<HitNode> sorted_children;
    // TODO: Calculate entry distance for each child node's bounding box and sort them
    //       There are only 4 children, do not need to use complex sorting algorithms
    return sorted_children;
}

// Recursively builds the QuadTree
void build_tree(QuadTreeNode* node, int current_depth, int max_capacity, int max_depth) {
    // 1. Termination Check
    if (node->objects.size() <= max_capacity || current_depth >= max_depth) {
        return; 
    }

    // 2. Subdivide Space
    // TODO: Create 4 child nodes with appropriate bounding boxes
    node->children[NE] = new QuadTreeNode(/* ... */); 
    node->children[NW] = new QuadTreeNode(/* ... */); 
    node->children[SE] = new QuadTreeNode(/* ... */); 
    node->children[SW] = new QuadTreeNode(/* ... */); 

    // 3. Distribute Objects to Children
    for (Object* obj : node->objects) {
        for (int i = 0; i < 4; ++i) {
            if (isIntersecting(obj, node->children[i]->min_bound, node->children[i]->max_bound)) {
                node->children[i]->objects.push_back(obj);
            }
        }
    }

    // 4. Clean up and Recurse
    node->objects.clear(); 
    
    for (int i = 0; i < 4; ++i) {
        if (!node->children[i]->objects.empty()) {
            build_tree(node->children[i], current_depth + 1, max_capacity, max_depth);
        }
    }
}

// Traces the ray through the QuadTree to find the first collision
Intersection trace_ray(QuadTreeNode* node, const Ray& ray) {
    // 1. If this is a Leaf Node
    if (node->children[0] == nullptr) {
        return FindClosestIntersection(ray, node->objects);
    }

    // 2. If this is an Internal Node
    std::vector<HitNode> hit_children = SortChildrenByDistance(ray, node);

    // Recursively check children from nearest to farthest
    for (const HitNode& hit : hit_children) {
        Intersection isect = trace_ray(hit.node, ray);
        if (isect.hit) {
            return isect; // Stop traversal and return the closest valid hit
        }
    }

    return Intersection(false); // Ray hit nothing in this branch
}

// -----------------------------------------------------------------------------
// Visualization Function
// -----------------------------------------------------------------------------

// Generates an SVG file illustrating the scene, tree boundaries, and the ray path
void generate_svg(const std::string& filename, const std::vector<Object>& objects, QuadTreeNode* root, const std::vector<glm::vec2>& ray_path) {
    const char OBJ_COLOR[] = "#111111";
    const char RAY_COLOR[] = "#e03a2f";
    const char QUADTREE_COLOR[] = "#aadaff";

    // 1. Decide the world bounds based on objects and the ray path
    glm::vec2 min_w(std::numeric_limits<float>::max());
    glm::vec2 max_w(std::numeric_limits<float>::lowest());
    
    for (const Object& obj : objects) {
        min_w = glm::min(min_w, glm::min(obj.start, obj.end));
        max_w = glm::max(max_w, glm::max(obj.start, obj.end));
    }
    for (const glm::vec2& p : ray_path) {
        min_w = glm::min(min_w, p);
        max_w = glm::max(max_w, p);
    }

    // Add a 5% padding so objects don't touch the image borders
    glm::vec2 span = glm::max(glm::vec2(1e-6f), max_w - min_w);
    min_w -= span * 0.05f;
    max_w += span * 0.05f;
    span = max_w - min_w;

    // 2. Set up SVG canvas dimensions and coordinate transformation
    const float canvas_w = 1000.0f;
    const float canvas_h = 1000.0f;
    const float pixel_padding = 40.0f;
    const float scale = std::min((canvas_w - 2.0f * pixel_padding) / span.x,
                                 (canvas_h - 2.0f * pixel_padding) / span.y);

    // Lambda function to convert world coordinates to SVG canvas coordinates
    auto to_svg = [&](const glm::vec2& p) -> glm::vec2 {
        const float x = pixel_padding + (p.x - min_w.x) * scale;
        // Invert Y-axis because SVG (0,0) is at the top-left corner
        const float y = canvas_h - (pixel_padding + (p.y - min_w.y) * scale); 
        return glm::vec2(x, y);
    };

    // 3. Open the SVG file for writing
    std::ofstream out(filename);
    if (!out.is_open()) return;

    out << std::fixed << std::setprecision(3);
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" "
        << "viewBox=\"0 0 " << canvas_w << " " << canvas_h << "\" "
        << "width=\"" << canvas_w << "\" height=\"" << canvas_h << "\">\n";
    
    // Draw white background
    out << "  <rect x=\"0\" y=\"0\" width=\"100%\" height=\"100%\" fill=\"#ffffff\"/>\n";

    // ==========================================
    // TODO: Draw QuadTree bounding boxes (recursive)
    // Hint: Use the QUADTREE_COLOR and <rect ... />
    // ==========================================

    // ==========================================
    // TODO: Draw all Line Segments (Objects)
    // Hint: Use the to_svg() function, OBJ_COLOR, and <line ... />
    // ==========================================

    // ==========================================
    // TODO: Draw the Ray Path
    // Hint: Use the to_svg() function, RAY_COLOR, and <line ... />
    // ==========================================

    out << "</svg>\n";
    out.close();
}

// -----------------------------------------------------------------------------
// Main Function
// -----------------------------------------------------------------------------

int main() {
    int num_objects; 
    if (!(std::cin >> num_objects)) return 0; // Exit if input is empty

	int quadtree_max_capacity;
    int quadtree_max_depth;
    std::cin >> quadtree_max_capacity;
    std::cin >> quadtree_max_depth;

    std::vector<Object> environment_objects(num_objects);
    
    // Initialize boundaries to extreme opposite values
    glm::vec2 min_world(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    glm::vec2 max_world(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());

    for (int i = 0; i < num_objects; ++i) {
        std::cin >> environment_objects[i].start.x >> environment_objects[i].start.y 
                 >> environment_objects[i].end.x >> environment_objects[i].end.y;
                 
        // Dynamically update the world boundaries based on input coordinates
        min_world.x = std::min({min_world.x, environment_objects[i].start.x, environment_objects[i].end.x});
        min_world.y = std::min({min_world.y, environment_objects[i].start.y, environment_objects[i].end.y});
        
        max_world.x = std::max({max_world.x, environment_objects[i].start.x, environment_objects[i].end.x});
        max_world.y = std::max({max_world.y, environment_objects[i].start.y, environment_objects[i].end.y});
    }

    int max_bounces; 
    std::cin >> max_bounces;

    int num_rays; 
    std::cin >> num_rays;

    int use_visualization; 
    std::cin >> use_visualization;

    // Define the global bounding box for the root of the QuadTree
    QuadTreeNode* root = new QuadTreeNode(min_world, max_world);

    // Insert all objects into the root initially
    for (int i = 0; i < num_objects; ++i) {
        root->objects.push_back(&environment_objects[i]);
    }

    // Build the spatial partition tree
    build_tree(root, 0, quadtree_max_capacity, quadtree_max_depth);

    // Process each ray
    for (int i = 0; i <= num_rays; ++i) {
        Ray current_ray;
        std::cin >> current_ray.origin.x >> current_ray.origin.y 
                 >> current_ray.direction.x >> current_ray.direction.y;

        // Normalize direction vector to ensure correct distance calculations later
        current_ray.direction = glm::normalize(current_ray.direction);

        std::vector<glm::vec2> path;
        path.push_back(current_ray.origin);

        int current_bounce = 0;
        
        while (current_bounce < max_bounces) {
            Intersection hit = trace_ray(root, current_ray);
            
            if (hit.hit) {
                path.push_back(hit.hit_point);
                // TODO: Calculate reflection vector
            } else {
                // Ray goes off into infinity, break the bounce loop
                break;
            }
            current_bounce++;
        }

        // Print output to terminal in required format
        std::cout << "Ray " << i << ": ";
        std::cout << std::fixed << std::setprecision(2);
        for (size_t p = 0; p < path.size(); ++p) {
            std::cout << "(" << path[p].x << "," << path[p].y << ")";
            if (p < path.size() - 1) std::cout << ",";
        }
        std::cout << std::endl;

        // Generate SVG visualization if requested
        if (use_visualization == 1) {
            std::string filename = "ray_path_" + std::to_string(i) + ".svg";
            generate_svg(filename, environment_objects, root, path);
        }
    }

    // Clean up
    delete root;

    return 0;
}