#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <chrono>

using namespace std;

// Структура для представления ребра графа
struct Edge {
    int u, v, weight;
    Edge(int u, int v, int weight) : u(u), v(v), weight(weight) {}
};

// Структура для представления графа
class Graph {
public:
    int V; // Количество вершин
    vector<Edge> edges; // Список рёбер

    Graph(int V) : V(V) {}

    // Добавление ребра в граф
    void addEdge(int u, int v, int weight) {
        edges.push_back(Edge(u, v, weight));
    }
};

// Класс для системы непересекающихся множеств (DSU)
class DSU {
private:
    vector<int> parent;
    vector<int> rank;

public:
    DSU(int n) {
        parent.resize(n);
        rank.resize(n, 0);
        for (int i = 0; i < n; i++) {
            parent[i] = i;
        }
    }

    // Нахождение корня элемента с path compression
    int find(int x) {
        if (parent[x] != x) {
            parent[x] = find(parent[x]);
        }
        return parent[x];
    }

    // Объединение двух множеств по рангу
    void unite(int x, int y) {
        int rootX = find(x);
        int rootY = find(y);

        if (rootX != rootY) {
            if (rank[rootX] < rank[rootY]) {
                parent[rootX] = rootY;
            }
            else if (rank[rootX] > rank[rootY]) {
                parent[rootY] = rootX;
            }
            else {
                parent[rootY] = rootX;
                rank[rootX]++;
            }
        }
    }

    // Проверка, находятся ли элементы в одном множестве
    bool sameSet(int x, int y) {
        return find(x) == find(y);
    }
};

// Алгоритм Крускала
vector<Edge> kruskalMST(Graph& graph) {
    vector<Edge> result;
    int V = graph.V;

    // Сортируем рёбра по весу
    sort(graph.edges.begin(), graph.edges.end(),
        [](const Edge& a, const Edge& b) {
            return a.weight < b.weight;
        });

    DSU dsu(V);
    int edgesAdded = 0;

    // Проходим по всем рёбрам в отсортированном порядке
    for (const Edge& edge : graph.edges) {
        if (!dsu.sameSet(edge.u, edge.v)) {
            result.push_back(edge);
            dsu.unite(edge.u, edge.v);
            edgesAdded++;

            // MST содержит V-1 ребро
            if (edgesAdded == V - 1) {
                break;
            }
        }
    }

    return result;
}

// Алгоритм Борувки
vector<Edge> boruvkaMST(Graph& graph) {
    vector<Edge> result;
    int V = graph.V;

    DSU dsu(V);
    int components = V;

    // Повторяем, пока не останется одна компонента
    while (components > 1) {
        // Для каждой компоненты храним самое лёгкое исходящее ребро
        vector<Edge> cheapest(V, Edge(-1, -1, numeric_limits<int>::max()));

        // Находим самое лёгкое ребро для каждой компоненты
        for (const Edge& edge : graph.edges) {
            int compU = dsu.find(edge.u);
            int compV = dsu.find(edge.v);

            if (compU != compV) {
                // Обновляем для компоненты compU
                if (edge.weight < cheapest[compU].weight) {
                    cheapest[compU] = edge;
                }
                // Обновляем для компоненты compV
                if (edge.weight < cheapest[compV].weight) {
                    cheapest[compV] = edge;
                }
            }
        }

        // Добавляем найденные рёбра в MST
        for (int i = 0; i < V; i++) {
            if (cheapest[i].weight != numeric_limits<int>::max()) {
                int u = cheapest[i].u;
                int v = cheapest[i].v;

                if (!dsu.sameSet(u, v)) {
                    result.push_back(cheapest[i]);
                    dsu.unite(u, v);
                    components--;
                }
            }
        }
    }

    return result;
}

// Функция для вычисления общего веса MST
int calculateTotalWeight(const vector<Edge>& mst) {
    int total = 0;
    for (const Edge& edge : mst) {
        total += edge.weight;
    }
    return total;
}

// Функция для вывода MST
void printMST(const vector<Edge>& mst, const string& algorithmName) {
    cout << "MST построенный алгоритмом " << algorithmName << ":\n";
    for (const Edge& edge : mst) {
        cout << edge.u << " -- " << edge.v << " (вес: " << edge.weight << ")\n";
    }
    cout << "Общий вес: " << calculateTotalWeight(mst) << "\n\n";
}



int main() {
    setlocale(0, "");
    // Создаём тестовый граф
    Graph graph(6);

    // Добавляем рёбра (u, v, weight)
    graph.addEdge(0, 1, 4);
    graph.addEdge(0, 2, 4);
    graph.addEdge(1, 2, 2);
    graph.addEdge(1, 0, 4);
    graph.addEdge(2, 0, 4);
    graph.addEdge(2, 1, 2);
    graph.addEdge(2, 3, 3);
    graph.addEdge(2, 5, 2);
    graph.addEdge(2, 4, 4);
    graph.addEdge(3, 2, 3);
    graph.addEdge(3, 4, 3);
    graph.addEdge(4, 2, 4);
    graph.addEdge(4, 3, 3);
    graph.addEdge(5, 2, 2);
    graph.addEdge(5, 4, 3);

    cout << "=== Сравнение алгоритмов построения MST ===\n\n";

    // Тестируем алгоритм Крускала
    vector<Edge> kruskalResult;
    kruskalResult = kruskalMST(graph);
    printMST(kruskalResult, "Крускала");

    // Тестируем алгоритм Борувки
    vector<Edge> boruvkaResult;
    boruvkaResult = boruvkaMST(graph);
    printMST(boruvkaResult, "Борувки");
}