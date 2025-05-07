#ifndef STEPS_H
#define STEPS_H

#include "header/pf_database.h"
#include "header/dyn_model_database.h"
#include "header/toolkit/pf_solver.h"
#include "header/toolkit/dyn_simulator.h"
#include "header/toolkit/sc_solver.h"
#include "header/network/network_matrix.h"
#include "header/basic/constants.h"
#include "header/apis/steps_api_search_buffer.h"
#include "header/data_imexporter/psse_imexporter.h"
#include <ctime>
#include <string>

using namespace std;

class iSTEPS
{
public:
   iSTEPS(const string& toolkit_name = "", const string& log_file = "isteps_results.log", const string& cfg_file = "isteps_config.json");
   virtual ~iSTEPS();
   void set_name(const string& name);
   string get_name() const;

   void set_fast_math_logic(const bool flag = false);
   bool get_fast_math_logic();

   bool get_correct_three_winding_transformer_impedance_logic();
   void set_correct_three_winding_transformer_impedance_logic(const bool flag);

   void enable_dynamic_blocks_automatic_large_time_step_logic();
   void disable_dynamic_blocks_automatic_large_time_step_logic();
   bool get_dynamic_blocks_automatic_large_time_step_logic();
   void set_dynamic_blocks_automatic_large_stepsize(const bool flag = false);

   void set_thread_number(size_t n);
   size_t get_thread_number() const;

   void update_device_thread_number();
   size_t get_bus_thread_number() const;
   size_t get_generator_thread_number() const;
   size_t get_wt_generator_thread_number() const;
   size_t get_pv_unit_thread_number() const;
   size_t get_energy_storage_thread_number() const;
   size_t get_load_thread_number() const;
   size_t get_fixed_shunt_thread_number() const;
   size_t get_ac_line_thread_number() const;
   size_t get_transformer_thread_number() const;
   size_t get_lcc_hvdc2t_thread_number() const;
   size_t get_vsc_hvdc_thread_number() const;
   size_t get_equivalent_device_thread_number() const;

   void set_dynamic_model_database_size_in_bytes(size_t n);
   size_t get_dynamic_model_database_size_in_bytes();

   void report_toolkit_memory_usage();

   char get_next_alphabeta();
   void open_log_file(const string& file, bool append_mode = false);
   void close_log_file();
   void enable_detailed_log();
   void disable_detailed_log();
   bool is_detailed_log_enabled();
   void enable_optimize_network();
   void disable_optimize_network();
   bool is_optimize_network_enabled();

   void show_information(const string& info);
   void show_information(ostringstream& stream);
   void show_set_get_model_data_with_index_error(const string& device, const string& model, const string& func, size_t index);
   void show_set_get_model_data_with_name_error(const string& device, const string& model, const string& func, const string& par_name);
   string get_system_time_stamp_string(bool simplified = true);

   void clear_all_data();
   //void reset();
//    void terminate();

   PF_DATA&       get_database();
   DYN_DATA&   get_dynamic_model_database();
   PF_SOLVER&               get_powerflow_solver();
   SC_SOLVER&           get_short_circuit_solver();
   DYN_SIMULATOR&           get_dynamic_simulator();
   NET_MATRIX&                   get_network_matrix();

   double get_system_base_power_in_MVA() const;
   double get_one_over_system_base_power_in_one_over_MVA() const;

   void set_dynamic_simulation_time_step_in_s(double delt);
   double get_dynamic_simulation_time_step_in_s();
   void set_dynamic_simulation_time_in_s(double time);
   double get_dynamic_simulation_time_in_s();

   char steps_char_buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
   API_SEARCH_BUFFER api_search_buffer;
   PSSE_IMEXPORTER importer;
   PSSE_IMEXPORTER exporter;

   // 声明友元类建立访问权限
   friend class  PSSE_IMEXPORTER;

   PF_DATA pf_data;
   DYN_DATA dyn_data;
   PF_SOLVER pf_solver;
   SC_SOLVER sc_solver;
   DYN_SIMULATOR dyn_simulator;
   NET_MATRIX network;

private:
   time_t clock_started_;
   string toolkit_name_;
   string log_file_name_;
   string raw_file_name_;
   string dyr_file_name_;
   string cfg_file_name_;
   ofstream log_file_handle_;
   bool detailed_log_enabled;
   bool optimize_network_enabled;
   bool correct_three_winding_transformer_impedance;
   char current_alphabeta;
   size_t thread_number;

   //怀疑下面的thread变量都无用，可用thread_number来代替
   //真正需要启动并行计算的，需要根据实际设备数和计算核心数来自动计算合适的并行值
   size_t generator_thread_number, wt_generator_thread_number, pv_unit_thread_number, energy_storage_thread_number,
          load_thread_number, fixed_shunt_thread_number, line_thread_number, transformer_thread_number,
          lcc_hvdc2t_thread_number, vsc_hvdc_thread_number, equivalent_device_thread_number;

   size_t dynamic_model_db_size;


};
#endif // STEPS_H
