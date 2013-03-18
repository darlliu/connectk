#include "master.h"
#include "smartplayer.h"
#include "threats.h"
int main()
{
	threats m1;
    //m1.main_routine();
    
	m1.test_init();
	m1.IDSearch();
	m1.update_frontier();
	//auto a=m1.Get_Move();
	return 1;
};
