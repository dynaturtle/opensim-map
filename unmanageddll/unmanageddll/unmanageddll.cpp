#include "unmanageddll.h"

void unmanagedstruct_summary(unmanaged_struct** us_list, int length)
{
	for (int i = 0; i < length; i ++)
	{
		cout << "unmanaged_struct summary:" << endl;
		cout << (*(us_list + i))->int_var << endl;
		cout << (*(us_list + i))->float_var << endl;
		cout << (*(us_list + i))->chars_var << endl;
		cout << (*(us_list + i))->vector_var->x << endl;
		cout << (*(us_list + i))->vector_var->y << endl;
	}
}	

int simple_add(int i1, int i2)
{
	return i1+i2;
}

void print_integer_array(int* a, int length)
{
	cout << "output array" << endl;
	for (int i = 0; i < length; i ++)
	{
		cout << a[i] << " ";
	}
	cout << endl;
}