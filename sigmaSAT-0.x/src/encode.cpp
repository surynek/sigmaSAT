/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                             sigmaSAT 0.71-sky                              */
/*                                                                            */
/*                   (C) Copyright 2009-2011 Pavel Surynek                    */
/*            http://www.surynek.com | <pavel.surynek@mff.cuni.cz>            */
/*                                                                            */
/*                                                                            */
/*============================================================================*/
// encode.cpp / 0.71-sky
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
//  Encoder module of sigmaSAT solver.

#include <stdio.h>

#include "defs.h"
#include "version.h"

int pocitej_rovnost(int sUNUSED(ia), int sUNUSED(ib), int l)
{
  return 2 * l;
}

void generuj_rovnost(int ia, int ib, int l)
{
  for (int i = 1; i <= l; ++i)
    {
      int va = ia * l + i;
      int vb = ib * l + i;
      printf("%d %d 0\n", -va, vb);
      printf("%d %d 0\n", -vb, va);
    }
}

int pocitej_nerovnost(int sUNUSED(ia), int sUNUSED(ib), int l, int &aux)
{
  aux += l;
  return 4 * l + 1;
}

void generuj_nerovnost(int ia, int ib, int l, int &aux)
{
  for (int i = 1; i <= l; ++i)
    {
      int va = ia * l + i;
      int vb = ib * l + i;

      ++aux;

      printf("%d %d %d 0\n", -aux, va, vb);
      printf("%d %d %d 0\n", -aux, -va, -vb);
      printf("%d %d %d 0\n", aux, -va, vb);
      printf("%d %d %d 0\n", aux, va, -vb);
    }

  for (int i = l - 1; i >= 0; --i)
    {
      printf("%d ", aux - i);
    }

  printf("0\n");
}


int pocitej_soucet(int sUNUSED(ia), int sUNUSED(ib), int sUNUSED(ic), int l, int &aux)
{
  int cl = 0;

  cl += 4;

  ++aux;

  cl += 3;

  for (int i = 2; i <= l; ++i)
  {
    ++aux;

    cl += 8;

    aux += 4;
    cl += 13;
  }

  return cl;
}


void generuj_soucet(int ia, int ib, int ic, int l, int &aux)
{
  int va = ia * l + 1;
  int vb = ib * l + 1;
  int vc = ic * l + 1;

  printf("%d %d %d 0\n", -vc, va, vb); 
  printf("%d %d %d 0\n", -vc, -va, -vb);
  printf("%d %d %d 0\n", vc, -va, vb);
  printf("%d %d %d 0\n", vc, va, -vb);

  int vk = ++aux;

  printf("%d %d 0\n", -vk, va); 
  printf("%d %d 0\n", -vk, vb); 
  printf("%d %d %d 0\n", vk, -va, -vb); 

  int vkk, vcc;
// oprava
  int vl, vm, vn;

  for (int i = 2; i <= l; ++i)
  {
    va = ia * l + i;
    vb = ib * l + i;
    vc = ic * l + i;

    vcc = ++aux;

    printf("%d %d %d 0\n", -vcc, vb, vk); 
    printf("%d %d %d 0\n", -vcc, -vb, -vk);
    printf("%d %d %d 0\n", vcc, -vb, vk); 
    printf("%d %d %d 0\n", vcc, vb, -vk); 

    printf("%d %d %d 0\n", -vc, va, vcc); 
    printf("%d %d %d 0\n", -vc, -va, -vcc); 
    printf("%d %d %d 0\n", vc, va, -vcc); 
    printf("%d %d %d 0\n", vc, -va, vcc); 

// oprava
    vl = ++aux;

    printf("%d %d %d 0\n", -vl, va, vb); 
    printf("%d %d %d 0\n", -vl, -va, -vb);
    printf("%d %d %d 0\n", vl, -va, vb);
    printf("%d %d %d 0\n", vl, va, -vb);

    vm = ++aux;

    printf("%d %d 0\n", -vm, vl);
    printf("%d %d 0\n", -vm, vk); 
    printf("%d %d %d 0\n", vm, -vl, -vk);

    vn = ++aux;

    printf("%d %d 0\n", -vn, va);
    printf("%d %d 0\n", -vn, vb); 
    printf("%d %d %d 0\n", vn, -va, -vb);

    vkk = ++aux;

    printf("%d %d 0\n", vkk, -vm);
    printf("%d %d 0\n", vkk, -vn); 
    printf("%d %d %d 0\n", -vkk, vm, vn);

/*
    printf("%d %d %d %d 0\n", -vkk, va, vk, vb);
    printf("%d %d 0\n", vkk, -va);
    printf("%d %d 0\n", vkk, -vk);
    printf("%d %d 0\n", vkk, -vb);
*/

    vk = vkk;
  }
}

int pocitej_mensi_rovno(int sUNUSED(ia), int sUNUSED(ib), int l, int &aux)
{
  ++aux;
  aux += l;
  aux += l;
  aux += l;

  int cl = 0;

  for (int i = 1; i <= l; ++i)
  {
    cl += 3;
    cl += 3;
  }

  for (int i = l - 1; i >= 0; --i)
  {
    cl += 1;
  }

  for (int i = l - 1; i >= 0; --i)
  {
    for (int j = l - 1; j >= i + 1; --j)
      {
	cl += 1;
      }

    cl += 1;
  }

// oprava
//  cl += 1;

  for (int i = l - 1; i >= 0; --i)
    {
      cl += 1;
    }

  cl += 1;
  ++aux;

  ++cl;

  for (int i = l - 1; i >= 0; --i)
    {
      ++cl;
    }
  ++cl;

  return cl;
}


int generuj_mensi_rovno(int ia, int ib, int l, int &aux)
{
  ++aux;
  int vleq0 = aux;
  aux += l;

  int vls0 = aux;
  aux += l;

  int vlx0 = aux;
  aux += l;


  int va = ia * l + 1;
  int vb = ib * l + 1; 

  int vls = vls0;
  int vleq = vleq0;

  int cl = 0;

  for (int i = 1; i <= l; ++i)
  {
    printf("-%d -%d 0\n", vls, va);
    printf("-%d %d 0\n", vls, vb);
    printf("%d %d -%d 0\n", vls, va, vb);

    cl += 3;

    printf("%d %d 0\n", vleq, va);
    printf("%d -%d 0\n", vleq, vb);
    printf("-%d -%d %d 0\n", vleq, va, vb);

    cl += 3;

    ++vls;
    ++vleq;
    ++va;
    ++vb;
  }

  int vlx = vlx0 + l - 1;
  vls = vls0 + l - 1;

  for (int i = l - 1; i >= 0; --i)
  {
    int vleq = vleq0 + l - 1;

    printf("%d ", vlx);

    for (int j = l - 1; j >= i + 1; --j)
      {
	printf("-%d ", vleq);
	--vleq;
      }

    printf("-%d 0\n", vls);
    cl += 1;

    --vls; // oprava
    --vlx;
  }

  vlx = vlx0 + l - 1;
  vls = vls0 + l - 1;

  for (int i = l - 1; i >= 0; --i)
  {
    int vleq = vleq0 + l - 1;

    for (int j = l - 1; j >= i + 1; --j)
      {
	printf("-%d %d 0\n", vlx, vleq);
	--vleq;
	cl += 1;
      }

    printf("-%d %d 0\n", vlx, vls);
    cl += 1;

    --vls; // oprava
    --vlx;
  }

  int vlx_1 = aux;
  vleq = vleq0 + l - 1;

  printf("%d ", vlx_1);

  for (int i = l - 1; i >= 0; --i)
    {
      printf("-%d ", vleq);
      --vleq;
    }
  printf("0\n");
  cl += 1;

  vleq = vleq0 + l - 1;

  for (int i = l - 1; i >= 0; --i)
    {
      printf("-%d %d 0\n", vlx_1, vleq);
      cl += 1;
      --vleq;
    }

// oprava
/*
  vlx = vlx0 + l - 1;

  for (int i = l - 1; i >= 0; --i)
    {
      printf("%d ", vlx);
      --vlx;
    }

  printf("%d 0\n", vlx_1);
  cl += 1;
*/
  int q = ++aux; /* pravdiva, kdyz plati nerovnost */

  vlx = vlx0 + l - 1;

  printf("-%d ", q);
  for (int i = l - 1; i >= 0; --i)
    {
      printf("%d ", vlx);
      --vlx;
    }
  printf("%d 0\n", vlx_1);
  ++cl;

  vlx = vlx0 + l - 1;

  for (int i = l - 1; i >= 0; --i)
    {
      printf("%d -%d 0\n", q, vlx);
      --vlx;
      ++cl;
    }
  printf("%d -%d 0\n", q, vlx_1);
  ++cl;

  return q;
}


int pocitej_rovno_konstante(int sUNUSED(ia), int sUNUSED(c), int l, int &sUNUSED(aux))
{
	int cl = 0;

	for (int i = 0; i < l; ++i)
	{
		++cl;
	}

	return cl;
}

void generuj_rovno_konstante(int ia, int c, int l, int &sUNUSED(aux))
{
	int va = ia * l + 1;

	for (int i = 0; i < l; ++i)
	{
		int bit = c % 2;
		c /= 2;

		if (bit)
		{
			printf("%d 0\n", va);
		}
		else
		{
			printf("-%d 0\n", va);
		}

		++va;
	}
}


int pocitej_podminky(int k, int *d, int D, int l, int &aux)
{
  int cl = 0;

  cl += pocitej_rovno_konstante(5*k, D, l, aux);

  for (int i = 0; i < k; ++i)
  {
	  cl += pocitej_rovno_konstante(2*k + i, d[i], l, aux);
	  cl += pocitej_soucet(i, 2*k + i, 3*k + i, l, aux);
	  cl += pocitej_soucet(k + i, 2*k + i, 4*k + i, l, aux);

	  cl += pocitej_mensi_rovno(i, 5*k, l, aux);
	  cl += pocitej_mensi_rovno(k + i, 5*k, l, aux);
	  cl += pocitej_mensi_rovno(3*k + i, 5*k, l, aux);
	  cl += pocitej_mensi_rovno(4*k + i, 5*k, l, aux);

	  cl += 4;

  }

  for (int i = 0; i < k - 1; ++i)
  {
	  for (int j = i + 1; j < k; ++j)
	  {
		  cl += pocitej_mensi_rovno(3*k + i, j, l, aux);
		  cl += pocitej_mensi_rovno(4*k + i, k + j, l, aux);
		  cl += pocitej_mensi_rovno(3*k + j, i, l, aux);
		  cl += pocitej_mensi_rovno(4*k + j, k + i, l, aux);

		  ++cl;
	  }
  }

  return cl;
}


void generuj_podminky(int k, int *d, int D, int l, int &aux)
{
  generuj_rovno_konstante(5*k, D, l, aux);

  for (int i = 0; i < k; ++i)
  {
	  generuj_rovno_konstante(2*k + i, d[i], l, aux);
	  generuj_soucet(i, 2*k + i, 3*k + i, l, aux);
	  generuj_soucet(k + i, 2*k + i, 4*k + i, l, aux);

	  int l1 = generuj_mensi_rovno(i, 5*k, l, aux);
	  int l2 = generuj_mensi_rovno(k + i, 5*k, l, aux);
	  int l3 = generuj_mensi_rovno(3*k + i, 5*k, l, aux);
	  int l4 = generuj_mensi_rovno(4*k + i, 5*k, l, aux);

	  printf("%d 0\n", l1);
	  printf("%d 0\n", l2);
	  printf("%d 0\n", l3);
	  printf("%d 0\n", l4);

  }

  for (int i = 0; i < k - 1; ++i)
  {
	  for (int j = i + 1; j < k; ++j)
	  {
		  int q1 = generuj_mensi_rovno(3*k + i, j, l, aux);
		  int q2 = generuj_mensi_rovno(4*k + i, k + j, l, aux);
		  int q3 = generuj_mensi_rovno(3*k + j, i, l, aux);
		  int q4 = generuj_mensi_rovno(4*k + j, k + i, l, aux);

		  printf("%d %d %d %d 0\n", q1, q2, q3, q4);
	  }
  }

}


const int L = 64;
const int k = 6;

int main(void)
{
  int iV = 5 * k + 1;

  int V = iV * L;
  int CL = 0;

  int d[k] = {3, 2, 1, 1, 1, 1};
  int D = 4;

  CL += pocitej_podminky(k, d, D, L, V);

  int aux = iV * L;
  printf("p cnf %d %d\n", V, CL);

  generuj_podminky(k, d, D, L, aux);

  return 0;
}


/*
int main(void)
{
  int iV = 4;

  int V = iV * L;
  int CL = 0;

  CL += pocitej_rovno_konstante(0, 23, L, V);
  CL += pocitej_rovno_konstante(1, 25, L, V);
  CL += pocitej_rovno_konstante(3, 48, L, V);
  CL += pocitej_soucet(0, 1, 2, L, V);

  CL += pocitej_mensi_rovno(2, 3, L, V);
  CL += 1;

  int aux = iV * L;
  printf("p cnf %d %d\n", V, CL);

  generuj_rovno_konstante(0, 23, L, aux);
  generuj_rovno_konstante(1, 25, L, aux);
  generuj_rovno_konstante(3, 47, L, aux);
  generuj_soucet(0, 1, 2, L, aux);

  int q = generuj_mensi_rovno(2, 3, L, aux);
  printf("%d 0\n", q);

  return 0;
}
*/

/*
int main(void)
{
  int iV = 5;

  int V = iV * L;
  int CL = 0;
  CL += pocitej_rovnost(0, 1, L);
  CL += pocitej_rovnost(1, 2, L);
  CL += pocitej_rovnost(2, 3, L);
  CL += pocitej_nerovnost(3, 0, L, V);
  CL += pocitej_nerovnost(0, 4, L, V);

  int aux = iV * L;
  printf("p cnf %d %d\n", V, CL);

  generuj_rovnost(0, 1, L);
  generuj_rovnost(1, 2, L);
  generuj_rovnost(2, 3, L);
  generuj_nerovnost(3, 0, L, V);
  generuj_nerovnost(0, 4, L, V);

  return 0;
}
*/

 /*
int main(void)
{
  int iV = 4;

  int aux, V;
  V = aux = iV * L;
  int CL = 0;

  CL += pocitej_soucet(0, 1, 2, L, V);
  CL += pocitej_soucet(1, 0, 3, L, V);
  //  CL += pocitej_rovnost(2, 3, L);
  CL += pocitej_nerovnost(2, 3, L, V);

  printf("p cnf %d %d\n", V, CL);
  generuj_soucet(0, 1, 2, L, aux);
  generuj_soucet(1, 0, 3, L, aux);
  //  generuj_rovnost(2, 3, L);
  generuj_nerovnost(2, 3, L, aux);

  return 0;
}
 */
/*
int main(void)
{
  int iV = 6;

  int aux, V;
  V = aux = iV * L;
  int CL = 0;

  CL += pocitej_soucet(0, 1, 2, L, V);
  CL += pocitej_soucet(1, 0, 5, L, V);
  CL += pocitej_rovnost(2, 3, L);
  CL += pocitej_rovnost(3, 4, L);
  CL += pocitej_rovnost(4, 5, L);
  //  CL += pocitej_nerovnost(5, 3, L, V);
  CL += pocitej_rovnost(5, 3, L);

  printf("p cnf %d %d\n", V, CL);
  generuj_soucet(0, 1, 2, L, aux);
  generuj_soucet(1, 0, 5, L, aux);
  generuj_rovnost(2, 3, L);
  generuj_rovnost(3, 4, L);
  generuj_rovnost(4, 5, L);
  // generuj_nerovnost(5, 3, L, aux);
  generuj_rovnost(5, 3, L);

  return 0;
}
*/
