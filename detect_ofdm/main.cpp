#include <iostream>
#include <complex>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <cstdio>
#include <stdexcept>
#include <utility>
#include <algorithm>

using namespace std;
using namespace complex_literals;
struct raw_pcm_sample {
    int16_t real;
    int16_t img;
    operator complex<double>() const {
        complex<double> value(real, img);
        return value;
    }
    static complex<double> make_sample(raw_pcm_sample& raw) {
        complex<double> value(raw.real, raw.img);
        return value;
    }
};

ifstream & operator >> (ifstream& in, raw_pcm_sample& raw) {

    in.read((char *)&raw.real, sizeof(int16_t));
    in.read((char *)&raw.img, sizeof(int16_t));
    return in;
}


class ReaderWindow {
    size_t windows_size_;
    ifstream input_;
    bool next_flag_;
    bool init_flag_;
    
    
    void read_block(vector<complex<double>> &block, size_t pos, size_t len) {
      
        for (size_t i = pos; i < len; ++i) {
            raw_pcm_sample raw;
            if (input_ >> raw) {
                block[i] = raw;
            }
            
            
            else {
                block[i] = 0;
                next_flag_ = false;
            }
        }
    }
public:
    const bool is_init() const {
        return init_flag_;
    } 
    const bool is_end() const {
        return ! next_flag_;
    } 
    ReaderWindow(string filename, size_t windows_size) : windows_size_(windows_size) {
        
        
        input_.open(filename, std::ios::binary);
        if (!input_.is_open()) {
            throw std::invalid_argument( "error open file" );
        }
        next_flag_ = true;
        init_flag_ = false;
    }
    ~ReaderWindow() {
        if (!input_.is_open()) {
            input_.close();
        }
    }
    vector<complex<double>> init() {
        init_flag_ = true;
        vector<complex<double>> block(windows_size_);
        read_block(block, 0, windows_size_);
        return block;
    }

    bool next(vector<complex<double>> &block) {
        if (next_flag_) {
            copy(block.begin() + windows_size_ / 2 + 1, block.end(), block.begin());
            read_block(block, windows_size_ / 2 - 1, windows_size_);
        }
        return next_flag_;
    }
    
};

    vector<complex<double>> compute_corr(vector<complex<double>>& data, size_t sinc_size, size_t data_size) {
        
        complex<double> value_corr = 0;
        vector<complex<double>> corr;
        
        size_t i = 0;
        for (; i < sinc_size; ++i) {
            value_corr += (data[i] * conj(data[data_size + i]));
        }

        for (size_t j = data_size + sinc_size; j < data.size(); ++j, ++i) {
            corr.push_back(value_corr);
            value_corr = value_corr - data[i - sinc_size] * conj(data[j - sinc_size]);
            value_corr = value_corr + data[i] * conj(data[j]);
            
        }
        return corr;
    }

class ProcessSignal {
    ReaderWindow block_generator_;
    size_t block_size_;
    size_t sync_size_;
    size_t data_size_;
    vector<complex<double>> last_read_block_;
    

    public:
        ProcessSignal(string signal_filename, size_t windows_size, size_t block_size, size_t sync_size, size_t data_size): 
            block_generator_(signal_filename, windows_size), 
            block_size_(block_size),
            sync_size_(sync_size),
            data_size_(data_size) {
            
        }
        bool sum_corr_func(vector<complex<double>>& corr_values) {
            size_t i = 0;
            if (!block_generator_.is_init()) {
                last_read_block_ = block_generator_.init();
                auto corr = compute_corr(last_read_block_, sync_size_, data_size_);
                std::transform (corr_values.begin(), corr_values.end(), corr.begin(), corr_values.begin(), plus<complex<double>>());
                i++;
            }
            
            for (; i < block_size_ && block_generator_.next(last_read_block_); ++i) {
                auto corr = compute_corr(last_read_block_, sync_size_, data_size_);
                std::transform (corr_values.begin(), corr_values.end(), corr.begin(), corr_values.begin(), plus<complex<double>>());
            }
            
            return block_generator_.is_end();
        }
};



int main()
{
    string filename = "../signal.pcm"; //"/Users/simba9/Desktop/signals/detect_ofdm/signal.pcm";
  /*  size_t size_block = 1152 * 2;
    ReaderWindow block_generator(filename, size_block);
    vector<complex<double>> block = block_generator.init();
    
    vector<double> all_corr_value;
    auto corr_win = compute_corr(block, 128, 1024);
    all_corr_value.insert(all_corr_value.end(), corr_win.begin(), corr_win.end());
    int i = 0;
    ofstream out;
    out.open("out.csv");
    for (double value : corr_win) 
            out << value << '\n';
    while (block_generator.next(block))
    {
        corr_win = compute_corr(block, 128, 1024);
        i++;
        for (double value : corr_win) 
            out << value << '\n';
        
    }
    out.close();

*/  
    vector<complex<double>> sum_corr(1152, 0);
    ProcessSignal process(filename, 1152 * 2, 10, 128, 1024);
    ofstream out;
    out.open("corr.csv");
    while(!process.sum_corr_func(sum_corr)) {
        
        for (complex<double> value : sum_corr) 
            out << std::norm(value) << '\n';
        fill(sum_corr.begin(), sum_corr.end(), 0);
    }
    out.close();
    return 0;

}

