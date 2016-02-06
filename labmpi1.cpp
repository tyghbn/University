#include <mpi.h>
#include <stdio.h>

#define NUM_DIMS 1 // количесто измерений


int main(int argc, char** argv)
{
	int packet = 10; // передаваемый пакет данных
    int rank, size, i, dims[NUM_DIMS], periods[NUM_DIMS], source, dest, reorder = 0; // служебные переменные
    
    MPI_Comm comm_cart; // объявление комуникатора
    MPI_Status status; // статус принимаемого сообщения
    
    MPI_Init(&argc, &argv); // Инициализация подсистемы MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Процесс узнаёт свой ранг
    MPI_Comm_size(MPI_COMM_WORLD, &size); // и свой номер

    //Обнуляем массив dims и заполняем массив periods для топологии "конвейер"
    for(i = 0; i < NUM_DIMS; i++)
    {
		dims[i] = 0;
		periods[i] = 0;
    }
    
    MPI_Dims_create(size, NUM_DIMS, dims); // Функция определения оптимальной конфигурации сетки
    // Создаем топологию "конвейер" с комуникатором comm_cart
    MPI_Cart_create(MPI_COMM_WORLD, NUM_DIMS, dims, periods, reorder, &comm_cart);
    // Каждый процесс находит своих соседей вдоль конвейера, в направлении больших значений рангов
    MPI_Cart_shift(comm_cart, 0, 1, &source, &dest);

	double startTime = MPI_Wtime(); // Начало отсчёта

	if(rank == 0)
    { 
    	/*0й-процесс инициирует передачу пакета вдоль конвейера*/
    	MPI_Send(&packet, 1, MPI_INT, dest, 12, comm_cart);
        printf("From = %d rank = %d  To = %d \n", source, rank, dest);
    }
    else
    { 	
    	//Работа всех остальных процессов
    	MPI_Recv(&packet, 1, MPI_INT, source, 12, comm_cart, &status); // Приём сообщения
		packet++; // Увеличение размера пакета
		MPI_Send(&packet, 1, MPI_INT, dest, 12, comm_cart); // Отправка пакета соседу

		printf("From = %d rank = %d  To = %d \n", source, rank, dest);

		if(rank == size - 1) // Последний процесс выводит время работы
		{
			printf("Runtime = %f\n", MPI_Wtime() - startTime);
		}
    }

	MPI_Comm_free(&comm_cart); // Уничтожение коммуникатора
	MPI_Finalize(); // Освобождаем подсистему MPI
}
