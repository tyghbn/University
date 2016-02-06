#include <omp.h>
#include <sys/time.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <algorithm>

using namespace std;

void GraphInicialization(int** &graph, int &n, int &t);
void RandomDataIni(int** graph, int n, int t);
void FloydRoy(int** graph, int n, int t);
void Out(int** graph, int n);
void Termination(int** graph, int n);

int main(void)
{
	int** graph;
	int n, t;
	double timeStart, timeEnd, resultTime;

	/*выдиление памяти под матрицу смежности
	  инициализация количества вершин и количества потоков*/
	GraphInicialization(graph, n, t);

	RandomDataIni(graph, n, t); // заполнение матрицы псевдослучайными значениями

	/*printf("Adjacency matrix: \n");
	Out(graph, n);*/

	timeStart = omp_get_wtime(); // начало отсчёта
	FloydRoy(graph, n, t);
	timeEnd = omp_get_wtime(); // конец отсчёта

	/*printf("The matrix of the shortest paths: \n");
	Out(graph, n);*/

	printf("Runtime: %f\n", timeEnd - timeStart);
	printf("\n");

	Termination(graph, n); // Освобождение памяти

	return 0;
}

void GraphInicialization(int** &graph, int &n, int &t)
{
	printf("Enter the number of edges: ");
	scanf("%d", &n);

	printf("Enter the number of threads: ");
	scanf("%d", &t);

	graph = new int* [n];
	for (int i = 0; i < n; i++)
		graph[i] = new int[n];
}

void RandomDataIni(int** graph, int n, int t)
{
	unsigned seed;
	// потокобезопастная генерация псевдослучайных чисел
	#pragma omp parallel private(seed) num_threads(t)
	{
		seed = 25234 + 17 * omp_get_thread_num();
		#pragma omp for
		for (int i = 0; i < n; ++i) {
			for (int j = 0; j < n; ++j) {
				graph[i][j] = (int)(rand_r(&seed) / 1000000);
			}
		}
		#pragma omp barrier
	}
}

void FloydRoy(int** graph, int n, int t)
{
	/*обнуляем значения на главной диагонали*/
	for (int i = 0; i < n; i++) 
		graph[i][i] = 0;
	int k, i, j;
	for (k = 0; k < n; ++k)
		// директива задаёт параллельную секцию и количество потоков
		// каждый поток частно изменяет счётчики циклов и имеет общую переменную k -
		// количество вершин
		#pragma omp parallel for private(i,j) shared(k) num_threads(t)
	       	for (i = 0; i < n; ++i)
	           	for (j = 0; j < n; ++j)
	               	if ((graph[i][k] * graph[k][j] != 0) && (i != j))
	                	if ((graph[i][k] + graph[k][j] < graph[i][j]) || (graph[i][j] == 0))
	                        graph[i][j] = graph[i][k] + graph[k][j];
}

void Out(int** graph, int n)
{
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++)
			printf("%d\t", graph[i][j]);
		printf("\n");
	}
	printf("\n");
}

void Termination(int** graph, int n)
{
	for (int i = 0; i < n; i++)	
		delete [] graph[i];
	delete [] graph;
}
