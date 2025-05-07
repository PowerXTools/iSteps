#include "header/tests/main_tests.h"
#include <iostream>
#include <filesystem>

using namespace std;

int main()
{
    auto path = std::filesystem::current_path();
    std::cout << "当前工作目录: " << path << std::endl;

    cout<<"Runing " << __FILE__ <<endl;

    string cfgfile = "../cases/isteps_config.json";
    string rawfile = "../cases/IEEE39_with_four_terminal_vsc.raw";
    string dyrfile = "../cases/IEEE39_with_four_terminal_vsc.dyr";

    test_powerflow(rawfile, cfgfile);
    //test_arxl_simulation(rawfile, dyrfile);
    //test_cct_search(rawfile, dyrfile);
    //test_dynamics_simulation(rawfile,  dyrfile, vscfile);
    //test_gprof(rawfile, dyrfile);
    //test_short_circuit(rawfile, seqfile);
    //test_small_signal_analysis(rawfile);
    //test_ufls_simulation(rawfile,  dyrfile);

    return 0;
}
