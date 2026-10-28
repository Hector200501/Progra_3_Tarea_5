#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <stack>
#include <stdexcept>

// Clase obligatoria: Node
class Node {
private:
    int id;
    std::vector<int> neighbors;
public:
    explicit Node(int id = 0) : id(id) {}

    int getId() const { return id; }

    const std::vector<int>& getNeighbors() const { return neighbors; }

    void addNeighbor(int neighborId) {
        neighbors.push_back(neighborId);
    }

    bool operator==(const Node& other) const {
        return id == other.id;
    }
};

// Interfaz obligatoria: GraphIterator
class GraphIterator {
public:
    virtual ~GraphIterator() = default;
    virtual bool hasNext() = 0;
    virtual int next() = 0;
};

class Graph;

// Iterador BFS
class BFSIterator : public GraphIterator {
private:
    const Graph& graph;
    std::queue<int> q;
    std::unordered_set<int> visited;
public:
    BFSIterator(const Graph& g, int start);

    bool hasNext() override {
        return !q.empty();
    }

    int next() override;
};

// Iterador DFS
class DFSIterator : public GraphIterator {
private:
    const Graph& graph;
    std::stack<int> s;
    std::unordered_set<int> visited;
public:
    DFSIterator(const Graph& g, int start);

    bool hasNext() override {
        return !s.empty();
    }

    int next() override;
};

// Clase obligatoria: Graph
class Graph {
private:
    // Estructura interna encapsulada
    std::unordered_map<int, Node> nodes;

    friend class BFSIterator;
    friend class DFSIterator;
public:
    void addNode(int id) {
        if (nodes.find(id) == nodes.end()) {
            nodes.emplace(id, Node(id));
        }
    }

    void addEdge(int from, int to) {
        if (nodes.find(from) == nodes.end()) {
            nodes.emplace(from, Node(from));
        }
        if (nodes.find(to) == nodes.end()) {
            nodes.emplace(to, Node(to));
        }
        nodes[from].addNeighbor(to);
    }

    std::unique_ptr<GraphIterator> createBFSIterator(int start) const {
        return std::make_unique<BFSIterator>(*this, start);
    }

    std::unique_ptr<GraphIterator> createDFSIterator(int start) const {
        return std::make_unique<DFSIterator>(*this, start);
    }
};

BFSIterator::BFSIterator(const Graph& g, int start) : graph(g) {
    if (graph.nodes.find(start) != graph.nodes.end()) {
        q.push(start);
        visited.insert(start);
    }
}

int BFSIterator::next() {
    if (q.empty()) {
        throw std::out_of_range("No hay mas elementos en el iterador");
    }
    int current = q.front();
    q.pop();

    auto it = graph.nodes.find(current);
    if (it != graph.nodes.end()) {
        for (int neighbor : it->second.getNeighbors()) {
            if (visited.find(neighbor) == visited.end()) {
                visited.insert(neighbor);
                q.push(neighbor);
            }
        }
    }
    return current;
}

DFSIterator::DFSIterator(const Graph& g, int start) : graph(g) {
    if (graph.nodes.find(start) != graph.nodes.end()) {
        s.push(start);
        visited.insert(start);
    }
}

int DFSIterator::next() {
    if (s.empty()) {
        throw std::out_of_range("No hay mas elementos en el iterador");
    }
    int current = s.top();
    s.pop();

    auto it = graph.nodes.find(current);
    if (it != graph.nodes.end()) {
        const auto& neighbors = it->second.getNeighbors();
        for (auto rit = neighbors.rbegin(); rit != neighbors.rend(); ++rit) {
            if (visited.find(*rit) == visited.end()) {
                visited.insert(*rit);
                s.push(*rit);
            }
        }
    }
    return current;
}

int main() {
    Graph g;
    g.addEdge(1,2);
    g.addEdge(1,3);
    g.addEdge(2,4);
    g.addEdge(4,1); // Ciclo para mostrar que el conjunto visited evita repetir nodos.

    std::unique_ptr<GraphIterator> it = g.createBFSIterator(1);
    std::cout << "BFS: ";
    while(it->hasNext()) {
        std::cout << it->next() << " ";
    }

    it = g.createDFSIterator(1);
    std::cout << "\nDFS: ";
    while(it->hasNext()) {
        std::cout << it->next() << " ";
    }
    std::cout << std::endl;
}
