#include "matrix.h"

#include "hungarian.h"

hungarian::hungarian(double dist)
	:frame0(1,1),frame1(1,1),connections(1,1),assignment(1,1),prohibted(1,1),max_dist(dist)
{
}

hungarian::~hungarian(){}

double hungarian::dist(matrix p1, matrix p2) //matrizes linha
{
	double dx2 = pow(p1.get(0,0) - p2.get(0,0),2);
	double dy2 = pow(p1.get(0,1) - p2.get(0,1),2);
	//std::cout << dx2 << " , " << dy2 << std::endl;
	//std::cout << pow(dx2 + dy2,0.5) << std::endl;
	return pow(dx2 + dy2,0.5);
}

bool hungarian::read_files()
{
std::ifstream file;
std::string linha;
file.open("circulos.txt");
int circulos;
file >> circulos;
std::getline(file,linha);
//std::cout << circulos << " circulos" << std::endl;

//circulos = 10;
frame1 = matrix(circulos,2);
assignment = matrix(circulos,2);
for (int i = 0; i < circulos; ++i) 
{
	assignment.set(i,0,i);
	assignment.set(i,1,-1);
}

double x, y;		
for (int i = 0; i < circulos; ++i) {
	file >> x >> y;
	//std::cout << x << " , " << y << std::endl;
	frame1.set(i,0,x);
	frame1.set(i,1,y);
}

file.close();
//frame1.print();

file.open("circulos_ant.txt");
if (!file) 
{
	return true;
}
file >> circulos;
std::getline(file,linha);
//std::cout << circulos << " circulos" << std::endl;

//circulos = 10;
frame0 = matrix(circulos,2);

for (int i = 0; i < circulos; ++i) {
	file >> x >> y;
	//std::cout << x << " , " << y << std::endl;
	frame0.set(i,0,x);
	frame0.set(i,1,y);
}

file.close();
return false;
}

void hungarian::atualiza_arquivos()
{
std::ifstream atual;
std::ofstream antigo;

atual.open("circulos.txt", std::ios::in);
antigo.open("circulos_ant.txt", std::ios::trunc);

antigo << atual.rdbuf();
}

void hungarian::build_connections()
{
int num_circ_0 = frame0.lins;
	int num_circ_1 = frame1.lins;
connections = matrix(num_circ_1, num_circ_0);

for (int i = 0; i < num_circ_1; ++i) 
{
	for (int j = 0; j < num_circ_0; j++) 
	{
		connections.set(i,j,dist(frame1.slice(i,i+1,0,2),frame0.slice(j,j+1,0,2))*1.0);
		//frame0.slice(j,j+1,0,2).print();	
	}
}
}

int hungarian::find_minimum()
{ 
matrix zeros(0,3), col(connections.cols,1), lin(connections.lins,1);
// step 1
for (int i = 0; i < connections.lins; ++i) 
{
	int min_place; double min_value;
	min(connections, 0, i, min_place, min_value);
	for (int j = 0; j < connections.cols; j++) 
	{
		connections.set(i,j,connections.get(i,j)-min_value);
	}
}
make_assignments(zeros);
if (check_assignment(zeros)) 
{
	return 1;
}

//step 2
for (int i = 0; i < connections.cols; ++i) 
{
	int min_place; double min_value;
	min(connections, 1, i, min_place, min_value);
	for (int j = 0; j < connections.lins; j++) 
	{
		connections.set(j,i,connections.get(j,i)-min_value);
	}
}
make_assignments(zeros);
if (check_assignment(zeros)) 
{
	return 1;	
}

//loop
int count = 0;
make_assignments(zeros);
while (not(check_assignment(zeros))) //por not quando finalizado 
{
	//step 3
	//zeros.print();
	//lin.print(); col.print();
	//make_assignments(zeros);
	col = matrix(connections.cols,1); lin = matrix(connections.lins,1);
	for (int i = 0; i < zeros.lins; ++i) 
	{
		if (zeros.get(i,2) == 1) 
		{
			col.set(zeros.get(i,1),0,1); //cover column
		}
	}
	for (int i = 0; i < zeros.lins; ++i) 
	{
	//std::cout << i << "/" << zeros.lins << std::endl;
	if (zeros.get(i,2) == 0 && col.get(zeros.get(i,1),0) == 0 && lin.get(zeros.get(i,0),0) == 0) //uncoverd 0 not stared
		{
			//zeros.print();
			//lin.print(); col.print();
			zeros.set(i,2,2); //stared
			for (int j = 0; j < zeros.lins; ++j)
			{
				if (i != j && zeros.get(j,2) == 1 && zeros.get(i,0) == zeros.get(j,0)) //stared 0 in the same row
				{
					lin.set(zeros.get(i,0),0,1); //cover line
					col.set(zeros.get(j,1),0,0); //uncover column
					//lin.print(); col.print();
					i = 0;
					break;
				}
			}
			for (int j = 0; j < zeros.lins; ++j)
			{
				if (i != j && zeros.get(j,2) == 1 && zeros.get(i,1) == zeros.get(j,1)) //stared 0 in the same column 
				{
					find_path(i, zeros);
					zeros.set(i,2,1);
					for (int k = 0; k < zeros.lins; ++k) 
					{
						if (zeros.get(k,2) == 2) 
						{
							zeros.set(k,2,0);
						}
					}
					lin = matrix(connections.lins,1);
					col = matrix(connections.cols,1);
					i = -1;
					for (int k = 0; k < zeros.lins; ++k) 
					{
						if (zeros.get(k,2) == 1) 
						{
							col.set(zeros.get(k,1),0,1); //cover column
						}
					}
					break;
				}
				//lin.set(zeros.get(i,0),0,1); //cover line
			}
		}
	}

	//zeros.print();
	//col.print(); lin.print();
	//step 4

	double minimo = pow(10,10);
	for (int i = 0; i < connections.lins; ++i) 
	{
		for (int j = 0; j < connections.cols; ++j) 
		{
			if (lin.get(i,0) == 0 && col.get(j,0) == 0 && connections.get(i,j) < minimo) 
			{
				minimo = connections.get(i,j);
			}
		}
	}
	for (int i = 0; i < connections.lins; ++i) 
	{
		for (int j = 0; j < connections.cols; ++j)
		{
			if (lin.get(i,0) == 1 && col.get(j,0) == 1) 
			{
				bool flag = (connections.get(i,j) == 0) ? true : false;
				connections.set(i,j, connections.get(i,j) + minimo);
				if (connections.get(i,j) != 0 && flag) 
				{
					for (int k = 0; k < zeros.lins; ++k) 
					{
						if (zeros.get(k,0) == i && zeros.get(k,1) == j) 
						{
							//std::cout << "Tirou " << k << std::endl;
							zeros = zeros.pop(0,k);
							break;
						}
					}	
				}
			}
			else if (lin.get(i,0) == 0 && col.get(j,0) == 0) 
			{
				bool flag = (connections.get(i,j) == 0) ? false : true;
				connections.set(i,j, connections.get(i,j) - minimo);
				if (connections.get(i,j) == 0 && flag) 
				{
					//std::cout << "adicionou" << std::endl;
					matrix zero(1,3);
					zero.set(0,0,i);zero.set(0,1,j);zero.set(0,2,0);
					zeros.append(zero);
				}
			}
		}
	}
	for (int k = 0; k < zeros.lins; ++k) 
	{
		if (zeros.get(k,2) == 2) 
			{
				zeros.set(k,2,0);
			}
	}
	for (int i = 0; i < zeros.lins; ++i) 
	{
		if (zeros.get(i,2) == 0) 
		{
			bool flag = true;
			for (int j = 0; j < zeros.lins; ++j) 
			{
				if (i != j && zeros.get(j,2) == 1 && zeros.get(i,0) == zeros.get(j,0)) 
				{
					flag = false;
					break;
				}
				else if (i != j && zeros.get(j,2) == 1 && zeros.get(i,1) == zeros.get(j,1)) 
				{
					flag = false;
					break;
				}
			}
			if (flag) 
			{
				zeros.set(i,2,1);
			}
			else
			{
				zeros.set(i,2,0);
			}
		}
	}


	//std::cout << "Saiu: " << count << std::endl;

	//connections.print();
	//make_assignments(zeros);
	//zeros.print();
	
	if (count > 100) 
	{
		return -1;
	}

	count++;
}

make_assignments(zeros);
clear_prohibiteds(zeros);
return 1;
}

static std::mutex s_zeros;

static void check(matrix* zeros, int index, int* count)
{
	if (zeros->get(index,2) == 1)
	{
		std::lock_guard<std::mutex> lock(s_zeros);
		(*count) = (*count) + 1;
	}
}

bool hungarian::check_assignment(matrix& zeros)
{
	std::vector<std::future<void>> future_count;

	int target = (connections.lins < connections.cols) ? connections.lins : connections.cols;
	//std::cout << target << std::endl;

	int count=0;
	for (int i = 0; i < zeros.lins; ++i) 
	{
		future_count.push_back(std::async(std::launch::async, check, &zeros, i, &count));
	}
	
	//std::cout << "Zeros: " << count << std::endl;
	if (count < target) 
	{
		return false;
	}

	return true;
}

void hungarian::find_path(int index, matrix& zeros)
{
	for (int ci = 0; ci < zeros.lins; ++ci) 
	{
		bool flag = false;
		if (index != ci && zeros.get(ci,2) == 1 && zeros.get(index,1) == zeros.get(ci,1)) //stared 0 in the same column 
		{
			for (int cj = 0; cj < zeros.lins; cj++) 
			{
				if (cj != ci && zeros.get(cj,2) == 2 && zeros.get(cj,0) == zeros.get(ci,0)) //primed zero in the same row 
				{															      
					//std::cout << "Entrou " << k << " , " << j << " , " << i << std::endl;
					zeros.set(cj,2,1);
					zeros.set(ci,2,0);
					find_path(cj, zeros);
					flag = true;
					break;
				}
			}
		}
		if (flag) 
		{
			break;
		}
	}
}

void hungarian::build_prohibiteds(double cut)
{
	prohibted = matrix(1,2);
	prohibted.set(0,0,-1); prohibted.set(0,1,-1);

	for (int i = 0; i < connections.lins; ++i) 
	{
		for (int j = 0; j < connections.cols; j++) 
		{
			if (connections.get(i,j) >= cut) 
			{
				//connections.set(i,j,big_value);
				matrix exclude(1,2);
				exclude.set(0,0,i); exclude.set(0,1,j);
				prohibted.append(exclude);
			}
		}
	}
	//prohibted.print();
}

void hungarian::clear_prohibiteds(matrix& zeros)
{
	for (int i = 0; i < zeros.lins; ++i) 
	{
		if (zeros.get(i,2) == 1) 
		{
			bool flag = true;
			for (int j = 0; j < prohibted.lins; ++j) 
			{
				//std::cout << prohibted.get(j,0) << " , " << prohibted.get(j,1) << std::endl;
				if (prohibted.get(j,0) == zeros.get(i,0) && prohibted.get(j,1) == zeros.get(i,1)) 
				{
					flag = false;
					break;
				}
			}
			//std::cout << zeros.get(i,0) << " : " << flag << std::endl;
			if (flag) 
			{
				assignment.set(zeros.get(i,0),1,zeros.get(i,1));
			}
			else
			{
				assignment.set(zeros.get(i,0),1,-1);
			}
		}
	}


}

void hungarian::make_assignments(matrix& zeros)
{
	zeros = matrix(0,3);
	matrix zero(1,3);

	for (int i = 0; i < connections.lins; ++i) 
	{
		for (int j = 0; j < connections.cols; j++) 
		{
			if (connections.get(i,j) == 0) 
			{
				zero.set(0,0,i);
				zero.set(0,1,j);
				zero.set(0,2,1);
				for (int k = 0; k < zeros.lins; k++) 
				{
					if (zeros.get(k,1) == j && zeros.get(k,2) == 1) 
					{
						zero.set(0,2,0);
						break;
					}
					if (zeros.get(k,0) == i && zeros.get(k,2) == 1) 
					{
						zero.set(0,2,0);
						break;
					}
				}
				zeros.append(zero);

			}
		}
	}
}

void hungarian::make_figure(const char* path)
{
	using namespace cv;
	Mat src = imread(path, IMREAD_COLOR);

	for (int i = 0; i < assignment.lins; ++i) 
	{
		if (assignment.get(i,1) != -1) 
		{
			Point start(frame0.get(assignment.get(i,1),0),frame0.get(assignment.get(i,1),1));
			Point end(frame1.get(i,0),frame1.get(i,1));

			line(src, start, end,
					Scalar(0,0,256), //color
					2, //thickness
					LINE_8); //type
		}
	}

	imshow("circles path", src);
	waitKey(1);
}

void hungarian::run()
{
	bool first = read_files();
	if (!first) 
	{
		build_connections();
		build_prohibiteds(max_dist);
		int status = find_minimum();
		std::cout << "Status: " << status << std::endl;
		atualiza_arquivos();
	}
	else
	{
		atualiza_arquivos();
	}
}

void hungarian::run_test()
{
	build_prohibiteds(max_dist);

	matrix a(5,5);
	a.set(0,0,1);a.set(0,1,2);a.set(0,2,2);a.set(0,3,2);a.set(0,4,4);	
	a.set(1,0,2);a.set(1,1,1);a.set(1,2,2);a.set(1,3,2);a.set(1,4,4);
	a.set(2,0,2);a.set(2,1,2);a.set(2,2,2);a.set(2,3,1);a.set(2,4,4);
	a.set(3,0,2);a.set(3,1,2);a.set(3,2,1);a.set(3,3,2);a.set(3,4,1);
	a.set(4,0,2);a.set(4,1,3);a.set(4,2,3);a.set(4,3,3);a.set(4,4,4);

	connections = matrix(a);
	assignment = matrix(5,2);
	for (int i = 0; i < 5; ++i) 
	{
		assignment.set(i,0,i);
		assignment.set(i,1,-1);
	}
	int ok = find_minimum();

	connections.print();
	assignment.print();
}
