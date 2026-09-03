/*
 * alphaC.cpp - candidate alpha_C survey (analytic, no simulation).
 *
 * 1. Wyler-on-the-lattice: feed the Wyler expression with the lattice value
 *    of pi obtained by counting integer points inside spheres,
 *        pi_hat(R) = 3*N(R) / (4 R^3),  N(R) = #{ (x,y,z): x^2+y^2+z^2 <= R^2 }
 *    Wyler:  alpha_W = 9/(8 pi^4) * (pi^5 / (2^4 * 5!))^(1/4)  ->  1/137.03608
 * 2. Curated structural-integer ratios of the model at each L, reporting how
 *    close any of them comes to 1/137.035999177 and to 137.035999177.
 */

#include <cstdio>
#include <cmath>
#include <vector>

static double pi_lattice(int R)
{
  long long n = 0;
  long long R2 = (long long)R * R;
  for (int x = -R; x <= R; ++x)
    for (int y = -R; y <= R; ++y)
      for (int z = -R; z <= R; ++z)
        if ((long long)x*x + (long long)y*y + (long long)z*z <= R2) ++n;
  return 3.0 * (double)n / (4.0 * (double)R * (double)R * (double)R);
}

static double wyler(double p)
{
  // alpha_W = 9/(8 pi^4) * (pi^5/(2^4 5!))^(1/4)
  double t = p * p * p * p * p / (16.0 * 120.0);
  return 9.0 / (8.0 * p * p * p * p) * std::pow(t, 0.25);
}

int main()
{
  const double alphaTarget = 1.0 / 137.035999177;      // 0.00729735256
  const double invTarget   = 137.035999177;

  printf("=== alpha_C: Wyler-on-the-lattice ===\n");
  printf("  pi_target = %.12f ; Wyler(pi) = %.12f -> 1/alpha = %.6f\n",
         3.14159265358979, wyler(3.14159265358979), 1.0 / wyler(3.14159265358979));
  for (int R = 2; R <= 40; ++R)
  {
    double ph = pi_lattice(R);
    double aW = wyler(ph);
    if (R <= 10 || (R % 10) == 0)
      printf("  R=%3d  pi_hat=%.9f  alpha_W=%.10f  1/alpha_W=%.6f\n",
             R, ph, aW, 1.0 / aW);
  }

  printf("\n=== alpha_C: curated structural-integer ratios vs 137 ===\n");
  printf("L | RMAX R  W=3L2  nineL  phiFull | nearest ratio (value) |"
         "  dist to 137.036 (dex)\n");
  for (int L = 7; L <= 31; L += 2)
  {
    int RMAX = L / 2;
    int R    = RMAX - 2;
    if (R < 1) R = 1;
    long long W   = 3LL * L * L;
    long long n9  = 9LL * L;
    long long phi = 2LL * R * R;

    // curated ratios (large over small and reciprocals of small over large)
    std::vector<double> vals;
    std::vector<std::pair<long long,long long>> pairs = {
      { W, n9 }, { W, phi }, { W, 2LL*RMAX }, { n9, R }, { W, (long long)L },
      { phi, RMAX }, { n9, phi }, { W, R }
    };
    for (auto& pr : pairs)
    {
      if (pr.second == 0) continue;
      vals.push_back((double)pr.first / (double)pr.second);
      vals.push_back((double)pr.second / (double)pr.first);
    }

    // nearest to invTarget and to alphaTarget
    double best = 1e300; double bestv = 0;
    for (double v : vals)
    {
      double d = std::fabs(v - invTarget);
      if (d < best) { best = d; bestv = v; }
    }
    double dex = (best > 0) ? std::log10(best) : -99.0;
    printf("%2d | %4d %2d %5lld %6lld %7lld | %18.6f | %10.2f\n",
           L, RMAX, R, W, n9, phi, bestv, dex);
  }
  printf("\nnote: dist<0 means closer than 1 part in 10^0 (1 digit); the\n");
  printf("pre-registered criterion needs ~8 digits (dist ~ -8).\n");
  return 0;
}
