#ifndef TESTUTILS_H
#define TESTUTILS_H

#include <iostream>
#include <fstream>
#include <cstdio>
#include <map>
#include <chrono>
#include "func_templates.h"
#include "modemtypes.h"
#include "debug.h"

using namespace std;

// параметры обнаружител€
struct EstimatorParams {
	unsigned prefix_length = 128;
	unsigned data_length = 1024;
	unsigned frames_sum = 4;
	double corr_burst = 6e10;
	string find_flag_path = "find_flag";
	string correlation_path = "corr.txt";
	size_t correlation_file_max_size = 100000;
};

/**
 * @brief »нициализаци€ параметров из файла
 * @param params_file файл с параметрами обнаружени€
 * @param params структура с параметрами обнаружени€
*/
extern void init(const string& params_file, EstimatorParams& params);

/**
 * @brief тест обнаружени€ сигнала в записанном файле
 * @param in входной файл (PCM стерео I/Q 16-бит) 
 * @param params_file файл с параметрами обнаружени€
 * @param corr_pcm_file PCM-файл с выхода коррел€тора (одноканальный в формате float 4 байта на отсчет)
*/
extern void signal_frame_find_file(const string& in, const string& params_file, const string& corr_pcm_file);

/**
 * @brief тест обнаружени€ сигнала в записанном буфере
 * @param buf буфер (PCM стерео I/Q 16-бит)
 * @param sample_count количество отсчетов I/Q в буфере
 * @param params структура с параметрами обнаружени€
 * @param corr_buf буфер дл€ накоплени€ коррел€ции
 * @return сигнал обнаружен да/нет
*/
extern bool signal_frame_find_buf(int16_t* buf, size_t buf_size, const EstimatorParams& params, vector<double>& corr_buf);

#endif // TESTUTILS_H
