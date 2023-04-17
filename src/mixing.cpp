#include "../include/mixing.h"

Mixing::Mixing(int bottom, int left, float scale)
	:bottom(bottom), left(left), scale(scale)
{
	// mixing lacey index
	control BedWidth("thresholds.txt", "BedWidth");
	float bedWidth = BedWidth.returnThreshold();
	
	MixingField = eulerianField(100,3,bottom*scale,bedWidth,"Mixing");
}

void Mixing::interfaceMixing(cv::Mat* exibir, cv::Point& leftInterface, cv::Point& rightInterface, int frame)
{
	// read particles
	std::ifstream file;
	std::string linha;
	file.open("Files/circulos.txt", std::ios::in);
	int circulos;
	file >> circulos;
	std::getline(file,linha);
	
//	std::vector<circles_data> particles;
	particles.clear();
	particles.reserve(circulos);

	float x, y, r;		
	bool plug;
	int type;
	for (int i = 0; i < circulos; ++i) {
		file >> x >> y >> r >> plug >> type;
		//std::cout << x << " , " << y << std::endl;
		particles.push_back(circles_data(x, y, r, plug, type));
	}

	file.close();

	// add particles
	for (int i = 0; i < particles.size(); i++)
	{
		MixingField.addParticle(particles[i].type - 1, (bottom - particles[i].x)*scale, (left - particles[i].y)*scale);
	}

	MixingField.consolidateField();

	// find interface
	findInterface(leftInterface, rightInterface);
	leftInterface.y = 0;
	rightInterface.y = exibir->rows;
	cv::rectangle(*exibir,leftInterface,rightInterface,cv::Scalar(0,255,0), 3);

	MixingField.writeFrame(frame);
}

void Mixing::findInterface(cv::Point& leftInterface, cv::Point& rightInterface)
{
	control mixingThreshold("thresholds.txt", "MixingStep");
	int step = mixingThreshold.returnThreshold();

	control minParticlesThreshold("thresholds.txt", "MinParticleCount");
	int minParticleCount = minParticlesThreshold.returnThreshold();

	int top_interface = 0;
	int most_removed = -1;
	while (top_interface < bottom) 
	{
		int new_top = top_interface + step;

		int type_1 = 0, type_2 = 0;
		for (int i = 0; i < particles.size(); i++)
		{
			if (particles[i].x < top_interface)
				continue;
			if (particles[i].x > new_top)
				continue;
			if (particles[i].type == 1)
				type_1++;
			else
				type_2++;
		}

//		std::cout << type_1 << " , " << type_2 << std::endl;
		if (type_1 < minParticleCount && type_2 < minParticleCount)
		{
			top_interface = new_top;
			continue;
		}
		int new_removed = (type_2 > type_1) ? 2 : 1;
		if (most_removed < 0)
		{
			most_removed = new_removed;
			top_interface = new_top;
			continue;
		}
//		if (most_removed != new_removed)
		if (most_removed == 1 && type_2 > minParticleCount)
			break;
		if (most_removed == 2 && type_1 > minParticleCount)
			break;
		top_interface = new_top;
	}

	int bottom_interface = bottom;
	most_removed = -1;
	while (bottom_interface > 0) 
	{
		int new_bottom = bottom_interface - step;

		int type_1 = 0, type_2 = 0;
		for (int i = 0; i < particles.size(); i++)
		{
			if (particles[i].x > bottom_interface)
				continue;
			if (particles[i].x < new_bottom)
				continue;
			if (particles[i].type == 1)
				type_1++;
			else
				type_2++;
		}

		if (type_1 < minParticleCount && type_2 < minParticleCount)
		{
			bottom_interface = new_bottom;
			continue;
		}

		int new_removed = (type_2 > type_1) ? 2 : 1;
		if (most_removed < 0)
		{
			most_removed = new_removed;
			bottom_interface = new_bottom;
			continue;
		}
//		if (most_removed != new_removed)
		if (most_removed == 1 && type_2 > minParticleCount)
			break;
		if (most_removed == 2 && type_1 > minParticleCount)
			break;
		bottom_interface = new_bottom;
	}

	leftInterface.x = top_interface;
	rightInterface.x = bottom_interface;


}
