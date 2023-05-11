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

	float x, y, r, brightness;		
	bool plug;
	int type;
	for (int i = 0; i < circulos; ++i) {
		file >> x >> y >> r >> plug >> brightness >> type;
		//std::cout << x << " , " << y << std::endl;
		particles.push_back(circles_data(x, y, r, plug, brightness, type));
	}

	file.close();

	// add particles
	for (int i = 0; i < particles.size(); i++)
	{
		MixingField.addParticle(particles[i].type - 1, (bottom - particles[i].x)*scale, (left - particles[i].y)*scale);
	}

	MixingField.consolidateField();

	// find interface
	findInterface(leftInterface, rightInterface, 2, exibir);
	cv::line(*exibir,leftInterface,rightInterface,cv::Scalar(0,255,0), 3);

	MixingField.writeFrame(frame);
}

void Mixing::findInterface(cv::Point& leftInterface, cv::Point& rightInterface, int type, cv::Mat* exibir)
{
	leftInterface.y = 0;
	rightInterface.y = exibir->rows;
	int dy = exibir->rows;

	if ( type == 2 )
	{
		leftInterface.x = exibir->cols;
		rightInterface.x = exibir->cols;

		for ( auto circle : particles )
		{
			if ( circle.type != type)
				continue;

			cv::Point testPoint = cv::Point(circle.x,circle.y);
			if ( (testPoint.x - leftInterface.x) - (rightInterface.x - leftInterface.x)/(rightInterface.y - 1.0*leftInterface.y)*(testPoint.y - leftInterface.y) < 0)
			{
				float mLeft = (testPoint.x - leftInterface.x) / (1.0 * (testPoint.y - leftInterface.y));
				float mRight= (testPoint.x - rightInterface.x) / (1.0 * (testPoint.y - rightInterface.y));
				if (abs(mLeft) > abs(mRight))
				{
					rightInterface.x = (int)(leftInterface.x + mLeft * dy);
				}
				else
				{
					leftInterface.x = (int)(rightInterface.x - mRight * dy);
				}

			}
		}
	}

	if ( type == 1 )
	{
		leftInterface.x = 0;
		rightInterface.x = 0;

		for ( auto circle : particles )
		{
			if ( circle.type != type)
				continue;

			cv::Point testPoint = cv::Point(circle.x,circle.y);
			if ( (testPoint.x - leftInterface.x) - (rightInterface.x - leftInterface.x)/(rightInterface.y - 1.0*leftInterface.y)*(testPoint.y - leftInterface.y) > 0)
			{
				float mLeft = (testPoint.x - leftInterface.x) / (1.0 * (testPoint.y - leftInterface.y));
				float mRight= (testPoint.x - rightInterface.x) / (1.0 * (testPoint.y - rightInterface.y));
				if (abs(mLeft) > abs(mRight))
				{
					rightInterface.x = (int)(leftInterface.x + mLeft * dy);
				}
				else
				{
					leftInterface.x = (int)(rightInterface.x - mRight * dy);
				}

			}
		}
	}
}
