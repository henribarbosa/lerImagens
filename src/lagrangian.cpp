#include "../include/lagrangian.h"

void GotoLine(std::fstream& infile, std::fstream& outfile, int* num_line)
{
	if (*num_line == 0)
	{
		return;
	}
	std::string line;
	int buf_ini = infile.tellg();
	int buf_size = infile.tellg();
	for (int i = 0; i < *num_line; ++i)
	{
		std::getline(infile,line);
		buf_size = infile.tellg();
	}

	char* buffer = (char*) malloc((buf_size - buf_ini)*sizeof(char));
	infile.seekg(buf_ini);
	infile.read(buffer, buf_size - buf_ini);
	outfile.write(buffer, buf_size - buf_ini);
}

void CopyToFinal(std::fstream& infile, std::fstream& outfile)
{
	outfile << infile.rdbuf();
	//std::string line;
	//while ( std::getline(infile, line) )
	//{
	//	outfile << line << std::endl;
	//}
}

void lagrangian::setScale(float InputScale)
{
	scale = InputScale;
}

void lagrangian::setBoundaries(int InputBottom, int InputLeft)
{
	bottom = InputBottom;
	left = InputLeft;
}

void lagrangian::startFields()
{
	control BedWidth("thresholds.txt", "BedWidth");
	float bedWidth = BedWidth.returnThreshold();
	//Temperature = eulerianField(30,3,bottom*scale,3,"GranularTemperature");
	Temperature = eulerianFieldGranularTemp(20,2,bottom*scale,bedWidth,"GranularTemperature");
	VelocityX = eulerianField(50,5,bottom*scale,bedWidth,"VelocityX");
	VelocityY = eulerianField(50,5,bottom*scale,bedWidth,"VelocityY");
}

void lagrangian::createFile()
{
	//std::cout << "New file" << std::endl;
	file.open("Files/lagrangian.txt", std::ios::trunc | std::ios::out);
	file << "--- particles ---" << std::endl;
	file << "0000000000 particles" << std::endl;
	file.close();
	file.open("Files/lagrangian_points.txt", std::ios::trunc | std::ios::out);
	file << "--- points ---" << std::endl;
	file << "0000000000 points" << std::endl;
	file.close();
}

void lagrangian::writePoint(const float x, const float y, const float z)
{
	pointsFile.open("Files/lagrangian_points.txt", std::ios::out | std::ios::in);
	
	std::string number = std::to_string(last_point+1);
	std::ostringstream ostr;
	ostr << std::setfill('0') << std::setw(10) << number;
	char charNumber[11];
	std::strcpy(charNumber,ostr.str().c_str());

	pointsFile.seekg(15u);
	pointsFile.write(charNumber, 10);
	pointsFile.close();

	//std::fstream outfile;
	//outfile.open("Files/replace_points.txt", std::ios::trunc | std::ios::out);

	//int destination = 1;
	//GotoLine(file, outfile, &destination);
	//int place = file.tellg();
	//std::string line;
	//std::getline(file, line);
	//outfile << last_point + 1 << " points" << std::endl;

	//CopyToFinal(file, outfile);

	//outfile.close();
	//file.close();

	//std::rename("Files/replace_points.txt","Files/lagrangian_points.txt");

	pointsFile.open("Files/lagrangian_points.txt",std::ios::app);
	pointsFile << last_point << " : " << (bottom - x)*scale << " " << (left - y)*scale << " " << z*scale << std::endl;
	pointsFile.close();
	last_point++; //one more point
}	

void lagrangian::writeNewParticle()
{
	file.open("Files/lagrangian.txt", std::ios::out | std::ios::in);

	std::string number = std::to_string(last_label+1);
	std::ostringstream ostr;
	ostr << std::setfill('0') << std::setw(10) << number;
	char charNumber[11];
	std::strcpy(charNumber,ostr.str().c_str());

	file.seekg(18u);
	file.write(charNumber, 10);
	file.close();

	//file.open("Files/lagrangian.txt", std::ios::out | std::ios::in);
	
	//std::fstream outfile;
	//outfile.open("Files/replace.txt", std::ios::trunc | std::ios::out);

	//int destination = 1;
	//GotoLine(file, outfile, &destination);
	//int place = file.tellg();
	//std::string line;
	//std::getline(file, line);
	//outfile << last_label + 1 << " particles" << std::endl;

	//file.seekg(place);
	//if (last_label == 0)
	//	file.seekg(place);
	
	//destination = last_label - 1;
	//GotoLine(file, outfile, &destination);
	//std::string stringc;
	//std::getline(file, stringc); //skip to end of line
	//if (last_label == 0)
	//	stringc = "";
	//else
	//	stringc = stringc + "\n";
	//stringc = stringc + std::to_string(last_label) + " :";
	//outfile << stringc << std::endl;

	//CopyToFinal(file, outfile);

	//outfile.close();
	//file.close();
	
	file.open("Files/lagrangian.txt", std::ios::out | std::ios::app);
	file << last_label << " :" << std::endl;
	file.close();

	last_label++;

	//std::rename("Files/replace.txt","Files/lagrangian.txt");
}	

void lagrangian::writeToMultipleParticles(std::vector<int>& orderedParticles, std::vector<float>& orderedX, std::vector<float>& orderedY, std::vector<float>& orderedZ)
{
	file.open("Files/lagrangian.txt", std::ios::in);

	std::fstream outfile;
	outfile.open("Files/replace.txt", std::ios::out | std::ios::trunc);
	
	std::string line;

	std::getline(file, line);
	outfile << line << std::endl;
	std::getline(file, line);
	outfile << line << std::endl;            //copy first 2 lines

	int destination = 2;
	for (int i = 0; i < orderedParticles.size(); ++i)
	//for (int i = 0; i < 2; ++i)
	{
		//std::cout << orderedParticles[i] << std::endl;

		if (i > 0)
			destination = orderedParticles[i] - orderedParticles[i-1] - 1;
		else
			destination = (orderedParticles[i] + 2) - destination;
		GotoLine(file, outfile, &destination);
		std::getline(file, line);
		outfile << line;
		outfile << " " + std::to_string(last_point) << std::endl;

		writePoint(orderedX[i], orderedY[i], orderedZ[i]);
	}

	CopyToFinal(file, outfile);

	file.close();
	outfile.close();

	std::rename("Files/replace.txt","Files/lagrangian.txt");
}

void lagrangian::SingleFile(int frame, std::vector<int>& orderedParticles, std::vector<float>& orderedX, std::vector<float>& orderedY, std::vector<float>& orderedZ)
{
	std::string number = std::to_string(last_label+1);
	std::ostringstream ostr;
	ostr << std::setfill('0') << std::setw(10) << frame;

	file.open("Files/Temp/frame" + ostr.str() + ".txt", std::ios::out | std::ios::trunc);

	for (int i = 0; i < orderedParticles.size(); i++)
	{
		file << orderedParticles[i] << " : " << std::to_string(last_point) << std::endl;

		writePoint(orderedX[i], orderedY[i], orderedZ[i]);
	}

	file.close();
}

void lagrangian::SingleFileDouble(int frame, std::vector<int>& orderedParticles, std::vector<std::pair<float,float>>& orderedX, std::vector<std::pair<float,float>>& orderedY,
	std::vector<std::pair<float,float>>& orderedZ)
{
	std::string number = std::to_string(last_label+1);
	std::ostringstream ostr;
	ostr << std::setfill('0') << std::setw(10) << frame;

	file.open("Files/Temp/frame" + ostr.str() + "d.txt", std::ios::out | std::ios::trunc);

	for (int i = 0; i < orderedParticles.size(); i++)
	{
		file << orderedParticles[i] << " : " << std::to_string(last_point);

		writePoint(orderedX[i].first, orderedY[i].first, orderedZ[i].first);

		file << " " << std::to_string(last_point) << std::endl;

		writePoint(orderedX[i].second, orderedY[i].second, orderedZ[i].second);
	}

	file.close();
}
void lagrangian::writeToMultipleParticlesDouble(std::vector<int>& orderedParticles, std::vector<std::pair<float,float>>& orderedX, std::vector<std::pair<float,float>>& orderedY,
	std::vector<std::pair<float,float>>& orderedZ)
{
	file.open("Files/lagrangian.txt", std::ios::in);

	std::fstream outfile;
	outfile.open("Files/replace.txt", std::ios::out | std::ios::trunc);
	
	std::string line;

	std::getline(file, line);
	outfile << line << std::endl;
	std::getline(file, line);
	outfile << line << std::endl;            //copy first 2 lines

	int destination = 2;
	for (int i = 0; i < orderedParticles.size(); ++i)
	//for (int i = 0; i < 2; ++i)
	{
		//std::cout << orderedParticles[i] << std::endl;

		if (i > 0)
			destination = orderedParticles[i] - orderedParticles[i-1] - 1;
		else
			destination = (orderedParticles[i] + 2) - destination;
		GotoLine(file, outfile, &destination);
		std::getline(file, line);
		outfile << line;
		outfile << " " + std::to_string(last_point);

		writePoint(orderedX[i].first, orderedY[i].first, orderedZ[i].first);

		outfile << " " + std::to_string(last_point) << std::endl;

		writePoint(orderedX[i].second, orderedY[i].second, orderedZ[i].second);
	}

	CopyToFinal(file, outfile);

	file.close();
	outfile.close();

	std::rename("Files/replace.txt","Files/lagrangian.txt");
}

void lagrangian::writeToParticle(int& particle, const float x, const float y, const float z)
{
	file.open("Files/lagrangian.txt", std::ios::in | std::ios::out);

	std::fstream outfile;
	outfile.open("Files/replace.txt", std::ios::trunc | std::ios::out);

	int destination = particle + 1;
	GotoLine(file, outfile, &destination);
	std::string line;
	std::getline(file, line);
	outfile << line;
	outfile << " " + std::to_string(last_point) << std::endl;

	CopyToFinal(file, outfile);

	outfile.close();
	file.close();

	std::rename("Files/replace.txt","Files/lagrangian.txt");

	writePoint(x,y,z);
}

static std::vector<std::string> globVector(const std::string& pattern){
    glob_t glob_result;
	//const std::filesystem::path ImagesPath{ pattern };
    glob(pattern.c_str(),GLOB_TILDE,NULL,&glob_result);
	std::vector<std::string> files;
    //for(auto const& dir_entry : std::filesystem::directory_iterator{ImagesPath}){
	for(unsigned int i=0;i<glob_result.gl_pathc;++i){
        files.push_back(std::string(glob_result.gl_pathv[i]));
    }
    globfree(&glob_result);
    return files;
}

void lagrangian::mergeFiles()
{
	std::cout << "Merging files" << std::endl;
	
	std::vector<std::string> filesNames = globVector("Files/Temp/frame*.txt");

	file.open("Files/lagrangian.txt", std::ios::out | std::ios::trunc);
	
	std::string number = std::to_string(last_label+1);
	std::ostringstream ostr;
	ostr << std::setfill('0') << std::setw(10) << last_label;
	
	file << "--- particles ---" << std::endl;
	file << ostr.str() << " particles" << std::endl;

	std::string line, partial;
	int currentLabel;

	for (int label = 0; label < last_label; label++)
	{
		file << std::to_string(label) << " :";
		for (int i = 0; i < filesNames.size(); i++)
		{
			std::fstream currentFile;
			//std::cout << filesNames[i] << std::endl;
			currentFile.open(filesNames[i], std::ios::in);
			
			//read last line
			currentFile.seekg(-1,std::ios::end);
			int endFile = currentFile.tellg();
			int index = 1;
			bool keepLooking = true;
			while (keepLooking)
			{
				currentFile.seekg(endFile - index, std::ios::beg);
				char ch;
				currentFile.get(ch);

				if ((int)currentFile.tellg() <= 0)
				{
					currentFile.seekg(0);
					keepLooking = false;
				}
				else if (ch == '\n')
				{
					keepLooking = false;
				}
				index++;
			}
			//std::getline(currentFile, line);
			if (endFile == -1)
				currentLabel = -1;
			else
				currentFile >> currentLabel;
			
			//std::cout << filesNames[i] << " , " << endFile << " : " << currentLabel << std::endl;
			if (currentLabel < label)
				continue;

			currentFile.seekg(0);

			while (std::getline(currentFile,line))
			{
				std::stringstream ss(line);
				ss >> currentLabel;
				if (currentLabel > label)
					break;
				ss >> line; ss >> line;
				while (ss >> partial)
					line = line + " " + partial;

				if (currentLabel == label)
				{
					file << " " << line;
					break;
				}
			}
			currentFile.seekg(0);
			currentFile.close();
		}
		file << std::endl;
	}
	
	file.close();
}

void lagrangian::firstPass(std::vector<circles_data>& points)
{
	createFile();
	labels.reserve(points.size());
	usedlabels.reserve(points.size());
	possiblePoints.reserve(points.size());

	std::vector<int> labelsWrite;
	std::vector<float> xWrite,yWrite,zWrite;

	for (int i = 0; i < points.size(); i++)
	{
		//std::cout << circle.x << " , " << circle.y << std::endl;
		//writeNewParticle();
		//writeToParticle(last_label, circle.x, circle.y, 0);
		labels.push_back(-1*i);
		singlePoints.push_back({points[i].x,points[i].y});
		//labelsWrite.push_back(i);
		//xWrite.push_back(points[i].x);
		//yWrite.push_back(points[i].y);
		//zWrite.push_back(0);
		//labels.push_back(last_label);
		//possiblePoints.push_back({0,0});
	}

	//last_label = points.size()-1;

	SingleFile(0,labelsWrite,xWrite,yWrite,zWrite);
}

void lagrangian::updateLabels(cv::Mat* exibir, std::vector<circles_data>& points, std::vector<circles_data>& pastPoints, int* lines_assignment, int lines_size)
{
	frame++;

	//for (int i = 0; i < labels.size(); i++)
	//	std::cout << labels[i] << " ";
	//std::cout << std::endl;
	//for (int i = 0; i < lines_size; i++)
	//	std::cout << lines_assignment[i] << " ";
	//std::cout << std::endl;

	std::vector<int> new_labels; new_labels.reserve(points.size());
	std::vector<std::pair<float,float>> possiblePointsNew; possiblePointsNew.reserve(points.size());
	std::vector<int> possiblePointsPositionNew; possiblePointsPositionNew.reserve(points.size());
	std::vector<std::pair<float,float>> kalmanPointsNew; kalmanPointsNew.reserve(points.size());
	std::vector<std::pair<float,float>> kalmanMidlePointsNew; kalmanMidlePointsNew.reserve(points.size());
	std::vector<int> kalmanLabelsNew; kalmanLabelsNew.reserve(points.size());
	std::vector<std::pair<float,float>> singleParticlesNew;

	std::vector<int> orderedParticles;
	std::vector<int> orderedParticlesDouble;
	std::vector<float> orderedX, orderedY, orderedZ;
	std::vector<std::pair<float,float>> orderedXDouble, orderedYDouble, orderedZDouble;

	usedlabels.clear();
	usedlabels.reserve(labels.size());
	for (int i = 0; i < labels.size(); i++)
		usedlabels.push_back(false);

	for (int i = 0; i < lines_size; ++i)
	{
		if (lines_assignment[i] == -1)
			continue;
		else 
		{
			//writeToParticle((labels[lines_assignment[i]]) , points[i].x, points[i].y, 0);
			int xProjected = 2*points[i].x - pastPoints[lines_assignment[i]].x;
			int yProjected = 2*points[i].y - pastPoints[lines_assignment[i]].y;
			int xKalman = 3*points[i].x - 2*pastPoints[lines_assignment[i]].x;
			int yKalman = 3*points[i].y - 2*pastPoints[lines_assignment[i]].y;
			
			possiblePointsNew.push_back({xProjected, yProjected});
			possiblePointsPositionNew.push_back(i);

			kalmanPointsNew.push_back({xKalman, yKalman});
			kalmanMidlePointsNew.push_back({xProjected, yProjected});

//			std::cout << i << " , " << lines_assignment[i] << std::endl;
			makeFigureUpdate(exibir, points[i], pastPoints[lines_assignment[i]], last_label);

			if (labels[lines_assignment[i]] <= 0)
			{
				int index = 1 - labels[lines_assignment[i]];
				kalmanLabelsNew.push_back(last_label);
				orderedParticlesDouble.push_back(last_label);
				orderedXDouble.push_back({singlePoints[index].first, points[i].x});
				orderedYDouble.push_back({singlePoints[index].second, points[i].y});
				orderedZDouble.push_back({0,0});
				new_labels.push_back(last_label);
				last_label++;
			}
			else
			{
				kalmanLabelsNew.push_back(labels[lines_assignment[i]]);
			
				orderedParticles.push_back(labels[lines_assignment[i]]);
				orderedX.push_back(points[i].x);
				orderedY.push_back(points[i].y);
				orderedZ.push_back(0);
				new_labels.push_back(labels[lines_assignment[i]]);
			}	

			Temperature.addParticle(-1*(points[i].y - pastPoints[lines_assignment[i]].y)*scale, -1*(points[i].x - pastPoints[lines_assignment[i]].x)*scale, (bottom-points[i].x)*scale, (left-points[i].y)*scale);
			VelocityX.addParticle(-1*(points[i].y - pastPoints[lines_assignment[i]].y)*scale, (bottom - points[i].x)*scale, (left - points[i].y)*scale);
			VelocityY.addParticle(-1*(points[i].x - pastPoints[lines_assignment[i]].x)*scale, (bottom - points[i].x)*scale, (left - points[i].y)*scale);
			
			usedlabels[lines_assignment[i]] = true;
		}
	}
	for (int i = 0; i < lines_size; ++i)
	{
		if (lines_assignment[i] == -1)
		{
			bool foundCorrespondent = false;
			for (int j = 0; j < possiblePoints.size(); j++)
			{
				if (possiblePoints[j].first == 0)
					continue;
				if (usedlabels[j])
					continue;
				int dx = points[i].x - possiblePoints[j].first;
				int dy = points[i].y - possiblePoints[j].second;
				float dist = sqrt(pow(dx,2)+pow(dy,2));
				if (dist < 3)
				{
					//std::cout << "Entered " << labels.size() << " , " << possiblePoints.size() << " : " << j << " , " << labels[j] << std::endl;
					orderedParticles.push_back(labels[j]);
					//std::cout << labels[j] << " , " << labels[possiblePointsPositions[j]] << std::endl;
					//orderedParticlesDouble.push_back(labels[j]);
					orderedX.push_back(points[i].x);
					//orderedXDouble.push_back({pastPoints[possiblePointsPositions[j]].x, points[i].x});
					//orderedXDouble.push_back({0,0});
					orderedY.push_back(points[i].y);
					//orderedYDouble.push_back({pastPoints[possiblePointsPositions[j]].y, points[i].y});
					//orderedYDouble.push_back({0,0});
					orderedZ.push_back(0);
					//orderedZDouble.push_back({0, 0});
					new_labels.push_back(labels[j]);


					int xProjected = 2*points[i].x - pastPoints[possiblePointsPositions[j]].x;
					int yProjected = 2*points[i].y - pastPoints[possiblePointsPositions[j]].y;
					int xKalman = 3*points[i].x - 2*pastPoints[possiblePointsPositions[j]].x;
					int yKalman = 3*points[i].y - 2*pastPoints[possiblePointsPositions[j]].y;

					possiblePointsNew.push_back({xProjected, yProjected});
					possiblePointsPositionNew.push_back(i);

					kalmanPointsNew.push_back({xKalman, yKalman});
					kalmanMidlePointsNew.push_back({xProjected, yProjected});
					kalmanLabelsNew.push_back(labels[j]);

					makeFigureUpdate(exibir, points[i], pastPoints[possiblePointsPositions[j]], last_label);

					Temperature.addParticle(-1*(points[i].y - pastPoints[possiblePointsPositions[j]].y)*scale, -1*(points[i].x - pastPoints[possiblePointsPositions[j]].x)*scale, (bottom - points[i].x)*scale, (left-points[i].y)*scale);
					VelocityX.addParticle(-1*(points[i].y - pastPoints[possiblePointsPositions[j]].y)*scale, (bottom - points[i].x)*scale, (left - points[i].y)*scale);
					VelocityY.addParticle(-1*(points[i].x - pastPoints[possiblePointsPositions[j]].x)*scale, (bottom - points[i].x)*scale, (left - points[i].y)*scale);

					foundCorrespondent = true;
					usedlabels[j] = true;
					break;
				}
			}

			if (not foundCorrespondent)
			{
				for (int j = 0; j < kalmanPoints.size(); j++)
				{
					if (kalmanPoints[j].first == 0)
						continue;
					int dx = points[i].x - kalmanPoints[j].first;
					int dy = points[i].y - kalmanPoints[j].second;
					float dist = sqrt(pow(dx,2)+pow(dy,2));
					if (dist < 3)
					{
						bool skip = false;
						for (int k = 0; k < labels.size(); k++)
						{
							if (labels[k] == kalmanLabel[j] && usedlabels[k])
							{
								skip = true;
								break;
							}
						}
						if (skip)
							continue;
						
						for (int k = 0; k < singlePoints.size(); k++)
						{
							int dxi = kalmanMidlePoints[j].first - singlePoints[k].first;
							int dyi = kalmanMidlePoints[j].second - singlePoints[k].second;
							float disti = sqrt(pow(dxi,2)+pow(dyi,2));
							if (disti < 3)
							{
								singlePoints.erase(singlePoints.begin() + k);
							}
						}

						//std::cout << "Entrou " << frame << std::endl;
						//std::cout << "Entered " << labels.size() << " , " << possiblePoints.size() << " : " << j << " , " << labels[j] << std::endl;
						//orderedParticles.push_back(labels[j]);
						orderedParticlesDouble.push_back(kalmanLabel[j]);
						//orderedX.push_back(points[i].x);
						orderedXDouble.push_back({kalmanMidlePoints[j].first, points[i].x});
						//orderedXDouble.push_back({0,0});
						//orderedY.push_back(points[i].y);
						orderedYDouble.push_back({kalmanMidlePoints[j].second, points[i].y});
						//orderedYDouble.push_back({0,0});
						//orderedZ.push_back(0);
						orderedZDouble.push_back({0, 0});
						new_labels.push_back(kalmanLabel[j]);


						int xProjected = 2*points[i].x - kalmanMidlePoints[j].first;
						int yProjected = 2*points[i].y - kalmanMidlePoints[j].second;
						int xKalman = 3*points[i].x - 2*kalmanMidlePoints[j].first;
						int yKalman = 3*points[i].y - 2*kalmanMidlePoints[j].second;

						possiblePointsNew.push_back({xProjected, yProjected});
						possiblePointsPositionNew.push_back(i);

						kalmanPointsNew.push_back({xKalman, yKalman});
						kalmanMidlePointsNew.push_back({xProjected, yProjected});
						kalmanLabelsNew.push_back(kalmanLabel[j]);

						circles_data currentPoint(kalmanMidlePoints[j].first, kalmanMidlePoints[j].second, 0, false, 1);
						makeFigureUpdate(exibir, points[i], currentPoint, last_label);

						Temperature.addParticle(-1*(points[i].y - kalmanMidlePoints[j].second)*scale, -1*(points[i].x - kalmanMidlePoints[j].first)*scale, (bottom - points[i].x)*scale, (left-points[i].y)*scale);
						VelocityX.addParticle(-1*(points[i].y - kalmanMidlePoints[j].second)*scale, (bottom - points[i].x)*scale, (left - points[i].y)*scale);
						VelocityY.addParticle(-1*(points[i].x - kalmanMidlePoints[j].first)*scale, (bottom - points[i].x)*scale, (left - points[i].y)*scale);

						foundCorrespondent = true;
						for (int k = 0; k < labels.size(); k++)
						{
							if (kalmanLabel[j] == labels[k])
								usedlabels[k] = true;
						}
						break;
					}
				}
			}

			if (not foundCorrespondent)
			{
				//writeNewParticle();
				//writeToParticle(last_label, points[i].x, points[i].y, 0);
				singleParticlesNew.push_back({points[i].x,points[i].y});
				//orderedParticles.push_back(last_label); last_label++;
				//orderedX.push_back(points[i].x);
				//orderedY.push_back(points[i].y);
				//orderedZ.push_back(0);
				//last_label++;
				new_labels.push_back(-1*singleParticlesNew.size());
				//possiblePointsNew.push_back({0,0});
				//possiblePointsPositionNew.push_back(0);
				//kalmanPointsNew.push_back({0,0});
				//kalmanMidlePointsNew.push_back({0,0});
				//kalmanLabelsNew.push_back(0);
				//makeFigureNew(exibir, points[i], last_label);
			}
		}
	}

	//for (int i = 0; i < singlePoints.size(); i++)
	//{
	//	orderedParticles.push_back(last_label); last_label++;
	//	orderedX.push_back(singlePoints[i].first);
	//	orderedY.push_back(singlePoints[i].second);
	//	orderedZ.push_back(0);
	//	new_labels.push_back(last_label);
	//	possiblePointsNew.push_back({0,0});
	//	possiblePointsPositionNew.push_back(0);
	//	kalmanPointsNew.push_back({0,0});
	//	kalmanMidlePointsNew.push_back({0,0});
	//	kalmanLabelsNew.push_back(0);
	//	//circles_data current(singlePoints[i].first, singlePoints[i].second, 0, false);
	//	//makeFigureNew(exibir, current, last_label);
	//}

	orderVectors(orderedParticles, orderedX, orderedY, orderedZ);
	//writeToMultipleParticles(orderedParticles, orderedX, orderedY, orderedZ);
	SingleFile(frame, orderedParticles, orderedX, orderedY, orderedZ);

	orderVectors(orderedParticlesDouble, orderedXDouble, orderedXDouble, orderedZDouble);
	//writeToMultipleParticlesDouble(orderedParticlesDouble, orderedXDouble, orderedYDouble, orderedZDouble);
	SingleFileDouble(frame, orderedParticlesDouble, orderedXDouble, orderedYDouble, orderedZDouble);

	Temperature.consolidateField();
	Temperature.writeFrame(frame);
	VelocityX.consolidateField();
	VelocityX.writeFrame(frame);
	VelocityY.consolidateField();
	VelocityY.writeFrame(frame);

	labels = new_labels;
	possiblePointsPositions = possiblePointsPositionNew;
	possiblePoints = possiblePointsNew;
	kalmanLabel = kalmanLabelsNew;
	kalmanMidlePoints = kalmanMidlePointsNew;
	kalmanPoints = kalmanPointsNew;
	singlePoints = singleParticlesNew;
}

float lagrangian::velocitySquared(circles_data& p1, circles_data& p2)
{
	float dx2 = pow((p1.x - p2.x)*scale, 2);
	float dy2 = pow((p1.y - p2.y)*scale, 2);
	return dx2 + dy2;
}

void lagrangian::merge(int* array, int const left, int const mid, int const right, std::vector<int>& particles)
{
	auto const subArrayOne = mid - left + 1;
	auto const subArrayTwo = right - mid;

	// Create temp arrays
	auto *leftArray = new int[subArrayOne], *rightArray = new int[subArrayTwo];

	// Copy data to temp arrays leftArray[] and rightArray[]
	for (auto i = 0; i < subArrayOne; i++)
		leftArray[i] = array[left + i];
	for (auto j = 0; j < subArrayTwo; j++)
		rightArray[j] = array[mid + 1 + j];

	auto indexOfSubArrayOne = 0, // Initial index of first sub-array
	indexOfSubArrayTwo = 0; // Initial index of second sub-array
	int indexOfMergedArray = left; // Initial index of merged array

	// Merge the temp arrays back into array[left..right]
	while (indexOfSubArrayOne < subArrayOne && indexOfSubArrayTwo < subArrayTwo) {
		if (particles[leftArray[indexOfSubArrayOne]] <= particles[rightArray[indexOfSubArrayTwo]]) {
			array[indexOfMergedArray] = leftArray[indexOfSubArrayOne];
			indexOfSubArrayOne++;
		}
		else {
			array[indexOfMergedArray] = rightArray[indexOfSubArrayTwo];
			indexOfSubArrayTwo++;
		}
		indexOfMergedArray++;
	}
	// Copy the remaining elements of
	// left[], if there are any
	while (indexOfSubArrayOne < subArrayOne) {
		array[indexOfMergedArray] = leftArray[indexOfSubArrayOne];
		indexOfSubArrayOne++;
		indexOfMergedArray++;
	}
	// Copy the remaining elements of
	// right[], if there are any
	while (indexOfSubArrayTwo < subArrayTwo) {
		array[indexOfMergedArray] = rightArray[indexOfSubArrayTwo];
		indexOfSubArrayTwo++;
		indexOfMergedArray++;
	}
	delete[] leftArray;
	delete[] rightArray;
}

// begin is for left index and end is
// right index of the sub-array
// of arr to be sorted */
void lagrangian::mergeSort(int* array, int const begin, int const end, std::vector<int>& particles)
{
	if (begin >= end)
		return; // Returns recursively

	auto mid = begin + (end - begin) / 2;
	mergeSort(array, begin, mid, particles);
	mergeSort(array, mid + 1, end, particles);
	merge(array, begin, mid, end, particles);
}

template <typename T>
void lagrangian::orderVectors(std::vector<int>& particles, std::vector<T>& x, std::vector<T>& y, std::vector<T> z)
{
	int* labelsArray = new int[particles.size()];
	for (int i = 0; i < particles.size(); ++i)
	{
		labelsArray[i] = i;
	}

	mergeSort(labelsArray, 0, particles.size()-1, particles);

	std::vector<int> returnParticles;
	std::vector<T> returnX, returnY, returnZ;
	for (int i = 0; i < particles.size(); ++i)
	{
		returnParticles.push_back(particles[labelsArray[i]]);
		returnX.push_back(x[labelsArray[i]]);
		returnY.push_back(y[labelsArray[i]]);
		returnZ.push_back(z[labelsArray[i]]);
	}

	particles = returnParticles;
	x = returnX;
	y = returnY;
	z = returnZ;
}

void lagrangian::makeFigureNew(cv::Mat* exibir, circles_data& point, int& label)
{
	cv::circle(*exibir, cv::Point(point.x, point.y), 5, cv::Scalar(0,0,255), 0.1, 8);
	//cv::putText(*exibir, std::to_string(label), cv::Point(point.x, point.y), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0,0,255));
}

void lagrangian::makeFigureUpdate(cv::Mat* exibir, circles_data& point, circles_data& pastPoint, int& label)
{
//	std::cout << pastPoint.x << " , " << pastPoint.y << " -> " << point.x << " , " << point.y << std::endl;
	cv::arrowedLine(*exibir, cv::Point(pastPoint.x,pastPoint.y), cv::Point(point.x, point.y), cv::Scalar(0,255,255));
//	cv::circle(*exibir, cv::Point(point.x, point.y), 5, cv::Scalar(0,255,0), 0.1, 8);
	//cv::putText(*exibir, std::to_string(label), cv::Point(point.x, point.y), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0,255,0));
}



