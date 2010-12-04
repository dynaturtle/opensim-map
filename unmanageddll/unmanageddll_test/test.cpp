#include "unmanageddll.h"

int main()
{
	// Test
	unmanaged_struct** us_list = new unmanaged_struct*[3];
	vector* vec_0 = new vector(1,1);
	us_list[0] = new unmanaged_struct(1,1.0,"abc", vec_0);

	vector* vec_1 = new vector(2,2);
	us_list[1] = new unmanaged_struct(2, 2.0, "efg", vec_1);

	vector* vec_2 = new vector(3,3);
	us_list[2] = new unmanaged_struct(3, 3.0, "hij", vec_2);

	unmanagedstruct_summary(us_list, 3);
}