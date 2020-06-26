#ifndef GRAPH_H_
#define GRAPH_H_

#include <iostream>
#include <ctime>
#include <queue>
#include <algorithm>
#include <fstream>
#include <set>
#include <string>

using namespace std;
/* 
   This is an undirection, weighted graph. It is represented by an ordered 
   collection of vertices, where each pair of vertices has an associated 
   weight.
   
*/
template <class Weight> class Graph {
private:
    unsigned int noOfVertices;        // Number of vertices in Graph
    unsigned long size;               // Size of the weights array
    Weight* weights;                  // Weights between all directly 
                                      // connected vertices
    // If MST has been calculated for some subsection of an MST
    // then calculating the new MST is faster
    Graph* cachedKMST = NULL;   // Kruskals
    bool changedKMST = true;
    const Weight DISCONNECTED;  // For disconnected vertices
public:
    // Creates a deep copy of another graph
    Graph(const Graph<Weight>& graph): noOfVertices(graph.noOfVertices), size(graph.size), DISCONNECTED(graph.DISCONNECTED){
            weights = new Weight[size];
            for (unsigned long i = 0; i < size; i++) {
               weights[i] = graph.weights[i];
            }
            if (graph.cachedKMST != NULL) {
                  cachedKMST = new Graph<Weight>(*graph.cachedKMST);
            }
            changedKMST = graph.changedKMST;
   }

    // Creates an empty Graph
    Graph(Weight disconnected, unsigned int noOfVertices): DISCONNECTED(disconnected) {
        init(noOfVertices);
   }
    Graph(Weight disconnected, std::string filename):DISCONNECTED(disconnected) {
         // Read through the file
         string line;
         ifstream datafile(filename);
         
         if (datafile.is_open()) {
            unsigned int vertex1, vertex2;
            Weight weight;
            // First line is the number of vertices
            datafile >> noOfVertices;
            init(noOfVertices);
            // The remainder is a connection list
            while (datafile >> vertex1 >> vertex2 >> weight) {
                     setWeight(vertex1, vertex2, weight);
            }
            datafile.close();
         } else {
            throw runtime_error("Could not open file");
         }
   }

    Graph(Weight disconnected, unsigned int noOfVertices, double edgeDensity, 
         Weight min, Weight max, bool fullyConnected):Graph(disconnected, noOfVertices) {
         
         // Calcuate size (number of edges if fully connected) and number of edges
         unsigned long noOfEdges = (unsigned long)(this->size * edgeDensity);

         // Check the input arguments are valid
         if (edgeDensity < 0 || edgeDensity > 1) {
            throw out_of_range("edgeDensity must be between 0 and 1 inclusive");
         }
         if (min < Weight(0) || min > max) {
            throw out_of_range("Input arguments must be 0 <= min <= max");
         }
         // Check if there are enough edges to create fully connected graph
         if (fullyConnected && noOfVertices - 1 > noOfEdges) {
            throw out_of_range("edgeDensity/noOfVertices too low to create a \
                                       fully connected graph");
         }

         // Populate weights
         // Initialize random seed based on current time
         srand(time(NULL));
         // Initially all edges are unallocated
         unsigned long unallocatedEdges = noOfEdges;

         // If requested, make a random fully connected graph
         if (fullyConnected) {
            // Create a shuffled array of vertices
            unsigned int* shuffled = new unsigned int[noOfVertices];
            for (unsigned int i = 0; i < noOfVertices; i++) {
                  shuffled[i] = i;
            }
            random_shuffle(&shuffled[0], &shuffled[noOfVertices]);
            // Go through the array, indicies < i are parted of the connected
            // graph. Randomly choose part of the connected graph to connect
            // to the next unconnected index
            for (unsigned int i = 1; i < noOfVertices; i++) {
                  // Select a random vertex in the connected graph (random index 
                  // from 0 to i (exclusing i)) to connect to the next index.
                  int cIndex = rand() % i;
                  // Add the edge with a random weight within the range
                  unallocatedEdges--;
                  Weight weight = (Weight)(rand() % (max - min + 1) + min);
                  setWeight(shuffled[i], shuffled[cIndex], weight);
            }
            // Remove shuffled array
            delete[] shuffled;
         }
         for (unsigned long i = 0; i < size; i++) {
            // If it doesn't yet exist
            if (weights[i] == DISCONNECTED) {
                  // Decide if this edge will exist
                  if ((unsigned long)rand() * (size - i) < (unsigned long)
                     unallocatedEdges * ((unsigned long)RAND_MAX + 1)) {
                     // Select a random weight within the range
                     unallocatedEdges--;
                     weights[i] = (Weight)(rand() % (max - min + 1) + min);
                  }
                  else {
                     // Unconnected edge
                     weights[i] = Weight(DISCONNECTED);
                  }
            }
         }
      }

    ~Graph(){
         delete[] weights;
         delete cachedKMST;
      }
    // Returns the direct weight between the vertices
   // Returns weight between 2 vertices. <0 indicates they are not connected.
   // The weight between a vertex and itself is 0

    Weight getWeight(unsigned int vertex1, unsigned int vertex2){
         if (vertex1 == vertex2) {
            return Weight(0);
         }
         else {
            return weights[getIndex(vertex1, vertex2)];
         }
      }


   // Returns an array of weights where the value corresponds to the weight
   // between the argument vertex and the arrays index
    // Returns an array of weights between the specified vertex and all others
    Weight* getWeights(unsigned int vertex){
         Weight* weights = new Weight[noOfVertices];
         for (unsigned int i = 0; i < noOfVertices; i++) {
            weights[i] = getWeight(vertex, i);
         }
         return weights;
      }

   // Sets weight between 2 vertices. weight <0 indicates they are not connected
    void setWeight(unsigned int vertex1, unsigned int vertex2, Weight weight){
         if (vertex1 == vertex2) {
            throw out_of_range("setWeight cannot be called with vertex1 == vertex2");
         }
         else {
            weights[getIndex(vertex1, vertex2)] = weight;
            if (cachedKMST != NULL) {
                  cachedKMST->setWeight(vertex1, vertex2, weight);
                  changedKMST = true;
            }
         }
      }

    /* Returns true if any vertex1s are connected to vertex2s, directly or indirectly
       Set cache to true if this method will be called multiple times on the same
       (possibly changing) large graph to improve performance
    */
    bool isConnected(unsigned int vertex1, unsigned int vertex2, bool cache = false){
         // If this method is to be performed on a small changing graph
         // use the MST to improve performance on large graphs.
         Graph* graph = cache ? getKruskalsMinimumSpanningTree() : this;
         set<unsigned int> pending;
         set<unsigned int> checked;
         pending.insert(vertex1);
         while (!pending.empty()) {
            set<unsigned int>::iterator itr = pending.begin();
            checked.insert(*itr);
            Weight* weights = graph->getWeights(*itr);
            pending.erase(itr);
            for(unsigned int vertex = 0; vertex < noOfVertices; vertex++) {
                  if (weights[vertex] != DISCONNECTED) {
                     if (vertex == vertex2) {
                        return true;
                     }
                     if (checked.find(vertex) == checked.end()) {
                        pending.insert(vertex);
                     }
                  }
            }
         }
         return false;
      }
    // Returns a new graph which the a minimum spanning tree of this graph using
    // Kruskal's algorithm
    Graph* getKruskalsMinimumSpanningTree(){
         if (!changedKMST) {
            return cachedKMST;
         }
         Graph<Weight>* currentMST = (cachedKMST == NULL) ? this : cachedKMST;
         // Create a priority queue of edges
         priority_queue<Edge> edges;
         for (unsigned int i = 0; i < noOfVertices; i++) {
            for (unsigned int j = i + 1; j < noOfVertices; j++) {
                  Weight weight = currentMST->getWeight(i, j);
                  if (weight != DISCONNECTED) {
                     edges.push(*(new Edge(i, j, weight)));
                  }
            }
         }
         // Create an array where index corresponds to the vertex and the
         // value corresponds to the tree (initially there is 1 tree for each
         // vertex)
         unsigned int* trees = new unsigned int[noOfVertices];
         for (unsigned int i = 0; i < noOfVertices; i++) {
            trees[i] = i;
         }
         unsigned int noOfDiffTrees = noOfVertices;
         // Create empty graph copy of this one
         Graph<Weight>* graph = new Graph<Weight>(DISCONNECTED, noOfVertices);
         // Go through the edges priority queue
         while (!edges.empty() && noOfDiffTrees > 1) {
            // Get the next the edge
            Edge edge = edges.top();
            // If this edge connects 2 different trees
            unsigned int tree1 = trees[edge.vertex1];
            unsigned int tree2 = trees[edge.vertex2];
            if (tree1 != tree2) {
                  // Add it to the new Graph
                  graph->setWeight(edge.vertex1, edge.vertex2, edge.weight);
                  // Join the separate trees
                  for (unsigned int i = 0; i < noOfVertices; i++) {
                     if (trees[i] == tree2) {
                        trees[i] = tree1;
                     }
                  }
                  noOfDiffTrees--;
            }
            // Remove the edge from the priority queue
            edges.pop();
         }
         // Clean up
         delete[] trees;
         while (!edges.empty()){
            edges.pop();
         }
         // Return minimum spanning tree
         cachedKMST = graph;
         changedKMST = false;
         return graph;
      }

private:
    // Initializes as empty graph of required size
    void init(unsigned int noOfVertices){
         this->noOfVertices = noOfVertices;
         this->size = ((unsigned long)noOfVertices * (noOfVertices - 1)) / 2;
         // Setup weights array as completely unconnected
         this->weights = new Weight[size];
         for (unsigned long i = 0; i < size; i++) {
            weights[i] = DISCONNECTED;
         }
      }
    // Returns the index in weights array for the connection between the 
    // specified vertices
    unsigned long getIndex(unsigned int vertex1, unsigned int vertex2){
         if (vertex1 == vertex2) {
            throw out_of_range("getIndex cannot be called with vertex1 == vertex2");
         }
         else {
            // This is undirectional graph so can assume vertex1 < vertex2 or make 
            // it so.
            if (vertex1 > vertex2) {
                  unsigned int temp = vertex1;
                  vertex1 = vertex2;
                  vertex2 = temp;
            }
            // Return the index in weights array
            return (size - 1) - ((unsigned long)(noOfVertices - 1
                  - vertex1) * ((noOfVertices - vertex1)) / 2) + (vertex2 - vertex1);
         }
      }

    /* This Edge's call is used in the priority queues in Kruskals and Prims
       minimum spanning tree methods: getKruskalsMinimumSpanningTree,
       getPrimsMinimumSpanningTree.
    */
    class Edge {
    public:
        unsigned int vertex1;
        unsigned int vertex2;
        Weight weight;
        Edge(unsigned int vertex1, unsigned int vertex2, Weight weight):
            vertex1(vertex1), vertex2(vertex2), weight(weight){}
        bool operator<(const Edge& rhs) const{
               return this->weight > rhs.weight;
        }
    };
};

#endif  // GRAPH_H_