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

// ��������� ������������
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
 * @brief ������������� ���������� �� �����
 * @param params_file ���� � ����������� �����������
 * @param params ��������� � ����������� �����������
*/
extern void init(const string& params_file, EstimatorParams& params);

/**
 * @brief ���� ����������� ������� � ���������� �����
 * @param in ������� ���� (PCM ������ I/Q 16-���) 
 * @param params_file ���� � ����������� �����������
 * @param corr_pcm_file PCM-���� � ������ ����������� (������������� � ������� float 4 ����� �� ������)
*/
extern void signal_frame_find_file(const string& in, const string& params_file, const string& corr_pcm_file);

/**
 * @brief ���� ����������� ������� � ���������� ������
 * @param buf ����� (PCM ������ I/Q 16-���)
 * @param sample_count ���������� �������� I/Q � ������
 * @param params ��������� � ����������� �����������
 * @param corr_buf ����� ��� ���������� ����������
 * @return ������ ��������� ��/���
*/
extern bool signal_frame_find_buf(int16_t* buf, size_t buf_size, const EstimatorParams& params, vector<double>& corr_buf);

#endif // TESTUTILS_H
