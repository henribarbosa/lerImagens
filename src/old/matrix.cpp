#include <cmath>
#include <iostream>

#include "matrix.h"

matrix::matrix(const int lins, const int cols)
    :lins(lins), cols(cols), mat(lins * cols)
{
    
    for (int i = 0; i < lins * cols; i++)
    {
        mat[i] = 0;
    }
    
}

matrix::matrix(const matrix& other)
    :lins(other.lins), cols(other.cols), mat(other.mat)
{}

matrix eye(const int n)
{
    matrix retorno(n,n);
    for (int i = 0; i < n; i++)
    {
        retorno.set(i,i,1.0);
    }
    return retorno;
}

matrix::~matrix()
{
    //std::cout << "Matriz deletada" << std::endl;
    //delete[] mat;
}

float matrix::get(int lin, int col)
{
    return mat[lin * cols + col];
}

float matrix::get(int lin, int col) const
{
    return mat[lin * cols + col];
}

void matrix::set(int lin, int col, float val)
{
    mat[lin * cols + col] = val;
}

matrix matrix::mult(const matrix& other)
{
    // check sizes
    if (cols != other.lins)
    {
        std::cout << "Tamanhos nao correspondentes" << std::endl;
        return matrix(0,0);
    }

    matrix retorno(lins,other.cols);
    for (int i = 0; i < lins; i++)
    {
        for (int j = 0; j < other.cols; j++)
        {
            float sum = 0;
            for (int k = 0; k < cols; k++)
            {
                sum += get(i,k) * other.get(k,j);
            }
            retorno.set(i, j, sum);
        }   
    }
    return retorno;
}

matrix matrix::mult(matrix& other)
{
    // check sizes
    if (cols != other.lins)
    {
        std::cout << "Tamanhos nao correspondentes" << std::endl;
        return matrix(0,0);
    }

    matrix retorno(lins,other.cols);
    for (int i = 0; i < lins; i++)
    {
        for (int j = 0; j < other.cols; j++)
        {
            float sum = 0;
            for (int k = 0; k < cols; k++)
            {
                sum += get(i,k) * other.get(k,j);
            }
            retorno.set(i, j, sum);
        }   
    }
    return retorno;
}

matrix matrix::mult(float x)
{
    matrix retorno(lins,cols);
    for (int i = 0; i < lins; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            retorno.set(i,j, get(i,j) * x);
        }
    }
    return retorno;        
}

matrix matrix::operator*(matrix& other)
{
    return mult(other);
}

matrix matrix::operator*(const matrix& other)
{
    return mult(other);
}

matrix matrix::operator*(float x)
{
    return mult(x);
}

matrix matrix::T()
{
    matrix retorno(cols, lins);
    for (int i = 0; i < lins; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            retorno.set(j, i, matrix::get(i,j));
        }
    }

    return retorno;
}

void matrix::print()
{
    for (int i = 0; i < lins; i++)
    {
        for (int j = 0; j < cols-1; j++)
        {
            std::cout << get(i,j) << " , ";
        }
        std::cout << get(i,cols-1) << std::endl;
    }
    
}

matrix matrix::add(matrix& other)
{
    if (!(lins == other.lins && cols == other.cols))
    {
        std::cout << "Tamanhos nao correspondentes" << std::endl;
        return matrix(0,0);
    }

    matrix retorno(lins,cols);
    
    for (int i = 0; i < lins; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            //std::cout << get(i,j) << " , " << other.get(i,j) << std::endl;
            retorno.set(i, j, get(i,j) + other.get(i,j));
        }
    }
    return retorno;     
}

matrix matrix::operator+(matrix& other)
{
    return add(other);
}

matrix matrix::join(matrix& other)
{
    matrix retorno(lins, 2*cols);
    for (int i = 0; i < lins; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            retorno.set(i,j,get(i,j));
            retorno.set(i,j+cols,other.get(i,j));
        }
    }

    return retorno;
}

matrix matrix::split(const int indice)
{
    matrix retorno(lins,cols/2);
    for (int i = 0; i < lins; i++)
    {
        for (int j = 0; j < cols/2; j++)
        {
            retorno.set(i,j,get(i,j+indice*cols/2));
        }
    }
    
    return retorno;
}

bool matrix::equal(matrix& other)
{
	if (lins != other.lins) 
	{
		return false;
	}
	if (cols != other.cols) 
	{
		return false;
	}
	for (int i = 0; i < lins; ++i) 
	{
		for (int j = 0; j < cols; j++) 
		{
			if (get(i,j) != other.get(i,j)) 
			{
				return false;
			}
		}
	}
	return true;
}

bool matrix::operator==(matrix& other)
{
	return equal(other);
}

matrix matrix::slice(const int lin_i, const int lin_f, const int col_i, const int col_f)
{
    matrix retorno(lin_f - lin_i, col_f - col_i);
    for (int i = 0; i < lin_f-lin_i; i++)
    {
        for (int j = 0; j < col_f-col_i; j++)
        {
            retorno.set(i,j,get(i+lin_i, j+col_i));
        }
    }
    return retorno;
}

void matrix::append(matrix join)
{
	if (cols != join.cols) 
	{
		std::cout << "Tamanhos nao correspondentes" << std::endl;
		//return matrix(1,1);
	}
	
    lins += join.lins;
    for (int i = 0; i < join.lins; i++)
    {
        for (int j = 0; j < join.cols; j++)
        {
            mat.push_back(join.get(i, j));
        }
    }
}

matrix matrix::pop(int axis, int index)
{
	matrix retorno(1,1);
	if (axis == 0) 
	{
		if (index >= lins) 
		{
			std::cout << "Index outside matrix" << std::endl;
			return retorno;
		}
		retorno = matrix(lins-1,cols);
		int k = 0;
		for (int i = 0; i < lins-1; ++i) 
		{
			if (i == index) 
			{
				k++;
			}
			for (int j = 0; j < cols; ++j) 
			{
				retorno.set(i,j,get(k,j));
			}
			k++;
		}
	}
	else if (axis == 1) 
	{
		if (index >= cols) 
		{
			std::cout << "Index outside matrix" << std::endl;
			return retorno;
		}
		retorno = matrix(lins,cols-1);
		int k = 0;
		for (int i = 0; i < cols-1; ++i) 
		{
			if (i == index) 
			{
				k++;
			}
			for (int j = 0; j < lins; ++j) 
			{
				retorno.set(j,i,get(j,k));
			}
			k++;
		}
		

	}
	else
	{
		std::cout << "Axis not suported, use 0 for lines or 1 for columns" << std::endl;
		matrix retorno(1,1);
	}

	return retorno;
}

matrix thomas(matrix& A, matrix& v) //A[n,n] v[n,1]
{
    int n = A.lins;

    matrix cl(n,1), dl(n,1), sol(n,1);

    //step 1
    cl.set(0,0,A.get(0,1)/A.get(0,0));
    dl.set(0,0,v.get(0,0)/A.get(0,0));

    //step 2->n-1
    for (int i = 1; i < n-1; i++)
    {
        cl.set(i,0,A.get(i,i+1)/(A.get(i,i) - A.get(i,i-1) * cl.get(i-1,0)));
        dl.set(i,0,(v.get(i,0) - A.get(i,i-1) * dl.get(i-1,0))/
            (A.get(i,i) - A.get(i,i-1) * cl.get(i-1,0)));
    }

    //step n
    dl.set(n-1,0,(v.get(n-1,0) - A.get(n-1,n-2) * dl.get(n-2,0))/
            (A.get(n-1,n-1) - A.get(n-1,n-2) * cl.get(n-2,0)));

    //sol n
    sol.set(n-1,0,dl.get(n-1,0));

    //sol n-1 -> 1
    for (int i = n-2; i >= 0; i--)
    {
        sol.set(i,0,dl.get(i,0) - cl.get(i,0) * sol.get(i+1,0));
    }
    
    return sol;
}

matrix thomas_p(matrix& A, matrix& v) //A[n,n] v[n,1]
{
    int n = A.lins;
    matrix simple = A.slice(0,n-2,0,n-2); //simple.print();
    matrix y1 = v.slice(0,n-2,0,1); //y1.T().print();
    matrix y2 = A.slice(0,n-2,n-1,n) * (-1.0); //y2.T().print();
    matrix y3 = A.slice(0,n-2,n-2,n-1) * (-1.0); //y3.T().print();

    matrix sol1 = thomas(simple, y1);
    matrix sol2 = thomas(simple, y2);
    matrix sol3 = thomas(simple, y3);

    double a11 = A.get(n-1,n-2) + (A.slice(n-1,n,0,n-2) * sol3).get(0,0);
    double a12 = A.get(n-1,n-1) + (A.slice(n-1,n,0,n-2) * sol2).get(0,0);
    double a21 = A.get(n-2,n-2) + (A.slice(n-2,n-1,0,n-2) * sol3).get(0,0);
    double a22 = A.get(n-2,n-1) + (A.slice(n-2,n-1,0,n-2) * sol2).get(0,0);
    double z1 = v.get(n-1,0) - (A.slice(n-1,n,0,n-2) * sol1).get(0,0);
    double z2 = v.get(n-2,0) - (A.slice(n-2,n-1,0,n-2) * sol1).get(0,0);

    matrix retorno(n,1);
    retorno.set(n-2,0,(a22 * z1 - a12 * z2)/(a11*a22 - a12*a21));
    retorno.set(n-1,0,(a11 * z2 - a21 * z1)/(a11*a22 - a12*a21));

    for (int i = 0; i < n-2; i++)
    {
        retorno.set(i,0,sol1.get(i,0) + retorno.get(n-1,0) * sol2.get(i,0) + retorno.get(n-2,0) * sol3.get(i,0));
    }

    return retorno;

}

void min(matrix mat, int axis, int index, int& min_place, double& min_value)
{
	if (axis == 0) 
	{
		matrix line = mat.slice(index,index+1,0,mat.cols);
		min_value = line.get(0,0);
		min_place = 0;
		for (int i = 1; i < mat.cols; ++i) 
		{
			if (line.get(0,i) < min_value) 
			{
				min_place = i;
				min_value = line.get(0,i);
			}	
		}	
	}
	else if (axis == 1) {
		matrix column = mat.slice(0,mat.lins,index,index+1);
		min_value = column.get(0,0);
		min_place = 0;
		for (int i = 1; i < mat.lins; ++i) 
		{
			if (column.get(0,i) < min_value) 
			{
				min_place = i;
				min_value = column.get(0,i);
			}
		}
	}

	else
	{
		std::cout << "Axis not defined, use 0 for line and 1 for column" << std::endl;
	}
}

void max(matrix mat, int axis, int index, int& max_place, double& max_value)
{
	if (axis == 0) 
	{
		matrix line = mat.slice(index,index+1,0,mat.cols);
		max_value = line.get(0,0);
		max_place = 0;
		for (int i = 1; i < mat.cols; ++i) 
		{
			if (line.get(0,i) > max_value) 
			{
				max_place = i;
				max_value = line.get(0,i);
			}	
		}	
	}
	else if (axis == 1) {
		matrix column = mat.slice(0,mat.lins,index,index+1);
		max_value = column.get(0,0);
		max_place = 0;
		for (int i = 1; i < mat.lins; ++i) 
		{
			if (column.get(0,i) > max_value) 
			{
				max_place = i;
				max_value = column.get(0,i);
			}
		}
	}

	else
	{
		std::cout << "Axis not defined, use 0 for line and 1 for column" << std::endl;
	}
}
