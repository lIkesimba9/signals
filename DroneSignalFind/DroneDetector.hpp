#ifndef DRONEDETECTOR_H
#define DRONEDETECTOR_H

#include <vector>
#include <algorithm>
#include <functional>
#include <string>
#include <fstream>
#include <iostream>
#include <cstdint>
#include <complex>
#include "ThreadPool.hpp"

using namespace std;

/**
 * @brief ћногопоточный детектор сигнала
 * @tparam IN_TYPE - тип результата вычислений
 * @tparam OUT_TYPE - тип входных отсчетов
*/
template<typename IN_TYPE = int16_t, typename OUT_TYPE = double>
class DroneDetector
{
	typedef complex<OUT_TYPE> cplx_type;	// комплексный тип дл€ вычислений

	// параметры обнаружител€
	struct EstimatorParams {
		unsigned prefix_length = 128;
		unsigned data_length = 1024;
		unsigned frames_sum = 4;
		double corr_burst = 6e10;
		string find_flag_path = "find_flag";
		string correlation_path = "corr.txt";
		size_t correlation_file_max_size = 100000;
		unsigned channel = 0;
	};

public:
	/**
	 * @brief »нициализаци€ параметров обнаружител€
	 * @param params_file файл с параметрами обнаружител€
	*/
	DroneDetector(const string& params_file);

	~DroneDetector();

	/**
	 * @brief обработать очередной фрейм
	 * @param buf входной буфер (PCM стерео I/Q 16-бит), содержит 2-канальный сигнал
	 * @param buf_size число элементов int16_t в буфере
	*/
	void process(IN_TYPE* buf, size_t buf_size);

private:
	/**
	 * @brief вычисление коррел€ционного отклика. вызываетс€ в потоке
	 * @param ptr_begin начало буфера с сигналом
	 * @param res указатель, куда будет помещен результат
	 * @return обнаружен ли сигнал
	*/
	void count(IN_TYPE* ptr_begin, OUT_TYPE* res);

	vector<IN_TYPE> m_buf;				// буфер с входным сигналом
	vector<OUT_TYPE> m_corrBuf;			// буфер с вычисленной коррел€цией
	bool m_detectFlag = false;			// флаг обраружени€ сигнала
	EstimatorParams m_estParams;
	const unsigned m_numChannels = 2;	// число каналов в сигнале
	IN_TYPE* m_currentPos;				// текуща€ позици€ заполнени€ буфера
	IN_TYPE* m_bufEnd;					// позици€ конца буфера (указывает на элемент после последнего)
	int m_ptrSampleShift;
	int m_ptrFrameShift;

	ThreadPool<IN_TYPE, OUT_TYPE> m_threadPool;
};

template<typename IN_TYPE, typename OUT_TYPE>
DroneDetector<IN_TYPE, OUT_TYPE>::DroneDetector(const string& params_file)
{
	ifstream par_file(params_file);
	if (!par_file.is_open()) {
		cerr << "can't open parameters file" << endl;
		return;
	}

	// чтение параметров обнаружител€ из файла
	while (!par_file.eof()) {
		string par;
		par_file >> par;
		if (par == "prefix_length")
			par_file >> m_estParams.prefix_length;
		else if (par == "data_length")
			par_file >> m_estParams.data_length;
		else if (par == "frames_sum")
			par_file >> m_estParams.frames_sum;
		else if (par == "corr_burst")
			par_file >> m_estParams.corr_burst;
		else if (par == "find_flag_path")
			par_file >> m_estParams.find_flag_path;
		else if (par == "correlation_path")
			par_file >> m_estParams.correlation_path;
		else if (par == "correlation_file_max_size")
			par_file >> m_estParams.correlation_file_max_size;
		else if (par == "channel")
			par_file >> m_estParams.channel;
	}
	par_file.close();

	// размер буфера равен frames_sum+1 отсчетов I/Q с учетом количества каналов
	// т.к. циклическа€ последовательность первого кадра может быть обрезана
	int buf_length = (m_estParams.prefix_length + m_estParams.data_length) * (m_estParams.frames_sum + 1) * 2 * m_numChannels;
	m_buf.resize(buf_length, 0);
	int corr_buf_length = (m_estParams.prefix_length + m_estParams.data_length) * m_estParams.frames_sum;
	m_corrBuf.resize(corr_buf_length, 0);
	m_currentPos = m_buf.data();
	m_bufEnd = m_currentPos + buf_length;

	m_ptrSampleShift = m_numChannels * 2;
	m_ptrFrameShift = m_estParams.data_length * m_ptrSampleShift;

	m_threadPool.Start();
}

template<typename IN_TYPE, typename OUT_TYPE>
DroneDetector<IN_TYPE, OUT_TYPE>::~DroneDetector()
{
	m_threadPool.Stop();
}

template<typename IN_TYPE, typename OUT_TYPE>
void DroneDetector<IN_TYPE, OUT_TYPE>::process(IN_TYPE* buf, size_t buf_size)
{
	// если вычислени€ еще не закончились, то данные не копируем
	if (m_threadPool.busy())
		return;

	int copy_size = min(static_cast<int>(buf_size), m_bufEnd - m_currentPos);	// копируем или все данные или до заполнени€ буфера
	memcpy(m_currentPos, buf, copy_size * sizeof(IN_TYPE));
	m_currentPos += copy_size;

	if (m_currentPos == m_bufEnd) {
		// после заполнени€ буфера запускаем вычислени€
		function<ThreadPool<IN_TYPE, OUT_TYPE>::thread_func_type> count_func = [=](IN_TYPE* ptr, OUT_TYPE* res) {
			this->count(ptr, res);
		};
		m_threadPool.QueueJob(count_func, m_buf.data(), m_corrBuf.data());
	}
}

template<typename IN_TYPE, typename OUT_TYPE>
void DroneDetector<IN_TYPE, OUT_TYPE>::count(IN_TYPE* ptr_begin, OUT_TYPE* res)
{
	IN_TYPE* ptr_1 = ptr_begin;
	IN_TYPE* ptr_2 = ptr_begin + m_ptrFrameShift;

	// вычисление коррел€ции
	cplx_type sum_corr{ 0, 0 };
	for (int k = 0; k < m_estParams.frames_sum; k++) {				// сумма коррел€ций за множество кадров
		cplx_type corr_val{ 0, 0 };
		for (int i = 0; i < m_estParams.prefix_length; i++) {
			cplx_type arg_1{ static_cast<OUT_TYPE>(*ptr_1), static_cast<OUT_TYPE>(*(ptr_1 + 1)) };
			cplx_type arg_2{ static_cast<OUT_TYPE>(*ptr_2), -static_cast<OUT_TYPE>(*(ptr_2 + 1)) };
			corr_val += arg_1 * arg_2;

			ptr_1 += m_ptrSampleShift;
			ptr_2 += m_ptrSampleShift;
		}
		sum_corr += corr_val;

		ptr_1 += m_ptrFrameShift;
		ptr_2 += m_ptrFrameShift;
	}
	*res = norm(sum_corr);
}

#endif // DRONEDETECTOR_H