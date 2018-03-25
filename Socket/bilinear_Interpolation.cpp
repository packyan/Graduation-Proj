#include<iostream>
#include<vector>
int main(int argc, char** argv)
{
	std::vector<float> data{570.89, 572.77, 576.59, 583.5};
	const int x = 0, y = 0;
	int i = 0, scale = 2;
	int x1 = x, x2 = x + 1, y1 = y, y2 = y + 1;
	for(std::vector<float>::size_type i = 0;i < data.size(); ++i) 
		std::cout<<data[i]<<std::endl;
	std::cout<<(float)(x2 - 1.0 / scale) * data[i]<<std::endl;
	std::cout<<(float)(1.0 / scale - x1) * data[i + 1]<<std::endl;
	float R1 = (float)(x2 - 1.0 / scale) * data[i] + (float)(1.0 / scale - x1) * data[i + 1];
	std::cout<<"R1 = "<<R1<<std::endl;
	float R2 = (float)(x2 - 1.0 / scale) * data[i + 2] + (float)(1.0 / scale - x1) * data[i + 3];
	std::cout<<"R2 = "<<R2<<std::endl;
	float interpolate_data = (y2 - 1.0 / scale) * R1 + (1.0 / scale -y1) * R2;
	std::cout<<"interpolate_data: "<<interpolate_data<<std::endl;
	return 0;	
}
