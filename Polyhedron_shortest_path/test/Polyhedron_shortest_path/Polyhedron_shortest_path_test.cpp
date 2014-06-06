#include <CGAL/Simple_cartesian.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel_with_sqrt.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polyhedron_shortest_path/Polyhedron_shortest_path_traits.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/IO/Polyhedron_iostream.h>
#include <CGAL/Polyhedron_shortest_path/Internal/Barycentric.h>
#include <CGAL/Polyhedron_shortest_path/Polyhedron_shortest_path.h>
#include <CGAL/boost/graph/graph_traits_Polyhedron_3.h>
#include <CGAL/boost/graph/iterator.h>

#include <CGAL/test_macros.h>
#include <CGAL/test_util.h>
#include <iostream>
#include <sstream>
#include <fstream>

#define CHECK_CLOSE(expected, result, e) (CGAL::abs((expected) - (result)) < (e))

// This is a tetrahedron (edge length 6) with another tetrahedron (edge length 2) glued onto one of its faces, such that it has exactly 3 saddle vertices, specifically vertices 4, 5, and 6.
const char* SADDLE_VERTEX_MESH_OFF =
  "OFF\n"
  "8 12 0\n"
  "0.0 -3.0 0.0\n"
  "-3.0 0.0 -1.73205\n"
  "3.0 0.0 -1.73205\n"
  "0.0 0.0 3.4641\n"
  "-1.0 0.0 0.57735\n"
  "1.0 0.0 0.57735\n"
  "0.0 0.0 -1.1547\n"
  "0.0 3.0 0.0\n"
  "3 0 2 1\n"
  "3 0 3 2\n"
  "3 0 1 3\n"
  "3 1 2 6\n"
  "3 2 5 6\n"
  "3 2 3 5\n"
  "3 3 4 5\n"
  "3 3 1 4\n"
  "3 1 6 4\n"
  "3 4 6 7\n"
  "3 6 5 7\n"
  "3 5 4 7\n";

int main(int argc, char** argv)
{
  CGAL::set_pretty_mode(std::cerr);
  CGAL_TEST_START;
  
  // Test project 3D triangle to 2D
  {
    typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
    typedef CGAL::Polyhedron_3<Kernel> Polyhedron_3;
    typedef CGAL::Polyhedron_shortest_path_default_traits<Kernel, Polyhedron_3> Traits;
    
    Traits traits;
    Traits::Compute_squared_distance_2 compute_squared_distance_2(traits.compute_squared_distance_2_object());
    Traits::Project_triangle_3_to_triangle_2 project_triangle_3_to_triangle_2(traits.project_triangle_3_to_triangle_2_object());
    
    Traits::Triangle_3 sourceTriangle(
      Traits::Point_3(Kernel::FT(0), Kernel::FT(1), Kernel::FT(2)), 
      Traits::Point_3(Kernel::FT(5), Kernel::FT(4), Kernel::FT(3)),
      Traits::Point_3(Kernel::FT(4), Kernel::FT(9), Kernel::FT(16)));

    Traits::Triangle_2 transformedTriangle = project_triangle_3_to_triangle_2(sourceTriangle);
    CGAL_TEST(CHECK_CLOSE(compute_squared_distance_2(sourceTriangle[1], sourceTriangle[0]), compute_squared_distance_2(transformedTriangle[1], transformedTriangle[0]), Kernel::FT(0.0000001)));
    CGAL_TEST(CHECK_CLOSE(compute_squared_distance_2(sourceTriangle[2], sourceTriangle[0]), compute_squared_distance_2(transformedTriangle[2], transformedTriangle[0]), Kernel::FT(0.0000001)));
    CGAL_TEST(CHECK_CLOSE(compute_squared_distance_2(sourceTriangle[2], sourceTriangle[1]), compute_squared_distance_2(transformedTriangle[2], transformedTriangle[1]), Kernel::FT(0.0000001)));
    
  }
  
  // Test barycentric coordinates (simple 2D)
  {
    typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
    typedef CGAL::Polyhedron_3<Kernel> Polyhedron_3;
    typedef CGAL::Polyhedron_shortest_path_default_traits<Kernel, Polyhedron_3> Traits;
    
    Traits traits;
    
    Traits::Construct_barycentric_coordinate_2 construct_barycentric_coordinate_2(traits.construct_barycentric_coordinate_2_object());
    Traits::Construct_triangle_location_2 construct_triangle_location_2(traits.construct_triangle_location_2_object());
    
    // effectively a 1-1 mapping triangle for barycentric coords
    Traits::Triangle_2 simpleTriangle(
      Traits::Point_2(Kernel::FT(0), Kernel::FT(0)), 
      Traits::Point_2(Kernel::FT(1), Kernel::FT(0)),
      Traits::Point_2(Kernel::FT(0), Kernel::FT(1)));

    Traits::Barycentric_coordinate b0 = construct_barycentric_coordinate_2(simpleTriangle, simpleTriangle[0]);
    
    CGAL_TEST(CHECK_CLOSE(Kernel::FT(1.0), b0[0], Kernel::FT(0.000001)));
    CGAL_TEST(CHECK_CLOSE(Kernel::FT(0.0), b0[1], Kernel::FT(0.000001)));
    CGAL_TEST(CHECK_CLOSE(Kernel::FT(0.0), b0[2], Kernel::FT(0.000001)));
    
    size_t outVertex0;
    CGAL::internal::Barycentric_coordinate_type b0Type = CGAL::internal::classify_barycentric_coordinate(b0, outVertex0);
    
    CGAL_TEST(b0Type == CGAL::internal::BARYCENTRIC_COORDINATE_VERTEX);
    CGAL_TEST(outVertex0 == 0);

    Traits::Barycentric_coordinate b1 = construct_barycentric_coordinate_2(simpleTriangle, simpleTriangle[1]);
    
    CGAL_TEST(CHECK_CLOSE(Kernel::FT(0.0), b1[0], Kernel::FT(0.000001)));
    CGAL_TEST(CHECK_CLOSE(Kernel::FT(1.0), b1[1], Kernel::FT(0.000001)));
    CGAL_TEST(CHECK_CLOSE(Kernel::FT(0.0), b1[2], Kernel::FT(0.000001)));
    
    size_t outVertex1;
    CGAL::internal::Barycentric_coordinate_type b1Type = CGAL::internal::classify_barycentric_coordinate(b1, outVertex1);
    
    CGAL_TEST(b1Type == CGAL::internal::BARYCENTRIC_COORDINATE_VERTEX);
    CGAL_TEST(outVertex1 == 1);
    
    Traits::Barycentric_coordinate b2 = construct_barycentric_coordinate_2(simpleTriangle, simpleTriangle[2]);
    
    CGAL_TEST(CHECK_CLOSE(Kernel::FT(0.0), b2[0], Kernel::FT(0.000001)));
    CGAL_TEST(CHECK_CLOSE(Kernel::FT(0.0), b2[1], Kernel::FT(0.000001)));
    CGAL_TEST(CHECK_CLOSE(Kernel::FT(1.0), b2[2], Kernel::FT(0.000001)));
    
    size_t outVertex2;
    CGAL::internal::Barycentric_coordinate_type b2Type = CGAL::internal::classify_barycentric_coordinate(b2, outVertex2);
    
    CGAL_TEST(b2Type == CGAL::internal::BARYCENTRIC_COORDINATE_VERTEX);
    CGAL_TEST(outVertex2 == 2);
    
    Traits::Point_2 location(Kernel::FT(0.3), Kernel::FT(0.2));
    Traits::Barycentric_coordinate bLocation = construct_barycentric_coordinate_2(simpleTriangle, location);
    
    CGAL::internal::Barycentric_coordinate_type bLocationType = CGAL::internal::classify_barycentric_coordinate(bLocation);
    
    CGAL_TEST(bLocationType == CGAL::internal::BARYCENTRIC_COORDINATE_INTERNAL);
    
    CGAL_TEST(CHECK_CLOSE(Kernel::FT(1.0) - location[0] - location[1], bLocation[0], Kernel::FT(0.000001)));
    CGAL_TEST(CHECK_CLOSE(location[0], bLocation[1], Kernel::FT(0.000001)));
    CGAL_TEST(CHECK_CLOSE(location[1], bLocation[2], Kernel::FT(0.000001)));
    
    Traits::Point_2 outLocation = construct_triangle_location_2(simpleTriangle, bLocation);
    CGAL_TEST(CHECK_CLOSE(location[0], outLocation[0], Kernel::FT(0.000001)));
    CGAL_TEST(CHECK_CLOSE(location[1], outLocation[1], Kernel::FT(0.000001)));
  }
  
  // TODO: more complex (and 3D) tests for barycentric coords.
  {
    typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
    typedef CGAL::Polyhedron_3<Kernel> Polyhedron_3;
    typedef CGAL::Polyhedron_shortest_path_default_traits<Kernel, Polyhedron_3> Traits;
    
    Traits traits;
    
    Traits::Construct_barycentric_coordinate_3 construct_barycentric_coordinate_3(traits.construct_barycentric_coordinate_3_object());
    Traits::Construct_triangle_location_3 construct_triangle_location_3(traits.construct_triangle_location_3_object());

    Traits::Triangle_3 quadrantTriangle(
      Traits::Point_3(Kernel::FT(1.0), Kernel::FT(0.0), Kernel::FT(1.0)),
      Traits::Point_3(Kernel::FT(-1.0), Kernel::FT(-1.0), Kernel::FT(-1.0)),
      Traits::Point_3(Kernel::FT(-1.0), Kernel::FT(1.0), Kernel::FT(-1.0)));
      
    Traits::Barycentric_coordinate b0 = construct_barycentric_coordinate_3(quadrantTriangle, quadrantTriangle[0]);
    
    CGAL_TEST(CHECK_CLOSE(Kernel::FT(1.0), b0[0], Kernel::FT(0.000001)));
    CGAL_TEST(CHECK_CLOSE(Kernel::FT(0.0), b0[1], Kernel::FT(0.000001)));
    CGAL_TEST(CHECK_CLOSE(Kernel::FT(0.0), b0[2], Kernel::FT(0.000001)));
    
    Traits::Barycentric_coordinate b1 = construct_barycentric_coordinate_3(quadrantTriangle, quadrantTriangle[1]);
    
    CGAL_TEST(CHECK_CLOSE(Kernel::FT(0.0), b1[0], Kernel::FT(0.000001)));
    CGAL_TEST(CHECK_CLOSE(Kernel::FT(1.0), b1[1], Kernel::FT(0.000001)));
    CGAL_TEST(CHECK_CLOSE(Kernel::FT(0.0), b1[2], Kernel::FT(0.000001)));
    
    Traits::Barycentric_coordinate b2 = construct_barycentric_coordinate_3(quadrantTriangle, quadrantTriangle[2]);
    
    CGAL_TEST(CHECK_CLOSE(Kernel::FT(0.0), b2[0], Kernel::FT(0.000001)));
    CGAL_TEST(CHECK_CLOSE(Kernel::FT(0.0), b2[1], Kernel::FT(0.000001)));
    CGAL_TEST(CHECK_CLOSE(Kernel::FT(1.0), b2[2], Kernel::FT(0.000001)));
    
    Traits::Barycentric_coordinate bOrigin = construct_barycentric_coordinate_3(quadrantTriangle, Traits::Point_3(CGAL::ORIGIN));

    CGAL_TEST(CHECK_CLOSE(Kernel::FT(0.5), bOrigin[0], Kernel::FT(0.000001)));
    CGAL_TEST(CHECK_CLOSE(Kernel::FT(0.25), bOrigin[1], Kernel::FT(0.000001)));
    CGAL_TEST(CHECK_CLOSE(Kernel::FT(0.25), bOrigin[2], Kernel::FT(0.000001)));
    
    Traits::Point_3 originOutAgain = construct_triangle_location_3(quadrantTriangle, bOrigin);
    
    CGAL_TEST(CHECK_CLOSE(Kernel::FT(0.0), originOutAgain[0], Kernel::FT(0.000001)));
    CGAL_TEST(CHECK_CLOSE(Kernel::FT(0.0), originOutAgain[1], Kernel::FT(0.000001)));
    CGAL_TEST(CHECK_CLOSE(Kernel::FT(0.0), originOutAgain[2], Kernel::FT(0.000001)));
    
    Traits::Point_3 pNegative(Kernel::FT(-0.5), Kernel::FT(-0.5), Kernel::FT(-0.5));
    Traits::Barycentric_coordinate bNegative = construct_barycentric_coordinate_3(quadrantTriangle, pNegative);

    CGAL_TEST(CHECK_CLOSE(Kernel::FT(0.25), bNegative[0], Kernel::FT(0.000001)));
    CGAL_TEST(CHECK_CLOSE(Kernel::FT(0.625), bNegative[1], Kernel::FT(0.000001)));
    CGAL_TEST(CHECK_CLOSE(Kernel::FT(0.125), bNegative[2], Kernel::FT(0.000001)));
    
    Traits::Point_3 negativeOutAgain = construct_triangle_location_3(quadrantTriangle, bNegative);
    
    CGAL_TEST(CHECK_CLOSE(Kernel::FT(-0.5), negativeOutAgain[0], Kernel::FT(0.000001)));
    CGAL_TEST(CHECK_CLOSE(Kernel::FT(-0.5), negativeOutAgain[1], Kernel::FT(0.000001)));
    CGAL_TEST(CHECK_CLOSE(Kernel::FT(-0.5), negativeOutAgain[2], Kernel::FT(0.000001)));
  }

  // Test flattening a triangle along an adjacent edge (simple)
  {
    typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
    typedef CGAL::Polyhedron_3<Kernel> Polyhedron_3;
    typedef CGAL::Polyhedron_shortest_path_default_traits<Kernel, Polyhedron_3> Traits;
    
    Traits traits;
    
    Traits::Flatten_triangle_3_along_segment_2 flatten_triangle_3_along_segment_2(traits.flatten_triangle_3_along_segment_2_object());
    Traits::Compute_squared_distance_2 compute_squared_distance_2(traits.compute_squared_distance_2_object());
    Traits::Compute_squared_distance_3 compute_squared_distance_3(traits.compute_squared_distance_3_object());
    Traits::Orientation_2 orientation_2(traits.orientation_2_object());
    
    const Kernel::FT baseDistance(10.0);
    const Kernel::FT height(6.0);
  
    Traits::Triangle_3 sourceTriangle(
      Traits::Point_3(Kernel::FT(-4), Kernel::FT(0), Kernel::FT(3)), 
      Traits::Point_3(Kernel::FT(4), Kernel::FT(0), Kernel::FT(-3)),
      Traits::Point_3(Kernel::FT(0), Kernel::FT(height), Kernel::FT(0)));
      
    Traits::Segment_2 layoutEdge(Traits::Point_2(CGAL::ORIGIN), Traits::Point_2(baseDistance, Kernel::FT(0.0)));
    
    Traits::Triangle_2 flattened = flatten_triangle_3_along_segment_2(sourceTriangle, 0, layoutEdge);

    CGAL_TEST(CHECK_CLOSE(compute_squared_distance_3(sourceTriangle[0], sourceTriangle[1]), Kernel::FT(100.0), Kernel::FT(0.000001)));
    CGAL_TEST(orientation_2(flattened[0], flattened[1], flattened[2]) == CGAL::LEFT_TURN);
    
    CGAL_TEST(CHECK_CLOSE(Kernel::FT(flattened[0].x()), layoutEdge[0].x(), Kernel::FT(0.000001)));
    CGAL_TEST(CHECK_CLOSE(Kernel::FT(flattened[0].y()), layoutEdge[0].y(), Kernel::FT(0.000001)));
    
    CGAL_TEST(CHECK_CLOSE(Kernel::FT(flattened[1].x()), layoutEdge[1].x(), Kernel::FT(0.000001)));
    CGAL_TEST(CHECK_CLOSE(Kernel::FT(flattened[1].y()), layoutEdge[1].y(), Kernel::FT(0.000001)));

    CGAL_TEST(CHECK_CLOSE(Kernel::FT(flattened[2].x()), baseDistance * (-sourceTriangle[0].x() / (sourceTriangle[1].x() - sourceTriangle[0].x())), Kernel::FT(0.000001)));
    CGAL_TEST(CHECK_CLOSE(Kernel::FT(flattened[2].y()), height, Kernel::FT(0.000001)));
    
    CGAL_TEST(CHECK_CLOSE(compute_squared_distance_3(sourceTriangle[0], sourceTriangle[1]), compute_squared_distance_2(flattened[0], flattened[1]), Kernel::FT(0.000001)));
    CGAL_TEST(CHECK_CLOSE(compute_squared_distance_3(sourceTriangle[0], sourceTriangle[2]), compute_squared_distance_2(flattened[0], flattened[2]), Kernel::FT(0.000001)));
    CGAL_TEST(CHECK_CLOSE(compute_squared_distance_3(sourceTriangle[1], sourceTriangle[2]), compute_squared_distance_2(flattened[1], flattened[2]), Kernel::FT(0.000001)));
  }

  // TODO: Test flattening a triangle along an adjacent edge (more complex version)
  {
    typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
    typedef CGAL::Polyhedron_3<Kernel> Polyhedron_3;
    typedef CGAL::Polyhedron_shortest_path_default_traits<Kernel, Polyhedron_3> Traits;
    
    Traits traits;
    
    Traits::Flatten_triangle_3_along_segment_2 flatten_triangle_3_along_segment_2(traits.flatten_triangle_3_along_segment_2_object());
    Traits::Compute_squared_distance_2 compute_squared_distance_2(traits.compute_squared_distance_2_object());
    Traits::Compute_squared_distance_3 compute_squared_distance_3(traits.compute_squared_distance_3_object());
    Traits::Orientation_2 orientation_2(traits.orientation_2_object());
  
    Traits::Triangle_3 sourceTriangle(
      Traits::Point_3(Kernel::FT(-5), Kernel::FT(16), Kernel::FT(-3)), 
      Traits::Point_3(Kernel::FT(5), Kernel::FT(-9), Kernel::FT(7)),
      Traits::Point_3(Kernel::FT(0), Kernel::FT(4), Kernel::FT(5)));
      
    for (size_t edgeIndex = 0; edgeIndex < 3; ++edgeIndex)
    {    
      const Kernel::FT baseDistance = CGAL::sqrt(compute_squared_distance_3(sourceTriangle.vertex(edgeIndex), sourceTriangle.vertex(edgeIndex + 1)));
      const Traits::Vector_2 direction(Kernel::FT(3.0) / Kernel::FT(5.0), Kernel::FT(4.0) / Kernel::FT(5.0));  
      
      Traits::Segment_2 layoutEdge(Traits::Point_2(CGAL::ORIGIN), Traits::Point_2(CGAL::ORIGIN) + direction * baseDistance);

      Traits::Triangle_2 flattened = flatten_triangle_3_along_segment_2(sourceTriangle, edgeIndex, layoutEdge);
      
      CGAL_TEST(orientation_2(flattened.vertex(edgeIndex), flattened.vertex(edgeIndex + 1), flattened.vertex(edgeIndex + 2)) == CGAL::LEFT_TURN);
      
      CGAL_TEST(CHECK_CLOSE(Kernel::FT(flattened.vertex(edgeIndex).x()), layoutEdge[0].x(), Kernel::FT(0.000001)));
      CGAL_TEST(CHECK_CLOSE(Kernel::FT(flattened.vertex(edgeIndex).y()), layoutEdge[0].y(), Kernel::FT(0.000001)));
      
      CGAL_TEST(CHECK_CLOSE(Kernel::FT(flattened.vertex(edgeIndex + 1).x()), layoutEdge[1].x(), Kernel::FT(0.000001)));
      CGAL_TEST(CHECK_CLOSE(Kernel::FT(flattened.vertex(edgeIndex + 1).y()), layoutEdge[1].y(), Kernel::FT(0.000001)));
      
      CGAL_TEST(CHECK_CLOSE(compute_squared_distance_3(sourceTriangle[0], sourceTriangle[1]), compute_squared_distance_2(flattened[0], flattened[1]), Kernel::FT(0.000001)));
      CGAL_TEST(CHECK_CLOSE(compute_squared_distance_3(sourceTriangle[0], sourceTriangle[2]), compute_squared_distance_2(flattened[0], flattened[2]), Kernel::FT(0.000001)));
      CGAL_TEST(CHECK_CLOSE(compute_squared_distance_3(sourceTriangle[1], sourceTriangle[2]), compute_squared_distance_2(flattened[1], flattened[2]), Kernel::FT(0.000001)));
    }
  }
  
  // Test check for 'saddle' vertices on a polyhedron(vertices with negative discrete curvature)
  {
    typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
    typedef CGAL::Polyhedron_3<Kernel> Polyhedron_3;
    typedef CGAL::Polyhedron_shortest_path_default_traits<Kernel, Polyhedron_3> Traits;
    
    typedef boost::graph_traits<Polyhedron_3> GraphTraits;
    typedef GraphTraits::vertex_descriptor vertex_descriptor;
    typedef GraphTraits::vertex_iterator vertex_iterator;
    
    Traits traits;
    Traits::Construct_triangle_location_3 construct_triangle_location_3(traits.construct_triangle_location_3_object());
    Traits::Project_triangle_3_to_triangle_2 project_triangle_3_to_triangle_2(traits.project_triangle_3_to_triangle_2_object());
    Traits::Flatten_triangle_3_along_segment_2 flatten_triangle_3_along_segment_2(traits.flatten_triangle_3_along_segment_2_object());
    Traits::Orientation_2 orientation_2(traits.orientation_2_object());
    
    Traits::Is_saddle_vertex is_saddle_vertex(traits.is_saddle_vertex_object());

    std::istringstream iss(SADDLE_VERTEX_MESH_OFF);
    
    Polyhedron_3 P;
    
    iss >> P;
    
    size_t currentVertexIndex = 0;
    
    vertex_iterator currentVertex;
    vertex_iterator endVertex;

    for (boost::tie(currentVertex, endVertex) = boost::vertices(P); currentVertex != endVertex; ++currentVertex)
    {
      if (currentVertexIndex <= 3 || currentVertexIndex == 7)
      {
        CGAL_TEST(!is_saddle_vertex(*currentVertex, P));
      }
      else
      {
        CGAL_TEST(is_saddle_vertex(*currentVertex, P));
      }

      ++currentVertexIndex;
    }
  }
 
  // very simple test of the algorithm on a regular tetrahedron
  {
    typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
    typedef CGAL::Polyhedron_3<Kernel> Polyhedron_3;
    typedef CGAL::Polyhedron_shortest_path_default_traits<Kernel, Polyhedron_3> Traits;
    typedef Traits::Barycentric_coordinate Barycentric_coordinate;
    typedef Traits::FT FT;
    typedef boost::graph_traits<Polyhedron_3> GraphTraits;
    typedef GraphTraits::vertex_descriptor vertex_descriptor;
    typedef GraphTraits::vertex_iterator vertex_iterator;
    typedef GraphTraits::halfedge_descriptor halfedge_descriptor;
    typedef GraphTraits::halfedge_iterator halfedge_iterator;
    typedef GraphTraits::face_descriptor face_descriptor;
    typedef GraphTraits::face_iterator face_iterator;
    typedef CGAL::Polyhedron_shortest_path<Traits> Polyhedron_shortest_path;
    
    Traits traits;
    
    Polyhedron_shortest_path shortestPaths(traits);
    
    Polyhedron_3 P;
    
    CGAL::test::make_regular_tetrahedron(P);
    
    Barycentric_coordinate b(FT(1.0) / FT(3.0), FT(1.0) / FT(3.0), FT(1.0) / FT(3.0));
    
    face_iterator startFace;
    face_iterator endFace;
    
    boost::tie(startFace,endFace) = CGAL::faces(P);
    
    face_descriptor firstFace = *startFace;
    
    //shortestPaths.m_debugOutput = true;
    shortestPaths.compute_shortest_paths(P, firstFace, b);
    
    vertex_iterator currentVertex;
    vertex_iterator endVertex;
    
    size_t vertexIndex = 0;
    
    Kernel::FT sideLength = Kernel::FT(2.0);
    Kernel::FT halfSideLength = sideLength / Kernel::FT(2.0);
    Kernel::FT triangleHeight = CGAL::sqrt((sideLength*sideLength) - (halfSideLength*halfSideLength));

    for (boost::tie(currentVertex, endVertex) = CGAL::vertices(P); currentVertex != endVertex; ++currentVertex)
    {
      if (vertexIndex == 0)
      {
        CGAL_TEST(CHECK_CLOSE(shortestPaths.shortest_distance_to_vertex(*currentVertex), Kernel::FT((triangleHeight * Kernel::FT(4.0)) / Kernel::FT(3.0)), Kernel::FT(0.000001)));
      }
      else
      {
        CGAL_TEST(CHECK_CLOSE(shortestPaths.shortest_distance_to_vertex(*currentVertex), Kernel::FT((triangleHeight * Kernel::FT(2.0)) / Kernel::FT(3.0)), Kernel::FT(0.000001)));
      }
      
      ++vertexIndex;
    }
  
  }
  
  // Slightly more complicated test, originating from a vertex, and involving saddle vertices
  {
    typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
    typedef CGAL::Polyhedron_3<Kernel> Polyhedron_3;
    typedef CGAL::Polyhedron_shortest_path_default_traits<Kernel, Polyhedron_3> Traits;
    typedef Traits::Barycentric_coordinate Barycentric_coordinate;
    typedef Traits::FT FT;
    typedef Traits::Point_3 Point_3;
    typedef Traits::Point_2 Point_2;
    typedef Traits::Triangle_3 Triangle_3;
    typedef Traits::Triangle_2 Triangle_2;
    typedef Traits::Segment_2 Segment_2;
    typedef boost::graph_traits<Polyhedron_3> GraphTraits;
    typedef GraphTraits::vertex_descriptor vertex_descriptor;
    typedef GraphTraits::vertex_iterator vertex_iterator;
    typedef GraphTraits::halfedge_descriptor halfedge_descriptor;
    typedef GraphTraits::halfedge_iterator halfedge_iterator;
    typedef GraphTraits::face_descriptor face_descriptor;
    typedef GraphTraits::face_iterator face_iterator;
    typedef CGAL::Polyhedron_shortest_path<Traits> Polyhedron_shortest_path;
    typedef boost::property_map<Polyhedron_3, CGAL::vertex_point_t>::type VPM;

    Traits traits;
  
    Traits::Compute_squared_distance_3 compute_squared_distance_3(traits.compute_squared_distance_3_object());
    Traits::Compute_squared_distance_2 compute_squared_distance_2(traits.compute_squared_distance_2_object());
    Traits::Flatten_triangle_3_along_segment_2 flatten_triangle_3_along_segment_2(traits.flatten_triangle_3_along_segment_2_object());
    
    std::istringstream iss(SADDLE_VERTEX_MESH_OFF);
    
    Polyhedron_3 P;
    
    iss >> P;

    vertex_iterator startVertex;
    vertex_iterator endVertex;
    boost::tie(startVertex, endVertex) = CGAL::vertices(P);
    
    vertex_iterator currentVertex = startVertex;
    
    ++currentVertex;
    vertex_descriptor rootSearchVertex = *currentVertex;
    
    face_descriptor currentFace = CGAL::face(CGAL::halfedge(rootSearchVertex, P), P);
    size_t vertexIndex = CGAL::test::face_vertex_index(currentFace, rootSearchVertex, P);
    Barycentric_coordinate baryCoord(vertexIndex == 0 ? FT(1.0) : FT(0.0), vertexIndex == 1 ? FT(1.0) : FT(0.0), vertexIndex == 2 ? FT(1.0) : FT(0.0));
    
    Polyhedron_shortest_path shortestPaths(traits);

    //shortestPaths.m_debugOutput = true;
    shortestPaths.compute_shortest_paths(P, currentFace, baryCoord);
    
    VPM vpm = CGAL::get(CGAL::vertex_point, P);
    
    Point_3 vertexLocations[8];
    
    currentVertex = startVertex;
    
    for (size_t i = 0; i < 8; ++i)
    {
      vertexLocations[i] = vpm[*currentVertex];
      ++currentVertex;
    }
    
    FT distanceToBottom = CGAL::sqrt(compute_squared_distance_3(vertexLocations[1], vertexLocations[0]));
    FT largerSideLength = CGAL::sqrt(compute_squared_distance_3(vertexLocations[1], vertexLocations[2]));
    FT distanceToSaddle = CGAL::sqrt(compute_squared_distance_3(vertexLocations[1], vertexLocations[4]));
    FT shorterSideLength = CGAL::sqrt(compute_squared_distance_3(vertexLocations[4], vertexLocations[6]));
    
    Triangle_3 lower(vertexLocations[6], vertexLocations[4], vertexLocations[1]);
    Triangle_3 upper(vertexLocations[4], vertexLocations[6], vertexLocations[7]);
    
    Segment_2 base(Point_2(CGAL::ORIGIN), Point_2(shorterSideLength, FT(0.0)));
    
    Triangle_2 flatLower(flatten_triangle_3_along_segment_2(lower, 0, base));
    Triangle_2 flatUpper(flatten_triangle_3_along_segment_2(upper, 0, Segment_2(base[1], base[0])));
    
    FT distanceToApex = CGAL::sqrt(compute_squared_distance_2(flatLower[2], flatUpper[2]));
    
    FT expectedDistances[8] = 
    {
      distanceToBottom, // a vertex of the larger tetrahedron
      FT(0.0), // the initial vertex
      largerSideLength, // a vertex of the larger tetrahedron
      largerSideLength, // a vertex of the larger tetrahedron
      distanceToSaddle,  // direct line of sight from root
      distanceToSaddle + shorterSideLength,  // around the corner from a pseudo-source (not in direct line of geodesic sight)
      distanceToSaddle, // direct line of sight from root
      distanceToApex,
    };
    
    currentVertex = startVertex;
    
    for (size_t i = 0; i < 8; ++i)
    {
      CGAL_TEST(CHECK_CLOSE(shortestPaths.shortest_distance_to_vertex(*currentVertex), expectedDistances[i], Kernel::FT(0.00001)));
      ++currentVertex;
    }
    
    // Now test with 2 source vertices
    currentVertex = startVertex;
    
    for (size_t i = 0; i < 5; ++i)
    {
      ++currentVertex;
    }
    
    vertex_descriptor rootSearchVertex2 = *currentVertex;
    
    face_descriptor currentFace2 = CGAL::face(CGAL::halfedge(rootSearchVertex2, P), P);
    size_t vertexIndex2 = CGAL::test::face_vertex_index(currentFace2, rootSearchVertex2, P);
    Barycentric_coordinate baryCoord2(vertexIndex2 == 0 ? FT(1.0) : FT(0.0), vertexIndex2 == 1 ? FT(1.0) : FT(0.0), vertexIndex2 == 2 ? FT(1.0) : FT(0.0));
    
    std::vector<Polyhedron_shortest_path::FaceLocationPair> faceLocations;
    faceLocations.push_back(Polyhedron_shortest_path::FaceLocationPair(currentFace, baryCoord));
    faceLocations.push_back(Polyhedron_shortest_path::FaceLocationPair(currentFace2, baryCoord2));
    
    shortestPaths.compute_shortest_paths(P, faceLocations.begin(), faceLocations.end());
    
    FT distanceToApexFrom2 = CGAL::sqrt(compute_squared_distance_3(vertexLocations[5], vertexLocations[7]));

    Triangle_3 lower2(vertexLocations[2], vertexLocations[3], vertexLocations[5]);
    Triangle_3 upper2(vertexLocations[3], vertexLocations[2], vertexLocations[0]);
    
    Segment_2 base2(Point_2(CGAL::ORIGIN), Point_2(largerSideLength, FT(0.0)));
    
    Triangle_2 flatLower2(flatten_triangle_3_along_segment_2(lower2, 0, base2));
    Triangle_2 flatUpper2(flatten_triangle_3_along_segment_2(upper2, 0, Segment_2(base2[1], base2[0])));
    
    FT distanceToBottom2 = CGAL::sqrt(compute_squared_distance_2(flatLower2[2], flatUpper2[2]));
    
    FT expectedDistances2[8] = 
    {
      distanceToBottom2, // a vertex of the larger tetrahedron
      FT(0.0), // an initial vertex
      distanceToSaddle, // a vertex of the larger tetrahedron
      distanceToSaddle, // a vertex of the larger tetrahedron
      shorterSideLength,  // direct line of sight from root
      FT(0.0),  // around the corner from a pseudo-source (not in direct line of geodesic sight)
      shorterSideLength, // direct line of sight from root
      distanceToApexFrom2,
    };
    
    currentVertex = startVertex;
    
    for (size_t i = 0; i < 8; ++i)
    {
      CGAL_TEST(CHECK_CLOSE(shortestPaths.shortest_distance_to_vertex(*currentVertex), expectedDistances2[i], Kernel::FT(0.00001)));
      ++currentVertex;
    }
  }

  CGAL_TEST_END;
}
