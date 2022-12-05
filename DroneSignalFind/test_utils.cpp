#include "test_utils.h"
#include "DroneDetector.hpp"

void init(const string& params_file, EstimatorParams& params)
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
			par_file >> params.prefix_length;
		else if (par == "data_length")
			par_file >> params.data_length;
		else if (par == "frames_sum")
			par_file >> params.frames_sum;
		else if (par == "corr_burst")
			par_file >> params.corr_burst;
		else if (par == "find_flag_path")
			par_file >> params.find_flag_path;
		else if (par == "correlation_path")
			par_file >> params.correlation_path;
		else if (par == "correlation_file_max_size")
			par_file >> params.correlation_file_max_size;
	}
	par_file.close();
}

void signal_frame_find_file(const string& in, const string& params_file, const string& corr_pcm_file)
{
	FILE* in_file = std::fopen(in.c_str(), "rb");
	if (!in_file)
		return;

	std::fseek(in_file, 0, SEEK_END);
	size_t filesize = ftell(in_file);
	std::fseek(in_file, 0, SEEK_SET);

	FILE* corr_file = std::fopen(corr_pcm_file.c_str(), "wb");
	if (!corr_file)
		return;

	int progress_counter = 0;
	unsigned cur_progress = 0;
	std::cout << "Signal processing..." << endl;

	//std::ofstream dbg_out("dbg_out.txt");

	EstimatorParams params;
	init(params_file, params);

	ofstream cf(params.correlation_path);
	cf.close();

	// размер буфера должен быть не меньше чем frames_sum+1 отсчетов I/Q
	size_t frame_length = params.prefix_length + params.data_length;
	size_t buf_size = frame_length * (params.frames_sum + 1) * 2;
	int16_t * buf = new int16_t[buf_size];

	vector<double> corr_buf(frame_length);

	DroneDetector<int16_t, double> dd(params_file);

	auto start_time = std::chrono::steady_clock::now();
	int16_t re;
	int16_t im;
	int i = 0;
	int fragment_counter = 0;
	ofstream corr_file_txt("corr.txt");
	while (tC::read_real<int16_t, int16_t>(in_file, re) &&
		tC::read_real<int16_t, int16_t>(in_file, im)) {
		buf[i] = re;
		buf[i + 1] = im;
		i += 2;

		if (i >= buf_size) {
			i = 0;
			memset(corr_buf.data(), 0, sizeof(double) * frame_length);
			//string corr_fname = "corr_" + to_string(fragment_counter) + ".txt";
			//ofstream corr_file(corr_fname);
			bool res = signal_frame_find_buf(buf, buf_size, params, corr_buf);
			for (auto& x : corr_buf)
				corr_file_txt << x << endl;
			//corr_file.close();
			fragment_counter++;
			//if (res)
			//	cout << "signal finded" << endl;
		}

		if (++progress_counter == 1000) {
			unsigned prg = static_cast<size_t>(ftell(in_file)) * 100 / filesize;
			if (prg != cur_progress) {
				PROGRESS_OUT(prg);
				cur_progress = prg;
			}
			progress_counter = 0;
		}
	}
	auto end_time = std::chrono::steady_clock::now();

	PROGRESS_OUT(100);
	std::cout << endl;

	std::chrono::duration<double> diff_time = end_time - start_time;
	std::cout << "duration: " << diff_time.count() << "s\n";

	delete[] buf;
	corr_file_txt.close();
	//dbg_out.close();
	std::fclose(in_file);
	std::fclose(corr_file);
}

bool signal_frame_find_buf(int16_t* buf, size_t buf_size, const EstimatorParams& params, vector<double>& corr_buf)
{
	// проверка и обнуление файла с коррел€цией
	//ofstream corr_file(params.correlation_path, ios::app);
	//corr_file.seekp(0, ios_base::end);
	//size_t sz = corr_file.tellp();
	//if (sz > params.correlation_file_max_size) {
	//	corr_file.close();
	//	corr_file.open(params.correlation_path);
	//}

	int ptrSampleShift = 2;
	int ptrDataShift = params.data_length * ptrSampleShift;

	int16_t* ptr_1 = buf;
	int16_t* ptr_2 = buf + ptrDataShift;

	size_t frame_length = params.prefix_length + params.data_length;
	bool res = false;	// флаг обнаружени€ сигнала
	for (size_t i = 0; i < frame_length; i++) {
		// вычисление коррел€ции
		cplx_dbl sum_corr{ 0, 0 };
		int16_t* it_1 = ptr_1;
		int16_t* it_2 = ptr_2;
		for (int k = 0; k < params.frames_sum; k++) {				// сумма коррел€ций за множество кадров
			cplx_dbl corr_val{ 0, 0 };
			for (int i = 0; i < params.prefix_length; i++) {
				cplx_dbl val_1{ static_cast<double>(*it_1), static_cast<double>(*(it_1 + 1)) };
				cplx_dbl val_2{ static_cast<double>(*it_2), -static_cast<double>(*(it_2 + 1)) };
				corr_val += val_1 * val_2;
				it_1 += ptrSampleShift;
				it_2 += ptrSampleShift;
			}
			sum_corr += corr_val;
			it_1 += ptrDataShift;
			it_2 += ptrDataShift;
		}
		corr_buf[i] = norm(sum_corr);				// квадрат абсолютного значени€ коррел€ции
		ptr_1 += ptrSampleShift;
		ptr_2 += ptrSampleShift;

		// пишем значение коррел€ции в текстовый файл
		//corr_file << corr_val_real << endl;
		// обнаружение сигнала по порогу коррел€ции
		//if (corr_val_real > params.corr_burst)
		//	res = true;
	}

	// обозначаем результат обнаружени€ флаговым файлом
	if (res) {
		ofstream flag(params.find_flag_path);
		flag.close();
	}
	//corr_file.close();

	return res;
}
