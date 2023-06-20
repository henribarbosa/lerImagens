#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include "tclap/CmdLine.h"

#include "../include/altura.h"
#include "../include/circulos.h"
#include "../include/auction.h"
#include "../include/preProcess.h"
#include "../include/read_control.h"
#include "../include/mixing.h"
#include "../include/simulation.h"


#if defined _WIN32
#include <filesystem>

std::vector<std::string> globVector(const std::string& pattern){
    //glob_t glob_result;
	const std::filesystem::path ImagesPath{ pattern };
    //glob(pattern.c_str(),GLOB_TILDE,NULL,&glob_result);
	std::vector<std::string> files;
    for(auto const& dir_entry : std::filesystem::directory_iterator{ImagesPath}){
	//for(unsigned int i=0;i<glob_result.gl_pathc;++i){
        files.push_back(std::string(dir_entry.path().generic_string()));
    }
    //globfree(&glob_result);
    return files;
}
#else
#include <glob.h>

std::vector<std::string> globVector(const std::string& pattern){
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
#endif

int SimulationProcess(std::string FramesPath, std::string Method, bool bidisperse)
{
	std::vector<std::string> files = globVector(FramesPath);
	
	Simulation_data simulation;

	for (std::string file : files)
	{
		simulation.readFile(file, Method, bidisperse);
	}

}

int PreProcessing(std::string Method, std::string CalibrationPath, std::string ImagesPath)
{
	if (Method == "all")
	{
		std::vector<std::string> files = globVector(CalibrationPath);

		MakeAveragePicture(files);

		files = globVector(ImagesPath);

		SubtractMask(files);
	}
	else if (Method == "build_mask")
	{
		std::vector<std::string> files = globVector(CalibrationPath);

		MakeAveragePicture(files);
	}
	else if (Method == "apply_mask")
	{
		std::vector<std::string> files = globVector(ImagesPath);

		SubtractMask(files);
	}
	else if (Method == "binary")
	{
		std::vector<std::string> files = globVector(ImagesPath);

		MakeBinary(files);
	}
	else if (Method == "rotate")
	{
		std::vector<std::string> files = globVector(ImagesPath);

		RotateImages(files);
	}

	return 0;
}

int Processing(std::string ImagesPath, std::string Method, bool bidisperse)
{
	std::vector<std::string> files = globVector(ImagesPath);
	lagrangian particle_track;
	int bottom, right, left;
	int height = 0, interfacePosition;
	double scale; // mm/pixel

	int ret = system("rm Files/circulos_ant.txt");
	ret = system("rm -r Files/Temp");
	ret = system("mkdir Files/Temp");
	firstPass(files[0].c_str(), bottom, right, left);

	control BedWidth("thresholds.txt", "BedWidth");
	scale = BedWidth.returnThreshold() / (left - right);
	particle_track.setScale(scale);
	particle_track.setBoundaries(bottom, left);
	particle_track.startFields();

	Mixing mixing_track(bottom, left, scale);
	
	if (Method == "expansion" || Method == "plugs" || Method == "all")
	{
		std::ofstream file;
		file.open("Files/Alturas.txt",std::ios::trunc);
		file << "";
		file.close();

		file.open("Files/Interface.txt", std::ios::trunc);
		file << "";
		file.close();

		cv::namedWindow("Expansao leito", cv::WINDOW_NORMAL);
		cv::resizeWindow("Expansao leito", 1200,50);
	}
	if (Method == "plugs" || Method == "all")
	{
		std::ofstream file;
		file.open("Files/Plugs.txt", std::ios::trunc);
		file << "";
		file.close();
	}
	if (Method == "temperature" || Method == "all")
	{

		bool status = remove("circulos_ant.txt");

		cv::namedWindow("detected circles", cv::WINDOW_NORMAL);
		cv::resizeWindow("detected circles", 1200,50);

		cv::namedWindow("circles path", cv::WINDOW_NORMAL);
		cv::resizeWindow("circles path", 1200,50);

		std::ofstream file;
		file.open("Files/mixing_number.txt", std::ios::trunc);
		file << "";
		file.close();

	}

	if (Method == "expansion")
	{
		for (int i = 0; i < files.size(); i++)
		{
			cv::Mat image = cv::imread(files[i].c_str(), cv::IMREAD_GRAYSCALE);
			cv::Mat exibir(image.rows, image.cols, CV_8UC3);
			cv::cvtColor(image, exibir, cv::COLOR_GRAY2RGB);

			altura(&image, &exibir, height);
			cv::Rect bed(height, right, bottom - height, left - right);
			//interface(&image, &exibir, bed, interfacePosition);

			if (bidisperse)
			{
				cv::Point rightInterface, leftInterface;
//				interfaceNonPerpendicular(&image, &exibir, bed, leftInterface, rightInterface);
				mixing_track.interfaceMixing(&exibir, leftInterface, rightInterface, i);
//
				float interfaceHeight = (bottom - (rightInterface.x + leftInterface.x)/2.0) * scale;
				float interfaceAngle = std::atan( float(rightInterface.x - leftInterface.x) / float(rightInterface.y - leftInterface.y) );

				std::ofstream file;
				file.open("Files/Interface.txt", std::ios::app);
				file << interfaceHeight << "   " << interfaceAngle << std::endl;
				file.close();
			}

			cv::imshow("Expansao leito", exibir);

			cv::waitKey(1);
			char tempo[6];
			memcpy(tempo, files[i].c_str() + strlen(files[i].c_str()) - 9, 5);
			tempo[5] = 0;

			std::ofstream file;
			file.open("Files/Alturas.txt",std::ios::app);
			file << tempo << "   " << (bottom - height) * scale << std::endl;
			file.close();
		}
	}
	else if (Method == "plugs")
	{
		for (int i = 0; i < files.size(); i++)
		{
			cv::Mat image = cv::imread(files[i].c_str(), cv::IMREAD_GRAYSCALE);
			cv::Mat exibir;
			cv::cvtColor(image, exibir, cv::COLOR_GRAY2RGB);
		
			altura(&image, &exibir, height);
			cv::Rect bed(height, right, bottom - height, left - right);
			std::vector<cv::Rect> rectangles;
			plugs(&image, &exibir, bed, rectangles, bottom, scale);

			cv::imshow("Expansao leito", exibir);
			cv::waitKey(1);

		}
	}
	else if (Method == "temperature")
	{
		for (int i = 0; i < files.size(); i++)
		{
			cv::Mat image = cv::imread(files[i].c_str(), cv::IMREAD_GRAYSCALE);
			cv::Mat exibir(image.rows, image.cols, CV_8UC3);
			cv::cvtColor(image, exibir, cv::COLOR_GRAY2RGB);
			cv::Mat exibirTrajectories; exibir.copyTo(exibirTrajectories);

			altura(&image, &exibir, height);
			cv::Rect bed(height, right, bottom - height, left - right);
			std::vector<cv::Rect> rectangles;
			plugs(&image, &exibir, bed, rectangles, bottom, scale);

			circulos(&image, &exibir, bed, rectangles);
			//cv::Mat blackWhite;
			//cv::inRange(image(bed), 200, 255, blackWhite);
			//generalMoviment(&blackWhite, &exibir, &bed, rectangles);
			control LagrangianThreshold("thresholds.txt","AuctionMaxDist");
			auction pathFinder(LagrangianThreshold.returnThreshold());
			pathFinder.run(&exibirTrajectories, particle_track);

			cv::imshow("detected circles", exibir);
			cv::imshow("circles path", exibirTrajectories);
			cv::waitKey(1);
		}
	}
	else if (Method == "all")
	{
		for (int i = 0; i < files.size(); i++)
		{
			cv::Mat image = cv::imread(files[i].c_str(), cv::IMREAD_GRAYSCALE);
			cv::Mat exibir(image.rows, image.cols, CV_8UC3);
			cv::cvtColor(image, exibir, cv::COLOR_GRAY2RGB);
			cv::Mat exibirCircles; exibir.copyTo(exibirCircles);
			cv::Mat exibirTrajectories; exibir.copyTo(exibirTrajectories);
			
			altura(&image, &exibir, height);
			cv::Rect bed(height, right, bottom - height, left - right);
			//interface(&image, &exibir, bed, interfacePosition);
			if (bidisperse)
			{
				cv::Point rightInterface, leftInterface;
				mixing_track.interfaceMixing(&exibir, leftInterface, rightInterface, i);
//				interfaceNonPerpendicular(&image, &exibir, bed, leftInterface, rightInterface);

				float interfaceHeight = (bottom - (rightInterface.x + leftInterface.x)/2.0) * scale;
				float interfaceAngle = std::atan( float(rightInterface.x - leftInterface.x) / float(rightInterface.y - leftInterface.y) );

				std::ofstream file;
				file.open("Files/Interface.txt", std::ios::app);
				file << interfaceHeight << "   " << interfaceAngle << std::endl;
				file.close();
			}		

			std::vector<cv::Rect> rectangles;
			plugs(&image, &exibir, bed, rectangles, bottom, scale);
	
			char tempo[6];
			memcpy(tempo, files[i].c_str() + strlen(files[i].c_str()) - 9, 5);
			tempo[5] = 0;

			std::ofstream file;
			file.open("Files/Alturas.txt",std::ios::app);
			file << tempo << "   " << (bottom - height) * scale << std::endl;
			file.close();
	
			circulos(&image, &exibirCircles, bed, rectangles);
			control LagrangianThreshold("thresholds.txt","AuctionMaxDist");
			auction pathFinder(LagrangianThreshold.returnThreshold());
			pathFinder.run(&exibirTrajectories, particle_track);
				
//			exibir = quantizeImage(&image, 3);
			std::ostringstream ostr;
			ostr << std::setfill('0') << std::setw(6) << i;
			cv::imshow("Expansao leito", exibir);
			cv::imshow("detected circles", exibirCircles);
			cv::imshow("circles path", exibirTrajectories);
			if (i % 2 == 0) {
				cv::imwrite("Expansao_Images/image_"+ostr.str()+".png", exibir);
				cv::imwrite("Circles_Images/image_"+ostr.str()+".png", exibirCircles);
				cv::imwrite("Vectors_Images/image_"+ostr.str()+".png", exibirTrajectories);
			}
			cv::waitKey(1);

			int barWidth = 40;
			float progress = ((float)i / (float)(files.size()-1));

			std::cout << "[";
			int pos = int(barWidth * progress);
//			std::cout << progress << " " << pos << std::endl;
			for (int j = 0; j < barWidth; ++j) {
				if (j < pos) std::cout << "=";
				else if (j == pos) std::cout << ">";
				else std::cout << " ";
			}
			std::cout << "] " << int(progress * 100.0) << " %\r";
			std::cout.flush();
		}
		std::cout << std::endl;
	}

	//particle_track.mergeFiles();

	return 0;

}

int PostProcessing(std::string Method, std::string PostFolder)
{
	if (Method == "all")
	{
			int ret = system(("python3 src/postProcess.py -all " + PostFolder).c_str());
	}
	else if (Method == "expansion")
	{
		int ret = system(("python3 src/postProcess.py -expansion " + PostFolder).c_str());
	}
	else if (Method == "temperature")
	{
		int ret = system(("python3 src/postProcess.py -temperature " + PostFolder).c_str());
	}
	else if (Method == "plugs")
	{
		int ret = system(("python3 src/postProcess.py -plugs " + PostFolder).c_str());
	}

	return 0;
}

int main(int argc, char** argv)
{
	//Define useful variaables
	std::string ImagesPath, Mode, CalibrationPath, PreMethod, RunMethod, PostMethod, PostFolder;
	bool bidisperse;

	try
	{
		// Build command line arguments for this program
		TCLAP::CmdLine cmd("Program to process fluidized bed images",' ',"1.0");

		// Requires a path to access the images
		TCLAP::ValueArg<std::string> ImagesNamePattern("p", "ImagesNamePattern", "Images name [p]attern to search, includes the folder path", true, "", "string");
		cmd.add(ImagesNamePattern);

		// Path to calibration images
		TCLAP::ValueArg<std::string> CalibrationImagesNamePattern("c", "CalibrationNamePattern", "[C]alibration images name pattern to search, includes the folder path", false, "", "string");
		cmd.add(CalibrationImagesNamePattern);

		// Path to post-process file
		TCLAP::ValueArg<std::string> PostProcessFilesFolder("f", "PostProcessFolder", "[F]older of post process files", false, "", "string");
		cmd.add(PostProcessFilesFolder);

		// Execution mode
		TCLAP::ValueArg<std::string> ExecutionMode("m", "mode", "Execution [m]ode: simulation, pre, process or post", true, "process", "string");
		cmd.add(ExecutionMode);

		// Methods called to process
		TCLAP::ValueArg<std::string> ProcessMethod("r", "RunMethod", "Methods to [r]un (all, expansion, temperature, plugs)", false, "all", "string");
		cmd.add(ProcessMethod);
		
		// Type of bed
		TCLAP::ValueArg<bool> Bidisperse("t", "BedType", "[T]ype of bed to process (true for bidisperse)", false, false, "bool");
		cmd.add(Bidisperse);

		//Methods called to pre-process
		TCLAP::ValueArg<std::string> PreProcessMethod("b", "BuildMethod", "Methods to [b]uild files (all, build_mask, apply_mask, rotate)", false, "all", "string");
		cmd.add(PreProcessMethod);

		// Methods called to post-process data
		TCLAP::ValueArg<std::string> PostProcessMethod("g", "GraphMethods", "Methods to post-process to [g]raphs (all, expansion, temperature, plugs)", false, "all", "string");
		cmd.add(PostProcessMethod);

		// Reads the arguments
		cmd.parse(argc, argv);

		// Put values on the variables
		ImagesPath = ImagesNamePattern.getValue();
		CalibrationPath = CalibrationImagesNamePattern.getValue();
		Mode = ExecutionMode.getValue();
		bidisperse = Bidisperse.getValue();
		PreMethod = PreProcessMethod.getValue();
		RunMethod = ProcessMethod.getValue();
		PostMethod = PostProcessMethod.getValue();
		PostFolder = PostProcessFilesFolder.getValue();

	}
	catch (TCLAP::ArgException &e)
	{
		std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
		return 0;
	}

	// Processing simulations
	if (Mode == "simulation")
	{
		int ok = SimulationProcess(ImagesPath, RunMethod, bidisperse);
	}
	// Pre-processing
	else if (Mode == "pre")
	{
		int ok = PreProcessing(PreMethod, CalibrationPath, ImagesPath);
	}
	// Processing
	else if (Mode == "process")
	{
		int ok = Processing(ImagesPath, RunMethod, bidisperse);
	}
	// Post-processing
	else if (Mode == "post")
	{
		int ok = PostProcessing(PostMethod, PostFolder);
	}
	// Wrong mode
	else
	{
		std::cout << "Wrong Method Specified, use: pre, process or post" << std::endl;
	}
}
