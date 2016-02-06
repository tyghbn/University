#include <omp.h>
#include <sys/time.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>

void MatrixInicialization(double** &matrix, int &n, int &m, int &t);
void RandDataIni(double** matrix, int n, int m, int t);
void Out(double** matrix, int n, int m);
void MatrixOddEvenSort(double** matrix, int n, int m, int t);
void Termination(double** matrix, int n);

int main(void) {
	double** matrix; //указатель на массив массивов
	int n, m, t; // размерность и количество потоков
	double timeStart, timeEnd; // переменные измерения времени

	MatrixInicialization(matrix, n, m, t); // Выделение памяти под матрицу и инициализация переменных

	RandDataIni(matrix, n, m, t); // Заполнение матрицы случайными числами

	/*printf("\nNot sorted matrix: \n");
	Out(matrix, n, m);*/

	timeStart = omp_get_wtime(); // Начало отсчёта	
	MatrixOddEvenSort(matrix, n, m, t); // Сортировка
	timeEnd = omp_get_wtime(); // Конец отсчёта

	/*printf("Sorted matrix: \n");
	Out(matrix, n, m);*/

	printf("Runtime: %f\n", timeEnd - timeStart);
	printf("\n");

	Termination(matrix, n); // Освобождение памяти

	return 0;
}

void MatrixInicialization(double** &matrix, int &n, int &m, int &t)
{
	printf("Enter matrix size: ");
	scanf("%d%d", &n, &m);

	printf("Enter the number of threads: ");
	scanf("%d", &t);

	matrix = new double* [n];
	for (int i = 0; i < n; i++)
		matrix[i] = new double[m];
}

void RandDataIni(double** matrix, int n, int m, int t)
{
	unsigned seed;
	// потокобезопастная генерация псевдослучайных чисел
	#pragma omp parallel private(seed) num_threads(t)
	{
		seed = 25234 + 17 * omp_get_thread_num();
		#pragma omp for
		for (int i = 0; i < n; ++i) {
			for (int j = 0; j < m; ++j) {
				matrix[i][j] = rand_r(&seed) / 100000;
			}
		}
		#pragma omp barrier
	}
}

void Out(double** matrix, int n, int m)
{
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++)
			printf("%f ", matrix[i][j]);
		printf("\n");
	}
	printf("\n");
}

void MatrixOddEvenSort(double** matrix, int n, int m, int t)
{
	double temp;
	int upper_bound;
	if (m % 2 == 0)
		upper_bound = m / 2 - 1;
	else
		upper_bound = m / 2;

	int k, i, j;
	// директива задаёт параллельную секцию и количество потоков
	// каждый поток оперирует своей лоальной копией массива и частно изменяет счётчики циклов
	#pragma omp parallel firstprivate(matrix) private(i, j, k)  num_threads(t)
	{
		for (i = 0; i < n; ++i) {
			for (j = 0; j < m; ++j) {
				if (j % 2 == 0)
					/*распараллеливание циклов сравнения и перестановки значений строки
					без неявной синхронизации*/
					#pragma omp for nowait
					for (int k = 0; k < m / 2; ++k) {
						if (matrix[i][2 * k] < matrix[i][2 * k + 1]) {
							temp = matrix[i][2 * k];
							matrix[i][2 * k] = matrix[i][2 * k + 1];
							matrix[i][2 * k + 1] = temp;
						}
					}
					else
					#pragma omp for nowait
					for (k = 0; k < upper_bound; ++k) {
						if (matrix[i][2 * k + 1] < matrix[i][2 * k + 2]) {
							temp = matrix[i][2 * k + 1];
							matrix[i][2 * k + 1] = matrix[i][2 * k + 2];
							matrix[i][2 * k + 2] = temp;
						}
					}
			}
		}
	}
}

void Termination(double** matrix, int n)
{
	for (int i = 0; i < n; i++)	
		delete [] matrix[i];
	delete [] matrix;
}
