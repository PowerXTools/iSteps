#ifndef MAIN_TEST
#define MAIN_TEST
#include <string>

using namespace std;

void test_powerflow(const string rawfile, const string cfgfile);
void test_arxl_simulation(const string rawfile,const string dyrfile);
void test_cct_search(const string rawfile,const string dyrfile);
void test_dynamics_simulation(const string rawfile,const string dyrfile, const string faulttabs);
void test_gprof(const string rawfile,const string dyrfile);
void test_short_circuit(const string rawfile, const string seqfile);
void test_small_signal_analysis(const string rawfile);
void test_ufls_simulation(const string rawfile,const string dyrfile);

#endif
