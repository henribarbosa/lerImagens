#include "../include/auction.h"

// functions in the auction class

// constructor
auction::auction(float dist)
	:maxDist(dist) // largest possible travel of a particle in 1 frame
{}

// virtual destructor
auction::~auction()
{
	//delete[] connections;
	//delete[] lines_assignment;
	//delete[] columns_assignment;
	//delete[] prices;
}

// finds the minimum value in a row, correcting with the price for the evolution of the algorithm
void auction::minimumFunction(float* min, float* second_min, int* min_index, int* index)
{
	for (int i = 0; i < connections_size; ++i)
	{
		float value_corrected = connections[*index * connections_size + i] + prices[i]; // value and price
		if (value_corrected >= *second_min) // not a minimum
			continue;
		if (value_corrected < *min) // a new minimum, uptades the second smallest value as well
		{
			*second_min = *min;
			*min = value_corrected;
			*min_index = i;
		}
		else // is the second smallest
			*second_min = value_corrected;
	}
}

// tests the end of the running, stops when all small values are selected
bool auction::testEnd()
{
	bool retornar = true;
	if (connections_columns >= connections_lines) // more particles in the new frame
	// tests if all small values are selected, does not care for larger values that will be discarted anyway
	{
		for (auto index : lines_to_do)
		{
			float* min = std::min_element(connections + index * connections_size, connections + (index+1) * connections_size);
			if (*min < maxDist)
			{
				retornar = false;
				break;
			}
		}
	}
	else // more particles in the last frame
	// there are still particles to try to track
	{
		for (int* col_assigned = columns_assignment + connections_columns; col_assigned != columns_assignment + connections_size; ++col_assigned)
		{
			if (*col_assigned != -1)
			{
				//std::cout << *col_assigned << std::endl;
				retornar = false;
				break;
			}
		}
		if (lines_to_do.size() > (connections_size - connections_columns))
		{
			retornar = false;
		}
	}
	return retornar;
}

// runs the algorithm
int auction::findMinimum()
// each line will bid for the column with smallest value and update the offered price based on the difference to the second smallest value
{
	//ThreadPool thread_manage;
	//thread_manage.Start();

	int count = 0; // number of iterations
	while (lines_to_do.size() > 0 && not testEnd()) // there is still particles untracked
	{
		int index = lines_to_do[0]; // start auxiliary variables
		float min = big_value, second_min = big_value;
		int min_index = 0;
		for (int i = 0; i < connections_size; ++i) // looks for minimum in row considering prices
		{
			float value_corrected = connections[index * connections_size + i] + prices[i];
			if (value_corrected >= second_min)
				continue;
			if (value_corrected < min)
			{
				second_min = min;
				min = value_corrected;
				min_index = i;
			}
			else
				second_min = value_corrected;
		}

		//thread_manage.QueueJob([this, &min, &second_min, &min_index, &index]{minimumFunction(&min,&second_min,&min_index,&index);});

		lines_assignment[index] = min_index; // which column correspond to the line
		if (columns_assignment[min_index] != -1) // column already selected
		{
			lines_to_do.push_back(columns_assignment[min_index]); // removes the line that have sected this column before
			lines_assignment[columns_assignment[min_index]] = -1;
			//std::cout << "Put back: " << columns_assignment[min_index] << std::endl;
		}
		columns_assignment[min_index] = index; // which line correspond to the column
		prices[min_index] += second_min - min + epsilon; // updates the offered prices for each column
		lines_to_do.pop_front(); // moves to next column

		// maximum number of iterations to avoid infinity loop
		// usually it converges with few iterations
		control maxAuctionIterations("thresholds.txt","MaxAuctionIterations");	
		if (count > maxAuctionIterations.returnThreshold())
		{
			//thread_manage.Stop();
			//std::cout << connections_lines - connections_columns << std::endl;
			//for (auto i : lines_to_do)
			//	std::cout << i << " ";
			//std::cout << std::endl;
			std::cout << lines_to_do.size() << std::endl;
			return -1; // return failed value
		}

		++count; // update iteration
		//std::cout << "Lines to do: " << lines_to_do.size() << std::endl;
		//for (auto& line : lines_to_do)
		//{
		//	std::cout << line << " ";
		//}
		//std::cout << std::endl;
		//std::cout << "ep " << epsilon << std::endl;
		//std::cout << "Prices: ";
		//for (int i = 0; i < connections_size; ++i)
		//{
		//	std::cout << prices[i] << " ";
		//}
		//std::cout << std::endl;
		//std::cin.get();
	}

	//thread_manage.Stop();
	return 1; // return success value
}

// euclidian distance between 2 points
float auction::dist(circles_data* p1, circles_data* p2)
{
	float dx2 = pow(p1->x - p2->x, 2);
	float dy2 = pow(p1->y - p2->y, 2);

	return sqrtf(dx2 + dy2);
}

// read the files for frame0 and frame1
bool auction::read_files()
{
	std::ifstream file;
	std::string linha;
	file.open("Files/circulos.txt", std::ios::in);
	int circulos;
	file >> circulos;
	std::getline(file,linha);
	//std::cout << circulos << " circulos" << std::endl;
	
	//circulos = 10;
	frame1.reserve(circulos);

	float x, y, r, brightness;		
	bool plug;
	int type;
	for (int i = 0; i < circulos; ++i) {
		file >> x >> y >> r >> plug >> brightness >> type;
		//std::cout << x << " , " << y << std::endl;
		frame1.push_back(circles_data(x, y, r, plug, brightness, type)); // organize values in the appropriate structure
	}

	file.close();

	file.open("Files/circulos_ant.txt");
	if (!file)
		return true; // it is the first frame of the movie
	file >> circulos;
	std::getline(file, linha);

	frame0.reserve(circulos);
	for (int i = 0; i < circulos; ++i)
	{
		file >> x >> y >> r >> plug >> brightness >> type;
		frame0.push_back(circles_data(x, y, r, plug, brightness, type)); // organize values in the appropriate structure
	}

	file.close();
	return false; // must run the tracking
}

// build matrix with the distance between every particle in each frame
void auction::buildConnections()
{
	connections_lines = frame1.size(); // one line per particles in current frame
	connections_columns = frame0.size(); // one column per particle in previous frame
	connections_size = std::max(connections_lines,connections_columns); // builds a suare matrix
	connections = new float[connections_size * connections_size];

	//lines_to_do.resize(connections_size);
	epsilon = 1.0f / connections_size; // small increase to prices to avoid been stuck in a loop

	// start vectors to store connections
	lines_assignment = new int[connections_size];
	columns_assignment = new int[connections_size];
	prices = new float[connections_size];

	for (int i = 0; i < connections_size; ++i)
	{
		for (int j = 0; j < connections_size; ++j)
		{
			if (i >= connections_lines) // not enough particles, puts large values to be a square matrix
			{
				connections[i * connections_size + j] = maxDist;
				continue;
			}
			if (j >= connections_columns) // not enough particles, puts large values to be a square matrix
			{
				connections[i * connections_size + j] = maxDist;
				continue;
			}
			float distance = dist(&frame1[i], &frame0[j]);
			if (distance > maxDist) // same value for large distances that does not matter, speed up the algorithm that does not try to solve irrelevant pairings
			{
				connections[i * connections_size + j] = maxDist;
				continue;
			}
			connections[i * connections_size + j] = distance;
		}

		lines_to_do.push_back(i); // start the list of lines without an assigned column
		lines_assignment[i] = -1; // -1 = without pair
		columns_assignment[i] = -1;
		prices[i] = 0;
	}

}

// impossible pairs that are too far apart
void auction::buildProhibiteds()
{
	unsigned int index = 0;
	for (auto value = connections; value < connections + connections_size*connections_size; ++value, ++index)
	{
		if ((int)(*value) >= maxDist)
		{
			int line = (int)index / connections_size;
			int column = index % connections_size;
			prohibited.push_back({ line, column });
		}
		//else
			//std::cout << maxDist << " > " << *value << std::endl;
	}
}

// exclude trackings that are going against the group movement in their region
void auction::buildProhibitedsOutlier()
{
	for (int i = 0; i < connections_lines; i++)
	{
		if (lines_assignment[i] == -1)
			continue;

		int xPos = frame1[i].x;
		int yPos = frame1[i].y;
		int xOrig = frame0[lines_assignment[i]].x;
		int yOrig = frame0[lines_assignment[i]].y;

		std::pair<float,float> VelocityVector(xPos-xOrig,yPos-yOrig); // velocity of the partile
		std::vector<std::pair<float,float>> VelocityVectorNeighbor; // velocities of near particles
		for (int j = 0; j < connections_lines; j++)
		{
			if (i == j) // same particle
				continue;
			if (lines_assignment[j] == -1) // particle not tracked
				continue;
			
			int deltaX = xPos - frame1[j].x;
			int deltaY = yPos - frame1[j].y;
			float distance = sqrt(pow(deltaX,2)+pow(deltaY,2));
			if (distance < maxDist) // close particle
			{
				int xOrigNeighbor = frame0[lines_assignment[j]].x;
				int yOrigNeighbor = frame0[lines_assignment[j]].y;
				VelocityVectorNeighbor.push_back({frame1[j].x - xOrigNeighbor, frame1[j].y - yOrigNeighbor}); // add velocity of close particle
			}
		}

		if (VelocityVectorNeighbor.size() == 0) // no particle close
			continue;

		std::pair<float,float> VelocityVectorNeighborAverag; // group velocity
		for (int j = 0; j < VelocityVectorNeighbor.size(); j++) // makes the average
		{
			VelocityVectorNeighborAverag.first = VelocityVectorNeighborAverag.first + VelocityVectorNeighbor[j].first;
			VelocityVectorNeighborAverag.second = VelocityVectorNeighborAverag.second + VelocityVectorNeighbor[j].second;
		}

		VelocityVectorNeighborAverag.first = VelocityVectorNeighborAverag.first / VelocityVectorNeighbor.size();
		VelocityVectorNeighborAverag.second = VelocityVectorNeighborAverag.second / VelocityVectorNeighbor.size();
		
		// compare magnitudes (exclude particles much faster then the group, if the group is nearly static)
		float VectorMagnitude = sqrt(pow(VelocityVector.first,2)+pow(VelocityVector.second,2));
		float VectorNeighborMagnitude = sqrt(pow(VelocityVectorNeighborAverag.first,2)+pow(VelocityVectorNeighborAverag.second,2));

		if (VectorNeighborMagnitude < 2)
		{
			if (VectorMagnitude > 2)
				prohibited.push_back({i,lines_assignment[i]});
			continue;
		}

		// maximum allowed ratio of magnitudes
		control auctionMagnitudeRatioMax("thresholds.txt","AuctionMagnitudeRatioMax");
		float MagnitudeRatio = VectorMagnitude / VectorNeighborMagnitude;
		float threshold = auctionMagnitudeRatioMax.returnThreshold();

		if (MagnitudeRatio < 1/threshold || MagnitudeRatio > threshold)
		{
			prohibited.push_back({i,lines_assignment[i]});
			continue;
		}

		// computes difference of direction of particle and group velocities
		float dotProduct = VelocityVector.first * VelocityVectorNeighborAverag.first + VelocityVector.second * VelocityVectorNeighborAverag.second;
		dotProduct = dotProduct / ( sqrt(pow(VelocityVector.first,2)+pow(VelocityVector.second,2)) *
			sqrt(pow(VelocityVectorNeighborAverag.first,2)+pow(VelocityVectorNeighborAverag.second,2)) );

		//std::cout << dotProduct << std::endl;
		// excludes particles with dot product value bellow minimum acceptable
		control auctionInternalProductMin("thresholds.txt","AuctionInternalProductMin");
		if (dotProduct < auctionInternalProductMin.returnThreshold())
		{
			prohibited.push_back({i,lines_assignment[i]});
			continue;
		}
	}
}

// clear the pairs that are errors in tracking
void auction::clearProhibiteds()
{
	for (auto& pair : prohibited)
	{
		//std::cout << connections[pair.first * connections_lines + pair.second] << std::endl;
		if (lines_assignment[pair.first] == pair.second)
		{
			//std::cout << "Removed " << pair.first << " , " << pair.second << std::endl;
			lines_assignment[pair.first] = -1;
		}
	}
}

// frees memory after computations
void auction::clearMemory()
{
	delete[] connections;
	delete[] lines_assignment;
	delete[] columns_assignment;
	delete[] prices;
}

//  current frame is the previous frame for the next calculation
void auction::updateFiles()
{
	std::ifstream current;
	std::ofstream old;

	current.open("Files/circulos.txt", std::ios::in); // open for read
	old.open("Files/circulos_ant.txt", std::ios::trunc); // open for write, deleting previous content

	old << current.rdbuf(); // ovewrites file
}

// visual aid
void auction::makeFigure(cv::Mat* exibir)
{
	for (int i = 0; i < connections_lines; ++i)
	{
		if (lines_assignment[i] == -1)
			continue;
		cv::Point start(frame0[lines_assignment[i]].x, frame0[lines_assignment[i]].y);
		cv::Point end(frame1[i].x, frame1[i].y);
		cv::line(*exibir, start, end, cv::Scalar(255, 0, 0), 3, 8);
	}
}

// quality of the tracking, ratio of particles that are tracked in the frame
float correspondenceRatio(int* lines_assignment, int number_lines)
{
	int correspondence = 0;
	for (int* value = lines_assignment; value != lines_assignment + number_lines; value++)
	{
		if (*value != -1)
			correspondence++;
	}

	return (float)correspondence/(float)number_lines;
}

// call to run every step of the algorithm
void auction::run(cv::Mat* exibir, lagrangian& particle_track)
{
	bool first = read_files();
	if (!first)
	{
		buildConnections();
		buildProhibiteds();
		int status = findMinimum();
		buildProhibitedsOutlier();
		clearProhibiteds();
		#ifdef DEBUG_MODE
		std::cout << "Status: " << status << " , Correspondence ratio: " <<  correspondenceRatio(lines_assignment, connections_lines) << std::endl;
		#endif
		particle_track.updateLabels(exibir, frame1, frame0, lines_assignment, connections_lines);
		//makeFigure(exibir);
		clearMemory();
		updateFiles();
	}
	else
	{
		updateFiles();
		particle_track.firstPass(frame1);
	}

}

// function to test the algorithm
void auction::runTest()
{
	float test[25] = { 1,2,1,2,2,
			2,1,1,2,2,
			1,2,2,2,2,
			2,2,2,1,2,
			10,10,10,10,10 };
	connections = test;
	connections_size = 5;
	connections_lines = 5;
	connections_columns = 5;
	//lines_to_do.resize(connections_size);
	epsilon = 1.0f / connections_size;

	lines_assignment = new int[connections_size];
	columns_assignment = new int[connections_size];
	prices = new float[connections_size];

	for (int i = 0; i < connections_size; ++i)
	{
		lines_to_do.push_back(i);
		lines_assignment[i] = -1;
		columns_assignment[i] = -1;
		prices[i] = 0;
	}


	buildProhibiteds();
	int status = findMinimum();
	std::cout << "Status: " << status << std::endl;
	clearProhibiteds();
	for (int i = 0; i < connections_size; ++i)
	{
		std::cout << i << " : " << lines_assignment[i] << std::endl;
	}
}
