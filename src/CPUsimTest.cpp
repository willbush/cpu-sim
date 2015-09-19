#include "cute.h"
#include "CPUsimTest.h"

void thisIsACPUsimTestTest() {
	ASSERTM("start writing tests", false);	
}

cute::suite make_suite_CPUsimTest(){
	cute::suite s;
	s.push_back(CUTE(thisIsACPUsimTestTest));
	return s;
}
