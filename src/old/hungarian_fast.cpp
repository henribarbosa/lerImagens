#include "hungarian_fast.h"

hungarian_fast::hungarian_fast(float dist)
	:maxDist(dist)
{}

hungarian_fast::~hungarian_fast()
{}

float hungarian_fast::dist(circles_data* p1, circles_data* p2)
{
	float dx2 = pow(p1->x - p2->y, 2);
	float dy2 = pow(p1->y - p2->y, 2);

	return sqrtf(dx2 + dy2);
}

bool hungarian_fast::read_files()
{
	std::ifstream file;
	std::string linha;
	file.open("circulos.txt");
	int circulos;
	file >> circulos;
	std::getline(file,linha);
	//std::cout << circulos << " circulos" << std::endl;
	
	//circulos = 10;
	frame1.resize(circulos, {});
	assignment.resize(circulos,std::pair(0,-1));
	for (int i = 0; i < circulos; ++i)
	{
		assignment[i].first = i;
	}

	double x, y;		
	float plug;
	for (int i = 0; i < circulos; ++i) {
		file >> x >> y >> plug;
		//std::cout << x << " , " << y << std::endl;
		frame1[i] = circles_data(x, y, plug);
	}

	file.close();

	file.open("circulos_ant.txt");
	if (!file)
		return true;
	file >> circulos;
	std::getline(file, linha);

	frame0.resize(circulos, {});
	for (int i = 0; i < circulos; ++i)
	{
		file >> x >> y >> plug;
		frame0[i] = circles_data(x, y, plug);
	}

	file.close();
	return false;
}

void hungarian_fast::buildConnections()
{
	connection_lines = frame1.size();
	connection_columns = frame0.size();
	connections_size = connection_columns * connection_lines;
	connections = new float[connection_lines * connection_columns];

	for (int i = 0; i < connection_lines; ++i)
	{
		for (int j = 0; j < connection_columns; ++j)
		{
			connections[i * connection_columns + j] = dist(&frame1[i], &frame0[j]);
		}
	}
}

void hungarian_fast::buildProhibiteds(float cut)
{
	unsigned int index = 0;
	for (auto value = connections; value < connections + connections_size; ++value, ++index)
	{
		if (*value >= cut)
		{
			int line = (int)index / connection_columns;
			int column = index % connection_columns;
			prohibited.push_back({ line, column });
		}
	}
}

void hungarian_fast::clearProhibiteds()
{
	for (auto &zeroValue : zeros)
	{
		if (zeroValue.third == 1)
		{
			bool flag = true;
			for (auto line = prohibited.begin(); line != prohibited.end(); ++line)
			{
				if (line->first == zeroValue.first && line->second == zeroValue.second)
				{
					flag = false;
					break;
				}
			}
			if (flag)
				assignment[zeroValue.first].second = zeroValue.second;
			else
				assignment[zeroValue.first].second = -1;
		}
	}
}

void hungarian_fast::makeAssignments()
{
	zeros.clear();
	unsigned int index = 0;
	for (auto value = connections; value < connections + connections_size; ++value, ++index)
	{
		if (*value == 0)
		{
			int line = (int)index / connection_columns;
			int column = index % connection_columns;
			int type = 1;
			for (auto& previousZero : zeros)
			{
				if (previousZero.third == 1)
				{
					if (previousZero.second == column || previousZero.first == line)
					{
						type = 0;
						break;
					}
				}
			}
			zeros.push_back({ line,column,type });
		}
	}

}

static std::mutex mZeros;

static void remakeZeros(int indexI, int indexJ, std::vector<tripleInt>* zeros, float* connection, std::vector<int>* col, std::vector<int>* lin, float* min)
{
	if ((*lin)[indexI] == 1 && (*col)[indexJ] == 1)
	{
		bool flag = (*connection == 0) ? true : false;
		*connection += *min;
		if (flag)
		{
			std::lock_guard<std::mutex> lock(mZeros);
			zeros->erase(std::remove_if(zeros->begin(), zeros->end(), [&indexI, &indexJ](tripleInt compare) {if (compare.first != indexI)
				return false;
			if (compare.second != indexJ)
				return false;
			return true; }));
		}
	}
	else if ((*lin)[indexI] == 0 && (*col)[indexJ] == 0)
	{
		bool flag = (*connection != 0) ? true : false;
		*connection -= *min;
		if (*connection == 0 && flag)
		{
			std::lock_guard<std::mutex> lock(mZeros);
			zeros->push_back({ indexI,indexJ,0 });
			(*col)[indexJ] = 1;
		}
	}
}

int hungarian_fast::findMinimum()
{
	std::vector<int> col; col.resize(connection_columns, 0);
	std::vector<int> lin; lin.resize(connection_lines, 0);

	//step 1
	for (auto i = connections; i != connections + connections_size; i += connection_columns)
	{
		float* min = std::min_element(i, i + connection_columns);
		float minimumValue = *min;
		for (auto j = i; j != i + connection_columns; ++j)
		{
			*j -= minimumValue;
		}
	}
	makeAssignments();
	if (checkAssignment())
	{
		clearProhibiteds();
		return 1;
	}

	//step 2
	for (int j = 0; j < connection_columns; ++j)
	{
		int min = big_value;
		for (int i = 0; i < connection_lines; ++i)
		{
			if (connections[i*connection_columns+j] < min)
			{
				min = connections[i * connection_columns + j];
			}
		}
		for (int i = 0; i < connection_lines; ++i)
		{
			connections[i * connection_columns + j] -= min;
		}
	}

	//step 3
	makeAssignments();
	if (checkAssignment())
	{
		clearProhibiteds();
		return 1;
	}
	

	//loop
	int count = 0;
	step4ini:
	while (not checkAssignment())
	{
		//step 4
		//std::cout << "Step 4 initial" << std::endl;
		lin.assign(connection_lines, 0);
		col.assign(connection_columns, 0);
		for (auto& zero : zeros)
		{
			if (zero.third == 1)
				col[zero.second] = 1;
		}
		
		step4:
		while (not zerosCovered(&col, &lin))
		{
			//std::cout << "Step 4" << std::endl;
			int indexI = 0;
			for (auto &zeroi : zeros)
			{
				if (lin[zeroi.first] == 1) //skip covered zero
				{
					indexI++;
					continue;
				}
				if (col[zeroi.second] == 1) //skip covered zero
				{
					indexI++;
					continue;
				}
				zeroi.third = 2; //prime zero
				//std::cout << indexI << " : " << zeroi.first << " , " << zeroi.second << " , " << zeroi.third << std::endl;
				//std::cin.get();
				for (auto &zeroj : zeros)
				{
					if (zeroj.third != 1) //not stared
						continue;
					if (zeroi.first== zeroj.first && zeroi.second == zeroj.second) //same zero
						continue;
					if (zeroi.first != zeroj.first && zeroi.second != zeroj.second) // not same line or column
						continue;
					if (zeroi.first == zeroj.first) // same row only
					{
						//std::cout << "entered same row" << std::endl;
						lin[zeroi.first] = 1; // cover row
						col[zeroj.second] = 0; //uncover column
						goto step4; //restart and see if all zeros are covered
					}
					else // same column only
					{
						//std::cout << "Entered same column" << std::endl;
						findPath(indexI); //find path to stared 0 in the same column
						zeroi.third = 1; //star zero
						for (auto zerok : zeros) //unprime zeros
						{
							if (zerok.third == 2)
								zerok.third = 0;
						}
						lin.assign(connection_lines, 0);
						col.assign(connection_columns, 0); //uncover lines and columns
						goto step4ini;
					}
				}
				//std::cout << "marked new column: " << zeroi.second << std::endl;
				col[zeroi.second] = 1;
				zeroi.third = 1;
				indexI++;
			}
			//std::cout << "Chegou ao fim" << std::endl;
			//std::cin.get();
		}

		//step 5
		//std::cout << "Step 5" << std::endl;
		float min = big_value;
		int index = 0;
		for (auto value = connections; value < connections + connections_size; ++value, ++index)
		{
			if (*value >= min)
				continue;
			int line = (int)index / connection_columns;
			int column = index % connection_columns;
			if (lin[line] == 1)
				continue;
			if (col[column] == 1)
				continue;
			min = *value;
		}

		//std::vector<std::future<void>> future_vector; future_vector.reserve(connections_size);
		index = 0;
		for (auto value = connections; value < connections + connections_size; ++value, ++index)
		{
			int indexI = (int)index / connection_columns;
			int indexJ = index % connection_columns;
			//future_vector.push_back(std::async(std::launch::async, remakeZeros, indexI, indexJ, &zeros, value, &col, &lin, &min));
			remakeZeros(indexI, indexJ, &zeros, value, &col, &lin, &min);
		}

		//for (auto& outerZero : zeros)
		//{
		//	if (outerZero.third == 0)
		//	{
		//		bool flag = true;
		//		for (auto& innerZero : zeros)
		//		{
		//			if (innerZero.third == 1)
		//			{
		//				if (outerZero.first == innerZero.first || outerZero.second != innerZero.second)
		//				{
		//					flag = false;
		//					break;
		//				}
		//				if (outerZero.first != innerZero.first || outerZero.second == innerZero.second)
		//				{
		//					flag = false;
		//					break;
		//				}
		//			}
		//		}
		//		if (flag)
		//			outerZero.third = 1;
		//	}
		//}
		
		if (count > 10000)
		{
			clearProhibiteds();
			return -1;
		}
		
		count++;
	}

	clearProhibiteds();

	return 1;
}

static std::mutex s_zeros;

static void check(std::vector<tripleInt>* zeros, unsigned int index, unsigned int* count)
{
	if ((*zeros)[index].third == 1)
	{
		std::lock_guard<std::mutex> lock(s_zeros);
		(*count) = (*count) + 1;
	}
}

bool hungarian_fast::checkAssignment()
{
	std::vector<std::future<void>> future_count; future_count.reserve(zeros.size());

	unsigned int target = (connection_lines < connection_columns) ? connection_lines : connection_columns;

	unsigned int count = 0;
	for (unsigned int i = 0; i < zeros.size(); ++i)
	{
		future_count.push_back(std::async(std::launch::async, check, &zeros, i, &count));
	}

	if (count < target)
		return false;

	return true;
}

void hungarian_fast::updateFiles()
{
	std::ifstream current;
	std::ofstream old;

	current.open("circulos.txt", std::ios::in);
	old.open("circulos_ant.txt", std::ios::trunc);

	old << current.rdbuf();
}

void hungarian_fast::run()
{
	bool first = read_files();
	if (!first)
	{
		buildConnections();
		buildProhibiteds(maxDist);
		int status = findMinimum();
		std::cout << "Status: " << status << std::endl;
		delete[] connections;
		updateFiles();
	}
	else
		updateFiles();
}

void hungarian_fast::findPath(int index)
{
	for (int ci = 0; ci < zeros.size(); ++ci)
	{
		bool flag = false;
		if (index != ci && zeros[ci].third == 1 && zeros[index].second == zeros[ci].second) //stared zero in the same column
		{
			for (int cj = 0; cj < zeros.size(); ++cj)
			{
				if (cj != ci && zeros[cj].third == 2 && zeros[cj].first == zeros[ci].first) //primed zero in the same line
				{
					//std::cout << "Achou" << std::endl;
					zeros[cj].third = 1;
					zeros[ci].third = 0;
					findPath(cj);
					flag = true;
					break;
				}
			}
		}
		if (flag)
			break;
	}
}

bool hungarian_fast::zerosCovered(std::vector<int>* col, std::vector<int>* lin)
{
	for (auto zero : zeros)
	{
		if ((*lin)[zero.first] == 1)
			continue;
		if ((*col)[zero.second] == 1)
			continue;
		//std::cout << zero.first << " , " << zero.second << " : " << zero.third << std::endl;
		return false;
	}
	return true;
}

void hungarian_fast::makeFigure(cv::Mat* exibir)
{
	for (auto& line : assignment)
	{
		if (line.second != -1)
		{
			cv::Point start(frame0[line.second].x, frame0[line.second].y);
			cv::Point end(frame1[line.first].x, frame1[line.first].y);

			cv::line(*exibir, start, end, cv::Scalar(0, 0, 255), 2, 8);
		}
	}
}

void hungarian_fast::runTest()
{
	float test[25] = { 1,2,2,2,4,
					2,1,2,2,4,
					2,2,2,1,4,
					2,2,1,2,1,
					2,3,3,3,4 };
	connections = test;
	connections_size = 25;
	connection_lines = 5;
	connection_columns = 5;
	assignment.resize(5,std::pair(0,-1));
	for (int i = 0; i < 5; ++i)
	{
		assignment[i].first = i;
	}
	makeAssignments();
	int ok = findMinimum();
	clearProhibiteds();
	std::cout << "Status: " << ok << std::endl;

	for (int i = 0; i < 25; ++i)
	{
		std::cout << connections[i] << " ";
	}
	std::cout << '\n';
	for (auto lin : assignment)
	{
		std::cout << lin.first << " : " << lin.second << std::endl;
	}
	for (auto lin : zeros)
	{
		std::cout << lin.first << " , " << lin.second << " : " << lin.third << std::endl;
	}
}


