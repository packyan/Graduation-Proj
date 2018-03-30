#include <iostream>
#include <string>
#include <cmath>

using std::cout;
using std::endl;
using std::string;
using std::ostream;
using namespace std;
#ifndef MY_POINT_H
#define MY_POINT_H 1

class Point
{
public:
	//Point():point_x(0),point_y(0) {}
	Point(float x = 0, float y = 0): point_x(x), point_y(y) {}
	~Point(){}
	friend ostream& operator <<(ostream & output, const Point &p);
	Point &operator=(const Point &equ);
	float Point_dst(const Point &p);
	// friend float operator -(const Point &sub){
	// return sqrt((sub.getX() - point_x) * (sub.getX() - point_x) + (sub.getY() - point_y) * (sub.getY() - point_y));
	// }	
	Point& set(Point &set_point);
	float getX() const;
	float getY() const;
	float mod2() const;

private:
	float point_x;
	float point_y;
};

float Point::getX() const{
	return point_x;
}

float Point::getY() const{
	return point_y;
}
float Point::mod2() const{
	return (point_x*point_x + point_y*point_y);
}

ostream& operator <<(ostream & output, const Point &p){
	cout << '(' << p.getX() << ' ' << p.getY() << ')' << endl;
	return output;
}

Point& Point::set(Point &set_point){
	this -> point_x = set_point.getX();
	this -> point_y = set_point.getY();
	return *this;
}

float Point::Point_dst(const Point &p){
	return sqrt((p.getX() - point_x) * (p.getX() - point_x) + (p.getY() - point_y) * (p.getY() - point_y));
}	

Point &Point::operator =(const Point &equ){
	point_x = equ.getX();
	point_y = equ.getY();
	return *this;
}

#endif
