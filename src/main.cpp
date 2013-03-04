#include "master.h"

int main()
{
	Master m1;
	//m1.main_routine();
	m1.test_init();
	m1.update_frontier();
	m1.IDSearch();
	return 1;
};
