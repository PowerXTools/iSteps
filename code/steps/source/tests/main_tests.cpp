//#include <istream>
#include <iostream>
#include "header/pf_database.h"
#include "header/steps_namespace.h"
#include "header/toolkit/pf_solver.h"
#include "header/basic/utility.h"
#include "header/model/equivalent_model/equivalent_models.h"
#include "header/data_imexporter/equivalent_model_imexporter.h"
#include "header/toolkit/sc_solver.h"
#include "header/toolkit/cct_searcher.h"
#include "header/model/all_supported_models.h"
#include "header/tests/main_tests.h"

using namespace std;
void prepare_training_load_scale_case(const string rawfile, const string dyrfile);
void prepare_training_generation_shed_case(const string rawfile, const string dyrfile);

void test_powerflow(const string rawfile, const string cfgfile)
{
    std::cout << "Entering: " <<  __FUNCTION__ << std::endl;

    //default_toolkit.pf_data. set_default_capacity(cfgfile);
    default_toolkit.pf_data.set_max_bus_number(10000);

    default_toolkit.importer.load_powerflow_data(rawfile);

    cout<<"Done loading powerflow file"<<endl; //输出到终端
    //default_toolkit.importer.load_vsc_powerflow_data("../cases/4_terminal_vsc_hvdc.vscraw");

    //Parameter configure setting
    default_toolkit.pf_solver.set_max_iteration(150);
    default_toolkit.pf_solver.set_non_divergent_solution_logic(false);
    default_toolkit.pf_solver.set_active_power_mismatch_threshold(0.0001);
    default_toolkit.pf_solver.set_reactive_power_mismatch_threshold(0.0001);
    default_toolkit.pf_solver.set_flat_start_logic(true);
    default_toolkit.pf_solver.set_transformer_tap_adjustment_logic(true);

    //Perform PF calculation
    default_toolkit.pf_solver.solve_with_full_Newton_Raphson_solution();

    default_toolkit.network.check_network_connectivity(false);
    default_toolkit.network.report_network_matrix();

    default_toolkit.pf_solver.save_network_Y_matrix(rawfile+"_net_matrix.csv");
    default_toolkit.pf_solver.save_jacobian_matrix(rawfile+"_Jacobian_matrix.csv");
    default_toolkit.pf_solver.save_powerflow_result(rawfile+"_bus_powerflow_result.csv");
    default_toolkit.pf_solver.save_extended_powerflow_result(rawfile+"_detailed_result.csv");

    /*
    vector<DEVICE_ID> dids  = psdb.get_all_devices_device_id_connecting_to_bus(1);
    unsigned int n = dids.size();
    for(unsigned int i=0; i!=n; ++i)
    {
       cout<<dids[i].get_compound_device_name()<<endl;
    }
    */

};

void test_arxl_simulation(const string rawfile,const string dyrfile)
{
    std::cout << "开始执行函数: " <<  __FUNCTION__ << std::endl;
    //prepare_training_load_scale_case();
    //prepare_training_generation_shed_case();

    PF_DATA& pfdb = default_toolkit.get_database();
    DYN_SIMULATOR& dyn_simulator = default_toolkit.get_dynamic_simulator();

    pfdb.set_max_bus_number(1000);
    pfdb.set_system_base_power_in_MVA(100.0);

    default_toolkit.set_dynamic_simulation_time_step_in_s(0.001);
    dyn_simulator.set_allowed_max_power_imbalance_in_MVA(0.01);
    dyn_simulator.set_max_DAE_iteration(10);
    dyn_simulator.set_max_network_iteration(200);
    dyn_simulator.set_iteration_accelerator(0.7);


    PSSE_IMEXPORTER importer(default_toolkit);

    importer.load_powerflow_data(rawfile);
    importer.load_dynamic_data(dyrfile);

    EQUIVALENT_MODEL_IMEXPORTER eqimporter(default_toolkit);
    //eqimporter.load_equivalent_model("C:/Users/charles/Desktop/arx/arx_model.eqv");

    PF_SOLVER pf_solver(default_toolkit);

    pf_solver.set_max_iteration(30);
    pf_solver.set_active_power_mismatch_threshold(0.00001);
    pf_solver.set_reactive_power_mismatch_threshold(0.00001);
    pf_solver.set_flat_start_logic(false);
    pf_solver.set_transformer_tap_adjustment_logic(true);

    pf_solver.solve_with_fast_decoupled_solution();

    dyn_simulator.prepare_meters();

    dyn_simulator.set_output_file("../IEEE_39_bus_model_GENROU_SEXS_IEEEG1_with_ARXL");

    dyn_simulator.start();
    dyn_simulator.run_to(1.0);

    DEVICE_ID did;
    did.set_device_type(STEPS_LOAD);
    TERMINAL terminal;
    terminal.append_bus(3);
    did.set_device_terminal(terminal);
    did.set_device_identifier_index(get_index_of_string("1"));

    dyn_simulator.scale_load(did, -0.1);

    dyn_simulator.run_to(1.1);

    dyn_simulator.switch_on_equivalent_device();
    vector<size_t> buses_to_clear;
    buses_to_clear.push_back(19);
    buses_to_clear.push_back(20);
    buses_to_clear.push_back(33);
    buses_to_clear.push_back(34);
    buses_to_clear.push_back(21);
    buses_to_clear.push_back(22);
    buses_to_clear.push_back(23);
    buses_to_clear.push_back(24);
    buses_to_clear.push_back(35);
    buses_to_clear.push_back(36);
    dyn_simulator.trip_buses(buses_to_clear);

    dyn_simulator.run_to(5.0);

    dyn_simulator.reset_data();
    pfdb.clear_all_data();
};

void test_cct_search(const string rawfile,const string dyrfile)
{
    std::cout << "开始执行函数: " <<  __FUNCTION__ << std::endl;

    default_toolkit.set_dynamic_simulation_time_step_in_s(0.01);

    PF_DATA& psdb = default_toolkit.get_database(); // create a new database
    psdb.set_max_bus_number(1000); // set the max bus number of the database

    PSSE_IMEXPORTER importer(default_toolkit); // create an imexporter

    importer.load_powerflow_data(rawfile); // load powerflow

    vector<DEVICE_ID> lines = psdb.get_all_ac_lines_device_id(); // the powerflow data is only used for get line device id
    size_t n_lines = lines.size();

    vector<DEVICE_ID> fault_lines;// the three vectors are used to store searching result
    vector<size_t> fault_side_buses;
    vector<double> ccts;

    for(size_t i=0; i!=n_lines; ++i)
    {
        DEVICE_ID did = lines[i];
        fault_lines.push_back(did);
        fault_lines.push_back(did);

        TERMINAL terminal = did.get_device_terminal();
        vector<size_t> buses = terminal.get_buses();
        fault_side_buses.push_back(buses[0]);
        fault_side_buses.push_back(buses[1]);

        ccts.push_back(0.0);
        ccts.push_back(0.0);
    }

    size_t n_events = n_lines*2;

    //#pragma omp parallel for
    for(size_t i=0; i!=n_events; ++i)
    {
        char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
        CCT_SEARCHER searcher;
        searcher.set_power_system_database_maximum_bus_number(1000);
        searcher.set_search_title("");
        searcher.set_powerflow_data_filename(rawfile);
        searcher.set_dynamic_data_filename(dyrfile);

        DEVICE_ID did = fault_lines[i];
        searcher.set_fault_device(did);
        searcher.set_fault_side_bus(fault_side_buses[i]);
        searcher.set_fault_location_to_fault_side_bus_in_pu(0.0);
        searcher.set_fault_shunt_in_pu(complex<double>(0.0, -2e8));
        searcher.set_flag_trip_ac_line_after_clearing_fault(true);

        snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Now go searching CCT for fault at side %u of %s.",
                 searcher.get_fault_side_bus(),(did.get_compound_device_name()).c_str());
        default_toolkit.show_information(buffer);
        double cct = searcher.search_cct();
        snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Now done searching CCT for fault at side %u of %s.",
                 searcher.get_fault_side_bus(),(did.get_compound_device_name()).c_str());
        default_toolkit.show_information(buffer);
        ccts[i] = cct;

        searcher.run_case_with_clearing_time(cct);
        searcher.run_case_with_clearing_time(cct+0.1);
    }

    char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
    snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Searched CCT of all lines:\n");
    default_toolkit.show_information(buffer);
    for(size_t i=0; i!=n_events; ++i)
    {
        snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s at fault side %u: %fs.\n",(fault_lines[i].get_compound_device_name()).c_str(),
                 fault_side_buses[i], ccts[i]);
        default_toolkit.show_information(buffer);
    }
};

void test_dynamics_simulation(const string rawfile,const string dyrfile, const string vscfile)
{
    std::cout << "Entering: " <<  __FUNCTION__ << std::endl;
    default_toolkit.set_fast_math_logic(false);

    PF_DATA& psdb = default_toolkit.get_database();
    PF_SOLVER& pf_solver = default_toolkit.get_powerflow_solver();
    DYN_SIMULATOR& simulator = default_toolkit.get_dynamic_simulator();

    psdb.set_max_bus_number(10000);
    PSSE_IMEXPORTER importer(default_toolkit);

    //importer.load_powerflow_data("../cases/IEEE39.raw");
    //importer.load_dynamic_data("IEEE39_GENROU_SEXS_IEEEG1.dyr");
    //importer.load_powerflow_data("bench_shandong_v2.raw");
    //importer.load_dynamic_data("bench_shandong_v2.dyr");

    //importer.load_powerflow_data("../../../bench/IEEE39_with_four_vsc.raw");
    //cout<<"Done loading powerflow file"<<endl;
    //importer.load_vsc_powerflow_data("../../../bench/4_terminal_vsc_hvdc.vscraw");

    //importer.load_powerflow_data("../../../bench/shandong_original/bench_shandong.raw");
    cout<<"Done loading powerflow file"<<endl;
    //importer.load_vsc_powerflow_data("../../../bench/shandong/4_terminal_vsc_hvdc.vscraw");

    importer.load_powerflow_data(rawfile);
    cout<<"Done loading powerflow file"<<endl;
//    importer.load_vsc_powerflow_data("../../../bench/39_bus/2_VSC.vscraw");
    importer.load_vsc_powerflow_data(vscfile);

    pf_solver.set_max_iteration(30);
    pf_solver.set_active_power_mismatch_threshold(0.00001);
    pf_solver.set_reactive_power_mismatch_threshold(0.00001);
    pf_solver.set_flat_start_logic(false);
    pf_solver.set_transformer_tap_adjustment_logic(true);

    pf_solver.solve_with_full_Newton_Raphson_solution();
    pf_solver.save_network_Y_matrix("39_bus_Y_matrix.csv");
    pf_solver.save_jacobian_matrix("39_bus_jacobian.csv");
    pf_solver.save_extended_powerflow_result("39_bus.csv");

    //importer.load_dynamic_data("../../../bench/shandong_load/5_bus.dyr");
    importer.load_dynamic_data(dyrfile);

    //vector<GENERATOR*> gens = psdb.get_all_generators();
    //size_t n = gens.size();
    //for(size_t i=0; i!=n; ++i)
    //{
    //    GENERATOR* gen = gens[i];
    //    SYNC_GENERATOR_MODEL* model = (SYNC_GENERATOR_MODEL*) gen->get_model_of_type("SYNC GENERATOR");
    //    cout<<gen->get_compound_device_name()<<", gen model = "<<model<<endl;
    //}

    //psdb.check_dynamic_data();

    //VSC_HVDC* vsc_hvdc = psdb.get_vsc_hvdc("Vsc-Project1");
    //VSC_HVDC_CONVERTER_MODEL* model = vsc_hvdc->get_vsc_hvdc_converter_model(0);
    //model->report();

    default_toolkit.set_dynamic_simulation_time_step_in_s(0.001);
    simulator.set_allowed_max_power_imbalance_in_MVA(0.01);
    simulator.set_max_DAE_iteration(3);
    simulator.set_min_DAE_iteration(3);
    simulator.set_max_network_iteration(50);
    simulator.set_iteration_accelerator(0.8);
    simulator.set_allowed_max_power_imbalance_in_MVA(0.00001);

    /*
        simulator.prepare_bus_related_meter(9101, "voltage in pu");
        simulator.prepare_bus_related_meter(9102, "voltage in pu");
        simulator.prepare_bus_related_meter(9103, "voltage in pu");
        simulator.prepare_bus_related_meter(9104, "voltage in pu");
        simulator.prepare_bus_related_meter(9101, "frequency in pu");
        simulator.prepare_bus_related_meter(9102, "frequency in pu");
        simulator.prepare_bus_related_meter(9103, "frequency in pu");
        simulator.prepare_bus_related_meter(9104, "frequency in pu");
        simulator.prepare_bus_related_meter(9101, "ANGLE IN DEG");
        simulator.prepare_bus_related_meter(9102, "ANGLE IN DEG");
        simulator.prepare_bus_related_meter(9103, "ANGLE IN DEG");
        simulator.prepare_bus_related_meter(9104, "ANGLE IN DEG");
        simulator.prepare_bus_related_meter(9101, "FREQUENCY DEVIATION IN PU");
        simulator.prepare_bus_related_meter(9102, "FREQUENCY DEVIATION IN PU");
        simulator.prepare_bus_related_meter(9103, "FREQUENCY DEVIATION IN PU");
        simulator.prepare_bus_related_meter(9104, "FREQUENCY DEVIATION IN PU");
        simulator.prepare_bus_related_meter(9101, "FREQUENCY DEVIATION IN HZ");
        simulator.prepare_bus_related_meter(9102, "FREQUENCY DEVIATION IN HZ");
        simulator.prepare_bus_related_meter(9103, "FREQUENCY DEVIATION IN HZ");
        simulator.prepare_bus_related_meter(9104, "FREQUENCY DEVIATION IN HZ");
    */

    simulator.prepare_bus_related_meters();
    simulator.prepare_vsc_hvdc_related_meters();
    simulator.prepare_generator_related_meters();
    //simulator.prepare_load_related_meters();
    //simulator.prepare_wt_generator_related_meters();

    simulator.set_output_file("IEEE39_vsc");

    //simulator.set_output_file("IEEE39_2VSC");


    //simulator.show_dynamic_simulator_configuration();


    //simulator.set_output_file("test_log/IEEE_39_bus_model_GENROU_SEXS_IEEEG1_load_scale_down_at_bus_3_by_10%");
    //simulator.set_output_file("test_log/IEEE_39_bus_model_GENROU_SEXS_IEEEG1_generation_scale_down_at_bus_38_by_10%");
    //simulator.set_output_file("test_log/IEEE_39_bus_model_GENROU_SEXS_IEEEG1_trip_bus_");
    //simulator.set_output_file("test_log/bench_shandong_with_avr_fault_at_line_82_60");
    //simulator.set_output_file("test_log/bench_shandong_with_avr_fault_at_line_82_80");
    //simulator.set_output_file("test_log/bench_shandong_with_avr_trip_mac_140");


    simulator.start();
    simulator.run_to(0.01);
    /*
        DEVICE_ID did;
        did.set_device_type(STEPS_LOAD);
        TERMINAL terminal;
        terminal.append_bus(103);
        did.set_device_terminal(terminal);
        did.set_device_identifier_index(get_index_of_string("1"));

        simulator.scale_load(did, 0.05);
        */
    /*
    DEVICE_ID did;
    did.set_device_type(STEPS_AC_LINE);
    TERMINAL terminal;
    terminal.append_bus(16);
    terminal.append_bus(21);
    did.set_device_terminal(terminal);
    did.set_device_identifier_index(get_index_of_string("1"));
    simulator.set_ac_line_fault(did, 16, 0.0, complex<double>(0, -100));
    simulator.run_to(1.1);
    simulator.clear_ac_line_fault(did, 16, 0.0);
    */

    DEVICE_ID did;
    did.set_device_type(STEPS_LOAD);
    TERMINAL terminal;
    terminal.append_bus(23);
    did.set_device_terminal(terminal);
    did.set_device_identifier_index(get_index_of_string("1"));
    simulator.scale_load(did, 0.5);

    /*
        did.set_device_type(STEPS_WT_GENERATOR);
        TERMINAL terminal;
        terminal.append_bus(103);
        did.set_device_terminal(terminal);
        did.set_device_identifier_index(get_index_of_string("1"));

        simulator.shed_wt_generator(did, 0.01);
    */
    /*
    DEVICE_ID did;
    did.set_device_type(STEPS_GENERATOR);
    TERMINAL terminal;
    terminal.append_bus(104);
    did.set_device_terminal(terminal);
    did.set_device_identifier_index(get_index_of_string("1"));

    simulator.trip_generator(did);
    */

    /*DEVICE_ID did;
    did = get_2t_lcc_hvdc_device_id(57,201,"ZaQingDi");
    simulator.manual_block_2t_lcc_hvdc(did);*/
    //simulator.run_to(5);


    //simulator.scale_all_load(-0.05);
    simulator.run_to(5);


    /*did.set_device_type(STEPS_GENERATOR);
    terminal.clear();
    terminal.append_bus(38);
    did.set_device_terminal(terminal);
    did.set_device_identifier_index(get_index_of_string("1"));

    simulator.shed_generator(did, 0.1);
    */

    /*
        DEVICE_ID did;
        did.set_device_type(STEPS_AC_LINE);
        TERMINAL terminal;
        terminal.append_bus(82);
        terminal.append_bus(80);
        did.set_device_terminal(terminal);
        did.set_device_identifier_index(get_index_of_string("1"));

        simulator.set_ac_line_fault(did, 82, 0.0, complex<double>(0.0, -1e4));

        simulator.run_to(1.35);

        simulator.clear_ac_line_fault(did, 82, 0.0);*/
    /*
        DEVICE_ID did;
        did.set_device_type(STEPS_GENERATOR);
        TERMINAL terminal;
        terminal.append_bus(140);
        did.set_device_terminal(terminal);
        did.set_device_identifier_index(get_index_of_string("1"));

        simulator.trip_generator(did);


        DEVICE_ID did;
        did.set_device_type(STEPS_GENERATOR);
        TERMINAL terminal;
        terminal.append_bus(1);
        did.set_device_terminal(terminal);
        did.set_device_identifier_index(get_index_of_string("1"));

        simulator.shed_generator(did, 0.2);

        simulator.run_to(20.0);

        psdb.clear();
        simulator.clear();
    */
};
void test_gprof(const string rawfile,const string dyrfile)
  {
    std::cout << "开始执行函数: " <<  __FUNCTION__ << std::endl;
    clock_t start = clock();
    DYN_SIMULATOR& simulator = default_toolkit.get_dynamic_simulator();
    PF_DATA& psdb = default_toolkit.get_database();

    string file = "test_log/";
    file += __FUNCTION__;
    file += ".txt";
    default_toolkit.open_log_file(file);

    PSSE_IMEXPORTER importer(default_toolkit);

    psdb.set_max_bus_number(1000);

    importer.load_powerflow_data(rawfile);
    importer.load_dynamic_data(dyrfile);

    vector<LCC_HVDC2T*> hvdcs = psdb.get_all_2t_lcc_hvdcs();
    unsigned int n = hvdcs.size();
    for(unsigned int i=0; i!=n; ++i)
        hvdcs[i]->turn_rectifier_constant_power_mode_into_constant_current_mode();

    PF_SOLVER& pf_solver = default_toolkit.get_powerflow_solver();

    pf_solver.set_max_iteration(30);
    pf_solver.set_active_power_mismatch_threshold(0.00001);
    pf_solver.set_reactive_power_mismatch_threshold(0.00001);
    pf_solver.set_flat_start_logic(false);
    pf_solver.set_transformer_tap_adjustment_logic(true);

    pf_solver.solve_with_fast_decoupled_solution();

    //simulator.prepare_meters();

    simulator.set_output_file("test_log/bench_shandong_100_bus_model_dynamic_test_result_GENROU_CDC4T");
    simulator.set_allowed_max_power_imbalance_in_MVA(0.001);
    //simulator.set_max_DAE_iteration(20);
    //simulator.set_max_network_iteration(200);
    default_toolkit.set_dynamic_simulation_time_step_in_s(0.01);

    simulator.start();
    simulator.run_to(1.0);

    DEVICE_ID did;
    did.set_device_type(STEPS_AC_LINE);
    TERMINAL terminal;
    terminal.append_bus(60);
    terminal.append_bus(62);
    did.set_device_terminal(terminal);
    did.set_device_identifier_index(get_index_of_string("1"));

    simulator.set_ac_line_fault(did, 60, 0.0, complex<double>(0.0, -1e6));

    simulator.run_to(1.1);

    simulator.clear_ac_line_fault(did, 60, 0.0);
    simulator.trip_ac_line(did);

    simulator.run_to(5.0);
    default_toolkit.close_log_file();

    clock_t stop = clock();

    cout<<"time elapse: "<<double(stop-start)*1e-6<<" s"<<endl;
  };

void test_short_circuit(const string rawfile, const string seqfile)
{
    std::cout << "开始执行函数: " <<  __FUNCTION__ << std::endl;

    PSSE_IMEXPORTER importer(default_toolkit);
    importer.set_supplement_sequence_model_from_dynamic_model_logic(false);

//    importer.load_powerflow_data("../../../bench/IEEE9.raw");
//    importer.load_sequence_data("../../../bench/IEEE9.seq");
 //   importer.load_dynamic_data("../../../bench/IEEE9_detail.dyr");

    importer.load_powerflow_data(rawfile);
    importer.load_sequence_data(seqfile);

//    importer.load_powerflow_data("../../../bench/example_grid.raw");
//    importer.load_sequence_data("../../../bench/example_grid.seq");


    PF_SOLVER pf_solver(default_toolkit);
    pf_solver.set_max_iteration(30);
    pf_solver.set_active_power_mismatch_threshold(0.00001);
    pf_solver.set_reactive_power_mismatch_threshold(0.00001);
    pf_solver.set_flat_start_logic(false);
    pf_solver.set_transformer_tap_adjustment_logic(true);

    pf_solver.solve_with_fast_decoupled_solution();
    //pf_solver.solve_with_full_Newton_Raphson_solution();
    pf_solver.show_powerflow_result();
    pf_solver.save_network_Y_matrix("pf_matrix_steps.csv");
    pf_solver.save_bus_powerflow_result_to_file("pf_bus_result.csv");

    SC_SOLVER sc_solver(default_toolkit);
    sc_solver.set_generator_reactance_option(SUBTRANSIENT_REACTANCE);
    sc_solver.set_units_of_currents_and_voltages(PU);
    sc_solver.set_coordinates_of_currents_and_voltages(RECTANGULAR);
    sc_solver.set_consider_load_logic(true);
    sc_solver.set_consider_motor_load_logic(true);
//    sc_solver.set_option_of_DC_lines(CONVERT_TO_CONSTANT_ADMITTANCE_LOAD);
    sc_solver.set_option_of_DC_lines(BLOCK_AND_IGNORE);


//    DEVICE_ID did = get_ac_line_device_id(1, 2, "1");
//    sc_solver.set_ac_line_fault(did, 1, 0.3, SINGLE_PHASE_GROUNDED_FAULT, complex<double>(0.0, -1e3));


//    sc_solver.set_bus_fault(6, SINGLE_PHASE_GROUNDED_FAULT, complex<double>(0.0, -1e3));
//    sc_solver.solve();
//    sc_solver.show_short_circuit_result();
//
//    sc_solver.clear_fault();
//    sc_solver.set_bus_fault(7, SINGLE_PHASE_GROUNDED_FAULT, complex<double>(0.0, -1e3));
//    sc_solver.solve();
//    sc_solver.show_short_circuit_result();
//
//    sc_solver.clear_fault();
//    sc_solver.set_bus_fault(8, SINGLE_PHASE_GROUNDED_FAULT, complex<double>(0.0, -1e3));
//    sc_solver.solve();
//    sc_solver.show_short_circuit_result();

//    importer.export_sequence_data("IEEE9.seq");
//
//    sc_solver.save_positive_sequence_Y_matrix_to_file("positive_sequence_Y.csv");
//    sc_solver.save_negative_sequence_Y_matrix_to_file("negative_sequence_Y.csv");
//    sc_solver.save_zero_sequence_Y_matrix_to_file("zero_sequence_Y.csv");
//    sc_solver.save_short_circuit_result_to_file("short_circuit_result.csv");
//    sc_solver.save_extended_short_circuit_result_to_file("extended_short_circuit_result.csv");


    sc_solver.set_max_iteration_cout_for_iterative_method(10);
    sc_solver.set_voltage_threshold_for_iterative_method(0.001);

    sc_solver.set_bus_fault(6, SINGLE_PHASE_GROUNDED_FAULT, complex<double>(0.0, -1e3));
    sc_solver.solve_with_iteration();
    sc_solver.show_short_circuit_result();
};
void test_small_signal_analysis(const string rawfile)
{
    std::cout << "开始执行函数: " <<  __FUNCTION__ << std::endl;

    default_toolkit.set_fast_math_logic(false);
    default_toolkit.open_log_file("ssa-ieee9.log");
    //default_toolkit.enable_detailed_log();

    ostringstream osstream;

    PF_DATA& psdb = default_toolkit.get_database();
    PF_SOLVER& pf_solver = default_toolkit.get_powerflow_solver();
    DYN_SIMULATOR& simulator = default_toolkit.get_dynamic_simulator();

    psdb.set_max_bus_number(10000);
    PSSE_IMEXPORTER importer(default_toolkit);

    importer.load_powerflow_data(rawfile);

    pf_solver.set_max_iteration(30);
    pf_solver.set_active_power_mismatch_threshold(0.00001);
    pf_solver.set_reactive_power_mismatch_threshold(0.00001);
    pf_solver.set_flat_start_logic(false);
    pf_solver.set_transformer_tap_adjustment_logic(true);

    pf_solver.solve_with_full_Newton_Raphson_solution();
    pf_solver.save_network_Y_matrix("9_bus_Y_matrix.csv");
    pf_solver.save_jacobian_matrix("9_bus_jacobian.csv");
    pf_solver.save_extended_powerflow_result("9_bus.csv");

    importer.load_dynamic_data("IEEE9_ssa.dyr");
    psdb.check_dynamic_data();

    default_toolkit.set_dynamic_simulation_time_step_in_s(0.001);
    simulator.set_allowed_max_power_imbalance_in_MVA(0.01);
    simulator.set_max_DAE_iteration(3);
    simulator.set_min_DAE_iteration(3);
    simulator.set_max_network_iteration(50);
    simulator.set_iteration_accelerator(0.8);
    simulator.set_allowed_max_power_imbalance_in_MVA(0.00001);

    simulator.start(); // initialize all devices

    vector<GENERATOR*> gens = psdb.get_all_generators();
    size_t n = gens.size();
    for(size_t i = 0; i<n; ++i)
    {
        GENERATOR* gen = gens[i];
        gen->build_linearized_matrix_ABCD();
    }

    default_toolkit.close_log_file();
};
void test_ufls_simulation(const string rawfile,const string dyrfile)
  {
    std::cout << "开始执行函数: " <<  __FUNCTION__ << std::endl;

   PF_DATA& psdb = default_toolkit.get_database();

    PSSE_IMEXPORTER importer(default_toolkit);

    importer.load_powerflow_data(rawfile);
    importer.load_dynamic_data(dyrfile);

    PF_SOLVER pf_solver(default_toolkit);

    pf_solver.set_max_iteration(30);
    pf_solver.set_active_power_mismatch_threshold(0.00001);
    pf_solver.set_reactive_power_mismatch_threshold(0.00001);
    pf_solver.set_flat_start_logic(false);
    pf_solver.set_transformer_tap_adjustment_logic(true);

    pf_solver.solve_with_fast_decoupled_solution();

    DYN_SIMULATOR simulator(default_toolkit);
    default_toolkit.set_dynamic_simulation_time_step_in_s(0.01);

    simulator.set_allowed_max_power_imbalance_in_MVA(0.001);
    simulator.set_max_network_iteration(200);
    simulator.set_max_DAE_iteration(10);
    simulator.set_iteration_accelerator(0.8);

    simulator.prepare_meters();

    simulator.set_output_file("IEEE_39_bus_model_dynamic_test_result_GENROU_SEXS_IEEEG1_UFLS");

    // change spinning reserve
    double SR = 0.0001;
    vector<GENERATOR*> generators = psdb.get_all_generators();
    size_t n = generators.size();
    GENERATOR* gen;
    for(size_t i=0; i!=n; ++i)
    {
        gen = generators[i];
        double pgen = gen->get_p_generation_in_MW();
        double one_over_mbase = gen->get_one_over_mbase_in_one_over_MVA();
        double pmax = pgen*one_over_mbase+SR;

        TURBINE_GOVERNOR_MODEL* tg_model = gen->get_turbine_governor_model();
        if(tg_model!=NULL)
        {
            string model_name = tg_model->get_model_name();
            if(model_name=="IEEEG1")
            {
                IEEEG1* g1_model = (IEEEG1*) tg_model;
                g1_model->set_Pmax_in_pu(pmax);
            }
        }

        //SYNC_GENERATOR_MODEL* sync_model = gen->get_sync_generator_model();
        //sync_model->set_D(2.0);
    }


    simulator.start();
    simulator.run_to(1.0);

    for(size_t i=0; i!=n; ++i)
    {
        gen = generators[i];
        simulator.shed_generator(gen->get_device_id(), 0.25);
    }
/*
    DEVICE_ID did;
    did.set_device_type(STEPS_GENERATOR);
    TERMINAL terminal;
    terminal.append_bus(39);
    did.set_device_terminal(terminal);
    did.set_device_identifier_index(get_index_of_string("1"));

    simulator.trip_generator(did);

    terminal.clear();
    terminal.append_bus(38);
    did.set_device_terminal(terminal);
    simulator.trip_generator(did);

    terminal.clear();
    terminal.append_bus(37);
    did.set_device_terminal(terminal);
    simulator.trip_generator(did);
*/
    simulator.run_to(20);
  };

void prepare_training_load_scale_case(const string rawfile, const string dyrfile)
{
    PF_DATA& psdb = default_toolkit.get_database();
    DYN_SIMULATOR& simulator = default_toolkit.get_dynamic_simulator();

    psdb.set_max_bus_number(1000);
    psdb.set_system_base_power_in_MVA(100.0);

    simulator.set_allowed_max_power_imbalance_in_MVA(0.0001);
    default_toolkit.set_dynamic_simulation_time_step_in_s(0.001);

    PSSE_IMEXPORTER importer(default_toolkit);

    importer.load_powerflow_data(rawfile);
    importer.load_dynamic_data(dyrfile);

    PF_SOLVER pf_solver(default_toolkit);

    pf_solver.set_max_iteration(30);
    pf_solver.set_active_power_mismatch_threshold(0.00001);
    pf_solver.set_reactive_power_mismatch_threshold(0.00001);
    pf_solver.set_flat_start_logic(false);
    pf_solver.set_transformer_tap_adjustment_logic(true);

    pf_solver.solve_with_fast_decoupled_solution();

    simulator.prepare_meters();

    simulator.set_output_file("test_log/IEEE_39_bus_model_GENROU_SEXS_IEEEG1_load_scale_down_at_bus_3_by_10%");

    simulator.start();
    simulator.run_to(1.0);

    DEVICE_ID did;
    did.set_device_type(STEPS_LOAD);
    TERMINAL terminal;
    terminal.append_bus(3);
    did.set_device_terminal(terminal);
    did.set_device_identifier_index(get_index_of_string("1"));

    simulator.scale_load(did, -0.1);

    simulator.run_to(20.0);

    simulator.reset_data();

    psdb.clear_all_data();
}

void prepare_training_generation_shed_case(const string rawfile, const string dyrfile)
{
    PF_DATA& psdb = default_toolkit.get_database();
    DYN_SIMULATOR& simulator = default_toolkit.get_dynamic_simulator();

    psdb.set_max_bus_number(1000);
    psdb.set_system_base_power_in_MVA(100.0);

    simulator.set_allowed_max_power_imbalance_in_MVA(0.0001);
    default_toolkit.set_dynamic_simulation_time_step_in_s(0.001);

    PSSE_IMEXPORTER importer(default_toolkit);

    importer.load_powerflow_data(rawfile);
    importer.load_dynamic_data(dyrfile);

    PF_SOLVER  pf_solver(default_toolkit);

    pf_solver.set_max_iteration(30);
    pf_solver.set_active_power_mismatch_threshold(0.00001);
    pf_solver.set_reactive_power_mismatch_threshold(0.00001);
    pf_solver.set_flat_start_logic(false);
    pf_solver.set_transformer_tap_adjustment_logic(true);

    pf_solver.solve_with_fast_decoupled_solution();

    simulator.prepare_meters();

    simulator.set_output_file("test_log/IEEE_39_bus_model_GENROU_SEXS_IEEEG1_generation_shed_down_at_bus_38_by_10%");

    simulator.start();
    simulator.run_to(1.0);

    DEVICE_ID did;
    did.set_device_type(STEPS_GENERATOR);
    TERMINAL terminal;
    terminal.append_bus(38);
    did.set_device_terminal(terminal);
    did.set_device_identifier_index(get_index_of_string("1"));

    simulator.shed_generator(did, 0.1);

    simulator.run_to(20.0);

    simulator.reset_data();

    psdb.clear_all_data();
}

