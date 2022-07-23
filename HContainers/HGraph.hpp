#pragma once
#include "HContainers.hpp"

namespace HS
{

	// Steven Skiena the Algorithm Design Manual Graph traversal section
	using VertexIndex = int;
	
	// if EdgeData is struct you can make first element float and use it as weight/distance
	template<typename VertexData = int, typename EdgeData = float>
	class Graph
	{
	public:
		struct Edge
		{
			// we can chose unweighted graph, and add road data for example distance between 2 road
			// you can even use empty struct
			union { EdgeData data; float weight; float distance; };
			VertexIndex destination; // indicates destination vertex in Vertices Graph::array
			Edge* next; // linked list structure
			Edge(EdgeData _data, Edge* next, VertexIndex _destination) : data(_data), next(next), destination(_destination) {};
		};
		// todo remove vertex, destroy graph, 

		struct Vertex {
			// maybe we can add key here we can use that key for accessing vertices ie: string, guiid 
			VertexData data; //  this data can be for example: user id, country data etc.
			Edge* edgesRoot = nullptr; // linked list structure
			bool valid = false;
			Vertex() : edgesRoot(nullptr), valid(false) { }
		};

	public:
		~Graph() { Destroy(); }
		Graph() : nVertices(32), nEdges(0) { ConstructVertices(); }

		Graph(int _nVertices) : nVertices(_nVertices), nEdges(0) { ConstructVertices(); }

		void ConnectEdge(VertexIndex a, VertexIndex b, EdgeData edgeData)
		{
			Edge* a_to_b = new Edge(edgeData, vertices[a]->edgesRoot, b);
			Edge* b_to_a = new Edge(edgeData, vertices[b]->edgesRoot, a);
			vertices[a]->edgesRoot = a_to_b;
			vertices[b]->edgesRoot = b_to_a;
			nEdges++;
		}

		// find empty lodation in vertices
		// and generate/place new vertex
		// returns handle (index in vertices array of generated vertex)
		VertexIndex AddVertex(VertexData data)
		{
			VertexIndex index = FindEmptyIndex();
			vertices[index]->data = data;
			vertices[index]->valid = true;
			return index;
		}

		void RemoveVertex(Vertex* vertex)
		{

		}
		// slow approach 
		Vertex* FindVertexByData(VertexData data)
		{
			for (int i = 0; i < nVertices; ++i)
				if (Compare::Equal(vertices[i]->data, data)) return vertices[i];
			return nullptr;
		}
		// this is slower than getting index at vertex creation
		VertexIndex GetVertexIndex(Vertex* vertex)
		{
			VertexIndex index = 0;
			Vertex* currVertex = vertices[0];
			while (currVertex != vertex)
				currVertex = vertices[++index];
			return index;
		}

		void Destroy()
		{

		}
	private:
		void ConstructVertices()
		{
			vertices = (Vertex**)std::malloc(nVertices * sizeof(Vertex*));
			for (int i = 0; i < nVertices; ++i) vertices[i] = new Vertex();
		}
		// find invalid vertex index
		VertexIndex FindEmptyIndex()
		{
			VertexIndex emptyIndex = 0;
			Vertex* currVertex = vertices[0];
			while (currVertex->valid == true)
				currVertex = vertices[++emptyIndex];
			return emptyIndex;
		}
	public:
		Vertex** vertices;
		int nVertices;
		int nEdges;
	};

	// https://www.geeksforgeeks.org/depth-first-search-or-dfs-for-a-graph/
	template<typename VertexData = int, typename EdgeData = float>
	class DFS 
	{
	public:
		using GraphT = Graph<VertexData, EdgeData>;
		using Edge   = typename Graph<VertexData, EdgeData>::Edge;
		using Vertex = typename Graph<VertexData, EdgeData>::Vertex;

		typedef void(*IterateFunc)(Edge*, Vertex*);

		template<class UserClass> struct ClassIterator
		{
			typedef void(*_Func)(UserClass*, Edge*, Vertex*);
			UserClass* userClass;
			_Func func;
			inline ClassIterator(UserClass* _class, _Func _func) : userClass(_class), func(_func) {}
			inline void Invoke(Edge* edge, Vertex* vertex) { func(userClass, edge, vertex); };
		};
	public:
		DFS(GraphT* _graph) : graph(_graph) {}

		template<class UserClass>
		void IterateClass(VertexIndex startIndex, ClassIterator<UserClass> iterator)
		{
			bool* visited = (bool*)malloc(graph->nVertices);
			std::memset(visited, 0, graph->nVertices);
			DFSRec(startIndex, visited, iterator);
			free(visited);
		}

		void Iterate(VertexIndex startIndex, IterateFunc func)
		{
			bool* visited = (bool*)malloc(graph->nVertices);
			std::memset(visited, 0, graph->nVertices);
			DFSRec(startIndex, visited, func);
			free(visited);
		}
	private:

		void DFSRec(VertexIndex v, bool* visited, IterateFunc func)
		{
			visited[v] = true;
			Edge* currEdge = graph->vertices[v]->edgesRoot;

			while (currEdge != nullptr)
			{
				func(currEdge, graph->vertices[v]);
				if (!visited[currEdge->destination])
					DFSRec(currEdge->destination, visited, func);
				currEdge = currEdge->next;
			}
		}
		template<class UserClass>
		void DFSRecClass(VertexIndex v, bool* visited, ClassIterator<UserClass>& iterator)
		{
			visited[v] = true;

			Edge* currEdge = graph->vertices[v]->edgesRoot;

			while (currEdge != nullptr)
			{
				iterator.Invoke(currEdge, graph->vertices[v]);
				if (!visited[currEdge->destination])
					DFSRec(currEdge->destination, visited, iterator);
				currEdge = currEdge->next;
			}
		}
	private:
		Graph<VertexData, EdgeData>* graph;
	};

	// https://www.geeksforgeeks.org/breadth-first-search-or-bfs-for-a-graph/?ref=lbp
	template<typename VertexData = int, typename EdgeData = float>
	class BFS 
	{
	public:
		using GraphT = Graph<VertexData, EdgeData>;
		using Edge   = typename Graph<VertexData, EdgeData>::Edge;
		using Vertex = typename Graph<VertexData, EdgeData>::Vertex;

		typedef void(*IterateFunc)(Edge*, Vertex*);

		template<class UserClass> struct ClassIterator
		{
			typedef void(*_Func)(UserClass*, Edge*, Vertex*);
			UserClass* userClass;
			_Func func;
			inline ClassIterator(UserClass* _class, _Func _func) : userClass(_class), func(_func) {}
			inline void Invoke(Edge* edge, Vertex* vertex) { func(userClass, edge, vertex); };
		};
	public:
		BFS(GraphT* _graph) : graph(_graph) {}

		void Iterate(VertexIndex s, IterateFunc func)
		{
			bool* visited = (bool*)malloc(graph->nVertices);
			std::memset(visited, 0, graph->nVertices);

			Queue<VertexIndex> queue;
			visited[s] = true;
			queue.Enqueue(s);

			while (queue.Any())
			{
				s = queue.Dequeue();
				Edge* currEdge = graph->vertices[s]->edgesRoot;

				while (currEdge != nullptr)
				{
					if (!visited[currEdge->destination])
					{
						func(currEdge, graph->vertices[s]);
						visited[currEdge->destination] = true;
						queue.Enqueue(currEdge->destination);
					}
					currEdge = currEdge->next;
				}
			}
			free(visited);
		}
	private:
		GraphT* graph;
	};

	// prim's algorithm for minimum spanning tree
	template<typename VertexData = int, typename EdgeData = float>
	class MinimumSpanningTreePrim
	{
		using GraphT = Graph<VertexData, EdgeData>;
		using Edge = typename Graph<VertexData, EdgeData>::Edge;
		using Vertex = typename Graph<VertexData, EdgeData>::Vertex;
	public:
		MinimumSpanningTreePrim(GraphT _graph) : graph(_graph) {}
	public:
		Vertex** Solve(VertexIndex v)
		{
			Vertex** result = (Vertex**)std::malloc(100 * sizeof(Vertex*));
			std::memset(result, 0, 100 * sizeof(Vertex*));
			Edge* edge;
			bool* inTree = (bool*)malloc(graph.nVertices);
			std::memset(inTree, 0, graph.nVertices);
			float* distances = (float*)malloc(graph.nVertices * sizeof(float));
			std::fill(distances, distances + graph.nVertices, FLT_MAX);

			distances[v] = 0;
			int index = 0;

			while (inTree[v] == false)
			{
				inTree[v] = true;
				edge = graph.vertices[v]->edgesRoot;
				// add distances to dist list 
				while (edge != nullptr)
				{
					VertexIndex w = edge->destination;
					float weight  = edge->weight;
					if (distances[w] > weight && inTree[w] == false)
					{
						distances[w] = weight;
						result[index++] = graph.vertices[v];
					}
					edge = edge->next;
				}
				v = 1;
				float dist = FLT_MAX;
				for (int i = 0; i <= graph.nVertices; i++)
				{
					// select minimum distance vertex for next iteration
					if (inTree[i] == false && dist > distances[i])
					{
						dist = distances[i];
						v = i;
					}
				}
			}
			free(distances);
			free(inTree);
			return result;
		}
	private:
		GraphT graph;
	};
}


