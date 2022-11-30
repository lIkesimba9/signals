#include <iostream>
#include <complex>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <cstdio>
using namespace std;
using namespace complex_literals;
using sample = complex<double>;
struct raw_pcm_sample {
	short int real;
	short int img;
	operator sample() const {
		return (real, img);
	}
	static sample make_sample(raw_pcm_sample& raw) {
		return (raw.real, raw.img);
	}
};

ifstream & operator >> (ifstream& in, raw_pcm_sample& raw) {

	in.read((char *)&raw.real, sizeof(int16_t));
	in.read((char *)&raw.img, sizeof(int16_t));
	return in;
}
/*
sample read_sample(FILE* file) {
	int16_t real, imag;
	fread((char*)& real, sizeof(int16_t), 1, file);
	fread((char*)& imag, sizeof(int16_t), 1, file);
	return (real, imag);
}*/


class SlidingWindows {
	size_t size_windows_;
	//FILE* fp_;
	ifstream input_;
	vector<sample> data_;
	bool first_next_;
	bool flag_last_;
public:
	class iterator {
		friend class SlidingWindows;
	private:

	public:
		iterator();
	};
	SlidingWindows(string filename, size_t size_windows) : size_windows_(size_windows) {
		
		
		input_.open(filename, std::ios::binary);
		if (!input_.is_open()) {
			std::runtime_error("error open file");
		}
		flag_last_ = false;
		first_next_ = true;
		for (size_t i = 0; i < size_windows_; ++i) {
			if (!input_.eof()) {
					raw_pcm_sample raw;
					input_ >> raw;
					data_.push_back(raw);
			}

			else {
				flag_last_ = true;
			}
		}

		if (data_.size() != size_windows) {
			std::runtime_error("small file, or biggest windows size!");
		}
	}
	~SlidingWindows() {
		if (!input_.is_open()) {
			input_.close();
		}
	}
	vector<sample> next() {
		if (first_next_) {
			first_next_ = false;
			return data_;
		}
		if (flag_last_) {
			return {};
		}
		copy(data_.begin() + size_windows_ / 2, data_.end(), data_.begin());
		for (size_t i = 0; i < size_windows_ / 2; ++i) {
			if (!input_.eof()) {
				raw_pcm_sample raw;
				input_ >> raw;
				data_[i + size_windows_ / 2] = raw;
			}
			
			else {
				data_[i + size_windows_ / 2] = 0;
				flag_last_ = true;
			}
		}

		return data_;
	}
};



vector<long long> compute_corr(vector<sample>& data, size_t sinc_size, size_t data_size) {
	size_t i = 0, j = data_size;
	vector<long long> corr;
	for (; j < data.size() - sinc_size; ++j, ++i) {
		sample value_corr = 0;
		for (size_t k = 0; k < sinc_size; ++k) {
			value_corr += (data[i + k] * conj(data[j + k]));
		}
		corr.push_back(value_corr.real());
	}
	return corr;
}

void dump_to_csv(vector<long long> & vec, string filename = "out.csv") {
	ofstream out;
	out.open(filename);
	if (!out.is_open())
		return;
	for (long long value : vec) {
		out << value << '\n';
	}
	out.close();
}

int main()
{
	string filename = "../signal.pcm";
	size_t size_windows = 1152 * 2;
	SlidingWindows winwdow_generator(filename, size_windows);
	vector<sample> window = winwdow_generator.next();
	vector<long long> all_corr_value;
	while (window.size() > 0)
	{
		auto corr_win = compute_corr(window, 128, 1024);
		all_corr_value.insert(all_corr_value.begin(), corr_win.begin(), corr_win.end());
		window = winwdow_generator.next();
	}
	dump_to_csv(all_corr_value);
	
	//fstream input_(filename, ios_base::binary | ios_base::in);
	/*
	
	int i = 0;
	while (!feof(fp))
	{
		sample one_sample = read_sample(fp);
		i++;
	}
	cout << i << '\n';
	*/
	return 0;

}

