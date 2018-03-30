#include <iostream>
#include <vector>
#include "include/my_point.h"


using std::cin;
using std::cout;
using std::endl;
using std::string;

int main(int argc, char const *argv[])
{
	std::vector<Point> v;

	Point p1(5.0,5.0), p2(2.5,2.5);
	cout << "p1 Modulus: "<<p1.mod2()<<endl;
	v.push_back(p1);
	v.push_back(p2);
	cout << p1 << p2;
	cout << p1.Point_dst(p2)<<' ' << 2.5*1.414 << endl;
	p1.set(p2);
	cout << p1 << p2;
	Point p3(6.0, 2.0);
	p1 = p3;
	v.push_back(p3);
	cout << p1 << p2 << p3<<endl;
	cout <<"vector test...\n";
	for(auto it = v.begin(), itend = v.end(); it != itend; ++it)
		cout<< *it;
	return 0;
}