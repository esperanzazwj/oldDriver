#ifndef _STUFF_H_
#define _STUFF_H_
#include "matrix.h"
MATRIX eye(int n)  
{
	MATRIX e(n,n);
	for (int i=1;i<=n;i++) e[i][i]=1;
	return e;
}
int sgn(double x)
{
	if (x>0) return 1;
		else if (x<0) return -1;
			else return 0;
}
MATRIX eig_Jacobi(const MATRIX &A, MATRIX &Q)
{
	int times=0;
	int i,j,p,q,time,t,n;
	double z,c,s,k,*b,*d;
	MATRIX ans(A);
	n=A.getrow();Q=eye(n);
	p=1;q=2;k=A[1][1];time=n*(n-1)/2;
	b=new double[n+1];d=new double[n+1];
	do
	{
		do		/*确定旋转平面*/ 
		{
			t=time;
			while (fabs(ans.m[p][q])<=k && t>0)
			{
				q++;t--;
				if (q>n) {p++;q=p+1;}
				if (p>=n) {p=1;q=2;}
			}
			if (t<=0) k/=10; else break;
		}while (k>1e-7);
		if (k<=1e-7) break;
		if (ans.m[p][q]!=0)	/*确定givens变换矩阵*/ 
		{
			z=(ans.m[q][q]-ans.m[p][p])/2/ans.m[p][q];
			z=sgn(z)/(fabs(z)+sqrt(1+z*z));
			c=1/sqrt(1+z*z);s=z*c;
		}
		else 
		{
			c=1;s=0;
		}
		z=c*c*ans.m[p][p]-2*s*c*ans.m[p][q]+s*s*ans.m[q][q]; /*计算变换*/ 
		ans.m[q][q]=s*s*ans.m[p][p]+2*s*c*ans.m[p][q]+c*c*ans.m[q][q];
		ans.m[p][p]=z;
		ans.m[p][q]=ans.m[q][p]=0;
		for (i=1;i<=n;i++)
			if (i!=p && i!=q)
			{
				b[i]=c*ans.m[i][p]-s*ans.m[i][q];
				d[i]=s*ans.m[i][p]+c*ans.m[i][q];
			}
		for (i=1;i<=n;i++)
			if (i!=p && i!=q)
			{
				ans.m[i][p]=ans.m[p][i]=b[i];
				ans.m[i][q]=ans.m[q][i]=d[i];
			}
		for (i=1;i<=n;i++)	/*累积变换矩阵*/ 
		{
			b[i]=c*Q.m[i][p]-s*Q.m[i][q];
			d[i]=s*Q.m[i][p]+c*Q.m[i][q];
		}
		for (i=1;i<=n;i++)
		{
			Q.m[i][p]=b[i];	
			Q.m[i][q]=d[i];
		}
		times++;
	}while (1);
	for (i=1;i<=n;i++)
		for (j=1;j<=i-1;j++)
			ans.m[i][j]=ans.m[j][i]=0;
	delete []b;delete []d;
	return ans;
}

#endif