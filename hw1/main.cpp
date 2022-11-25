#include <iostream>
#include <complex>
#include <cmath>
#include <random>
#include <fstream>
#include <memory>
using namespace std::complex_literals;


class Generator {
    double amplitude_ = 1;
    double rate_of_signal_;
    double sample_rate_;
    double t_;
    public:
        Generator(double sample_rate, double amplitude, double rate_of_signal): sample_rate_(sample_rate), amplitude_(amplitude),
            rate_of_signal_(rate_of_signal)
        {
            t_ = 0;
        }
        std::complex<double> next() {
            double phase = M_PI   * 2.  * t_ * rate_of_signal_ / sample_rate_;
            std::complex<double> current_value = amplitude_ * std::complex<double>(std::cos(phase), std::sin(phase));
            t_++;
            return current_value;
        }
};

class ModulatorFM2 {
    double amplitude_ = 1;
    double rate_of_signal_;
    double sample_rate_;
    double t_;
    public:
        ModulatorFM2(double sample_rate, double amplitude, double rate_of_signal): sample_rate_(sample_rate), amplitude_(amplitude),
            rate_of_signal_(rate_of_signal)
        {
            t_ = 0;
        }
        std::complex<double> next(bool value) {
            
            double phase = M_PI   * 2.  * t_ * rate_of_signal_ / sample_rate_;
            phase += value ? M_PI : 0;
            std::complex<double> current_value = amplitude_ * std::complex<double>(std::cos(phase), std::sin(phase));
            t_++;
            return current_value;
        }

    
};


int main() {
    double A = 1;
    double sample_rate = 8000;
    double rate_signal = 2000;
    size_t n_samples = 200;
    Generator gen(sample_rate, A, rate_signal);
    std::ofstream fout;

    fout.open("generator_output.txt");
    for (size_t i = 0; i < n_samples; ++i)
    {
        std::complex<double> cur_value = gen.next();
        fout << imag(cur_value) << "," << real(cur_value) << '\n';
    }
    fout.close();


    ModulatorFM2 mod(sample_rate, A, rate_signal);
    
    fout.open("modulator_output.txt");

    for (size_t i = 0; i < n_samples; ++i)
    {
        bool rand_value = rand() % 2;
        std::complex<double> cur_value = mod.next(rand_value);
        fout << imag(cur_value) << "," << real(cur_value) << "," << double(rand_value) << '\n';
    }

    fout.close();


}