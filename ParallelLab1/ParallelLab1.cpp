﻿//Андрей, код отсюда, тут еще блок схема есть https://pro-prof.com/forums/topic/simpson-method_cplusplus
#include <iostream>
#include <math.h>
#include <cmath>
#include <vector>
#include <chrono>
#include <Windows.h>
#include <pthread.h>

using namespace std;
typedef double(*pointFunc)(double);

int countOfThr;
double a = 1, b = 100, eps = 0.01, h;//Нижний и верхний пределы интегрирования (a, b), погрешность (eps).
double iMax;
int N;//кол-во вычислений на данной итерации внешнего цикла

struct Thread
{
	int num;
	double k1, k2;
};

double f(double x)
{
	//Sleep(1);//имитация сложных вычислений :D
	return 10-x;
}

void* ThrFunc(void* thrArg)
{
	Thread* thr = (Thread*)thrArg;
	int cicleLen = N / countOfThr;
	for (int i = 1 + cicleLen * (thr->num - 1); i < cicleLen * thr->num; i += 2)
	{
		thr->k1 += f(a + i * h);
		thr->k2 += f(a + (i + 1) * h);
	}
	return NULL;
}
double simpson_integral() 
{
	const double h = (b - a) / N;
	double k1 = 0.0, k2 = 0.0;

	//выделяем память под массив идентификаторов потоков
	pthread_t* threads = (pthread_t*)malloc(countOfThr * sizeof(pthread_t));
	//выделяем память под массив структур для потоков
	Thread* ThrArgs = (Thread*)malloc(countOfThr * sizeof(Thread));

	for (int i = 0; i < countOfThr; i++)
	{
		ThrArgs[i].num = i + 1;
		ThrArgs[i].k1 = 0;
		ThrArgs[i].k2 = 0;
		//запускаем потоки
		pthread_create(&(threads[i]), NULL, ThrFunc, &ThrArgs[i]);
	}
	//ожидаем выполнение всех потоков
	for (int i = 0; i < countOfThr; i++)
		pthread_join(threads[i], NULL);


	for (int i = 0; i < countOfThr; i++)
	{//суммируем
		k1 += ThrArgs[i].k1;
		k2 += ThrArgs[i].k2;
	}

	free(ThrArgs);
	free(threads);

	return h / 3 * (f(a) + 4 * k1 + 2 * k2);
}
int main()
{
	setlocale(LC_ALL, "ru");
	double s1, s;
	N = 1; //начальное число шагов
	cout << "Введите левую границу интегрирования a = ";
	cin >> a;
	cout << "\nВведите правую границу интегрирования b = ";
	cin >> b;
	cout << "\nВведите требуемую точность eps = ";
	cin >> eps;
	cout << "\nВведите кол-во потоков: " ;
	cin >> countOfThr;

	//начало отсчета времени вычисления
	auto startTime = std::chrono::high_resolution_clock::now();

	s1 = simpson_integral(); //первое приближение для интеграла
	do {
		s = s1;     //второе приближение
		N = 2 * N;  //увеличение числа шагов в два раза,
					//т.е. уменьшение значения шага в два раза
		s1 = simpson_integral();

	}   while (fabs(s1 - s) > eps);  //сравнение приближений с заданной точностью

	//окончание отсчета времениcicleLen
	auto endTime = std::chrono::high_resolution_clock::now() - startTime;
	//подсчет времени вычисления
	double elapseTime = std::chrono::duration<double>(endTime).count();

	cout << "Значение интеграла: " << s1 << endl;
	cout << "Время вычисления: " << elapseTime;
	return 0;
}