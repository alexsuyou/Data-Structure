/***********************************************************************
* Author: 蘇祐增(I133040010)
* Date: May. 10, 2026
* Purpose: Assignment 5 - QuadTree
***********************************************************************/
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <limits>

// Include GLM for vector math (Ensure GLM is in your include path)
#include "glm/glm.hpp" // need to change  to  #include<glm/glm.hpp> 

// -----------------------------------------------------------------------------
// Constants & Configurations
// -----------------------------------------------------------------------------
const int NE = 0, NW = 1, SE = 2, SW = 3;
const float EPSILON = 1e-6f;

// -----------------------------------------------------------------------------
// Data Structures
// -----------------------------------------------------------------------------

// Represents a reflective line segment in the 2D environment
struct Object {
    glm::vec2 start; // the start point(coordinate) of the reflection line segment
    glm::vec2 end; // the end point(coordinate) of the reflection line segment
};

// Represents the ray in the 2D environment
struct Ray {
    glm::vec2 origin; // the original point(coordinate) of the ray
    glm::vec2 direction; // the vector of the direction that the ray moves
};

// Represents the intersection of the ray and the reflction line segment
struct Intersection {
    bool hit;               // true if the ray hits an object(the reflection line segment)
    glm::vec2 hit_point;    // the coordinate of the intersection
    
	// the pointer is for counting normal vector(法向量) and reflection ray
    Object* hit_object;     // pointer to the object(the reflection line segment) that was hit by ray (for normal/reflection)
    
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
    QuadTreeNode* node; // The specific quadtree node that the ray has entered
    float distance; // Entry distance(Parameter) of the ray into the node's bounding box
};

// -----------------------------------------------------------------------------
// Function Prototypes
// -----------------------------------------------------------------------------

/* 
Using the mathematics formula:

for AB: P(t) = A + t * r
for CD: G(u) = C + u * s
** P(t), G(u) the intersection point
** A, C: the original point of A and B
** t, u: the interserction Parameter
** r, s: the edge vector(和線段平行的向量)

if there is a intersection, P(t) = G(u) =>  A + t * r = C + u * s => (A + t * r) x s = (C + u * s) x s (x: cross production)

then, the parameter t = |(C - A) x s| / |r x s|

*/
float getIntersectParam(glm::vec2 A, glm::vec2 B, glm::vec2 C, glm::vec2 D) {
    glm::vec2 r = B - A; // the edge vector of AB
    glm::vec2 s = D - C; // the edge vector of CD
    
    // 2D Cross Product(外積) of edge vectors
    float denom = r.x * s.y - r.y * s.x;
    
    // If the denominator is close to 0, the lines are parallel(平行) or collinear（共線）
    if (std::abs(denom) < EPSILON) {
        return std::numeric_limits<float>::max(); // return the max value of the float if the statement is correct
    }
    glm::vec2 diff = C - A;  // the difference of C and A
    
    // Calculate and return parameters t
    float t = (diff.x * s.y - diff.y * s.x) / denom;
    
    return t;
}

// Check if both parameter of object(segment line) and box are larger than 0 and less than 1
bool checkIntersectParam(glm::vec2 A, glm::vec2 B, glm::vec2 C, glm::vec2 D){
	float Param_AB, Param_CD;
	Param_AB = getIntersectParam(A, B, C, D); // parameter of line AB (object)
	Param_CD = getIntersectParam(C, D, A, B); // parameter of line CD (edge of the box)
	
	// Determine if the parameter is in range(Notice the EPSILON)
	if((Param_AB >= -EPSILON && Param_AB <= (1.0f + EPSILON)) &&
		(Param_CD >= -EPSILON && Param_CD <= (1.0f + EPSILON))){ 
			return true;
	}

	return false;
}


// Checks if an object(the segment line) intersects with an Axis-Aligned Bounding Box (AABB)

bool isIntersecting(Object* obj, const glm::vec2& min_b, const glm::vec2& max_b) {
	// obj: the pointer of the object(segment line)
    // min_b: the minimun point(coordinate) of the box(the bottom-left corner of the AABB)
    // max_b: the maximum point(coordinate) of the box(the top-right corner of the AABB)

	// Implement exact Box-Line intersection math (e.g., SAT or Line-AABB tests)
	// Use Line-AABB tests here

	// Circumstance 1 : One(or Both) of start and end of object is inside the box
	// In this circumstance, we don't have to find the intersection between object and box
	
	// Determine if the start and end of the object is inside the box or not
	// Notice the EPSILON
	bool isObjStartInside = obj->start.x <= max_b.x + EPSILON && obj->start.x >= min_b.x - EPSILON && 
                        	obj->start.y <= max_b.y + EPSILON && obj->start.y >= min_b.y - EPSILON;

	bool isObjEndInside   = obj->end.x <= max_b.x + EPSILON && obj->end.x >= min_b.x - EPSILON && 
                        	obj->end.y <= max_b.y + EPSILON && obj->end.y >= min_b.y - EPSILON;

	// If one of them is inside the box, the segment line must intersect with the box
	if(isObjStartInside || isObjEndInside){
		return true;
	}
	
	// Circumstance 2 : Object is outside the  the box
	// In this circumstance, we need to find if the object is intersect with the box 
	
	// Step 1: Find the four corner of the box
	glm::vec2 top_left_corner,top_right_corner, bottom_right_corner, bottom_left_corner;
	top_left_corner = glm::vec2(min_b.x, max_b.y);
	top_right_corner = max_b;
	bottom_left_corner = min_b;
	bottom_right_corner = glm::vec2(max_b.x, min_b.y);
	
	// Step 2: Determine if object intersect the box 
	
	bool top_intersection, bottom_intersection, right_intersection, left_intersection;
	top_intersection = checkIntersectParam(obj->start, obj->end, top_left_corner, top_right_corner);
	bottom_intersection = checkIntersectParam(obj->start, obj->end, bottom_right_corner, bottom_left_corner);
	right_intersection = checkIntersectParam(obj->start, obj->end, top_right_corner, bottom_right_corner);
	left_intersection = checkIntersectParam(obj->start, obj->end, top_left_corner, bottom_left_corner);
	
	// If the intersection of one of the edge is true, the object must intersect the box
	if(top_intersection || bottom_intersection || right_intersection || left_intersection){
		return true;
	}
	
    return false;
}


// Finds the closest intersection between a ray and a list of local objects
Intersection FindClosestIntersection(const Ray& ray, const std::vector<Object*>& objects) {
    Intersection closest(false); // Declare a structor of Intersection closest
    
	// Implement Ray-Line intersection math and find the closest hit
	
    int closest_obj_ind = -1; // default the index of the closest object as -1
    float param = std::numeric_limits<float>::max(); // default param as the max limit of float
    glm::vec2 ray_B = ray.origin + ray.direction; // Establish a point B along the ray's direction at t=1.0 as a reference for using getIntersectParam
    
    for (int i = 0; i < objects.size(); i++){
    	
		// Find the parameter of both ray and object
		float ray_param = getIntersectParam(ray.origin, ray_B, objects[i]->start, objects[i]->end);
		float obj_param = getIntersectParam(objects[i]->start, objects[i]->end, ray.origin, ray_B);
		
		// Check if parameter of ray is larger than 0(because ray will not reflect at the original point)
		// Check if parameter of object(the line segment) are larger than 0 and less than 1
		if(ray_param > -1e-6f && (obj_param >= -EPSILON && obj_param <= (1.0f + EPSILON))){
			
			// Find the least param(the closest object), store it in the variable "param" get the index of the object
			if(ray_param < param){
    			closest_obj_ind = i;
    			param = ray_param;
			}
		}
	}
    
    // If closest_obj_ind was updated, store the result in the Intersection object
	if(closest_obj_ind != -1){
		closest.hit = true;
		closest.hit_object = objects[closest_obj_ind];
		closest.hit_point = ray.origin + param * ray.direction;
	}
    return closest;
}

// Sorts the 4 child nodes based on how early the ray enters them
std::vector<HitNode> SortChildrenByDistance(const Ray& ray, QuadTreeNode* node) {
    std::vector<HitNode> sorted_children;
    // Calculate entry distance for each child node's bounding box and sort them
    
    // Establish a point B along the ray's direction at t=1.0 as a reference for using getIntersectParam
    glm::vec2 ray_B = ray.origin + ray.direction;
    
    for(int i = 0; i < 4; i++){
    	
		QuadTreeNode* curr_children = node->children[i];
		if(!curr_children) continue;
		
        // Check if the start point of the ray is in the boundary of the children node
        bool isInside = ray.origin.x <= curr_children->max_bound.x + EPSILON && ray.origin.x >= curr_children->min_bound.x - EPSILON &&
                        ray.origin.y <= curr_children->max_bound.y + EPSILON && ray.origin.y >= curr_children->min_bound.y - EPSILON;
        
        if(isInside) { 
        	// The start point of the ray is already inside this child node's boundary
    		// Set distance to 0.0f to ensure this node is processed first during traversal
            sorted_children.push_back({curr_children, 0.0f});
        } else {
    	
	    	glm::vec2 min_b = curr_children->min_bound; // minimum boundary of the children node
	    	glm::vec2 max_b = curr_children->max_bound; // maximum boundary of the children node
	    	
	    	// Find the four corner of the box of the children node
		    glm::vec2 top_left_corner,top_right_corner, bottom_right_corner, bottom_left_corner;
			top_left_corner = glm::vec2(min_b.x, max_b.y);
			top_right_corner = max_b;
			bottom_left_corner = min_b;
			bottom_right_corner = glm::vec2(max_b.x, min_b.y);
			
			// Find the intersect parameter of the ray 
			float Param_top, Param_bottom, Param_left, Param_right;
			Param_top = getIntersectParam(ray.origin, ray_B, top_left_corner, top_right_corner);
			Param_bottom = getIntersectParam(ray.origin, ray_B, bottom_left_corner, bottom_right_corner);
			Param_left = getIntersectParam(ray.origin, ray_B, top_left_corner, bottom_left_corner);
			Param_right = getIntersectParam(ray.origin, ray_B, top_right_corner, bottom_right_corner);
			
			// Put the valid parameter in the vector
			// valid parameter: larger than -EPISON and smaller than maximum limit of float
			std::vector<float> valid_params;
			for (float p : {Param_top, Param_bottom, Param_left, Param_right}) {
		    	if (p > -EPSILON && p < std::numeric_limits<float>::max()) {
		       		valid_params.push_back(p);
	    		}
			}
			
			// Get the minimum param in the "valid_params" and push it into "sorted_children" if is is not an empty vector
			if (!valid_params.empty()) { 
	    		float Param_min = *std::min_element(valid_params.begin(), valid_params.end());
	    		sorted_children.push_back({curr_children, Param_min});
			}
		}
	}
	
    // Sort the child nodes in ascending order based on their entry distance in the HitNode structure
	// This ensures that the ray checks the closest nodes first during traversal
	
	std::sort(sorted_children.begin(), sorted_children.end(),
		[](const HitNode& nearNode, const HitNode& farNode){
			// Return true if nearNode is closer than farNode to keep it at the front of the vector
			return nearNode.distance < farNode.distance;
		}
	);
    
    return sorted_children;
}

// Recursively builds the QuadTree
void build_tree(QuadTreeNode* node, int current_depth, int max_capacity, int max_depth) {
    // 1. Termination Check
    if (node->objects.size() <= max_capacity || current_depth >= max_depth) {
        return; 
    }

    // 2. Subdivide Space
    // Create 4 child nodes with appropriate bounding boxes
	glm::vec2 mid = (node->min_bound + node->max_bound) / 2.0f;
    node->children[NE] = new QuadTreeNode(mid, node->max_bound); 
    node->children[NW] = new QuadTreeNode(glm::vec2(node->min_bound.x, mid.y), glm::vec2(mid.x, node->max_bound.y)); 
    node->children[SE] = new QuadTreeNode(glm::vec2(mid.x, node->min_bound.y), glm::vec2(node->max_bound.x, mid.y)); 
    node->children[SW] = new QuadTreeNode(node->min_bound, mid); 

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
        if (!node->children[i]->objects.empty()) { // Determine is the object of the childred is empty or not
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
    /*std::vector<HitNode> hit_children = SortChildrenByDistance(ray, node);

    // Recursively check children from nearest to farthest
    for (const HitNode& hit : hit_children) {
        Intersection isect = trace_ray(hit.node, ray);
        if (isect.hit) {
                	
            return isect; // Stop traversal and return the closest valid hit
        }
    }
    return Intersection(false); // Ray hit nothing in this branch
	*/
	
	std::vector<HitNode> hit_children = SortChildrenByDistance(ray, node);

    Intersection best_hit(false); // Get the best(closest) intersection which was found among child nodes 
    
    float best_dist = std::numeric_limits<float>::max(); // Default the best distance as the max limit value of the float

    //Iterate through child nodes in order of proximity (nearest to farthest)
    for (const HitNode& hit : hit_children) {
        
		/* 
		If a hit has already been found and its distance is shorter than the entry distance
    	to the next child node, no other intersections in subsequent nodes can be closer.
		Then, break the loop
     	*/
        if (best_hit.hit && best_dist < hit.distance) {
			
            break; 
        }
        
		// Recursively check children from nearest to farthest
        Intersection isect = trace_ray(hit.node, ray);
        
		if (isect.hit) { 
            // Count the distance of the point of the ray and the hot point
            float dist = glm::distance(ray.origin, isect.hit_point);
            
            // Update best_dist and best_hit if the current distance is nearliest than the record in best_dist
            if (dist < best_dist) {
                best_dist = dist;
                best_hit = isect;
            }
        }
    }
    return best_hit;
	
}

// -----------------------------------------------------------------------------
// Visualization Function
// -----------------------------------------------------------------------------

// Generates an SVG file illustrating the scene, tree boundaries, and the ray path
void generate_svg(const std::string& filename, const std::vector<Object>& objects, QuadTreeNode* root, const std::vector<glm::vec2>& ray_path) {
    const char OBJ_COLOR[] = "#111111";
    const char RAY_COLOR[] = "#e03a2f";
    const char QUADTREE_COLOR[] = "#aadaff";
    const char RAY_START_POINT_COLOR[] = "#1f9d55";
    const char RAY_END_POINT_COLOR[] = "#1f77b4";

    // 1. Decide the world bounds based on objects and the ray path
    // We calculate the axis-aligned bounding box (AABB) that encompasses all objects 
	// and the entire trajectory of the ray to ensure everything is visible in the SVG
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
    glm::vec2 span = glm::max(glm::vec2(EPSILON), max_w - min_w);
    min_w -= span * 0.05f;
    max_w += span * 0.05f;
    span = max_w - min_w; // Total width and height of the scene

    // 2. Set up SVG canvas dimensions and coordinate transformation
    const float canvas_w = 1000.0f;
    const float canvas_h = 1000.0f;
    const float pixel_padding = 40.0f;
    /* 
	Scale factor (pixels per world unit) used to fit the entire scene into the 
	canvas while preserving the aspect ratio. 
 	*/
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


    // Draw QuadTree bounding boxes by using recursive mathed
    auto draw_node_recursive = [&](auto self, QuadTreeNode* node) -> void {
		// 1. Boundry Condition
		//    Return directlly while node is a nullptr
		if(node == nullptr) return; 
		
		// 2. Get svg coordinate
		glm::vec2 svg_min = to_svg(node->min_bound);
		glm::vec2 svg_max = to_svg(node->max_bound);
		
		// 3. Calculate the parameter of the rectangle(for <rect ... />)
		float rect_x = svg_min.x;
		float rect_y = svg_max.y;
		float rect_width = svg_max.x - svg_min.x;
		float rect_height = svg_min.y - svg_max.y;
		
		// 4. output to the file 
		// <rect x="X-coordinate" y="Y-coordinate" width="Width" height="Height" fill="none" stroke="Color code" /
		out << "  <rect x=\"" << rect_x << "\" y=\"" << rect_y << "\" width=\"" << rect_width
		    << "\" height=\"" << rect_height << "\" fill=\"none\" stroke=\"" << QUADTREE_COLOR
			<< "\" />\n" ;
		
		// 5. Recursive
		if(node->children[0] != nullptr){
			for(int i =0; i < 4; i++){
				self(self, node->children[i]);
			}
		}

	};
	
	draw_node_recursive(draw_node_recursive, root);

    // Draw all Line Segments (Objects)
    for(int i = 0; i < objects.size(); i++){
    	glm::vec2 line_start = to_svg(objects[i].start);
    	glm::vec2 line_end = to_svg(objects[i].end);
    	
    	// <line x1="Start X" y1="Start Y" x2="End X" y2="End Y" stroke="Color" stroke-width="Thickness" />
    	out << "  <line x1=\"" << line_start.x << "\" y1=\"" << line_start.y << "\" x2=\"" 
							 << line_end.x << "\" y2=\"" << line_end.y << "\" stroke=\"" 
							 << OBJ_COLOR << "\" stroke-width=\"2\" />\n" ; 
	}

    // Draw the Ray Path
    
    for(int i = 0; i < ray_path.size() - 1; i++){
    	glm::vec2 ray_start = to_svg(ray_path[i]);
    	glm::vec2 ray_end = to_svg(ray_path[i + 1]);
    	
    	// <line x1="Start X" y1="Start Y" x2="End X" y2="End Y" stroke="Color" stroke-width="Thickness" />
    	out << "  <line x1=\"" << ray_start.x << "\" y1=\"" << ray_start.y << "\" x2=\"" 
							 << ray_end.x << "\" y2=\"" << ray_end.y << "\" stroke=\"" 
							 << RAY_COLOR << "\" stroke-width=\"2\" />\n" ; 
	}
    

	// Draw the start and end of Ray Point
	// <circle cx="X-coordinate" cy="Y-coordinate" r="Radius" fill="Color" />
	
	glm::vec2 ray_start_total = to_svg(ray_path[0]);
    glm::vec2 ray_end_total = to_svg(ray_path[ray_path.size() - 1]);
    
	out << "  <circle cx=\"" << ray_start_total.x << "\" cy=\"" << ray_start_total.y << "\" fill=\""
		<< RAY_START_POINT_COLOR << "\" r=\"4\" />\n" ;
	out << "  <circle cx=\"" << ray_end_total.x << "\" cy=\"" << ray_end_total.y << "\" fill=\""
		<< RAY_END_POINT_COLOR << "\" r=\"4\" />\n" ;
	
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

	// Store all reflective line segments for the current test case
    std::vector<Object> environment_objects(num_objects); 
    
    // Initialize boundaries to extreme opposite values
    
    // min_world: set the maximum value of the float
    // mac_world: set the minimum value of the float
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

    int max_bounces; // the maximum times of the reflection
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
    for (int i = 0; i < num_rays; ++i) {
        Ray current_ray;
        std::cin >> current_ray.origin.x >> current_ray.origin.y 
                 >> current_ray.direction.x >> current_ray.direction.y;

        // Normalize direction vector to ensure correct distance calculations later
        current_ray.direction = glm::normalize(current_ray.direction);

		// Store the path of the ray, including its origin and all intersection points
        std::vector<glm::vec2> path;
        
		path.push_back(current_ray.origin);

        int current_bounce = 0;
        
        while (current_bounce < max_bounces) {
            Intersection hit = trace_ray(root, current_ray);
            
            if (hit.hit) {
            	
                path.push_back(hit.hit_point);
                // Calculate reflection vector
                
                // find the edge vector and the normal vector of the segmnet line which is hit by ray
                // normal vector is (-edge_vector.y, edge_vector.x) or (-edge_vector.y, edge_vector.x) (because normal vector dot edge vector is 0)
                // In Physics, the included angle(夾角) between direction of the ray and the normal vector must be na smaller than 90
                glm::vec2 hit_object_edge_vector = glm::vec2((hit.hit_object->end.x - hit.hit_object->start.x), (hit.hit_object->end.y - hit.hit_object->start.y));
                glm::vec2 hit_object_normal_vector = glm::normalize(glm::vec2(-hit_object_edge_vector.y, hit_object_edge_vector.x));
				if(glm::dot(hit_object_normal_vector, current_ray.direction) > 0){	
					// Inverting normal vector if the included angle is smaller than 90
					hit_object_normal_vector = -hit_object_normal_vector;
				}
				
				// Use the vector reflection formula to find the reflection vector of the ray 
				// vector reflection formula: new_ray_direction_vector = current_ray_direction_vector - 2 * (hit_object_normal_vector dot hit_object_edge_vector) * hit_object_normal_vector 
				// glm::vec2 reflection_vector = current_ray.direction - 2.0f * glm::dot(hit_object_normal_vector, current_ray.direction) * hit_object_normal_vector;
				
				glm::vec2 reflection_vector =  glm::reflect(current_ray.direction, hit_object_normal_vector);
				
				// update the point of the current ray, put it in the path vector and update the direction of the current ray
				// Offset the origin slightly along the normal to prevent the ray from immediately re-intersecting the same surface
				current_ray.origin = hit.hit_point + hit_object_normal_vector * 1e-3f; 
				current_ray.direction = reflection_vector;
				
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

