/*
 * alphaE_limit.cpp - analytic (no simulation) survey of the reconstruction
 * ratio alpha_E(R) = sum_j pol_v(j)^2 / sum_j pol_u(j)^2 over the uniform
 * arrival phase j in [0, 2R), plus the mean relative deficit of the integer
 * circle u^2 + v^2 = R^4 (the isqrt reconstruction error).
 *
 * u, v are the manuscript reconstruction formulas (polarization.h) with the
 * integer square root isqrt = floor(sqrt).
 *
 * Build:  cl /nologo /O2 /EHsc alphaE_limit.cpp /Fe:alphaE_limit.exe
 */

#include <cstdio>
#include <cmath>

static long long isqrtLL(long long n)
{
  if (n <= 0) return 0;
  long long r = (long long)std::sqrt((double)n);
  while (r > 0 && r * r > n) --r;
  while ((r + 1) * (r + 1) <= n) ++r;
  return r;
}

int main()
{
  printf("R   | alpha_E = sum v^2/sum u^2 (isqrt) | alpha_E (exact sqrt) |"
         " 1/alpha_E | mean rel. deficit | deficit*R\n");
  printf("----+----------------------------------+----------------------+----------+-------------------+-----------\n");

  for (int R = 1; R <= 1000; ++R)
  {
    long double sumU2 = 0, sumV2 = 0;
    long double sumU2e = 0, sumV2e = 0;   // exact-sqrt reference
    long double sumDef = 0;                // R^4 - (u^2 + v^2), isqrt version
    const long double R4 = (long double)R * R * R * R;

    for (int j = 0; j < 2 * R; ++j)
    {
      long long u, v;
      if (j < R)
      {
        u = (long long)R * (R - 2 * j);
        v = 2LL * R * isqrtLL((long long)j * (R - j));
      }
      else
      {
        int j2 = j - R;
        u = (long long)R * (2 * j - 3 * R);
        v = -2LL * R * isqrtLL((long long)j2 * (R - j2));
      }
      sumU2 += (long double)u * u;
      sumV2 += (long double)v * v;
      sumDef += R4 - ((long double)u * u + (long double)v * v);

      // exact-sqrt reference (real circle)
      long double ue, ve;
      if (j < R)
      {
        ue = (long double)R * (R - 2 * j);
        ve = 2.0L * R * std::sqrt((long double)j * (R - j));
      }
      else
      {
        int j2 = j - R;
        ue = (long double)R * (2 * j - 3 * R);
        ve = -2.0L * R * std::sqrt((long double)j2 * (R - j2));
      }
      sumU2e += ue * ue;
      sumV2e += ve * ve;
    }

    const long double E1 = sumU2 > 0 ? sumV2 / sumU2 : 0.0L;
    const long double E1e = sumU2e > 0 ? sumV2e / sumU2e : 0.0L;
    const long double meanDef = sumDef / ((long double)2 * R * R4);
    const long double dR = meanDef * R;

    // print selected R
    bool show = false;
    if (R <= 30) show = true;
    else if (R <= 200 && (R % 25 == 0)) show = true;
    else if (R > 200 && (R % 100 == 0)) show = true;
    if (show)
      printf("%4d | %32.9Lf | %22.9Lf | %10.6Lf | %17.9Lf | %10.6Lf\n",
             R, E1, E1e, 1.0L / E1, meanDef, dR);
    if (R == 1 || R == 2 || R == 1000)
      printf("%4d | %32.9Lf | %22.9Lf | %10.6Lf | %17.9Lf | %10.6Lf\n",
             R, E1, E1e, 1.0L / E1, meanDef, dR);
  }
  return 0;
}
