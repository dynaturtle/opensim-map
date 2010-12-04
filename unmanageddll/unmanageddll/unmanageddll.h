#include <iostream>

using namespace std;

struct vector
{
	float x;
	float y;

	vector(float x0, float y0)
	{
		x = x0;
		y = y0;
	}
};

struct unmanaged_struct
{
	int int_var;
	float float_var;
	char* chars_var;
	vector* vector_var;

	unmanaged_struct(int i, float f, char* ch, vector* vec)
	{
		int_var = i;
		float_var = f;
		chars_var = ch;
		vector_var = vec;
	}
};

extern "C" __declspec( dllexport )  void unmanagedstruct_summary(unmanaged_struct** us_list, int length);

__declspec( dllexport) int simple_add(int i1, int i2);

extern "C" __declspec( dllexport) void print_integer_array(int* a, int length);