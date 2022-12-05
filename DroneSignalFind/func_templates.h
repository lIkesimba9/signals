#ifndef FUNCTEMPLATES_H
#define FUNCTEMPLATES_H

#include <limits>
#include <complex>
#include <vector>

using namespace std;

namespace tC
{
	template<typename T_in, typename T_file> 
	bool write_complex(FILE* out, complex<T_in> num)
	{
		unsigned int size_T = sizeof(T_file) * 8 - 1;
		double max_num = pow(2.0, size_T)-1.0;

		if (num.real() > (T_in)max_num) 
			num.real((T_in)max_num);

		if (num.imag() > (T_in)max_num) 
			num.imag((T_in)max_num);

		T_file buf[2] = { (T_file)num.real(), (T_file)num.imag() };
		int size = fwrite(&buf, sizeof(T_file), 2, out);

		if (size != 2)
			return false;

		return true;
	}

	template<typename T> bool write_real(FILE* out, double num)
	{
		T buf[1] = { (T)num };
		int size = fwrite(&buf, sizeof(T), 1, out);

		if (size != 1)
			return false;

		return true;
	}

	//template<typename T_in, typename T_file>
	//bool write_block(FILE* in, vector<complex<T_in>>& in_sig,
	//	unsigned long int sample_count = 1, double itrp_shift = 0.0)
	//{
	//	dsp::interpolator<complex<T_in>> itrp(1.0, 1.0 + itrp_shift);

	//	if (sample_count > in_sig.size()) sample_count = in_sig.size();

	//	for (int i = 0; i < sample_count; i++)
	//	{
	//		itrp.process(in_sig[i]);
	//		while (itrp.next(in_sig[i]))
	//		{
	//			if(write_complex<T_in, T_file>(in, in_sig[i]) == false)
	//				return false;
	//		}
	//	}
	//		
	//	return true;
	//}


	template<typename T_file, typename T_out> 
	bool read_complex(FILE* in, complex<T_out>& in_sig)
	{
		T_file buf[2];
		int size = fread(&buf, sizeof(T_file), 2, in);

		if (size != 2)
			return false;

		in_sig.real((T_out)buf[0]);
		in_sig.imag((T_out)buf[1]);

		return true;
	}

	template<typename T_file, typename T_out> bool read_real(FILE* in, T_out& num)
	{
		T_file buf[1];
		int size = fread(&buf, sizeof(T_file), 1, in);

		if (size != 1)
			return false;

		num = (T_out)buf[0];

		return true;
	}

	//template<typename T_file, typename T_out> 
	//bool read_block(FILE* in, vector<complex<T_out>>& in_sig,
	//	unsigned long int sample_count = 1, double itrp_shift = 0.0)
	//{
	//	dsp::interpolator<complex<T_out>> itrp(1.0, 1.0 + itrp_shift);

	//	T_file buf[2];

	//	complex<T_file> read_sample;
	//	complex<T_out> next_sample;

	//	while (in_sig.size() < sample_count)
	//	{
	//		if(read_complex<T_file>(in, read_sample) == false)
	//			return false;

	//		next_sample.real((T_out)read_sample.real());
	//		next_sample.imag((T_out)read_sample.imag());
	//		
	//		itrp.process(next_sample);
	//		while (itrp.next(next_sample))
	//		{
	//			in_sig.push_back(next_sample);
	//		}
	//	}
	//	
	//	return true;
	//}

	/*template <typename T> complex<T> sqrt_complex(complex<T> in)
	{
		T a = in.real();
		T b = in.imag();

		T c = a * a;

		T real = sqrt(( a + sqrt(a * a + b * b)) / (T)2);
		T imag = sqrt((-a + sqrt(a * a + b * b)) / (T)2);

		if (in.imag() < (T)0)
			imag *= (T)(-1);

		return complex<T>(real, imag);
	}*/

	template <typename T> double power(complex<T> in)
	{
		return (pow(in.real(), 2) + pow(in.imag(), (2)));
	}

	template<typename Type> Type MaxPower_Element(vector<Type>& arr, int& pos)
	{
		Type max_power = 0;

		for (int i = 0; i < arr.size(); i++)
		{
			if (tC::power(arr[i]) > max_power)
			{
				max_power = tC::power(arr[i]);
				pos = i;
			}
		}

		return max_power;
	}

	template<typename Type> Type MaxPower_Element(vector<complex<Type>>& arr, int& pos)
	{
		Type max_power = 0;

		for (int i = 0; i < arr.size(); i++)
		{
			if (power(arr[i]) > max_power)
			{
				max_power = power(arr[i]);
				pos = i;
			}
		}

		return max_power;
	}

	template<typename Type> Type Max_Element(vector<Type>& arr, int& pos)
	{
		Type max = 0;

		for (int i = 0; i < arr.size(); i++)
		{
			if (arr[i] > max)
			{
				max = arr[i];
				pos = i;
			}
		}

		return max;
	}

	template<typename Type> complex<Type> Max_Element(vector<complex<Type>>& arr, int& pos)
	{
		complex<Type> max = 0;

		for (int i = 0; i < arr.size(); i++)
		{
			if (abs(arr[i]) > abs(max))
			{
				max = arr[i];
				pos = i;
			}
		}

		return max;
	}

	template<typename Type> Type max_abs_element(vector<Type>& arr, int& pos)
	{
		Type max = 0;

		for (int i = 0; i < arr.size(); i++)
		{
			if (abs(arr[i]) > abs(max))
			{
				max = arr[i];
				pos = i;
			}
		}

		return max;
	}

	template<typename Type> complex<Type> max_abs_element(vector<complex<Type>>& arr)
	{
		complex<Type> max = 0;

		for (int i = 0; i < arr.size(); i++)
		{
			if (abs(arr[i]) > abs(max))
			{
				max = arr[i];
			}
		}

		return max;
	}

	template<typename Type> Type max_abs_element(vector<Type>& arr)
	{
		Type max = 0;

		for (int i = 0; i < arr.size(); i++)
		{
			if (abs(arr[i]) > abs(max))
			{
				max = arr[i];
			}
		}

		return max;
	}

	template<typename Type> complex<Type> min_element(vector<complex<Type>>& arr, int& pos)
	{
		complex<Type> min = complex<Type>(numeric_limits<Type>::max(), numeric_limits<Type>::max());

		for (int i = 0; i < arr.size(); i++)
		{
			if (abs(arr[i]) < abs(min))
			{
				min = arr[i];
				pos = i;
			}
		}

		return min;
	}

	template<typename Type> Type min_element(vector<Type>& arr, int& pos)
	{
		Type min = numeric_limits<Type>::max();

		for (int i = 0; i < arr.size(); i++)
		{
			if (arr[i] < min)
			{
				min = arr[i];
				pos = i;
			}
		}

		return min;
	}

	template<typename Type> Type min_abs_element(vector<Type>& arr, int& pos)
	{
		Type min = numeric_limits<Type>::max();

		for (int i = 0; i < arr.size(); i++)
		{
			if (abs(arr[i]) < min)
			{
				min = abs(arr[i]);
				pos = i;
			}
		}

		return min;
	}

	template<typename Type> Type min_abs_element(vector<Type>& arr)
	{
		Type min = numeric_limits<Type>::max();

		for (int i = 0; i < arr.size(); i++)
		{
			if (abs(arr[i]) < min)
			{
				min = abs(arr[i]);
			}
		}

		return min;
	}

	template<typename Type> Type averangeValue(vector<Type>& vec, int vec_size)
	{
		Type averangeVal = 0;

		if (vec_size > vec.size())
			vec_size = vec.size();

		for (int i = 0; i < vec_size; i++)
			averangeVal += vec[i];

		return averangeVal /= vec_size;
	}

	template<typename Type> Type averangeValue(Type* vec, int vec_size)
	{
		Type averangeVal = 0;

		for (int i = 0; i < vec_size; i++)
			averangeVal += vec[i];

		return averangeVal /= vec_size;
	}


	template <class Value> int Sign(Value Val)
	{
		if (Val == 0.)  return 0;

		if (Val > 0.)  return 1;
		else return -1;
	}

	template <typename T> void left_shift(T* arr, int N, const T& val)
	{
		for (int i = 0; i < N - 1; i++)
			arr[i] = arr[i + 1];

		//memcpy(arr, arr + 1, N-1);

		arr[N - 1] = val;
	}

	template <typename T> void right_shift(T* arr, int N, const T& val)
	{
		for (int i = N - 1; i > 0; i--)
			arr[i] = arr[i - 1];

		arr[0] = val;
	}

	template <typename T> void right_shift(T* arr, int N)
	{
		T end = arr[N - 1];

		for (int i = N - 1; i > 0; i--)
			arr[i] = arr[i - 1];

		arr[0] = end;
	}

	template <class Value> void LeftShift(vector<Value>& v, vector<Value>& w)
	{
		v.erase(v.begin(), v.begin + w.size());
		v.insert(v.end(), w.begin(), w.end());
	}

	template <class Value> void LeftShift(vector<Value>& v, const Value& Val)
	{
		for (int i = 0; i < v.size() - 1; i++)
			v[i] = v[i + 1];

		v[v.size() - 1] = Val;
	}

	template <class Value> void LeftShift(vector<Value>& v)
	{
		Value beg = v[0];

		for (int i = 0; i < v.size() - 1; i++)
			v[i] = v[i + 1];

		v[v.size() - 1] = beg;
	}

	template <class Value> void RightShift(vector<Value>& v, const Value& Val)
	{
		for (int i = v.size() - 1; i > 0; i--)
			v[i] = v[i - 1];

		v[0] = Val;
	}

	template <class Value> void RightShift(vector<Value>& v)
	{
		Value end = v[v.size() - 1];

		for (int i = v.size() - 1; i > 0; i--)
			v[i] = v[i - 1];

		v[0] = end;
	}

	template <class Value> void get_diff_prb(vector<Value>& vecArr)
	{
		for (int i = 0; i < vecArr.size() - 1; i++)
		{
			vecArr[i] *= vecArr[i + 1];
		}
	}

	template <typename T> void GetDiffPreamb(vector<int8_t>& bit_set, vector<complex<T>>& dif_et)
	{
		for (int i = 1; i < bit_set.size(); i++)
		{
			complex<T> s_curr = bit_set[i] == 0 ? complex<T>(1.0, 0.0) : complex<T>(-1.0, 0.0);
			complex<T> s_prev = bit_set[i - 1] == 0 ? complex<T>(1.0, 0.0) : complex<T>(-1.0, 0.0);
			/*complex<T> s_curr = bit_set[i] == 0 ? complex<T>(-1.0, 0.0) : complex<T>(1.0, 0.0);
			complex<T> s_prev = bit_set[i - 1] == 0 ? complex<T>(-1.0, 0.0) : complex<T>(1.0, 0.0);*/
			dif_et.push_back(s_curr * conj(s_prev));
		}
	}

	template <typename T>
	void GetDiffPreamb(const int8_t bit_set[], complex<T> dif_et[], int size_)
	{
		for (int i = 1; i < size_; i++)
		{
			complex<T> s_curr = bit_set[i] == 0 ? complex<T>(1, 0) : complex<T>(-1, 0);
			complex<T> s_prev = bit_set[i - 1] == 0 ? complex<T>(1, 0) : complex<T>(-1, 0);
			/*complex<T> s_curr = bit_set[i] == 0 ? complex<T>(-1.0, 0.0) : complex<T>(1.0, 0.0);
			complex<T> s_prev = bit_set[i - 1] == 0 ? complex<T>(-1.0, 0.0) : complex<T>(1.0, 0.0);*/
			dif_et[i - 1] = s_curr * conj(s_prev);
		}
	}

	template <typename T> void std_etalon(vector<int8_t>& bit_set, vector<complex<T>>& std_et)
	{
		for (auto b : bit_set)
		{
			complex<T> s = b == 0 ? complex<T>(1.0, 0.0) : complex<T>(-1.0, 0.0);
		/*	complex<T> s = b == 0 ? complex<T>(-1.0, 0.0) : complex<T>(1.0, 0.0);*/
			std_et.push_back(s);
		}
	}

	template <typename T1, typename T2> void std_etalon(const T1 bit_set[], complex<T2> std_et[], int size_)
	{
		for (int i = 0; i < size_; i++)
		{
			complex<T2> s = bit_set[i] == 0 ? complex<T2>(1, 0) : complex<T2>(-1, 0);
			//complex<T> s_pre = b == 0 ? complex<T>(-1.0, 0.0) : complex<T>(1.0, 0.0);*/
			std_et[i] = s;
		}
	}

	template <class T1, class T2> void modulate_arr(const vector<T1>& in, vector<T2>& out)
	{
		for (int i = 0; i < in.size(); i++)
		{
			T2 inf_bit = (T2)1;

			if (in[i] == (T1)0) inf_bit = -(T2)1;

			out.push_back(inf_bit);
		}
	}

	template <class T1, class T2> void modulate_arr(const T1 in[], T2 out[], int size_)
	{
		for (int i = 0; i < size_; i++)
		{
			T2 inf_bit = 1;

			if (in[i] == 0) inf_bit = -1;

			out[i] = inf_bit;
		}
	}
	
	template <class T> void modulate_arr(vector<T>& in)
	{
		for (int i = 0; i < in.size(); i++)
		{
			T inf_bit = (T)1;

			if (in[i] == (T)0) in[i] = -(T)1;
		}
	}

	template <class T_in, class T_out> vector<complex<T_out>> 
		convert_data(const vector<complex<T_in>> &in)
		{
			vector<complex<T_out>> conv_out;

			for (int i = 0; i < in.size(); i++)
			{
				complex<T_out> conv_nxt((T_out)in[i].real(), (T_out)in[i].imag());
				conv_out.push_back(conv_nxt);
			}

			return conv_out;
		}

	template <class T_in, class T_out> complex<T_out>
		convert_data(const complex<T_in>& in)
		{
			return complex<T_out> ((T_out)in.real(), (T_out)in.imag());		
		}

		template <typename T> void norm_coeff(vector<T>& coeffs, T amp)
		{
			T max_coeff = 0;
			for (int i = 0; i < coeffs.size(); i++)
			{
				if (coeffs[i] > max_coeff)
					max_coeff = coeffs[i];
			}

			double mlt = (double)amp / (double)max_coeff;

			for (int i = 0; i < coeffs.size(); i++)
			{
				coeffs[i] = (T)((double)coeffs[i] * mlt);
			}
		}

		template <typename T> bool pack_in_byte(const vector<T>& bit_arr, T& out_byte)
		{
			if (bit_arr.size() != sizeof(out_byte) * 8)
				return false;

			T fbyte = 0x00;

			for (int i = 0; i < bit_arr.size(); i++)
			{
				fbyte >>= 1;
				fbyte = fbyte | (bit_arr[i] << 7);
			}

			out_byte = fbyte;
		}

		template <typename T> void pack_in_byte(T*& bit_arr, T& out_byte)
		{
			/*if (bit_arr)
				delete[]bit_arr;

			bit_arr = new T[sizeof(out_byte) * 8];*/

			T fbyte = 0x00;

			for (int i = 0; i < sizeof(out_byte) * 8; i++)
			{
				fbyte >>= 1;
				fbyte = fbyte | (bit_arr[i] << 7);
			}

			out_byte = fbyte;
		}

		template <typename T> void pack_byte_to_arr(const T& byte, vector<uint8_t>& out_bits)
		{
			int size = sizeof(byte) * 8;
			
			out_bits.clear();
			out_bits.assign(size, 0);

			for (int i = 0; i < out_bits.size(); i++)
			{
				bool b = (bool((1 << i) & byte));

				out_bits[i] = (uint8_t)b;
			}
		}

		template <typename T> void pack_byte_to_arr(const T& byte, uint8_t* out_bits)
		{
			int size = sizeof(byte) * 8;

			if (out_bits)
				delete[] out_bits;

			out_bits = new uint8_t[size];

			for (int i = 0; i < size; i++)
			{
				bool b = (bool((1 << i) & byte));

				out_bits[i] = (uint8_t)b;
			}
		}

		template<typename T>
		void reverse(T a[], int size_) 
		{
			int i = 0;
			int j = size_ -1;
			int tmp = 0;

			while (i < j)
			{
				tmp = a[i];

				a[i] = a[j];
				a[j] = tmp;

				i++;
				j--;
			}
		}

		template <typename T, enable_if_t<is_arithmetic<T>::value, bool> = true>
		void print(T arr[], int size_)
		{
			for (int i = 0; i < size_; i++)
				cout << arr[i] << " ";

			cout<< endl;
		}

		template <typename T, enable_if_t<is_arithmetic<T>::value, bool> = true>
		void print(complex<T>* arr, int size_)
		{
			cout << endl;

			for (int i = 0; i < size_; i++)
			{
				cout << arr[i].real() << " " << arr[i].imag() << endl;
			}

			cout << endl;
		}

		/*void print(int8_t arr[], int size_);
		void print(complex<int8_t>* arr, int size_);*/
}

#endif // FUNCTEMPLATES_H