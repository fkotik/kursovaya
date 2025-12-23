#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <chrono>
#include <set>
#include <random>
#include <ctime>
#include <climits>
#include <cstdlib>
#include <numeric> 
#include <iomanip>
#include <string>

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

// Функция для вывода исходного графа
void printOriginalGraph(const Graph& graph) {
    cout << "=== ИСХОДНЫЙ ГРАФ ===\n";
    cout << "Количество вершин: " << graph.V << "\n";
    cout << "Количество рёбер: " << graph.edges.size() << "\n\n";
    cout << "Список рёбер (u -- v, вес):\n";

    // Используем set для хранения уникальных рёбер (чтобы избежать дубликатов в неориентированном графе)
    set<pair<pair<int, int>, int>> uniqueEdges;

    for (const Edge& edge : graph.edges) {
        // Сортируем вершины, чтобы ребро (u,v) и (v,u) считались одинаковыми
        int u = min(edge.u, edge.v);
        int v = max(edge.u, edge.v);
        uniqueEdges.insert({ {u, v}, edge.weight });
    }

    int edgeNumber = 1;
    for (const auto& edge : uniqueEdges) {
        cout << edgeNumber << ". " << edge.first.first << " -- "
            << edge.first.second << " (вес: " << edge.second << ")\n";
        edgeNumber++;
    }
}

// Функция для генерации случайного графа
Graph generateRandomGraph() {
    // Инициализация генератора случайных чисел
    static mt19937 rng(time(nullptr));

    // Параметры генерации (можно менять)
    int minVertices = 10;
    int maxVertices = 100;
    int minEdgesPerVertex = 1; // Минимальное количество рёбер на вершину
    int maxEdgesPerVertex = 4; // Максимальное количество рёбер на вершину
    int minWeight = 1;
    int maxWeight = 100;

    // Генерация случайного количества вершин
    uniform_int_distribution<int> vertexDist(minVertices, maxVertices);
    int V = vertexDist(rng);

    Graph graph(V);

    // Генерация рёбер
    uniform_int_distribution<int> edgeDist(minEdgesPerVertex, maxEdgesPerVertex);
    uniform_int_distribution<int> weightDist(minWeight, maxWeight);
    uniform_int_distribution<int> vertexChoice(0, V - 1);

    // Множество для отслеживания уже добавленных рёбер
    set<pair<int, int>> addedEdges;

    // Генерация рёбер для каждой вершины
    for (int u = 0; u < V; u++) {
        // Случайное количество рёбер для текущей вершины
        int edgesForCurrentVertex = edgeDist(rng);

        for (int i = 0; i < edgesForCurrentVertex; i++) {
            // Выбираем случайную вершину для соединения
            int v = vertexChoice(rng);

            // Пропускаем петли (ребра из вершины в саму себя)
            if (u == v) continue;

            // Создаем упорядоченную пару для проверки
            pair<int, int> edgePair = (u < v) ? make_pair(u, v) : make_pair(v, u);

            // Проверяем, не было ли это ребро уже добавлено
            if (addedEdges.find(edgePair) == addedEdges.end()) {
                int weight = weightDist(rng);
                graph.addEdge(u, v, weight);
                addedEdges.insert(edgePair);
            }
        }
    }

    return graph;
}

int main() {
    setlocale(0, "");
    // Создаём тестовый граф

    Graph graph = generateRandomGraph();

    cout << "Количество вершин: " << graph.V << "\n";
    cout << "Количество рёбер: " << graph.edges.size() << "\n\n";

    printOriginalGraph(graph);


    cout << endl << "=== Сравнение алгоритмов построения MST ===\n\n";


    vector<chrono::microseconds> durations_kruskal;
    vector<chrono::microseconds> durations_boruvka;

    // Тестируем алгоритм Крускала
    auto start = chrono::high_resolution_clock::now();

    vector<Edge> kruskalResult;
    kruskalResult = kruskalMST(graph);

    auto end = chrono::high_resolution_clock::now();

    durations_kruskal.push_back(
        chrono::duration_cast<chrono::microseconds>(end - start)
    );

    printMST(kruskalResult, "Крускала");


    // Тестируем алгоритм Борувки
    start = chrono::high_resolution_clock::now();

    vector<Edge> boruvkaResult;
    boruvkaResult = boruvkaMST(graph);

    end = chrono::high_resolution_clock::now();

    durations_boruvka.push_back(
        chrono::duration_cast<chrono::microseconds>(end - start)
    );

    printMST(boruvkaResult, "Борувки");

    // Средние значения
    auto total_boruvka = accumulate(durations_boruvka.begin(), durations_boruvka.end(), chrono::microseconds(0));

    auto total_kruskal = accumulate(durations_kruskal.begin(), durations_kruskal.end(), chrono::microseconds(0));

    cout << endl << "Время алгоритма Борувки: " << total_boruvka.count() << " мкс" << endl;
    cout << endl << "Время алгоритма Краскала: " << total_kruskal.count() << " мкс" << endl;
        

    

}
