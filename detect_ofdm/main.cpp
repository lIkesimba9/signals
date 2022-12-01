#include <iostream>
#include <complex>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <cstdio>
#include <stdexcept>
#include <utility>

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


class Reader {
    size_t block_size_;
    ifstream input_;
    bool next_flag_;
    long long block_num_;
    
    void read_block(vector<complex<double>> &block, size_t pos, size_t len) {
        block_num_++;
        for (size_t i = pos; i < len; ++i) {
            raw_pcm_sample raw;
            if (input_ >> raw) {
                block[i] = raw;
            }
            
            else {
                //cout << "test " << block_num_ << '\n';
                block[i] = 0;
                next_flag_ = false;
            }
        }
    }
public:
    Reader(string filename, size_t block_size) : block_size_(block_size) {
        
        
        input_.open(filename, std::ios::binary);
        if (!input_.is_open()) {
            throw std::invalid_argument( "error open file" );
        }
        next_flag_ = true;
        block_num_=0;
        //cout << "construcotr " << block_num_ << '\n';
    }
    ~Reader() {
        if (!input_.is_open()) {
            input_.close();
        }
    }
    vector<complex<double>> init() {
        vector<complex<double>> block(block_size_);
        read_block(block, 0, block_size_);
        //cout << "after init " << block_num_ << '\n';
        return block;
    }

    bool next(vector<complex<double>> &block) {
        //cout << "before read " << block_num_ << '\n';
        copy(block.begin() +  block_size_ / 2 + 1, block.end(), block.begin());
        read_block(block, block_size_ / 2 - 1, block_size_ / 2 + 1);
        //cout << "after read " << block_num_ << '\n';
        return next_flag_;
    }
};



vector<double> compute_corr(vector<complex<double>>& data, size_t sinc_size, size_t data_size) {
    size_t i = 0;
    complex<double> value_corr = 0;
    vector<double> corr;
    
    for (; i < sinc_size; ++i) {
        value_corr += (data[i] * conj(data[data_size + i]));
    }
    corr.push_back(value_corr.real());

    for (size_t j = data_size + sinc_size; j < data.size(); ++j, ++i) {
        value_corr = value_corr - data[i - sinc_size] * conj(data[j - sinc_size]);
        value_corr = value_corr + data[i] * conj(data[j]);
        corr.push_back(value_corr.real());
    }
    return corr;
}



int main()
{
    string filename = "../signal.pcm";
    size_t size_block = 1152 * 2;
    Reader block_generator(filename, size_block);
    vector<complex<double>> block = block_generator.init();
    
    vector<double> all_corr_value;
    auto corr_win = compute_corr(block, 128, 1024);
    //cout << "one " << corr_win.size() << '\n';
    all_corr_value.insert(all_corr_value.end(), corr_win.begin(), corr_win.end());
    int i = 0;
    ofstream out;
    out.open(filename);
    for (double value : corr_win) 
            out << value << '\n';
    while (block_generator.next(block))
    {
        corr_win = compute_corr(block, 128, 1024);
        //std::copy(corr_win.begin(), corr_win.end(), std::back_inserter(all_corr_value));
        i++;
        //cout << i << "\n";
        for (double value : corr_win) 
            out << value << '\n';

        
    }
    cout << "len iter " << i << " " << all_corr_value.size() << '\n';

    return 0;

}

