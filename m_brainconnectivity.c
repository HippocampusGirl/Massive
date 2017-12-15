// This library is part of Massive, copyright 2017 Lea Waller.
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by the
// Free Software Foundation, either version 3 of the License, or (at your
// option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
// for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "m_brainconnectivity.h"

static const char usage[] = \
"Usage: m_brainconnectivity -i/-p <input> -o <output> <options>\n"\
"\n"\
"Required arguments:\n"\
"-i <filename> input 4d image\n"\
"-p <filename> alternatively, input text file where rows are time,\n"\
"columns are nodes\n"\
"-o <prefix> output prefix\n"\
"\n"\
"Options (one or more of each):\n"\
"-n <network_definition_scheme> specify how networks are constructed.\n"\
"All methods lead to weighted undirected networks, weights are scaled\n"\
"as correlations. The following network definition schemes are available:\n"\
"   corr pearson correlation\n"\
"   ridge ridge-regularized partial correlation with rho=1.0\n"\
"   ridge:<rho> ridge-regularized partial correlation with user-specified rho\n"\
"\n"\
"-t <threshold_type>:<value> specify how the network is thresholded. The \n"\
"following threshold types are available:\n"\
"   absolute all weights above the value are retained\n"\
"   proportional a proportion of the strongest weights is retained\n"\
"   nnegproportional also removes negative weights, important for pathlength\n"\
"The value can be specified either directly as a number, or as a range\n"\
"as <lthr>:<step>:<uthr>. For example, absolute:-1.0 is equivalent to no \n"\
"thresholding.\n"\
"\n"\
"-m <measure> specify which network properties are calculated. The following\n"\
"measures are available:\n"\
"   global:clustering_coef\n"\
"   global:charpath\n"\
"   global:efficiency\n"\
"\n"\
"-d enable debug messages, however these are not very useful at present\n";

static char const corr_str[] = "corr";
static char const ridge_str[] = "ridge";
enum networkdefinition {
  corr = 1 << 0,
  ridge = 1 << 1
};

static char const absolute_str[] = "absolute";
static char const proportional_str[] = "proportional";
static char const nnegproportional_str[] = "nnegproportional";
enum threshold {
  absolute = 1 << 0,
  proportional = 1 << 1,
  nnegproportional = 1 << 2
};

static char const global_str[] = "global";
static char const local_str[] = "local";
enum measuredimensionality {
  local = 1 << 0,
  global = 1 << 1
};

static char const charpath_str[] = "charpath";
static char const clustering_coef_str[] = "clustering_coef";
static char const efficiency_str[] = "efficiency";
enum measure {
  charpath = 1 << 2,
  clustering_coef = 1 << 3,
  efficiency = 1 << 4
};

static u4_t parsenetworkdefinition(char *c,
  f4_t *param) {
  char const d[] = ":";

  char s[4096];
  strcpy(&s[0], c);

  char* tok = strtok(c, d);

  u4_t m = 0;
  *param = 1.0f;

  if (strcmp(tok, corr_str) == 0) {
    m |= corr;
  } else if (strcmp(tok, ridge_str) == 0) {
    m |= ridge;
  } else {
    fprintf(stderr, RED "Error: undefined network definition scheme %s." WHITE "\n\n%s", s, usage);
    exit(EXIT_FAILURE);
  }

  tok = strtok(NULL, d);

  if (tok) {
    *param = atof(tok);
  }

  return m;
}

static void networkdefinitiontostr(char *c,
  u4_t networkdefinition, f4_t param) {

  if (networkdefinition & corr) {
    sprintf(c, "%s", corr_str);
  } else if (networkdefinition & ridge) {
    sprintf(c, "%s:%f", ridge_str, param);
  }
}

static u4_t parsethreshold(char *c,
  u4_t *t, f4_t *p) {
  char const d[] = ":";

  char s[4096];
  strcpy(&s[0], c);

  char* tok = strtok(c, d);

  u4_t type = 0;

  if (strcmp(tok, absolute_str) == 0) {
    type |= absolute;
  } else if (strcmp(tok, proportional_str) == 0) {
    type |= proportional;
  } else if (strcmp(tok, nnegproportional_str) == 0) {
    type |= nnegproportional;
  } else {
    fprintf(stderr, RED "Error: undefined threshold %s." WHITE "\n\n%s", s, usage);
    exit(EXIT_FAILURE);
  }

  tok = strtok(NULL, d);

  t[0] = type;
  p[0] = atof(tok);

  if (debug) {
    printf("thresholdparam %s -> %f\n", tok, p[0]);
  }

  tok = strtok(NULL, d);

  if (tok) {
    f4_t step = atof(tok);

    tok = strtok(NULL, d);
    if (tok) {
      f4_t max = atof(tok);

      u4_t i;
      for (i = 0; p[i] < (max + 0.5f * step); i++) {
        t[i+1] = type;
        p[i+1] = p[i] + step;
      }
      return i;
    } else {
      fprintf(stderr, RED "Error: undefined threshold %s." WHITE "\n\n%s" , s, usage);
      exit(EXIT_FAILURE);
    }
  } else {
    return 1;
  }
}

static void thresholdtostr(char *c,
  u4_t threshold, f4_t param) {

  if (threshold & absolute) {
    sprintf(c, "%s:%f", absolute_str, param);
  } else if (threshold & proportional) {
    sprintf(c, "%s:%f", proportional_str, param);
  } else if (threshold & nnegproportional) {
    sprintf(c, "%s:%f", nnegproportional_str, param);
  }
}

static u4_t parsemeasure(char *c) {
  char const d[] = ":";

  char s[4096];
  strcpy(&s[0], c);

  char* tok = strtok(c, d);

  u4_t m = 0;

  if (strcmp(tok, global_str) == 0) {
    m |= global;
  } else if (strcmp(tok, local_str) == 0) {
    m |= local;
  } else {
    fprintf(stderr, RED "Error: undefined measure %s." WHITE "\n\n%s", s, usage);
    exit(EXIT_FAILURE);
  }

  tok = strtok(NULL, d);

  if (strcmp(tok, charpath_str) == 0 && (m & global)) {
    m |= charpath;
  } else if (strcmp(tok, clustering_coef_str) == 0 && (m & global)) {
    m |= clustering_coef;
  } else if (strcmp(tok, efficiency_str) == 0 && (m & global)) {
    m |= efficiency;
  } else {
    fprintf(stderr, RED "Error: undefined measure %s." WHITE "\n\n%s", s, usage);
    exit(EXIT_FAILURE);
  }

  return m;
}

static void measuretostr(char *c,
  u4_t m) {

  const char * str1 = NULL;
  if (m & global) {
    str1 = global_str;
  } else if (m & local) {
    str1 = local_str;
  }

  if (m & charpath) {
    sprintf(c, "%s:%s", str1, charpath_str);
  } else if (m & clustering_coef) {
    sprintf(c, "%s:%s", str1, clustering_coef_str);
  } else if (m & efficiency) {
    sprintf(c, "%s:%s", str1, efficiency_str);
  }
}

int main(int argc, char* argv[]) {
  fputs(version, stdout);

  #pragma omp parallel
  #pragma omp single
  {
    u4_t mp = 1;
    mp = omp_get_num_threads();
    fprintf(stderr, "Using %u teams.\n", mp);
    #pragma omp parallel
    #pragma omp single
    {
      mp = omp_get_num_threads();
      fprintf(stderr, "Using %u threads.\n", mp);
    }
  }

  #pragma omp parallel
  {
    allocate_stack();
  }

  fprintf(stderr, "\n");

  char *fi = NULL;
  char *fp = NULL;
  char *fo = NULL;

  u4_t networkdefinitions[4096];
  f4_t networkdefinitionparams[4096];
  u4_t nnetworkdefinitions = 0;

  u4_t thresholds[4096];
  f4_t thresholdparams[4096];
  u4_t nthresholds = 0;

  u4_t measures[4096];
  u4_t nmeasures = 0;

  char cc;
  u4_t nt;
  while ((cc = getopt(argc, argv, "i:p:o:m:n:t:d")) != -1) {
    switch (cc) {
      case 'i':
        fi = optarg;
        break;
      case 'p':
        fp = optarg;
        break;
      case 'o':
        fo = optarg;
        break;

      case 'm':
        measures[nmeasures++] = parsemeasure(optarg);
        break;
      case 'n':
        networkdefinitions[nnetworkdefinitions] = \
          parsenetworkdefinition(optarg, &networkdefinitionparams[nnetworkdefinitions]);
        nnetworkdefinitions++;
        break;
      case 't':
        nt = parsethreshold(optarg, &thresholds[nthresholds], &thresholdparams[nthresholds]);
        nthresholds += nt;
        break;

      case 'd':
        debug = 1;
        printf("debug = %u\n", debug);
        break;

      default:
        break;
    }
  }

  u4_t n;
  u4_t m;

  f4_t *x = NULL;
  u4_t *z = NULL;

  if (fp) {
    u4_t s[] = {0, 0};
    u4_t nn[2];
    read_txt_f4(fp, &s[0], &nn[0], &x);

    n = nn[1]; // columns
    m = nn[0]; // rows

    f4_t *xx = allocate_f4(n*m);
    memcpy(xx, x, n*m * sizeof(f4_t));
    for (u4_t i = 0; i < m; i++) { // transpose
      for (u4_t j = 0; j < n; j++) {
        x[j*m+i] = xx[i*n+j];
      }
    }
    free_f4(n*m);
  } else if (fi) {
    u4_t nn[2];

    read_nii_f4(fi, &nn[0], &z, &x);

    n = nn[1];
    m = nn[0];
  } else {
    fprintf(stderr, RED "Error: no input specified." WHITE "\n\n%s", usage);
    exit(EXIT_FAILURE);
  }

  if (!fo) {
    fprintf(stderr, RED "Error: no output prefix specified." WHITE "\n\n%s", usage);
    exit(EXIT_FAILURE);
  }

  fprintf(stderr, "%u %u\n", n, m);

  for (u4_t i = 0; i < n; i++) {
    f4_t q = 0.0f;
    for (u4_t j = 0; j < m; j++) {
      q += x[i*m+j];
    }
    q /= ((f4_t) m);
    if (debug) {
      printf("mean[%u]=%f\n", i, q);
    }
    for (u4_t j = 0; j < m; j++) {
      x[i*m+j] -= q;
    }
  }

  u4_t nmeasuresglobal = 0;
  u4_t nmeasureslocal = 0;
  for (u4_t i = 0; i < nmeasures; i++) {
    if (measures[i] & global) {
      nmeasuresglobal++;
    } else if (measures[i] & local) {
      nmeasureslocal++;
    }
  }
  fprintf(stderr, "%u %u %u %u\n", nnetworkdefinitions, nthresholds, nmeasuresglobal, nmeasureslocal);

  f4_t *og = allocate_f4(nnetworkdefinitions*nthresholds*nmeasuresglobal);
  for (u4_t i = 0; i < nnetworkdefinitions*nthresholds*nmeasuresglobal; i++) {
    og[i] = 0.0f / 0.0f;
  }

  f4_t *ol = allocate_f4(nnetworkdefinitions*nthresholds*nmeasureslocal*n);
  for (u4_t i = 0; i < nnetworkdefinitions*nthresholds*nmeasureslocal*n; i++) {
    ol[i] = 0.0f / 0.0f;
  }

  omp_set_dynamic(0);
  #ifdef __INTEL_MKL__
    mkl_set_dynamic(0);
  #endif

  u4_t progress = 0;

  #pragma omp parallel
  {
    f4_t *w, *v, *ta, *tb;
    u4_t *ti;

    #pragma omp critical
    {
      w = allocate_f4(n*n);
      v = allocate_f4(n*n);
      ta = allocate_f4(nthresholds);
      tb = allocate_f4(nthresholds);
      ti = allocate_u4(nthresholds);
    }

    #pragma omp for schedule(static)
    for (u4_t i = 0; i < nnetworkdefinitions; i++) {
      if (networkdefinitions[i] & corr) {
        cov(x, w, n, m);
      } else if (networkdefinitions[i] & ridge) {
        ridgecov(x, w, networkdefinitionparams[i], n, m);
      }

      cov2corr(w, n);

      if (debug) {
        printmatrix(w, n, n);
      }

      u4_t tk = 0;
      for (u4_t j = 0; j < nthresholds; j++) {
        if (thresholds[j] & proportional || thresholds[j] & nnegproportional) {
          ta[tk++] = thresholdparams[j]; // extract
        }
      }
      memcpy(v, w, n*n * sizeof(f4_t));
      proportional2absolutethreshold(v, ta, n, tk); // convert

      tk = 0;
      memcpy(tb, thresholdparams, nthresholds * sizeof(f4_t)); // copy
      for (u4_t j = 0; j < nthresholds; j++) {
        if (thresholds[j] & proportional || thresholds[j] & nnegproportional) {
          float pp = ta[tk++];
          if (thresholds[j] & nnegproportional) {
            if (pp < 0.0f) {
              pp = 0.0f;
            }
          }

          tb[j] = pp; // insert converted thresholds into copy
        }
      }

      argsort(ti, tb, nthresholds);

      for (u4_t j = 0; j < nthresholds; j++) {
        u4_t jj = ti[j];
        float t = tb[jj];

        if (debug) {
          printf("using threshold #%u = %f (%f)\n", jj, t, thresholdparams[jj]);
        }

        applyabsolutethreshold(w, t, n);

        f4_t *eg = NULL, *cg = NULL, *cpg = NULL, *el = NULL, *cl = NULL;

        for (u4_t k = 0; k < nmeasures; k++) {
          if (measures[k] & global) {
            u4_t oi = (k*nnetworkdefinitions+i)*nthresholds+jj;
            if (debug) {
              printf("measure at og[%u]\n", oi);
            }
            if (measures[k] & charpath) {
              cpg = &og[oi];
            } else if (measures[k] & clustering_coef) {
              cg = &og[oi];
            } else if (measures[k] & efficiency) {
              eg = &og[oi];
            }
          } else if (measures[k] & local) {
            float *oo = &ol[((k*nnetworkdefinitions+i)*nthresholds+jj)*n];
            if (measures[k] & clustering_coef) {
              cl = oo;
            } else if (measures[k] & efficiency) {
              el = oo;
            }
          }
        }

        if (eg || cpg || el) {
          memcpy(v, w, n*n * sizeof(f4_t));

          pathlength(v, eg, el, cpg, n);
        }

        if (cg || cl) {
          memcpy(v, w, n*n * sizeof(f4_t));

          triangles(v, cg, cl, n);
        }

        #pragma omp atomic
        progress++;

        showprogress(progress, nnetworkdefinitions*nthresholds);
      }
    }
  }

  if (debug) {
    printmatrix(og, nnetworkdefinitions*nthresholds, nmeasuresglobal);
  }

  char *foe = &fo[strlen(fo)];
  strcpy(foe, ".txt");

  u4_t const charsize = 128;

  char **cn = (char**) allocate_ptr(nnetworkdefinitions*nthresholds*2);
  for (u4_t i = 0; i < nnetworkdefinitions; i++) {
    for (u4_t j = 0; j < nthresholds; j++) {
      cn[2*(i*nthresholds+j)+0] = (char*) allocate_u1(charsize);
      cn[2*(i*nthresholds+j)+1] = (char*) allocate_u1(charsize);
      networkdefinitiontostr(cn[2*(i*nthresholds+j)+0], networkdefinitions[i], networkdefinitionparams[i]);
      thresholdtostr(cn[2*(i*nthresholds+j)+1], thresholds[j], thresholdparams[j]);
    }
  }

  char **rn = (char**) allocate_ptr(nmeasuresglobal);
  for (u4_t i = 0; i < nmeasuresglobal; i++) {
    rn[i] = (char*) allocate_u1(charsize);
    measuretostr(rn[i], measures[i]);
  }

  u4_t ogn[] = {nnetworkdefinitions*nthresholds, nmeasuresglobal};
  u4_t ognn[] = {2, 1};
  write_ntxt_f4(fo, &ogn[0], og, cn, rn, &ognn[0]);
}
