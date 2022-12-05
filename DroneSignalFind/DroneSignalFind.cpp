#include <iostream>
#include <string>
#include "test_utils.h"

using namespace std;

const string param_file = "params.txt";
const string corr_pcm_file = "corr.pcm";	// PCM-файл с выхода коррелятора (одноканальный в формате float 4 байта на отсчет)

int main(int argc, char* argv[])
{
	if (argc < 2) {
		cout << "input file missing" << endl;
		return 0;
	}
	//string in_file = "123.pcm";
	//string in_file = "12333.pcm";
	string in_file(argv[1]);
    signal_frame_find_file(in_file, param_file, corr_pcm_file);

    return 0; 
}
