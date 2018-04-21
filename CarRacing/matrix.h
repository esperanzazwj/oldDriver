#ifndef _MATRIX_H
#define _MATRIX_H
#include <iostream>
#include <fstream>
#include "string.h"
class MATRIX
{
private:
	int row,col; /*���������������*/ 
	void CreateMatrix(); /*���󴴽�*/ 
	void CopyMatrix(const MATRIX &obj); /*������*/ 
	void DestroyMatrix(); /*��������*/ 
public:
	static int DEFAULT_ROW,DEFAULT_COL;
	double **m; /*��������*/ 
	MATRIX(int r=DEFAULT_ROW,int c=DEFAULT_COL); /*һ�㹹�캯��*/ 
	MATRIX(const MATRIX &obj); /*���ƹ��캯��*/ 
	~MATRIX();
	double* &operator[](int r) const; /*�����±������*/ 
	MATRIX &operator=(const MATRIX &obj); /*���¾�Ϊ�����������*/ 
	MATRIX operator-() const;
	MATRIX operator+(const MATRIX &obj) const;
	MATRIX operator-(const MATRIX &obj) const;
	MATRIX &operator+=(const MATRIX &obj);
	MATRIX &operator-=(const MATRIX &obj);
	MATRIX operator*(double k) const;
	MATRIX operator/(double k) const;
	MATRIX &operator*=(double k);
	MATRIX &operator/=(double k);	
	friend MATRIX operator*(double k,const MATRIX &obj);
	MATRIX operator*(const MATRIX &obj) const;
	MATRIX &operator*=(const MATRIX &obj); /*----------------*/ 
	operator double() const {return m[1][1];} /*����ǿ������ת��*/ 
	MATRIX gettrans() const; /*���ת�þ���*/ 
	MATRIX getsub(int r1,int r2,int c1,int c2) const; /*�������*/ 
	void cover(int r1, int r2, int c1, int c2, const MATRIX &obj); /*��Ŀ��������ݸ�������ǰ����[r1:r2,c1:c2]λ����*/ 
	MATRIX submul(int r1, int r2, int c1, int c2, const MATRIX &obj) const; /*��ȡ��ǰ����[r1:r2,c1:c2]�Ӿ�����Ŀ��������*/ 
	void reset(int r,int c); /*���þ�����к���*/ 
	int getrow() const; /*�������*/ 
	int getcol() const; /*�������*/ 
	void print(); /*�������Ļ*/ 
	void print(std::ofstream &fout); /*�����ָ���ļ�*/ 
};

int MATRIX::DEFAULT_ROW=1;
int MATRIX::DEFAULT_COL=1;

void MATRIX::CreateMatrix()
{
	m=new double*[row+1];
	m[0]=NULL;
	for (int i=1;i<=row;i++)
	{
		m[i]=new double[col+1];
		memset(m[i],0,sizeof(double)*(col+1));
	}
}
void MATRIX::CopyMatrix(const MATRIX &obj)
{
	for (int i=1;i<=row;i++)
		for (int j=1;j<=col;j++)
			m[i][j]=obj.m[i][j];
}
void MATRIX::DestroyMatrix()
{
	for (int i=1;i<=row;i++)
		delete []m[i];
	delete []m;
}
MATRIX::MATRIX(int r,int c):row(r),col(c) 
{
	CreateMatrix();
}
MATRIX::MATRIX(const MATRIX &obj):row(obj.row),col(obj.col)
{
	CreateMatrix();
	CopyMatrix(obj);
}
MATRIX::~MATRIX()
{
	DestroyMatrix();
}
double* &MATRIX::operator[](int r) const
{
	if (r>row || r<=0) 
		std::cout<<"Error!Out of Range!"<<std::endl;
	else	
		return m[r];
};
MATRIX &MATRIX::operator=(const MATRIX &obj)
{
	if (row!=obj.row || col!=obj.col)
	{
		DestroyMatrix();
		row=obj.row;col=obj.col;
		CreateMatrix();
	}
	CopyMatrix(obj);
	return *this;
}
MATRIX MATRIX::operator-() const
{
	MATRIX tmp(row,col);
	for (int i=1;i<=row;i++) 
		for (int j=1;j<=col;j++)
			tmp.m[i][j]=-m[i][j];
	return tmp;
}
MATRIX MATRIX::operator+(const MATRIX &obj) const
{
	MATRIX tmp(row,col);
	if (row!=obj.row || col!=obj.col)
		std::cout<<"Error!Different Size!"<<std::endl;
	else 
		for (int i=1;i<=row;i++) 
			for (int j=1;j<=col;j++)
				tmp.m[i][j]=m[i][j]+obj.m[i][j];
	return tmp;
}
MATRIX MATRIX::operator-(const MATRIX &obj) const
{
	MATRIX tmp(row,col);
	if (row!=obj.row || col!=obj.col)
		std::cout<<"Error!Different Size!"<<std::endl;
	else 
		for (int i=1;i<=row;i++) 
			for (int j=1;j<=col;j++)
				tmp.m[i][j]=m[i][j]-obj.m[i][j];
	return tmp;	
}
MATRIX &MATRIX::operator+=(const MATRIX &obj)
{
	if (row!=obj.row || col!=obj.col)
		std::cout<<"Error!Different Size!"<<std::endl;
	else 
		for (int i=1;i<=row;i++) 
			for (int j=1;j<=col;j++)
				m[i][j]+=obj.m[i][j];
	return *this;
}
MATRIX &MATRIX::operator-=(const MATRIX &obj)
{
	if (row!=obj.row || col!=obj.col)
		std::cout<<"Error!Different Size!"<<std::endl;
	else 
		for (int i=1;i<=row;i++) 
			for (int j=1;j<=col;j++)
				m[i][j]-=obj.m[i][j];
	return *this;
}
MATRIX MATRIX::operator*(double k) const
{
	MATRIX tmp(row,col);
	for (int i=1;i<=row;i++) 
		for (int j=1;j<=col;j++)
			tmp.m[i][j]=m[i][j]*k;
	return tmp;	
}
MATRIX MATRIX::operator/(double k) const
{
	MATRIX tmp(row,col);
	for (int i=1;i<=row;i++)
		for (int j=1;j<=col;j++)
			tmp.m[i][j]=m[i][j]/k;
	return tmp;
}
MATRIX &MATRIX::operator*=(double k)
{
	for (int i=1;i<=row;i++) 
		for (int j=1;j<=col;j++)
			m[i][j]*=k;
	return *this;
}
MATRIX &MATRIX::operator/=(double k)
{
	for (int i=1;i<=row;i++) 
		for (int j=1;j<=col;j++)
			m[i][j]/=k;
	return *this;
}
MATRIX operator*(double k,const MATRIX &obj)
{
	return obj*k;
}
MATRIX MATRIX::operator*(const MATRIX &obj) const
{
	MATRIX tmp(row,obj.col);
	if (col!=obj.row) 
		std::cout<<"Error!Cannot Multiple!"<<std::endl;
	else
		for (int i=1;i<=row;i++)
			for (int j=1;j<=obj.col;j++)
				for (int k=1;k<=col;k++)
					tmp.m[i][j]+=m[i][k]*obj.m[k][j];
	return tmp;	
}
MATRIX &MATRIX::operator*=(const MATRIX &obj)
{
	*this=*this*obj;
	return *this;	
}
MATRIX MATRIX::gettrans() const
{
	MATRIX tmp(col,row);
	for (int i=1;i<=col;i++)
		for (int j=1;j<=row;j++)
			tmp[i][j]=(*this)[j][i];
	return tmp;
}
MATRIX MATRIX::getsub(int r1,int r2,int c1,int c2) const
{
	int trow=r2-r1+1;
	int tcol=c2-c1+1;
	MATRIX tmp(trow,tcol);
	for (int i=1;i<=trow;i++)
		for (int j=1;j<=tcol;j++)
			tmp.m[i][j]=m[i+r1-1][j+c1-1];
	return tmp;
}
void MATRIX::cover(int r1, int r2, int c1, int c2, const MATRIX &obj)
{
	for (int i=r1;i<=r2;i++)
		for (int j=c1;j<=c2;j++)
			m[i][j]=obj.m[i-r1+1][j-c1+1];
}
MATRIX MATRIX::submul(int r1, int r2, int c1, int c2, const MATRIX &obj) const
{
	int r=r2-r1+1,c=c2-c1+1;
	MATRIX tmp(r2-r1+1,c2-c1+1);
	if (c!=obj.row)
		std::cout<<"Error!Cannot Multiple!"<<std::endl;
	else 
		for (int i=1;i<=r;i++)
			for (int j=1;j<=obj.col;j++)
				for (int k=1;k<=c;k++)
					tmp.m[i][j]+=m[i+r1-1][k+c1-1]*obj.m[k][j];
	return tmp;					
}	
void MATRIX::reset(int r,int c)
{
	DestroyMatrix();
	row=r;col=c;
	CreateMatrix();
}
int MATRIX::getrow() const
{
	return row;	
} 
int MATRIX::getcol() const
{
	return col;	
} 
void MATRIX::print()
{
	using namespace std;
	cout.precision(5);
	for (int i=1;i<=row;i++)
	{
		for (int j=1;j<=col;j++)
		{
			cout.width(6);
			cout<<m[i][j]<<" ";
		}
		cout<<std::endl;
	}
}
void MATRIX::print(std::ofstream &fout)
{
	using namespace std;
	fout.precision(5);
	for (int i=1;i<=row;i++)
	{
		for (int j=1;j<=col;j++)
		{
			fout.width(6);
			fout<<m[i][j]<<" ";
		}
		fout<<endl;
	}
}
#endif
