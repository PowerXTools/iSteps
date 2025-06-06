#include "header/toolkit/pf_solver.h"
#include "header/basic/utility.h"
#include "header/basic/constants.h"
#include "header/steps_namespace.h"
#include "header/network/jacobian_builder.h"
#include <format> // C++20

using namespace std;

#define ENABLE_OPENMP_FOR_POWERFLOW_SOLVER

PF_SOLVER::PF_SOLVER(iSTEPS& toolkit)
{
   this->toolkit = (&toolkit);
   jacobian_builder = new JACOBIAN_BUILDER(toolkit);
   clear_all_data();
}

PF_SOLVER::~PF_SOLVER()
{
   toolkit = NULL;
   delete jacobian_builder;
}

iSTEPS& PF_SOLVER::get_toolkit() const
{
   return *toolkit;
}

void PF_SOLVER::clear_all_data()
{
   set_flat_start_logic(false);
   set_transformer_tap_adjustment_logic(true);
   set_non_divergent_solution_logic(true);
   set_var_limit_check_logic(true);
   set_export_jacobian_matrix_step_by_step_logic(false);
   set_active_power_mismatch_threshold(0.001);
   set_reactive_power_mismatch_threshold(0.001);
   set_maximum_voltage_change_in_pu(999.0);
   set_maximum_angle_change_in_deg(999.0);

   set_convergence_flag(false);

   set_max_iteration(50);

   bus_current.clear();
   bus_power.clear();

   jacobian.clear();

   iteration_count = 0;
   set_iteration_accelerator(1.0);

   S_mismatch.clear();
   P_mismatch.clear();
   Q_mismatch.clear();
}

NET_MATRIX& PF_SOLVER::get_network_matrix()
{
   return toolkit->get_network_matrix();
}

void PF_SOLVER::set_max_iteration(size_t iteration)
{
   this->max_iteration = iteration;
}

size_t PF_SOLVER::get_max_iteration() const
{
   return max_iteration;
}

void PF_SOLVER::set_active_power_mismatch_threshold(double P)
{
   if ( P > 0.0 ) {
      P_threshold_in_MW = P;
   } else {
      toolkit->show_information(std::format( "P Mismatch threshold warning:  ({:.8f} MVar) is not allowed. 0.0001MVar is used Instead.", P));
      P_threshold_in_MW = 0.0001;
   }
}

void PF_SOLVER::set_reactive_power_mismatch_threshold(double Q)
{
   if ( Q > 0.0 ) {
      Q_threshold_in_MVar = Q;
   } else {
      toolkit->show_information(std::format( "Q Mismatch threshold warning:  ({:.8f} MVar) is not allowed. 0.0001MVar is used Instead.", Q));
      Q_threshold_in_MVar = 0.0001;
   }
}

void PF_SOLVER::set_allowed_max_power_mismatch( double P, double Q)
{
   if ( P > 0.0 ) {
      P_threshold_in_MW = P;
   } else {
      toolkit->show_information(std::format( "P Mismatch threshold warning:  ({:.8f} MVar) is not allowed. 0.0001MVar is used Instead.", P));
      P_threshold_in_MW = 0.0001;
   }

   if ( Q > 0.0 ) {
      Q_threshold_in_MVar = Q;
   } else {
      toolkit->show_information(std::format( "Q Mismatch threshold warning:  ({:.8f} MVar) is not allowed. 0.0001MVar is used Instead.", Q));
      Q_threshold_in_MVar = 0.0001;
   }
}

void PF_SOLVER::set_maximum_voltage_change_in_pu(double v)
{
   maximum_voltage_change_in_pu = v;
}

void PF_SOLVER::set_maximum_angle_change_in_deg(double a)
{
   maximum_angle_change_in_deg = a;
}

void PF_SOLVER::set_flat_start_logic(bool logic)
{
   flat_start_enabled = logic;
}

void PF_SOLVER::set_transformer_tap_adjustment_logic(bool logic)
{
   transformer_tap_adjustment_enabled = logic;
}

void PF_SOLVER::set_iteration_accelerator(double iter_alpha)
{
   if (iter_alpha > 0.0) {
      this->alpha = iter_alpha;
   } else {
      this->alpha = 1.0;
   }
}

void PF_SOLVER::set_non_divergent_solution_logic(bool logic)
{
   non_divergent_solution_enabled = logic;
}

void PF_SOLVER::set_var_limit_check_logic(bool logic)
{
   var_limit_check_enabled = logic;
}

void PF_SOLVER::set_export_jacobian_matrix_step_by_step_logic(bool flag)
{
   export_jacobian_matrix_step_by_step = flag;
}

double PF_SOLVER::get_allowed_max_active_power_imbalance_in_MW() const
{
   return P_threshold_in_MW;
}

double PF_SOLVER::get_allowed_max_reactive_power_imbalance_in_MVar() const
{
   return Q_threshold_in_MVar;
}

double PF_SOLVER::get_maximum_voltage_change_in_pu() const
{
   return maximum_voltage_change_in_pu;
}

double PF_SOLVER::get_maximum_angle_change_in_deg() const
{
   return maximum_angle_change_in_deg;
}

double PF_SOLVER::get_maximum_angle_change_in_rad() const
{
   return get_maximum_angle_change_in_deg() * PI_OVER_180;
}

bool PF_SOLVER::get_flat_start_logic() const
{
   return flat_start_enabled;
}

bool PF_SOLVER::get_transformer_tap_adjustment_logic() const
{
   return transformer_tap_adjustment_enabled;
}

double PF_SOLVER::get_iteration_accelerator() const
{
   return alpha;
}

bool PF_SOLVER::get_non_divergent_solution_logic() const
{
   return non_divergent_solution_enabled;
}

bool PF_SOLVER::get_var_limit_check_logic() const
{
   return var_limit_check_enabled;
}

bool PF_SOLVER::get_export_jacobian_matrix_step_by_step_logic() const
{
   return export_jacobian_matrix_step_by_step;
}

void PF_SOLVER::show_powerflow_solver_configuration() const
{
   ostringstream osstream;
   osstream << "  \n"
            << "Configuration of powerflow solver:\n"
            << "Fast sin/cos/tan functions: " << (use_fast_math ? "Enabled" : "Disabled") << "\n"
            << "Maximum iteration: " << get_max_iteration() << "\n"
            << "Iteration accelerator: " << get_iteration_accelerator() << "\n"
            << "Allowed maximum P mismatch: " << get_allowed_max_active_power_imbalance_in_MW() << " MW\n"
            << "Allowed maximum Q mismatch: " << get_allowed_max_reactive_power_imbalance_in_MVar() << "MVar\n"
            << "Maximum voltage change: " << get_maximum_voltage_change_in_pu() << " pu\n"
            << "Maximum angle change: " << get_maximum_angle_change_in_deg() << " deg (" << get_maximum_angle_change_in_rad() << " rad)\n"
            << "Flat start: " << (get_flat_start_logic() ? "Enabled" : "Disabled") << "\n"
            << "Transformer tap adjustment: " << (get_transformer_tap_adjustment_logic() ? "Enabled" : "Disabled") << "\n"
            << "Non-divergent solution: " << (get_non_divergent_solution_logic() ? "Enabled" : "Disabled") << "\n"
            << "Var limit check: " << (get_var_limit_check_logic() ? "Enabled" : "Disabled") << "\n"
            << "Network ordering optimization: " << toolkit->is_optimize_network_enabled() << "\n"
            << "Export jacobian matrix step by step: " << (get_export_jacobian_matrix_step_by_step_logic() ? "Enabled" : "Disabled");
   toolkit->show_information(osstream);
}

void PF_SOLVER::solve_with_full_Newton_Raphson_solution()
{
   ostringstream stream;
   nan_is_detected = false;
   PF_DATA& psdb = toolkit->get_database();

   if ( psdb.get_bus_count() == 0) {
      stream << "Total Bus Number is 0. Please check.\n" << endl;
      toolkit->show_information(stream);

      return;
   }

   stream << "Start solve powerflow with Full Newton Raphson solution." << endl;
   toolkit->show_information(stream);

   initialize_powerflow_solver();

   NET_MATRIX& network_matrix = get_network_matrix();

   double max_P_mismatch_in_MW, max_Q_mismatch_in_MVar;
   vector<double> bus_delta_voltage_angle;

   network_matrix.build_network_Y_matrix();

   update_P_and_Q_equation_internal_buses();
   jacobian_builder->build_seprate_jacobians();

   while (true) {
      char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
      snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Iteration %zu:", iteration_count);
      toolkit->show_information(buffer);

      try_to_solve_dc_system_steady_state();
      calculate_raw_bus_power_mismatch();

      max_P_mismatch_in_MW = get_maximum_active_power_mismatch_in_MW();
      max_Q_mismatch_in_MVar = get_maximum_reactive_power_mismatch_in_MVar();

      bool bus_type_changed = false;
      if (get_var_limit_check_logic() == true) {
         bus_type_changed = check_bus_type_constraints();
      } else {
         allocate_bus_power_mismatch_without_checking_bus_type_constraints();
      }

      if (bus_type_changed) {
         update_P_and_Q_equation_internal_buses();

         try_to_solve_dc_system_steady_state();
         calculate_raw_bus_power_mismatch();

         max_P_mismatch_in_MW = get_maximum_active_power_mismatch_in_MW();
         max_Q_mismatch_in_MVar = get_maximum_reactive_power_mismatch_in_MVar();
      }
      if (is_nan(max_P_mismatch_in_MW) or is_nan(max_Q_mismatch_in_MVar)) {
         snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "NAN is detected when checking powerflow convergence. No further powerflow solution will be attempted.");
         toolkit->show_information(buffer);
         set_convergence_flag(false);
         nan_is_detected = true;
         break;
      } else {
         if (max_P_mismatch_in_MW < get_allowed_max_active_power_imbalance_in_MW() and
               max_Q_mismatch_in_MVar < get_allowed_max_reactive_power_imbalance_in_MVar()) {
            set_convergence_flag(true);

            snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Powerflow converged within %zu iterations.", iteration_count);
            toolkit->show_information(buffer);
            break;
         } else {
            set_convergence_flag(false);
         }

         if (get_iteration_count() >= get_max_iteration()) {
            snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Powerflow failed to converge within %zu iterations.", get_max_iteration());
            toolkit->show_information(buffer);
            break;
         }

         build_bus_power_mismatch_vector_for_coupled_solution();

         jacobian_builder->update_seprate_jacobians();

         jacobian = jacobian_builder->get_full_coupled_jacobian_with_P_and_Q_equation_internal_buses(internal_P_equation_buses,
                    internal_Q_equation_buses);

         if (get_export_jacobian_matrix_step_by_step_logic() == true) {
            jacobian_builder->save_jacobian_matrix("Jacobian-NR-Iter-" + num2str(get_iteration_count()) + ".csv");
         }
         bus_delta_voltage_angle = S_mismatch / jacobian;
         if (jacobian.is_lu_factorization_successful()) {
            update_bus_voltage_and_angle(bus_delta_voltage_angle);
            ++iteration_count;
         } else {
            snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "No further powerflow solution will be attempted since LU factorization of N-R Jacobian matrix is failed.");
            toolkit->show_information(buffer);
            break;
         }
      }
   }
   //show_powerflow_result();
}

void PF_SOLVER::solve_with_fast_decoupled_solution()
{
   nan_is_detected = false;
   PF_DATA& psdb = toolkit->get_database();

   if (psdb.get_bus_count() != 0) {
      char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
      snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Start solve powerflow with Fast Decoupled solution.");
      toolkit->show_information(buffer);

      initialize_powerflow_solver();

      NET_MATRIX& network_matrix = get_network_matrix();

      double max_P_mismatch_in_MW, max_Q_mismatch_in_MVar;
      vector<double> bus_delta_voltage, bus_delta_angle;

      network_matrix.build_network_Y_matrix();

      network_matrix.build_decoupled_network_B_matrix();
      //const SPARSE_MATRIX& Y = network_matrix.get_network_Y_matrix();
      //cout<<"Y matrix identity is: "<<get_sparse_matrix_identity(Y)<<endl;

      update_P_and_Q_equation_internal_buses();
      BP = jacobian_builder->get_decoupled_B_jacobian_with_P_equation_internal_buses(internal_P_equation_buses);
      BQ = jacobian_builder->get_decoupled_B_jacobian_with_Q_equation_internal_buses(internal_Q_equation_buses);
      //BP.LU_factorization(1, 1e-13);
      //BQ.LU_factorization(1, 1e-13);
      //BP.report_brief();
      //BQ.report_brief();

      //size_t physical_bus;

      while (true) {
         char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
         snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Iteration %zu:", iteration_count);
         toolkit->show_information(buffer);

         try_to_solve_dc_system_steady_state();
         calculate_raw_bus_power_mismatch();

         max_P_mismatch_in_MW = get_maximum_active_power_mismatch_in_MW();
         max_Q_mismatch_in_MVar = get_maximum_reactive_power_mismatch_in_MVar();

         bool bus_type_changed = false;
         if (get_var_limit_check_logic() == true) {
            bus_type_changed = check_bus_type_constraints();
         } else {
            allocate_bus_power_mismatch_without_checking_bus_type_constraints();
         }

         if (bus_type_changed) {
            update_P_and_Q_equation_internal_buses();
            //BP = jacobian_builder->get_decoupled_B_jacobian_with_P_equation_internal_buses(internal_P_equation_buses);
            BQ = jacobian_builder->get_decoupled_B_jacobian_with_Q_equation_internal_buses(internal_Q_equation_buses);
            BQ.LU_factorization(1, 1e-13);

            try_to_solve_dc_system_steady_state();
            calculate_raw_bus_power_mismatch();

            max_P_mismatch_in_MW = get_maximum_active_power_mismatch_in_MW();
            max_Q_mismatch_in_MVar = get_maximum_reactive_power_mismatch_in_MVar();
            //continue;
         }

         /*if(get_export_jacobian_matrix_step_by_step_logic()==true)
         {
             BP.save_matrix_to_file("Jacobian-BP-PQ-Iter-"+num2str(get_iteration_count())+".csv");
             BQ.save_matrix_to_file("Jacobian-BQ-PQ-Iter-"+num2str(get_iteration_count())+".csv");
         }*/

         if (is_nan(max_P_mismatch_in_MW) or is_nan(max_Q_mismatch_in_MVar)) {
            snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "NAN is detected when checking powerflow convergence. No further powerflow solution will be attempted.");
            toolkit->show_information(buffer);
            set_convergence_flag(false);
            nan_is_detected = true;
            break;
         } else {
            if (max_P_mismatch_in_MW < get_allowed_max_active_power_imbalance_in_MW() and
                  max_Q_mismatch_in_MVar < get_allowed_max_reactive_power_imbalance_in_MVar()) {
               set_convergence_flag(true);

               snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Powerflow converged within %zu iterations.", iteration_count);
               toolkit->show_information(buffer);
               break;
            } else {
               set_convergence_flag(false);
            }

            if (get_iteration_count() >= get_max_iteration()) {
               snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Powerflow failed to converge within %zu iterations.", get_max_iteration());
               toolkit->show_information(buffer);
               break;
            }

            build_bus_P_power_mismatch_vector_for_decoupled_solution();
            size_t n = internal_P_equation_buses.size();

#ifdef ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
            set_openmp_number_of_threads(toolkit->get_thread_number());
            #pragma omp parallel for schedule(static)
#endif // ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
            for (size_t i = 0; i < n; ++i) {
               size_t internal_bus = internal_P_equation_buses[i];
               //physical_bus = network_matrix.get_physical_bus_number_of_internal_bus(internal_P_equation_buses[i]);
               //P_mismatch[i] /= psdb.get_bus_positive_sequence_voltage_in_pu(physical_bus);
               P_mismatch[i] /= get_bus_positive_sequence_voltage_in_pu_with_internal_bus_number(internal_bus);
            }
            if (BP.is_lu_factorization_successful()) {
               bus_delta_angle = P_mismatch / BP;
            } else {
               snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "No further powerflow solution will be attempted since LU factorization of P-Q BP matrix is failed.");
               toolkit->show_information(buffer);
               break;
            }
            //BP.report_brief();
            //for(size_t i=0; i<P_mismatch.size(); i++)
            //    cout<<i<<","<<P_mismatch[i]<<endl;
            //for(size_t i=0; i<internal_P_equation_buses.size(); ++i)
            //{
            //    cout<<bus_delta_angle[i]<<endl;
            //    //bus_delta_angle[i] /= abs(psdb.get_bus_positive_sequence_complex_voltage_in_pu(network_matrix.get_physical_bus_number_of_internal_bus(internal_P_equation_buses[i])));
            //}
            update_bus_angle(bus_delta_angle);

            try_to_solve_dc_system_steady_state();
            calculate_raw_bus_power_mismatch();

            max_P_mismatch_in_MW = get_maximum_active_power_mismatch_in_MW();
            max_Q_mismatch_in_MVar = get_maximum_reactive_power_mismatch_in_MVar();

            build_bus_Q_power_mismatch_vector_for_decoupled_solution();
            n = internal_Q_equation_buses.size();
#ifdef ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
            set_openmp_number_of_threads(toolkit->get_thread_number());
            #pragma omp parallel for schedule(static)
#endif // ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
            for (size_t i = 0; i < n; ++i) {
               size_t internal_bus = internal_Q_equation_buses[i];
               //physical_bus = network_matrix.get_physical_bus_number_of_internal_bus(internal_Q_equation_buses[i]);
               //Q_mismatch[i] /= psdb.get_bus_positive_sequence_voltage_in_pu(physical_bus);
               Q_mismatch[i] /= get_bus_positive_sequence_voltage_in_pu_with_internal_bus_number(internal_bus);
            }
            if (BQ.is_lu_factorization_successful()) {
               bus_delta_voltage = Q_mismatch / BQ;
            } else {
               snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "No further powerflow solution will be attempted since LU factorization of P-Q BQ matrix is failed.");
               toolkit->show_information(buffer);
               break;
            }

            update_bus_voltage(bus_delta_voltage);

            iteration_count ++;
         }
      }
      //show_powerflow_result();
   }
}

void PF_SOLVER::solve_with_modified_Gaussian_Seidel_solution()
{
   return;
}


void PF_SOLVER::initialize_powerflow_solver()
{
   toolkit->show_information(std::format("Initializing powerflow solver."));

   show_powerflow_solver_configuration();
   NET_MATRIX& network_matrix = get_network_matrix();
   jacobian_builder->set_network_matrix(network_matrix);

   PF_DATA& psdb = toolkit->get_database();
   psdb.update_in_service_bus_count();

   prepare_devices_for_solution();

   initialize_bus_type();
   initialize_bus_voltage_to_regulate();
   initialize_bus_voltage();
   optimize_bus_numbers();
   iteration_count = 0;
   set_convergence_flag(false);

   size_t n_bus = psdb.get_bus_count();
   S_mismatch.reserve(n_bus * 2);
   P_mismatch.reserve(n_bus);
   Q_mismatch.reserve(n_bus);

   toolkit->show_information(std::format("Done initializing powerflow solver."));
}

void PF_SOLVER::prepare_devices_for_solution()
{
   PF_DATA& psdb = toolkit->get_database();

   buses = psdb.get_all_buses();
   sources = psdb.get_all_sources();
   generators = psdb.get_all_generators();
   wt_generators = psdb.get_all_wt_generators();
   pv_units = psdb.get_all_pv_units();
   e_storages = psdb.get_all_energy_storages();

   loads = psdb.get_all_loads();

   lines = psdb.get_all_ac_lines();
   transformers = psdb.get_all_transformers();
   hvdcs = psdb.get_all_2t_lcc_hvdcs();
   vsc_hvdcs = psdb.get_all_vsc_hvdcs();

   e_devices = psdb.get_all_equivalent_devices();
}

void PF_SOLVER::initialize_bus_type()
{
   initialize_PQ_bus_type();
   initialize_PV_bus_type();
   initialize_SLACK_bus_type();
}

void PF_SOLVER::initialize_PQ_bus_type()
{
   PF_DATA& psdb = toolkit->get_database();

   size_t nbus = buses.size();

#ifdef ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
   set_openmp_number_of_threads(toolkit->get_thread_number());
   #pragma omp parallel for schedule(static)
#endif // ENABLE_OPENMP_FOR_POWERFLOW_SOLVER

   for (size_t i = 0; i < nbus; ++i) {
      size_t bus = buses[i]->get_bus_number();
      BUS_TYPE btype = buses[i]->get_bus_type();
      if (btype == PQ_TYPE) {
         vector<VSC_HVDC*> vscs = psdb.get_vsc_hvdcs_connecting_to_bus(bus);
         VSC_HVDC* vsc = NULL;
         size_t nvsc = vscs.size();
         for (size_t j = 0; j != nvsc; ++j) {
            //cout<<__FUNCTION__<<": "<<bus<<endl;
            vsc = vscs[j];
            if (vsc->get_status() == true) {
               size_t index = vsc->get_converter_index_with_ac_bus(bus);
               //cout<<"converter index: "<<index<<endl;
               //cout<<"converter status: "<<vsc->get_converter_status(index)<<endl;
               if (vsc->get_converter_status(index) == true) {
                  //cout<<"converter Q mode: "<<vsc->get_converter_reactive_power_operation_mode(index)<<endl;
                  //cout<<"converter P mode: "<<vsc->get_converter_active_power_operation_mode(index)<<endl;
                  if (vsc->get_converter_reactive_power_operation_mode(index) == VSC_AC_VOLTAGE_CONTROL) {
                     if (vsc->get_converter_active_power_operation_mode(index) == VSC_AC_VOLTAGE_ANGLE_CONTROL) {
                        buses[i]->set_bus_type(SLACK_TYPE);
                     } else {
                        if (buses[i]->get_bus_type() != SLACK_TYPE) {
                           //cout<<"bus type is not slack, change to PV"<<endl;
                           buses[i]->set_bus_type(PV_TYPE);
                           //cout<<buses[i]->get_bus_type()<<endl;
                        }
                     }
                  }
               }
            }
         }
      }
   }
}

void PF_SOLVER::initialize_PV_bus_type()
{
   PF_DATA& psdb = toolkit->get_database();

   //vector<BUS*> buses = psdb.get_all_buses();
   size_t nbus = buses.size();

   if (get_flat_start_logic() == true) {
#ifdef ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
      set_openmp_number_of_threads(toolkit->get_thread_number());
      #pragma omp parallel for schedule(static)
#endif // ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
      for (size_t i = 0; i < nbus; ++i) {
         BUS_TYPE btype = buses[i]->get_bus_type();
         if (btype == PV_TO_PQ_TYPE_1 or btype == PV_TO_PQ_TYPE_2 or
               btype == PV_TO_PQ_TYPE_3 or btype == PV_TO_PQ_TYPE_4) {
            buses[i]->set_bus_type(PV_TYPE);
         }
      }
   }

#ifdef ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
   set_openmp_number_of_threads(toolkit->get_thread_number());
   #pragma omp parallel for schedule(static)
#endif // ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
   for (size_t i = 0; i < nbus; ++i) {
      size_t bus = buses[i]->get_bus_number();
      BUS_TYPE btype = buses[i]->get_bus_type();
      if (btype == PV_TYPE) {
         vector<SOURCE*> sources = psdb.get_sources_connecting_to_bus(bus);
         size_t nsource = sources.size();
         size_t n_inservice = 0;
         for (size_t j = 0; j != nsource; ++j) {
            if (sources[j]->get_status() == true) {
               ++n_inservice;
            }
         }
         vector<VSC_HVDC*> vscs = psdb.get_vsc_hvdcs_connecting_to_bus(bus);
         VSC_HVDC* vsc = NULL;
         size_t nvsc = vscs.size();
         for (size_t j = 0; j != nvsc; ++j) {
            vsc = vscs[j];
            if (vsc->get_status() == true) {
               size_t index = vsc->get_converter_index_with_ac_bus(bus);
               if (vsc->get_converter_status(index) == true) {
                  if (vsc->get_converter_reactive_power_operation_mode(index) == VSC_AC_VOLTAGE_CONTROL) {
                     if (vsc->get_converter_active_power_operation_mode(index) != VSC_AC_VOLTAGE_ANGLE_CONTROL) {
                        ++n_inservice;
                     } else {
                        buses[i]->set_bus_type(SLACK_TYPE);
                        break;
                     }
                  }
               }
            }
         }
         if (buses[i]->get_bus_type() == SLACK_TYPE) {
            continue;
         } else {
            if (n_inservice != 0)
               ;
            else {
               buses[i]->set_bus_type(PQ_TYPE);
            }
         }
      }
   }
}

void PF_SOLVER::initialize_SLACK_bus_type()
{
   ostringstream osstream;
   PF_DATA& psdb = toolkit->get_database();

   size_t nbus = buses.size();

#ifdef ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
   set_openmp_number_of_threads(toolkit->get_thread_number());
   #pragma omp parallel for schedule(static)
#endif // ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
   for (size_t i = 0; i < nbus; ++i) {
      size_t bus = buses[i]->get_bus_number();
      BUS_TYPE btype = buses[i]->get_bus_type();
      if (btype == SLACK_TYPE) {
         vector<SOURCE*> sources = psdb.get_sources_connecting_to_bus(bus);
         size_t nsource = sources.size();

         size_t n_inservice_source = 0;
         for (size_t j = 0; j != nsource; ++j) {
            if (sources[j]->get_status() == true) {
               ++n_inservice_source;
            }
         }

         vector<VSC_HVDC*> vscs = psdb.get_vsc_hvdcs_connecting_to_bus(bus);
         size_t nvsc = vscs.size();

         size_t n_inservice_vsc = 0;
         VSC_HVDC* vsc = NULL;
         for (size_t j = 0; j != nvsc; ++j) {
            vsc = vscs[j];
            if (vsc->get_status() == true) {
               size_t index = vsc->get_converter_index_with_ac_bus(bus);
               if (vsc->get_converter_status(index) == true) {
                  ++n_inservice_vsc;
               }
            }
         }
         if (n_inservice_source == 0 and n_inservice_vsc == 0) {
            osstream << "Warning. Bus " << bus << " is a SLACK bus but has neither in-service source nor vsc-hvdc." << endl
                     << "Bus type is changed to PQ type";
            toolkit->show_information(osstream);
            buses[i]->set_bus_type(PQ_TYPE);
            continue;
         } else {
            if (n_inservice_source == 0 and n_inservice_vsc != 0) {
               size_t n_inservice_converter_with_SLACK_type = 0;
               for (size_t j = 0; j != nvsc; ++j) {
                  vsc = vscs[j];
                  if (vsc->get_status() == true) {
                     size_t index = vsc->get_converter_index_with_ac_bus(bus);
                     if (vsc->get_converter_status(index) == true) {
                        if (vsc->get_converter_reactive_power_operation_mode(index) == VSC_AC_VOLTAGE_CONTROL and
                              vsc->get_converter_active_power_operation_mode(index) == VSC_AC_VOLTAGE_ANGLE_CONTROL) {
                           ++n_inservice_converter_with_SLACK_type;
                        }
                     }
                  }
               }
               if (n_inservice_converter_with_SLACK_type == 0) {
                  osstream << "Error. Bus " << bus << " is a SLACK bus with in-service vsc-hvdc. However, "
                           << "no vsc-hvdc has converter with both VSC_AC_VOLTAGE_CONTROL and VSC_AC_VOLTAGE_ANGLE_CONTROL modes." << endl
                           << "Bus type is changed to PQ type";
                  toolkit->show_information(osstream);
                  buses[i]->set_bus_type(PQ_TYPE);
                  continue;
               }
            }
         }
      }
   }
}

void PF_SOLVER::initialize_bus_voltage_to_regulate()
{
   //vector<SOURCE*> sources = psdb.get_all_sources();
   size_t nsource = sources.size();
#ifdef ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
   set_openmp_number_of_threads(toolkit->get_thread_number());
   #pragma omp parallel for schedule(static)
#endif // ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
   for (size_t i = 0; i < nsource; ++i) {
      if (sources[i]->get_status() == true) {
         double qmax = sources[i]->get_q_max_in_MVar();
         double qmin = sources[i]->get_q_min_in_MVar();
         if (qmax != qmin) {
            double vreg = sources[i]->get_voltage_to_regulate_in_pu();
            BUS* busptr = sources[i]->get_bus_pointer();
            busptr->set_voltage_to_regulate_in_pu(vreg);
         }
      }
   }

   size_t nvsc = vsc_hvdcs.size();
#ifdef ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
   set_openmp_number_of_threads(toolkit->get_thread_number());
   #pragma omp parallel for schedule(static)
#endif // ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
   for (size_t i = 0; i < nvsc; ++i) {
      if (vsc_hvdcs[i]->get_status() == true) {
         size_t n_converter  = vsc_hvdcs[i]->get_converter_count();
         for (size_t index = 0; index != n_converter; ++index) {
            if (vsc_hvdcs[i]->get_converter_status(index) == true) {
               if (vsc_hvdcs[i]->get_converter_reactive_power_operation_mode(index) == VSC_AC_VOLTAGE_CONTROL) {
                  double qmax = vsc_hvdcs[i]->get_converter_Qmax_in_MVar(index);
                  double qmin = vsc_hvdcs[i]->get_converter_Qmin_in_MVar(index);
                  if (qmax != qmin) {
                     double vreg = vsc_hvdcs[i]->get_converter_nominal_ac_voltage_command_in_pu(index);
                     BUS* busptr = vsc_hvdcs[i]->get_converter_ac_bus_pointer(index);
                     busptr->set_voltage_to_regulate_in_pu(vreg);
                  }
               }
            }
         }
      }
   }

   size_t nbus = buses.size();
#ifdef ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
   set_openmp_number_of_threads(toolkit->get_thread_number());
   #pragma omp parallel for schedule(static)
#endif // ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
   for (size_t i = 0; i < nbus; ++i) {
      BUS_TYPE btype = buses[i]->get_bus_type();
      if (btype == PQ_TYPE or btype == OUT_OF_SERVICE) {
         continue;
      } else { // PV, SLACK
         if (fabs(buses[i]->get_voltage_to_regulate_in_pu()) < DOUBLE_EPSILON) {
            buses[i]->set_voltage_to_regulate_in_pu(1.0);
         }
      }
   }
}

void PF_SOLVER::initialize_bus_voltage()
{
   //vector<BUS*> buses = psdb.get_all_buses();
   size_t nbus = buses.size();

   if (get_flat_start_logic() == true) {
      for (size_t i = 0; i < nbus; ++i) {
         switch (buses[i]->get_bus_type()) {
         case PQ_TYPE:
            buses[i]->set_positive_sequence_voltage_in_pu(1.0);
            break;
         case OUT_OF_SERVICE:
            continue;
         default:
            buses[i]->set_positive_sequence_voltage_in_pu(buses[i]->get_voltage_to_regulate_in_pu());
         }
         buses[i]->set_positive_sequence_angle_in_rad(0.0);
      }
   } else {
      for (size_t i = 0; i < nbus; ++i) {
         switch (buses[i]->get_bus_type()) {
         case PQ_TYPE:
         case OUT_OF_SERVICE:
            break;
         default:
            buses[i]->set_positive_sequence_voltage_in_pu(buses[i]->get_voltage_to_regulate_in_pu());
         }
      }
   }

   toolkit->show_information(std::format("Initial bus voltage and angle are listed as follows."));
   toolkit->show_information(std::format("bus      voltage(pu) angle(deg)"));
   nbus = nbus > 200 ? 200 : nbus;
   for (size_t i = 0; i != nbus; ++i) {
      std::string buffer = std::format("{:8} {:10.6f} {:10.6f}",
                                       buses[i]->get_bus_number(), buses[i]->get_positive_sequence_voltage_in_pu(), buses[i]->get_positive_sequence_angle_in_deg());
      toolkit->show_information(buffer);
   }
}

void PF_SOLVER::optimize_bus_numbers()
{
   NET_MATRIX& network_matrix = get_network_matrix();

   network_matrix.optimize_network_ordering();

   set_internal_bus_pointer();

}

void PF_SOLVER::set_internal_bus_pointer()
{
   PF_DATA& psdb = toolkit->get_database();
   size_t nbus = psdb.get_in_service_bus_count();

   NET_MATRIX& network_matrix = get_network_matrix();

   internal_bus_pointers.clear();
   for (size_t internal_bus = 0; internal_bus != nbus; ++internal_bus) {
      size_t physical_bus = network_matrix.get_physical_bus_number_of_internal_bus(internal_bus);
      internal_bus_pointers.push_back(psdb.get_bus(physical_bus));
   }
}

complex<double> PF_SOLVER::get_bus_voltage_in_pu(size_t internal_bus) const
{
   BUS* busptr = internal_bus_pointers[internal_bus];
   return busptr->get_positive_sequence_complex_voltage_in_pu();
}

double PF_SOLVER::get_bus_positive_sequence_voltage_in_pu_with_internal_bus_number(size_t internal_bus) const
{
   BUS* busptr = internal_bus_pointers[internal_bus];
   return busptr->get_positive_sequence_voltage_in_pu();
}

void PF_SOLVER::update_P_and_Q_equation_internal_buses()
{
   PF_DATA& psdb = toolkit->get_database();

   char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
   snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Updating powerflow P equation buses and Q equation buses.");
   toolkit->show_information(buffer);

   size_t nbus = psdb.get_in_service_bus_count();

   internal_P_equation_buses.clear();
   internal_Q_equation_buses.clear();
   internal_P_equation_buses.reserve(nbus);
   internal_Q_equation_buses.reserve(nbus);

   for (size_t i = 0; i != nbus; ++i) {
      //size_t bus = network_matrix.get_physical_bus_number_of_internal_bus(i);
      //BUS* busptr = psdb.get_bus(bus);
      BUS* busptr = internal_bus_pointers[i];
      BUS_TYPE btype = busptr->get_bus_type();
      if (btype != OUT_OF_SERVICE and btype != SLACK_TYPE) {
         internal_P_equation_buses.push_back(i);
         if (btype != PV_TYPE) {
            internal_Q_equation_buses.push_back(i);
         }
      }
   }
   /*
   osstream<<"Buses with P equations (physical bus):" << endl;
   toolkit->show_information(osstream);
   size_t n = internal_P_equation_buses.size();
   for(size_t i=0; i!=n; ++i)
   {
       osstream<<network_matrix.get_physical_bus_number_of_internal_bus(internal_P_equation_buses[i]) << endl;
       toolkit->show_information(osstream);
   }
   osstream<<"Buses with Q equations (physical bus):"<<endl;
   toolkit->show_information(osstream);
   n = internal_Q_equation_buses.size();
   for(size_t i=0; i!=n; ++i)
   {
       osstream<< network_matrix.get_physical_bus_number_of_internal_bus(internal_Q_equation_buses[i]) << endl;
       toolkit->show_information(osstream);
   }*/

   snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Done updating powerflow P equation buses and Q equation buses.");
   toolkit->show_information(buffer);
}

void PF_SOLVER::set_convergence_flag(bool flag)
{
   converged = flag;
}

bool PF_SOLVER::get_convergence_flag() const
{
   return converged;
}

bool PF_SOLVER::is_converged()
{
   ostringstream osstream;
   osstream << "Check maximum active and reactive power mismatch.";
   toolkit->show_information(osstream);

   try_to_solve_dc_system_steady_state();
   calculate_raw_bus_power_mismatch();

   double max_P_mismatch_in_MW = get_maximum_active_power_mismatch_in_MW();
   double max_Q_mismatch_in_MVar = get_maximum_reactive_power_mismatch_in_MVar();

   if (is_nan(max_P_mismatch_in_MW) or is_nan(max_Q_mismatch_in_MVar)) {
      osstream << "Powerflow solution failed to converge since NAN is detected.";
      toolkit->show_information(osstream);
      return false;
   } else {
      if (max_P_mismatch_in_MW < get_allowed_max_active_power_imbalance_in_MW() and
            max_Q_mismatch_in_MVar < get_allowed_max_reactive_power_imbalance_in_MVar()) {
         return true;
      } else {
         return false;
      }
   }

   return get_convergence_flag();
}

bool PF_SOLVER::is_nan_detected()
{
   return nan_is_detected;
}
void PF_SOLVER::try_to_solve_dc_system_steady_state()
{
   try_to_solve_2t_lcc_hvdc_steady_state();
   try_to_solve_vsc_hvdc_steady_state();
}

void PF_SOLVER::try_to_solve_2t_lcc_hvdc_steady_state()
{
   //vector<LCC_HVDC2T*> hvdcs = psdb.get_all_2t_lcc_hvdcs();
   size_t nhvdc = hvdcs.size();
#ifdef ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
   set_openmp_number_of_threads(toolkit->get_thread_number());
   #pragma omp parallel for schedule(static)
#endif // ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
   for (size_t i = 0; i < nhvdc; ++i) {
      if (hvdcs[i]->get_status() == true) {
         hvdcs[i]->solve_steady_state();
         //hvdcs[i]->show_solved_steady_state();
      }
   }
}

void PF_SOLVER::try_to_solve_vsc_hvdc_steady_state()
{
   size_t nvsc = vsc_hvdcs.size();
#ifdef ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
   set_openmp_number_of_threads(toolkit->get_thread_number());
   #pragma omp parallel for schedule(static)
#endif // ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
   for (size_t i = 0; i < nvsc; ++i) {
      if (vsc_hvdcs[i]->get_status() == true) {
         //vsc_hvdcs[i]->report();
         vsc_hvdcs[i]->set_max_iteration(get_max_iteration());
         vsc_hvdcs[i]->set_allowed_max_P_mismatch_in_MW(get_allowed_max_active_power_imbalance_in_MW());
         vsc_hvdcs[i]->solve_steady_state();
         //vsc_hvdcs[i]->show_solved_steady_state();
      }
   }
}

void PF_SOLVER::calculate_raw_bus_power_mismatch()
{
   ostringstream osstream;
   PF_DATA& psdb = toolkit->get_database();
   NET_MATRIX& network_matrix = get_network_matrix();

   size_t nbus = psdb.get_in_service_bus_count();

   calculate_raw_bus_power_into_network();
   if (toolkit->is_detailed_log_enabled()) {
      for (size_t i = 0; i != nbus; ++i) {
         if (std::isnan(bus_power[i].real()) or std::isnan(bus_power[i].imag())) {
            osstream << "after  calculate_raw_bus_power_into_network NAN is detected at bus " << network_matrix.get_physical_bus_number_of_internal_bus(i) << endl;
            toolkit->show_information(osstream);
         }
      }
   }

#ifdef ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
   set_openmp_number_of_threads(toolkit->get_thread_number());
   #pragma omp parallel for schedule(static)
#endif // ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
   for (size_t i = 0; i < nbus; ++i) {
      bus_power[i] = -bus_power[i];
   }

   add_source_to_bus_power_mismatch();
   if (toolkit->is_detailed_log_enabled()) {
      for (size_t i = 0; i != nbus; ++i) {
         if (std::isnan(bus_power[i].real()) or std::isnan(bus_power[i].imag())) {
            osstream << "after adding source NAN is detected at bus " << network_matrix.get_physical_bus_number_of_internal_bus(i) << endl;
            toolkit->show_information(osstream);
         }
      }
   }

   add_load_to_bus_power_mismatch();
   if (toolkit->is_detailed_log_enabled()) {
      for (size_t i = 0; i != nbus; ++i) {
         if (std::isnan(bus_power[i].real()) or std::isnan(bus_power[i].imag())) {
            osstream << "after adding load NAN is detected at bus " << network_matrix.get_physical_bus_number_of_internal_bus(i) << endl;
            toolkit->show_information(osstream);
         }
      }
   }

   add_2t_lcc_hvdc_to_bus_power_mismatch();
   if (toolkit->is_detailed_log_enabled()) {
      for (size_t i = 0; i != nbus; ++i) {
         if (std::isnan(bus_power[i].real()) or std::isnan(bus_power[i].imag())) {
            osstream << "after adding hvdc NAN is detected at bus " << network_matrix.get_physical_bus_number_of_internal_bus(i) << endl;
            toolkit->show_information(osstream);
         }
      }
   }

   add_vsc_hvdc_to_bus_power_mismatch();
   if (toolkit->is_detailed_log_enabled()) {
      for (size_t i = 0; i != nbus; ++i) {
         if (std::isnan(bus_power[i].real()) or std::isnan(bus_power[i].imag())) {
            osstream << "after adding vsc-hvdc NAN is detected at bus " << network_matrix.get_physical_bus_number_of_internal_bus(i) << endl;
            toolkit->show_information(osstream);
         }
      }
   }
   /*
   osstream<<"Power mismatch of buses.";
   toolkit->show_information(osstream);
   osstream<<"bus      name           Pmismatch(MW) Qmismatch(MVar)";
   toolkit->show_information(osstream);
   */
   /*
       double sbase = psdb.get_system_base_power_in_MVA();
       for(size_t i=0; i!=nbus; ++i)
       {
           size_t bus = network_matrix.get_physical_bus_number_of_internal_bus(i);
           string bus_name = psdb.bus_number2bus_name(bus);
           BUS_TYPE btype = psdb.get_bus(bus)->get_bus_type();
           double p = bus_power[i].real()*sbase;
           double q = bus_power[i].imag()*sbase;
           if(btype==PV_TYPE or btype == SLACK_TYPE)
               q = 0.0;
           if(btype==SLACK_TYPE)
               p = 0.0;
           osstream<<left;
           osstream<<setw(7)<<setfill(' ')<<bus<<"  "<<setw(14)<<bus_name<<" ";
           osstream<<right;
           osstream<<setw(12)<<setprecision(6)<<p<<"   "<<setw(13)<<setprecision(6)<<q;
           switch(btype)
           {
               case PV_TYPE:
                   osstream<<"  <--PV";
                   break;
               case SLACK_TYPE:
                   osstream<<"  <--SLACK";
                   break;
               default:
                   break;
           }
           toolkit->show_information(osstream);
       }
   */
}

void PF_SOLVER::calculate_raw_bus_power_into_network()
{
   ostringstream osstream;
   PF_DATA& psdb = toolkit->get_database();
   NET_MATRIX& network_matrix = get_network_matrix();

   calculate_raw_bus_current_into_network();
   bus_power = bus_current;

   size_t nbus = psdb.get_in_service_bus_count();
   if (toolkit->is_detailed_log_enabled()) {
      for (size_t i = 0; i != nbus; ++i) {
         if (std::isnan(bus_power[i].real()) or std::isnan(bus_power[i].imag())) {
            osstream << "after  calculate_raw_bus_current_into_network NAN is detected at bus " << network_matrix.get_physical_bus_number_of_internal_bus(i) << endl;
            toolkit->show_information(osstream);
         }
      }
   }

   //complex<double> voltage;
   //size_t physical_bus_number;

#ifdef ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
   set_openmp_number_of_threads(toolkit->get_thread_number());
   #pragma omp parallel for schedule(static)
#endif // ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
   for (size_t i = 0; i < nbus; ++i) {
      //physical_bus_number = network_matrix.get_physical_bus_number_of_internal_bus(i);
      //voltage = psdb.get_bus_positive_sequence_complex_voltage_in_pu(physical_bus_number);
      complex<double> voltage = get_bus_voltage_in_pu(i);

      bus_power[i] = voltage * conj(bus_current[i]);
   }

   /*ostringstream osstream;
   osstream<<"Power flowing into network (physical bus).");
   toolkit->show_information(osstream);
   osstream<<"bus     P(pu)    P(pu)");
   toolkit->show_information(osstream);

   size_t bus;
   for(size_t i=0; i!=nbus; ++i)
   {
       bus = get_physical_bus_number_of_internal_bus(i);
       osstream<<"%-8u %-10f %-10f",bus, bus_power[i].real(), bus_power[i].imag());
       toolkit->show_information(osstream);
   }*/
}


void PF_SOLVER::calculate_raw_bus_current_into_network()
{
   PF_DATA& psdb = toolkit->get_database();
   NET_MATRIX& network_matrix = get_network_matrix();

   const STEPS_COMPLEX_SPARSE_MATRIX& Y = network_matrix.get_network_Y_matrix();

   size_t nbus = psdb.get_in_service_bus_count();
   if (bus_current.size() == 0) {
      bus_current.clear();
      bus_current.reserve(nbus);
      for (size_t i = 0; i != nbus; ++i) {
         bus_current.push_back(0.0);
      }
   } else {
#ifdef ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
      set_openmp_number_of_threads(toolkit->get_thread_number());
      #pragma omp parallel for schedule(static)
#endif // ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
      for (size_t i = 0; i < nbus; ++i) {
         bus_current[i] = 0.0;
      }
   }

   complex<double> voltage, y;
   //size_t physical_bus_number;

   //size_t column_physical_bus;

   int nsize = Y.get_matrix_size();
   int k_start = 0;
   for (int column = 0; column != nsize; ++column) {
      //column_physical_bus = network_matrix.get_physical_bus_number_of_internal_bus(column);
      //voltage = psdb.get_bus_positive_sequence_complex_voltage_in_pu(column_physical_bus);
      complex<double> voltage = get_bus_voltage_in_pu(column);

      int k_end = Y.get_starting_index_of_column(column + 1);
      for (int k = k_start; k < k_end; ++k) {
         int row = Y.get_row_number_of_entry_index(k);
         complex<double> y = Y.get_entry_value(k);
         bus_current[row] += y * voltage;
      }
      k_start = k_end;
   }
}

void PF_SOLVER::add_source_to_bus_power_mismatch()
{
   NET_MATRIX& network_matrix = get_network_matrix();

   double one_over_sbase = toolkit->get_one_over_system_base_power_in_one_over_MVA();

   size_t nsource = sources.size();

#ifdef ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
   set_openmp_number_of_threads(toolkit->get_generator_thread_number());
   #pragma omp parallel for schedule(static)
#endif // ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
   for (size_t i = 0; i < nsource; ++i) {
      if (sources[i]->get_status() == true) {
         size_t physical_bus = sources[i]->get_source_bus();

         BUS* busptr = sources[i]->get_bus_pointer();

         complex<double> Sgen;
         switch (busptr->get_bus_type()) {
         case PV_TYPE:
            Sgen = complex<double>(sources[i]->get_p_generation_in_MW(), 0.0);
            break;
         case OUT_OF_SERVICE:
         case SLACK_TYPE:
            continue;
         default:// PQ, PV2PQ
            Sgen = complex<double>(sources[i]->get_p_generation_in_MW(), sources[i]->get_q_generation_in_MVar());
            break;
         }
         size_t internal_bus = network_matrix.get_internal_bus_number_of_physical_bus(physical_bus);
         bus_power[internal_bus] += (Sgen * one_over_sbase);
      }
   }
}

void PF_SOLVER::add_load_to_bus_power_mismatch()
{
   NET_MATRIX& network_matrix = get_network_matrix();

   double one_over_sbase = toolkit->get_one_over_system_base_power_in_one_over_MVA();

   size_t nload = loads.size();

#ifdef ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
   set_openmp_number_of_threads(toolkit->get_load_thread_number());
   #pragma omp parallel for schedule(static)
#endif // ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
   for (size_t i = 0; i < nload; ++i) {
      if (loads[i]->get_status() == true) {
         size_t physical_bus = loads[i]->get_load_bus();
         size_t internal_bus = network_matrix.get_internal_bus_number_of_physical_bus(physical_bus);

         complex<double> Sload = loads[i]->get_actual_total_load_in_MVA() * one_over_sbase;

         bus_power[internal_bus] -= Sload;
      }
   }
}

void PF_SOLVER::add_2t_lcc_hvdc_to_bus_power_mismatch()
{
   NET_MATRIX& network_matrix = get_network_matrix();

   double one_over_sbase = toolkit->get_one_over_system_base_power_in_one_over_MVA();

   size_t nhvdc = hvdcs.size();

#ifdef ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
   set_openmp_number_of_threads(toolkit->get_lcc_hvdc2t_thread_number());
   #pragma omp parallel for schedule(static)
#endif // ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
   for (size_t i = 0; i < nhvdc; ++i) {
      if (hvdcs[i]->get_status() == true) {
         size_t physical_bus_rec = hvdcs[i]->get_converter_bus(RECTIFIER);
         size_t internal_bus_rec = network_matrix.get_internal_bus_number_of_physical_bus(physical_bus_rec);

         size_t physical_bus_inv = hvdcs[i]->get_converter_bus(INVERTER);
         size_t internal_bus_inv = network_matrix.get_internal_bus_number_of_physical_bus(physical_bus_inv);

         //hvdcs[i]->solve_steady_state();
         //hvdcs[i]->show_solved_steady_state();

         complex<double> S_rec = complex<double>(hvdcs[i]->get_converter_ac_active_power_in_MW(RECTIFIER), hvdcs[i]->get_converter_ac_reactive_power_in_MVar(RECTIFIER));
         complex<double> S_inv = complex<double>(hvdcs[i]->get_converter_ac_active_power_in_MW(INVERTER), -hvdcs[i]->get_converter_ac_reactive_power_in_MVar(INVERTER));

         //ostringstream osstream;
         //osstream<<hvdcs[i]->get_compound_device_name()<<": Srec = "<<S_rec<<" MVA, Sinv = "<<S_inv<<" MVA."<<endl;
         //toolkit->show_information(osstream);

         bus_power[internal_bus_rec] -= S_rec * one_over_sbase;
         bus_power[internal_bus_inv] += S_inv * one_over_sbase;
      }
   }
}

void PF_SOLVER::add_vsc_hvdc_to_bus_power_mismatch()
{
   NET_MATRIX& network_matrix = get_network_matrix();

   double one_over_sbase = toolkit->get_one_over_system_base_power_in_one_over_MVA();

   size_t nvsc = vsc_hvdcs.size();

#ifdef ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
   set_openmp_number_of_threads(toolkit->get_vsc_hvdc_thread_number());
   #pragma omp parallel for schedule(static)
#endif // ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
   for (size_t i = 0; i < nvsc; ++i) {
      if (vsc_hvdcs[i]->get_status() == true) {
         size_t n_converter = vsc_hvdcs[i]->get_converter_count();
         for (size_t index = 0; index != n_converter; ++index) {
            if (vsc_hvdcs[i]->get_converter_status(index) == true) {
               size_t physical_bus = vsc_hvdcs[i]->get_converter_ac_bus(index);
               BUS* busptr = vsc_hvdcs[i]->get_converter_ac_bus_pointer(index);
               BUS_TYPE bus_type = busptr->get_bus_type();
               size_t internal_bus = network_matrix.get_internal_bus_number_of_physical_bus(physical_bus);
               if (bus_type == PQ_TYPE or
                     bus_type == PV_TO_PQ_TYPE_4 or
                     bus_type == PV_TO_PQ_TYPE_3 or
                     bus_type == PV_TO_PQ_TYPE_2 or
                     bus_type == PV_TO_PQ_TYPE_1) {
                  complex<double> S = complex<double>(vsc_hvdcs[i]->get_converter_P_to_AC_bus_in_MW(index),
                                                      vsc_hvdcs[i]->get_converter_Q_to_AC_bus_in_MVar(index));
                  bus_power[internal_bus] += S * one_over_sbase;
               } else {
                  if (bus_type == PV_TYPE) {
                     complex<double> S = complex<double>(vsc_hvdcs[i]->get_converter_P_to_AC_bus_in_MW(index), 0.0);
                     bus_power[internal_bus] += S * one_over_sbase;
                     if (vsc_hvdcs[i]->get_converter_reactive_power_operation_mode(index) == VSC_AC_REACTIVE_POWER_CONTROL) {
                        complex<double> S = complex<double>(0.0, vsc_hvdcs[i]->get_converter_Q_to_AC_bus_in_MVar(index));
                        bus_power[internal_bus] += S * one_over_sbase;
                     }
                  } else {
                     if (bus_type == SLACK_TYPE) {
                        if (vsc_hvdcs[i]->get_converter_active_power_operation_mode(index) != VSC_AC_VOLTAGE_ANGLE_CONTROL) {
                           complex<double> S = complex<double>(vsc_hvdcs[i]->get_converter_P_to_AC_bus_in_MW(index), 0.0);
                           bus_power[internal_bus] += S * one_over_sbase;
                        }
                        if (vsc_hvdcs[i]->get_converter_reactive_power_operation_mode(index) == VSC_AC_REACTIVE_POWER_CONTROL) {
                           complex<double> S = complex<double>(0.0, vsc_hvdcs[i]->get_converter_Q_to_AC_bus_in_MVar(index));
                           bus_power[internal_bus] += S * one_over_sbase;
                        }
                     } else {
                        continue;   //out of service
                     }
                  }
               }
            }
         }
         //ostringstream osstream;
         //osstream<<hvdcs[i]->get_compound_device_name()<<": Srec = "<<S_rec<<" MVA, Sinv = "<<S_inv<<" MVA."<<endl;
         //toolkit->show_information(osstream);
      }
   }
}

double PF_SOLVER::get_maximum_active_power_mismatch_in_MW() const
{
   PF_DATA& psdb = toolkit->get_database();
   NET_MATRIX& network_matrix = toolkit->get_network_matrix();

   size_t nP = internal_P_equation_buses.size();
   size_t internal_bus, physical_bus;

   double max_P_error_in_pu = 0.0, max_P_error_in_MW;
   size_t max_P_error_physical_bus = 0;
   double bus_P_error = 0;
   for (size_t i = 0; i != nP; ++i) {
      internal_bus = internal_P_equation_buses[i];
      bus_P_error = fabs(bus_power[internal_bus].real());
      if (not std::isnan(bus_P_error)) {
         if (bus_P_error < max_P_error_in_pu) {
            continue;
         } else {
            physical_bus = network_matrix.get_physical_bus_number_of_internal_bus(internal_bus);
            max_P_error_in_pu = bus_P_error;
            max_P_error_physical_bus = physical_bus;
         }
      } else {
         max_P_error_in_pu = bus_P_error;
         break;
      }
   }

   max_P_error_in_MW = max_P_error_in_pu * psdb.get_system_base_power_in_MVA();

   string maxbusname = psdb.bus_number2bus_name(max_P_error_physical_bus);

   char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
   snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Maximum   active power mismatch found: %10.6f MW   at bus %zu [%s].",
            max_P_error_in_MW, max_P_error_physical_bus, maxbusname.c_str());
   toolkit->show_information(buffer);

   return max_P_error_in_MW;
}

double PF_SOLVER::get_maximum_reactive_power_mismatch_in_MVar() const
{
   PF_DATA& psdb = toolkit->get_database();
   NET_MATRIX& network_matrix = toolkit->get_network_matrix();

   size_t nQ = internal_Q_equation_buses.size();
   size_t internal_bus, physical_bus;

   double max_Q_error_in_pu = 0.0, max_Q_error_in_MVar;
   size_t max_Q_error_physical_bus = 0;
   double bus_Q_error = 0;
   for (size_t i = 0; i != nQ; ++i) {
      internal_bus = internal_Q_equation_buses[i];
      bus_Q_error = fabs(bus_power[internal_bus].imag());
      if (not std::isnan(bus_Q_error)) {
         if (bus_Q_error < max_Q_error_in_pu) {
            continue;
         } else {
            physical_bus = network_matrix.get_physical_bus_number_of_internal_bus(internal_bus);
            max_Q_error_in_pu = bus_Q_error;
            max_Q_error_physical_bus = physical_bus;
         }
      } else {
         max_Q_error_in_pu = bus_Q_error;
         break;
      }
   }
   max_Q_error_in_MVar =  max_Q_error_in_pu * psdb.get_system_base_power_in_MVA();

   string maxbusname = psdb.bus_number2bus_name(max_Q_error_physical_bus);

   char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
   snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Maximum reactive power mismatch found: %10.6f MVar at bus %zu [%s].",
            max_Q_error_in_MVar, max_Q_error_physical_bus, maxbusname.c_str());
   toolkit->show_information(buffer);

   return max_Q_error_in_MVar;
}

bool PF_SOLVER::check_bus_type_constraints()
{
   bool system_bus_type_changed = false;

   size_t nbus = buses.size();

   for (size_t i = 0; i < nbus; ++i) {
      BUS_TYPE btype = buses[i]->get_bus_type();
      if (btype == PQ_TYPE or btype == OUT_OF_SERVICE) {
         continue;
      }

      size_t physical_bus = buses[i]->get_bus_number();

      if (btype == SLACK_TYPE) {
         check_SLACK_bus_constraint_of_physical_bus(physical_bus);
         //system_bus_type_changed = false;
      } else {
         if (btype == PV_TYPE) {
            bool bus_type_changed = check_PV_bus_constraint_of_physical_bus(physical_bus);
            if (bus_type_changed) {
               system_bus_type_changed = true;
            }
         } else {
            bool bus_type_changed = check_PV_TO_PQ_bus_constraint_of_physical_bus(physical_bus);
            if (bus_type_changed) {
               system_bus_type_changed = true;
            }
         }
      }
   }
   return system_bus_type_changed;
}


void PF_SOLVER::check_SLACK_bus_constraint_of_physical_bus(size_t physical_bus)
{
   PF_DATA& psdb = toolkit->get_database();
   NET_MATRIX& network_matrix = get_network_matrix();

   //BUS* bus = psdb.get_bus(physical_bus);
   size_t internal_bus = network_matrix.get_internal_bus_number_of_physical_bus(physical_bus);
   BUS* bus = internal_bus_pointers[internal_bus];

   if (bus->get_bus_type() == SLACK_TYPE) {
      //size_t internal_bus = network_matrix.get_internal_bus_number_of_physical_bus(physical_bus);

      double sbase = psdb.get_system_base_power_in_MVA();
      double bus_P_mismatch_in_MW = -bus_power[internal_bus].real() * sbase;
      double bus_Q_mismatch_in_MVar = -bus_power[internal_bus].imag() * sbase;
      //cout<<"slack bus "<<physical_bus<<" power allocation:  mismatch "<<bus_P_mismatch_in_MW<<", "<<bus_Q_mismatch_in_MVar<<endl;

      double total_p_max_in_MW = psdb.get_regulatable_p_max_at_physical_bus_in_MW(physical_bus);
      double total_p_min_in_MW = psdb.get_regulatable_p_min_at_physical_bus_in_MW(physical_bus);
      double total_q_max_in_MVar = psdb.get_regulatable_q_max_at_physical_bus_in_MVar(physical_bus);
      double total_q_min_in_MVar = psdb.get_regulatable_q_min_at_physical_bus_in_MVar(physical_bus);

      double P_loading_percentage = (bus_P_mismatch_in_MW - total_p_min_in_MW);
      P_loading_percentage /= (total_p_max_in_MW - total_p_min_in_MW);
      double Q_loading_percentage = (bus_Q_mismatch_in_MVar - total_q_min_in_MVar);
      Q_loading_percentage /= (total_q_max_in_MVar - total_q_min_in_MVar);

      size_t n;
      vector<SOURCE*> sources = psdb.get_sources_connecting_to_bus(physical_bus);
      n = sources.size();
      for (size_t i = 0; i != n; ++i) {
         if (sources[i]->get_status() == true) {
            double P_loading_in_MW = sources[i]->get_p_max_in_MW() - sources[i]->get_p_min_in_MW();
            P_loading_in_MW = P_loading_in_MW * P_loading_percentage + sources[i]->get_p_min_in_MW();
            sources[i]->set_p_generation_in_MW(P_loading_in_MW);

            double Q_loading_in_MVar = sources[i]->get_q_max_in_MVar() - sources[i]->get_q_min_in_MVar();
            Q_loading_in_MVar = Q_loading_in_MVar * Q_loading_percentage + sources[i]->get_q_min_in_MVar();
            sources[i]->set_q_generation_in_MVar(Q_loading_in_MVar);
         }
      }

      vector<VSC_HVDC*> vsc_hvdcs = psdb.get_vsc_hvdcs_connecting_to_bus(physical_bus);
      n = vsc_hvdcs.size();
      for (size_t i = 0; i != n; ++i) {
         if (vsc_hvdcs[i]->get_status() == true) {
            size_t index = vsc_hvdcs[i]->get_converter_index_with_ac_bus(physical_bus);
            if (vsc_hvdcs[i]->get_converter_status(index) == true) {
               if (vsc_hvdcs[i]->get_converter_active_power_operation_mode(index) == VSC_AC_VOLTAGE_ANGLE_CONTROL) {
                  double P_loading_in_MW = vsc_hvdcs[i]->get_converter_Pmax_in_MW(index) - vsc_hvdcs[i]->get_converter_Pmin_in_MW(index);
                  P_loading_in_MW = P_loading_in_MW * P_loading_percentage + vsc_hvdcs[i]->get_converter_Pmin_in_MW(index);
                  vsc_hvdcs[i]->set_converter_P_to_AC_bus_in_MW(index, P_loading_in_MW);
               }
               if (vsc_hvdcs[i]->get_converter_reactive_power_operation_mode(index) == VSC_AC_VOLTAGE_CONTROL) {
                  double Q_loading_in_MVar = vsc_hvdcs[i]->get_converter_Qmax_in_MVar(index) - vsc_hvdcs[i]->get_converter_Qmin_in_MVar(index);
                  Q_loading_in_MVar = Q_loading_in_MVar * Q_loading_percentage + vsc_hvdcs[i]->get_converter_Qmin_in_MVar(index);
                  vsc_hvdcs[i]->set_converter_Q_to_AC_bus_in_MVar(index, Q_loading_in_MVar);
               }
            }
         }
      }
   }
}

bool PF_SOLVER::check_PV_bus_constraint_of_physical_bus(size_t physical_bus)
{
   PF_DATA& psdb = toolkit->get_database();
   NET_MATRIX& network_matrix = get_network_matrix();

   size_t internal_bus = network_matrix.get_internal_bus_number_of_physical_bus(physical_bus);
   BUS* bus = internal_bus_pointers[internal_bus];

   if (bus->get_bus_type() == PV_TYPE) {
      char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];

      bool bus_type_changed = false;
      //size_t internal_bus = network_matrix.get_internal_bus_number_of_physical_bus(physical_bus);

      double bus_Q_mismatch_in_MVar = -bus_power[internal_bus].imag() * psdb.get_system_base_power_in_MVA();
      //cout<<"PV bus "<<physical_bus<<" power allocation:  mismatch: "<<bus_Q_mismatch_in_MVar<<endl;

      double total_q_max_in_MVar = psdb.get_regulatable_q_max_at_physical_bus_in_MVar(physical_bus);
      double total_q_min_in_MVar = psdb.get_regulatable_q_min_at_physical_bus_in_MVar(physical_bus);

      if (fabs(total_q_max_in_MVar - total_q_min_in_MVar) < DOUBLE_EPSILON) {
         bus->set_bus_type(PV_TO_PQ_TYPE_3);
         bus_type_changed = true;
         set_all_sources_at_physical_bus_to_q_max(physical_bus);
         set_all_regulating_vsc_hvdcs_at_physical_bus_to_q_max(physical_bus);

         if (toolkit->is_detailed_log_enabled()) {
            string busname = psdb.bus_number2bus_name(physical_bus);
            snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Bus %zu [%s] changed from PV_TYPE to PV_TO_PQ_TYPE_3 with sources q max reached.",
                     physical_bus, busname.c_str());
            toolkit->show_information(buffer);
         }

         return bus_type_changed;
      }

      if (bus_Q_mismatch_in_MVar > total_q_max_in_MVar) {
         bus->set_bus_type(PV_TO_PQ_TYPE_3);
         bus_type_changed = true;

         if (toolkit->is_detailed_log_enabled()) {
            string busname = psdb.bus_number2bus_name(physical_bus);
            snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Bus %zu [%s] changed from PV_TYPE to PV_TO_PQ_TYPE_3 with sources q max reached.",
                     physical_bus, busname.c_str());
            toolkit->show_information(buffer);
         }

         set_all_sources_at_physical_bus_to_q_max(physical_bus);
         set_all_regulating_vsc_hvdcs_at_physical_bus_to_q_max(physical_bus);
         return bus_type_changed;
      }

      if (bus_Q_mismatch_in_MVar < total_q_min_in_MVar) {
         bus->set_bus_type(PV_TO_PQ_TYPE_3);
         bus_type_changed = true;

         if (toolkit->is_detailed_log_enabled()) {
            string busname = psdb.bus_number2bus_name(physical_bus);
            snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Bus %zu [%s] changed from PV_TYPE to PV_TO_PQ_TYPE_3 with sources q min reached.",
                     physical_bus, busname.c_str());
            toolkit->show_information(buffer);
         }

         set_all_sources_at_physical_bus_to_q_min(physical_bus);
         set_all_regulating_vsc_hvdcs_at_physical_bus_to_q_min(physical_bus);
         return bus_type_changed;
      }

      double Q_loading_percentage = (bus_Q_mismatch_in_MVar - total_q_min_in_MVar);
      Q_loading_percentage /= (total_q_max_in_MVar - total_q_min_in_MVar);

      vector<SOURCE*> sources = psdb.get_sources_connecting_to_bus(physical_bus);
      size_t n;
      n = sources.size();
      for (size_t i = 0; i != n; ++i) {
         if (sources[i]->get_status() == true) {
            double Q_loading_in_MVar = sources[i]->get_q_max_in_MVar() - sources[i]->get_q_min_in_MVar();
            Q_loading_in_MVar = Q_loading_in_MVar * Q_loading_percentage + sources[i]->get_q_min_in_MVar();
            sources[i]->set_q_generation_in_MVar(Q_loading_in_MVar);
         }
      }

      vector<VSC_HVDC*> vsc_hvdcs = psdb.get_vsc_hvdcs_connecting_to_bus(physical_bus);
      n = vsc_hvdcs.size();
      for (size_t i = 0; i != n; ++i) {
         if (vsc_hvdcs[i]->get_status() == true) {
            size_t index = vsc_hvdcs[i]->get_converter_index_with_ac_bus(physical_bus);
            if (vsc_hvdcs[i]->get_converter_status(index) == true and
                  vsc_hvdcs[i]->get_converter_reactive_power_operation_mode(index) == VSC_AC_VOLTAGE_CONTROL) {
               double Q_loading_in_MVar = vsc_hvdcs[i]->get_converter_Qmax_in_MVar(index) - vsc_hvdcs[i]->get_converter_Qmin_in_MVar(index);
               Q_loading_in_MVar = Q_loading_in_MVar * Q_loading_percentage + vsc_hvdcs[i]->get_converter_Qmin_in_MVar(index);
               //cout<<"index :"<<index<<", Q_loading_in_Mvar: "<<Q_loading_in_MVar<<endl;
               vsc_hvdcs[i]->set_converter_Q_to_AC_bus_in_MVar(index, Q_loading_in_MVar);
            }
         }
      }
      return bus_type_changed;
   } else {
      return false;
   }
}

bool PF_SOLVER::check_PV_TO_PQ_bus_constraint_of_physical_bus(size_t physical_bus)
{
   bool bus_type_changed = false;

   PF_DATA& psdb = toolkit->get_database();
   NET_MATRIX& network_matrix = get_network_matrix();

   size_t internal_bus = network_matrix.get_internal_bus_number_of_physical_bus(physical_bus);
   BUS* bus = internal_bus_pointers[internal_bus];

   if (bus->get_bus_type() != PV_TO_PQ_TYPE_1 and
         bus->get_bus_type() != PV_TO_PQ_TYPE_2 and
         bus->get_bus_type() != PV_TO_PQ_TYPE_3 and
         bus->get_bus_type() != PV_TO_PQ_TYPE_4) {
      return bus_type_changed;
   }

   ostringstream osstream;

   double total_q_max_in_MVar = psdb.get_regulatable_q_max_at_physical_bus_in_MVar(physical_bus);
   double total_q_min_in_MVar = psdb.get_regulatable_q_min_at_physical_bus_in_MVar(physical_bus);

   if (fabs(total_q_max_in_MVar - total_q_min_in_MVar) > DOUBLE_EPSILON)
      ;
   else {
      if (bus->get_bus_type() != PV_TO_PQ_TYPE_3) {
         bus->set_bus_type(PV_TO_PQ_TYPE_3);
         bus_type_changed = true;
         if (toolkit->is_detailed_log_enabled()) {
            string busname = psdb.bus_number2bus_name(physical_bus);
            osstream << "Bus " << physical_bus << " [" << busname << "] changed from PV_TYPE to PV_TO_PQ_TYPE_3.";
            toolkit->show_information(osstream);
         }

         set_all_sources_at_physical_bus_to_q_max(physical_bus);
         set_all_regulating_vsc_hvdcs_at_physical_bus_to_q_max(physical_bus);
      }
      return bus_type_changed;
   }


   double bus_voltage = bus->get_positive_sequence_voltage_in_pu();

   double voltage_to_regulated = psdb.get_voltage_to_regulate_of_physical_bus_in_pu(physical_bus);

   double total_regulating_q_generation_in_MVar = psdb.get_total_regulating_q_generation_at_physical_bus_in_MVar(physical_bus);

   if (bus_voltage > voltage_to_regulated) {
      if (total_regulating_q_generation_in_MVar > total_q_min_in_MVar) {
         switch (bus->get_bus_type()) {
         case PV_TO_PQ_TYPE_4:
            bus->set_bus_type(PV_TO_PQ_TYPE_3);
            //os<<"Bus %zu changed from PV_TO_PQ_TYPE_4 to PV_TO_PQ_TYPE_3.", physical_bus);
            //show_information(osstream);
            break;
         case PV_TO_PQ_TYPE_3:
            bus->set_bus_type(PV_TO_PQ_TYPE_2);
            //os<<"Bus %zu changed from PV_TO_PQ_TYPE_3 to PV_TO_PQ_TYPE_2.", physical_bus);
            //show_information(osstream);
            break;
         case PV_TO_PQ_TYPE_2:
            bus->set_bus_type(PV_TO_PQ_TYPE_1);
            //os<<"Bus %zu changed from PV_TO_PQ_TYPE_2 to PV_TO_PQ_TYPE_1.", physical_bus);
            //show_information(osstream);
            break;
         case PV_TO_PQ_TYPE_1:
            bus->set_bus_type(PV_TYPE);
            bus_type_changed = true;
            bus->set_positive_sequence_voltage_in_pu(voltage_to_regulated);
            if (toolkit->is_detailed_log_enabled()) {
               string busname = psdb.bus_number2bus_name(physical_bus);
               osstream << "Bus " << physical_bus << " [" << busname << "] changed from PV_TO_PQ_TYPE_1 to PV_TYPE.";
               toolkit->show_information(osstream);
            }
            break;
         default:
            break;
         }
      }
   } else { // bus_voltage<voltage_to_regulated
      if (total_regulating_q_generation_in_MVar < total_q_max_in_MVar) {
         switch (bus->get_bus_type()) {
         case PV_TO_PQ_TYPE_4:
            bus->set_bus_type(PV_TO_PQ_TYPE_3);
            //os<<"Bus %zu changed from PV_TO_PQ_TYPE_4 to PV_TO_PQ_TYPE_3.", physical_bus);
            //show_information(osstream);
            break;
         case PV_TO_PQ_TYPE_3:
            bus->set_bus_type(PV_TO_PQ_TYPE_2);
            //os<<"Bus %zu changed from PV_TO_PQ_TYPE_3 to PV_TO_PQ_TYPE_2.", physical_bus);
            //show_information(osstream);
            break;
         case PV_TO_PQ_TYPE_2:
            bus->set_bus_type(PV_TO_PQ_TYPE_1);
            //os<<"Bus %zu changed from PV_TO_PQ_TYPE_2 to PV_TO_PQ_TYPE_1.", physical_bus);
            //show_information(osstream);
            break;
         case PV_TO_PQ_TYPE_1:
            bus->set_bus_type(PV_TYPE);
            bus_type_changed = true;
            bus->set_positive_sequence_voltage_in_pu(voltage_to_regulated);
            if (toolkit->is_detailed_log_enabled()) {
               string busname = psdb.bus_number2bus_name(physical_bus);
               osstream << "Bus " << physical_bus << " [" << busname << "] changed from PV_TO_PQ_TYPE_1 to PV_TYPE.";
               toolkit->show_information(osstream);
            }
            break;
         default:
            break;
         }
      }
   }
   return bus_type_changed;

   /*
   if(bus_voltage<voltage_to_regulated)
   {
       if(bus_Q_mismatch_in_MVar>total_q_max_in_MVar)
       {
           bus->set_bus_type(PV_TO_PQ_TYPE_3);
           set_all_sources_at_physical_bus_to_q_max(physical_bus);
           set_all_regulating_vsc_hvdcs_at_physical_bus_to_q_max(physical_bus);

           string busname = psdb.bus_number2bus_name(physical_bus);
           osstream<<"Var of sources and/or vsc-hvdcs at bus "<<physical_bus<<" ["<<busname<<"] are set to max.";
           toolkit->show_information(osstream);
       }
       else
       {
           switch(bus->get_bus_type())
           {
               case PV_TO_PQ_TYPE_4:
                   bus->set_bus_type(PV_TO_PQ_TYPE_3);
                   if(toolkit->is_detailed_log_enabled())
                   {
                       string busname = psdb.bus_number2bus_name(physical_bus);
                       osstream<<"Bus "<<physical_bus<<" ["<<busname<<"] changed from PV_TO_PQ_TYPE_4 to PV_TO_PQ_TYPE_3.";
                       toolkit->show_information(osstream);
                   }
                   break;
               case PV_TO_PQ_TYPE_3:
                   bus->set_bus_type(PV_TO_PQ_TYPE_2);
                   if(toolkit->is_detailed_log_enabled())
                   {
                       string busname = psdb.bus_number2bus_name(physical_bus);
                       osstream<<"Bus "<<physical_bus<<" ["<<busname<<"] changed from PV_TO_PQ_TYPE_3 to PV_TO_PQ_TYPE_2.";
                       toolkit->show_information(osstream);
                   }
                   break;
               case PV_TO_PQ_TYPE_2:
                   bus->set_bus_type(PV_TO_PQ_TYPE_1);
                   if(toolkit->is_detailed_log_enabled())
                   {
                       string busname = psdb.bus_number2bus_name(physical_bus);
                       osstream<<"Bus "<<physical_bus<<" ["<<busname<<"] changed from PV_TO_PQ_TYPE_2 to PV_TO_PQ_TYPE_1.";
                       toolkit->show_information(osstream);
                   }
                   break;
               case PV_TO_PQ_TYPE_1:
                   bus->set_bus_type(PV_TYPE);
                   bus_type_changed = true;
                   bus->set_positive_sequence_voltage_in_pu(voltage_to_regulated);
                   if(toolkit->is_detailed_log_enabled())
                   {
                       string busname = psdb.bus_number2bus_name(physical_bus);
                       osstream<<"Bus "<<physical_bus<<" ["<<busname<<"] changed from PV_TO_PQ_TYPE_1 to PV_TYPE.";
                       toolkit->show_information(osstream);
                   }
                   break;
               default:
                   break;
           }
       }
   }
   else
   {
       if(bus_Q_mismatch_in_MVar<total_q_min_in_MVar)
       {
           bus->set_bus_type(PV_TO_PQ_TYPE_3);
           set_all_sources_at_physical_bus_to_q_min(physical_bus);
           set_all_regulating_vsc_hvdcs_at_physical_bus_to_q_min(physical_bus);
           string busname = psdb.bus_number2bus_name(physical_bus);
           osstream<<"Var of sources and/or vsc-hvdcs at bus "<<physical_bus<<" ["<<busname<<"] are set to min.";
           toolkit->show_information(osstream);
       }
       else
       {
           switch(bus->get_bus_type())
           {
               case PV_TO_PQ_TYPE_4:
                   bus->set_bus_type(PV_TO_PQ_TYPE_3);
                   if(toolkit->is_detailed_log_enabled())
                   {
                       string busname = psdb.bus_number2bus_name(physical_bus);
                       osstream<<"Bus "<<physical_bus<<" ["<<busname<<"] changed from PV_TO_PQ_TYPE_4 to PV_TO_PQ_TYPE_3.";
                       toolkit->show_information(osstream);
                   }
                   break;
               case PV_TO_PQ_TYPE_3:
                   bus->set_bus_type(PV_TO_PQ_TYPE_2);
                   if(toolkit->is_detailed_log_enabled())
                   {
                       string busname = psdb.bus_number2bus_name(physical_bus);
                       osstream<<"Bus "<<physical_bus<<" ["<<busname<<"] changed from PV_TO_PQ_TYPE_3 to PV_TO_PQ_TYPE_2.";
                       toolkit->show_information(osstream);
                   }
                   break;
               case PV_TO_PQ_TYPE_2:
                   bus->set_bus_type(PV_TO_PQ_TYPE_1);
                   if(toolkit->is_detailed_log_enabled())
                   {
                       string busname = psdb.bus_number2bus_name(physical_bus);
                       osstream<<"Bus "<<physical_bus<<" ["<<busname<<"] changed from PV_TO_PQ_TYPE_2 to PV_TO_PQ_TYPE_1.";
                       toolkit->show_information(osstream);
                   }
                   break;
               case PV_TO_PQ_TYPE_1:
                   bus->set_bus_type(PV_TYPE);
                   bus_type_changed = true;
                   bus->set_positive_sequence_voltage_in_pu(voltage_to_regulated);
                   if(toolkit->is_detailed_log_enabled())
                   {
                       string busname = psdb.bus_number2bus_name(physical_bus);
                       osstream<<"Bus "<<physical_bus<<" ["<<busname<<"] changed from PV_TO_PQ_TYPE_1 to PV_TYPE.";
                       toolkit->show_information(osstream);
                   }
                   break;
               default:
                   break;
           }
       }
   }
   return bus_type_changed;
   */
}

void PF_SOLVER::set_all_sources_at_physical_bus_to_q_min(size_t physical_bus)
{
   PF_DATA& psdb = toolkit->get_database();
   vector<SOURCE*> sources = psdb.get_sources_connecting_to_bus(physical_bus);

   size_t n = sources.size();
   for (size_t i = 0; i != n; ++i) {
      if (sources[i]->get_status() == true) {
         sources[i]->set_q_generation_in_MVar(sources[i]->get_q_min_in_MVar());
      }
   }
}
void PF_SOLVER::set_all_sources_at_physical_bus_to_q_max(size_t physical_bus)
{
   PF_DATA& psdb = toolkit->get_database();
   vector<SOURCE*> sources = psdb.get_sources_connecting_to_bus(physical_bus);

   size_t n = sources.size();
   for (size_t i = 0; i != n; ++i) {
      if (sources[i]->get_status() == true) {
         sources[i]->set_q_generation_in_MVar(sources[i]->get_q_max_in_MVar());
      }
   }
}

void PF_SOLVER::set_all_regulating_vsc_hvdcs_at_physical_bus_to_q_min(size_t physical_bus)
{
   PF_DATA& psdb = toolkit->get_database();
   vector<VSC_HVDC*> vsc_hvdcs = psdb.get_vsc_hvdcs_connecting_to_bus(physical_bus);

   size_t n = vsc_hvdcs.size();
   for (size_t i = 0; i != n; ++i) {
      if (vsc_hvdcs[i]->get_status() == true) {
         size_t index = vsc_hvdcs[i]->get_converter_index_with_ac_bus(physical_bus);
         if (vsc_hvdcs[i]->get_converter_status(index) == true and
               vsc_hvdcs[i]->get_converter_reactive_power_operation_mode(index) == VSC_AC_VOLTAGE_CONTROL) {
            vsc_hvdcs[i]->set_converter_Q_to_AC_bus_in_MVar(index, vsc_hvdcs[i]->get_converter_Qmin_in_MVar(index));
         }
      }
   }
}

void PF_SOLVER::set_all_regulating_vsc_hvdcs_at_physical_bus_to_q_max(size_t physical_bus)
{
   PF_DATA& psdb = toolkit->get_database();
   vector<VSC_HVDC*> vsc_hvdcs = psdb.get_vsc_hvdcs_connecting_to_bus(physical_bus);

   size_t n = vsc_hvdcs.size();
   for (size_t i = 0; i != n; ++i) {
      if (vsc_hvdcs[i]->get_status() == true) {
         size_t index = vsc_hvdcs[i]->get_converter_index_with_ac_bus(physical_bus);
         if (vsc_hvdcs[i]->get_converter_status(index) == true and
               vsc_hvdcs[i]->get_converter_reactive_power_operation_mode(index) == VSC_AC_VOLTAGE_CONTROL) {
            vsc_hvdcs[i]->set_converter_Q_to_AC_bus_in_MVar(index, vsc_hvdcs[i]->get_converter_Qmax_in_MVar(index));
         }
      }
   }
}


void PF_SOLVER::allocate_bus_power_mismatch_without_checking_bus_type_constraints()
{
   size_t nbus = buses.size();

#ifdef ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
   set_openmp_number_of_threads(toolkit->get_thread_number());
   #pragma omp parallel for schedule(static)
#endif // ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
   for (size_t i = 0; i < nbus; ++i) {
      BUS_TYPE btype = buses[i]->get_bus_type();
      if (btype == PQ_TYPE or btype == OUT_OF_SERVICE) {
         continue;
      }

      size_t physical_bus = buses[i]->get_bus_number();

      if (btype == SLACK_TYPE) {
         allocate_SLACK_bus_power_mismatch_without_checking_bus_type_constraint(physical_bus);
      } else {
         if (btype == PV_TYPE) {
            allocate_PV_bus_power_mismatch_without_checking_bus_type_constraint(physical_bus);
         } else {
            continue;
         }
      }
   }
   return;
}

void PF_SOLVER::allocate_SLACK_bus_power_mismatch_without_checking_bus_type_constraint(size_t physical_bus)
{
   check_SLACK_bus_constraint_of_physical_bus(physical_bus);
}

void PF_SOLVER::allocate_PV_bus_power_mismatch_without_checking_bus_type_constraint(size_t physical_bus)
{
   PF_DATA& psdb = toolkit->get_database();
   NET_MATRIX& network_matrix = get_network_matrix();

   size_t internal_bus = network_matrix.get_internal_bus_number_of_physical_bus(physical_bus);
   BUS* bus = internal_bus_pointers[internal_bus];

   if (bus->get_bus_type() == PV_TYPE) {
      double bus_Q_mismatch_in_MVar = -bus_power[internal_bus].imag() * psdb.get_system_base_power_in_MVA();

      size_t n;
      vector<SOURCE*> sources = psdb.get_sources_connecting_to_bus(physical_bus);
      n = sources.size();
      size_t n_regulating_source =  0;
      for (size_t i = 0; i != n; ++i) {
         if (sources[i]->get_status() == true) {
            ++n_regulating_source;
         }
      }

      vector<VSC_HVDC*> vsc_hvdcs = psdb.get_vsc_hvdcs_connecting_to_bus(physical_bus);
      n = vsc_hvdcs.size();
      size_t n_regulating_vsc =  0;
      for (size_t i = 0; i != n; ++i) {
         if (vsc_hvdcs[i]->get_status() == true) {
            size_t index = vsc_hvdcs[i]->get_converter_index_with_ac_bus(physical_bus);
            if (vsc_hvdcs[i]->get_converter_status(index) == true) {
               if (vsc_hvdcs[i]->get_converter_reactive_power_operation_mode(index) != VSC_AC_VOLTAGE_CONTROL) {
                  bus_Q_mismatch_in_MVar -= vsc_hvdcs[i]->get_converter_Q_to_AC_bus_in_MVar(index);
               } else {
                  ++n_regulating_vsc;
               }
            }
         }
      }

      size_t n_regulating_source_and_vsc = n_regulating_source + n_regulating_vsc;

      double total_q_max_in_MVar = psdb.get_regulatable_q_max_at_physical_bus_in_MVar(physical_bus);
      double total_q_min_in_MVar = psdb.get_regulatable_q_min_at_physical_bus_in_MVar(physical_bus);

      double Q_loading_percentage = (bus_Q_mismatch_in_MVar - total_q_min_in_MVar);
      if (total_q_max_in_MVar != total_q_min_in_MVar) {
         Q_loading_percentage /= (total_q_max_in_MVar - total_q_min_in_MVar);
      } else {
         Q_loading_percentage = 1 / n_regulating_source_and_vsc;
      }

      n = sources.size();
      for (size_t i = 0; i != n; ++i) {
         if (sources[i]->get_status() == true) {
            double Q_loading_in_MVar = sources[i]->get_q_max_in_MVar() - sources[i]->get_q_min_in_MVar();
            if (total_q_max_in_MVar != total_q_min_in_MVar) {
               Q_loading_in_MVar = Q_loading_in_MVar * Q_loading_percentage + sources[i]->get_q_min_in_MVar();
            } else {
               Q_loading_in_MVar = bus_Q_mismatch_in_MVar * Q_loading_percentage;
            }
            sources[i]->set_q_generation_in_MVar(Q_loading_in_MVar);
         }
      }

      n = vsc_hvdcs.size();
      for (size_t i = 0; i != n; ++i) {
         if (vsc_hvdcs[i]->get_status() == true) {
            size_t index = vsc_hvdcs[i]->get_converter_index_with_ac_bus(physical_bus);
            if (vsc_hvdcs[i]->get_converter_status(index) == true) {
               if (vsc_hvdcs[i]->get_converter_reactive_power_operation_mode(index) == VSC_AC_VOLTAGE_CONTROL) {
                  double Q_loading_in_MVar = 0;
                  if (total_q_max_in_MVar != total_q_min_in_MVar) {
                     Q_loading_in_MVar = vsc_hvdcs[i]->get_converter_Qmax_in_MVar(index) - vsc_hvdcs[i]->get_converter_Qmin_in_MVar(index);
                     Q_loading_in_MVar = Q_loading_in_MVar * Q_loading_percentage + vsc_hvdcs[i]->get_converter_Qmin_in_MVar(index);
                  } else {
                     Q_loading_in_MVar = bus_Q_mismatch_in_MVar * Q_loading_percentage;
                  }

                  vsc_hvdcs[i]->set_converter_Q_to_AC_bus_in_MVar(index, Q_loading_in_MVar);
               }
            }
         }
      }
   }
}


void PF_SOLVER::build_bus_power_mismatch_vector_for_coupled_solution()
{
   size_t nP = internal_P_equation_buses.size();
   size_t nQ = internal_Q_equation_buses.size();

   S_mismatch.resize(nP + nQ);

#ifdef ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
   set_openmp_number_of_threads(toolkit->get_thread_number());
   #pragma omp parallel for schedule(static)
#endif // ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
   for (size_t i = 0; i < nP; ++i) {
      size_t internal_bus = internal_P_equation_buses[i];
      //s_mismatch.push_back(-bus_power[internal_bus].real());
      S_mismatch[i] = -bus_power[internal_bus].real();
   }
#ifdef ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
   set_openmp_number_of_threads(toolkit->get_thread_number());
   #pragma omp parallel for schedule(static)
#endif // ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
   for (size_t i = 0; i < nQ; ++i) {
      size_t internal_bus = internal_Q_equation_buses[i];
      //s_mismatch.push_back(-bus_power[internal_bus].imag());
      S_mismatch[nP + i] = -bus_power[internal_bus].imag();
   }
}

void PF_SOLVER::build_bus_P_power_mismatch_vector_for_decoupled_solution()
{
   size_t nP = internal_P_equation_buses.size();

   P_mismatch.resize(nP);

#ifdef ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
   set_openmp_number_of_threads(toolkit->get_thread_number());
   #pragma omp parallel for schedule(static)
#endif // ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
   for (size_t i = 0; i < nP; ++i) {
      size_t internal_bus = internal_P_equation_buses[i];
      P_mismatch[i] = -bus_power[internal_bus].real();
      //cout<<"Pmismatch vector, "<<i<<", "<<-bus_power[internal_bus].real()<<endl;
   }
}

void PF_SOLVER::build_bus_Q_power_mismatch_vector_for_decoupled_solution()
{
   size_t nQ = internal_Q_equation_buses.size();

   Q_mismatch.resize(nQ);

#ifdef ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
   set_openmp_number_of_threads(toolkit->get_thread_number());
   #pragma omp parallel for schedule(static)
#endif // ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
   for (size_t i = 0; i < nQ; ++i) {
      size_t internal_bus = internal_Q_equation_buses[i];
      Q_mismatch[i] = -bus_power[internal_bus].imag();
      //cout<<"Qmismatch vector, "<<i<<", "<<-bus_power[internal_bus].imag()<<endl;
   }
}

void PF_SOLVER::update_bus_voltage_and_angle(vector<double>& update)
{
   ostringstream osstream;
   size_t nP = internal_P_equation_buses.size();
   size_t nQ = internal_Q_equation_buses.size();

   double max_dangle = 0.0, max_dv = 0.0;
   size_t nv = update.size();
   for (size_t i = 0; i != nP; ++i) {
      if (max_dangle < abs(update[i])) {
         max_dangle = abs(update[i]);
      }
   }
   for (size_t i = nP; i != nv; ++i) {
      if (max_dv < abs(update[i])) {
         max_dv = abs(update[i]);
      }
   }
   osstream << "Maximum angle   change is: " << max_dangle << " rad (" << rad2deg(max_dangle) << " deg).\n"
            << "Maximum voltage change is: " << max_dv << " pu.";
   toolkit->show_information(osstream);

   double limit = get_maximum_angle_change_in_rad();
   if (max_dangle > limit) {
      double scale = limit / max_dangle;
#ifdef ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
      set_openmp_number_of_threads(toolkit->get_thread_number());
      #pragma omp parallel for schedule(static)
#endif // ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
      for (size_t i = 0; i < nP; ++i) {
         update[i] *= scale;
      }
   }
   limit = get_maximum_voltage_change_in_pu();
   if (max_dv > limit) {
      double scale = limit / max_dv;
#ifdef ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
      set_openmp_number_of_threads(toolkit->get_thread_number());
      #pragma omp parallel for schedule(static)
#endif // ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
      for (size_t i = nP; i < nv; ++i) {
         update[i] *= scale;
      }
   }


   double alpha = get_iteration_accelerator();
   double alpha_P = alpha;
   double alpha_Q = alpha;

#ifdef ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
   set_openmp_number_of_threads(toolkit->get_thread_number());
   #pragma omp parallel for schedule(static)
#endif // ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
   for (size_t i = 0; i < nP; ++i) {
      size_t internal_bus = internal_P_equation_buses[i];
      //size_t physical_bus = network_matrix.get_physical_bus_number_of_internal_bus(internal_bus);
      //BUS* bus = psdb.get_bus(physical_bus);
      BUS* bus = internal_bus_pointers[internal_bus];

      double current_angle = bus->get_positive_sequence_angle_in_rad();

      double delta_angle = update[i];

      bus->set_positive_sequence_angle_in_rad(current_angle + alpha_P * delta_angle);
   }

#ifdef ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
   set_openmp_number_of_threads(toolkit->get_thread_number());
   #pragma omp parallel for schedule(static)
#endif // ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
   for (size_t i = 0; i < nQ; ++i) {
      size_t internal_bus = internal_Q_equation_buses[i];
      //size_t physical_bus = network_matrix.get_physical_bus_number_of_internal_bus(internal_bus);
      //BUS* bus = psdb.get_bus(physical_bus);
      BUS* bus = internal_bus_pointers[internal_bus];

      double current_voltage = bus->get_positive_sequence_voltage_in_pu();

      double delta_voltage = update[nP + i];

      bus->set_positive_sequence_voltage_in_pu(current_voltage + alpha_Q * delta_voltage);
   }

   if (get_non_divergent_solution_logic() == true) {
      double Perror0 = get_maximum_active_power_mismatch_in_MW();
      double Qerror0 = get_maximum_reactive_power_mismatch_in_MVar();
      for (size_t i = 0; i < 10; ++i) {
         try_to_solve_2t_lcc_hvdc_steady_state();
         calculate_raw_bus_power_mismatch();
         double Perror = get_maximum_active_power_mismatch_in_MW();
         double Qerror = get_maximum_reactive_power_mismatch_in_MVar();
         bool P_is_worse = Perror > Perror0 and Perror > get_allowed_max_active_power_imbalance_in_MW();
         bool Q_is_worse = Qerror > Qerror0 and Qerror > get_allowed_max_reactive_power_imbalance_in_MVar();
         if (not (P_is_worse or Q_is_worse)) {
            break;
         } else {
            alpha_P *= 0.5;
#ifdef ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
            set_openmp_number_of_threads(toolkit->get_thread_number());
            #pragma omp parallel for schedule(static)
#endif // ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
            for (size_t i = 0; i < nP; ++i) {
               size_t internal_bus = internal_P_equation_buses[i];
               //size_t physical_bus = network_matrix.get_physical_bus_number_of_internal_bus(internal_bus);
               //BUS* bus = psdb.get_bus(physical_bus);
               BUS* bus = internal_bus_pointers[internal_bus];

               double current_angle = bus->get_positive_sequence_angle_in_rad();

               double delta_angle = update[i];

               bus->set_positive_sequence_angle_in_rad(current_angle - alpha_P * delta_angle);
            }
            alpha_Q *= 0.5;
#ifdef ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
            set_openmp_number_of_threads(toolkit->get_thread_number());
            #pragma omp parallel for schedule(static)
#endif // ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
            for (size_t i = 0; i < nQ; ++i) {
               size_t internal_bus = internal_Q_equation_buses[i];
               //size_t physical_bus = network_matrix.get_physical_bus_number_of_internal_bus(internal_bus);
               //BUS* bus = psdb.get_bus(physical_bus);
               BUS* bus = internal_bus_pointers[internal_bus];

               double current_voltage = bus->get_positive_sequence_voltage_in_pu();

               double delta_voltage = update[nP + i];

               bus->set_positive_sequence_voltage_in_pu(current_voltage - alpha_Q * delta_voltage);
            }
         }
      }
   }
}

void PF_SOLVER::update_bus_voltage(vector<double>& update)
{
   PF_DATA& psdb = toolkit->get_database();
   ostringstream osstream;
   NET_MATRIX& network_matrix = get_network_matrix();

   double limit = get_maximum_voltage_change_in_pu();
   if (fabs(limit) < DOUBLE_EPSILON) {
      return;
   }

   double max_dv = 0.0;
   size_t max_delta_v_bus = 0;
   size_t nv = update.size();
   for (size_t i = 0; i != nv; ++i) {
      if (max_dv < abs(update[i])) {
         max_dv = abs(update[i]);
         size_t internal_bus = internal_Q_equation_buses[i];
         max_delta_v_bus = network_matrix.get_physical_bus_number_of_internal_bus(internal_bus);
      }
   }
   osstream << "Maximum voltage change is: " << max_dv << " pu at physical bus " << max_delta_v_bus << " [" << psdb.bus_number2bus_name(max_delta_v_bus) << "]";
   toolkit->show_information(osstream);

   if (max_dv > limit) {
      double scale = limit / max_dv;
#ifdef ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
      set_openmp_number_of_threads(toolkit->get_thread_number());
      #pragma omp parallel for schedule(static)
#endif // ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
      for (size_t i = 0; i < nv; ++i) {
         update[i] *= scale;
      }
   }

   double Qerror0 = get_maximum_reactive_power_mismatch_in_MVar();

   //size_t nP = internal_P_equation_buses.size();
   size_t nQ = internal_Q_equation_buses.size();

   double alpha = get_iteration_accelerator();

#ifdef ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
   set_openmp_number_of_threads(toolkit->get_thread_number());
   #pragma omp parallel for schedule(static)
#endif // ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
   for (size_t i = 0; i < nQ; ++i) {
      size_t internal_bus = internal_Q_equation_buses[i];
      //BUS* bus = psdb.get_bus(physical_bus);
      BUS* bus = internal_bus_pointers[internal_bus];

      double current_voltage = bus->get_positive_sequence_voltage_in_pu();

      double delta_voltage = update[i];

      bus->set_positive_sequence_voltage_in_pu(current_voltage + alpha * delta_voltage);
   }

   if (get_non_divergent_solution_logic() == true) {
      for (size_t i = 0; i < 10; ++i) {
         try_to_solve_2t_lcc_hvdc_steady_state();
         calculate_raw_bus_power_mismatch();
         double Qerror = get_maximum_reactive_power_mismatch_in_MVar();
         if (Qerror > Qerror0 and Qerror > get_allowed_max_reactive_power_imbalance_in_MVar()) {
            alpha *= 0.5;
#ifdef ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
            set_openmp_number_of_threads(toolkit->get_thread_number());
            #pragma omp parallel for schedule(static)
#endif // ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
            for (size_t i = 0; i < nQ; ++i) {
               size_t internal_bus = internal_Q_equation_buses[i];
               //size_t physical_bus = network_matrix.get_physical_bus_number_of_internal_bus(internal_bus);
               //BUS* bus = psdb.get_bus(physical_bus);
               BUS* bus = internal_bus_pointers[internal_bus];

               double current_voltage = bus->get_positive_sequence_voltage_in_pu();

               double delta_voltage = update[i];

               bus->set_positive_sequence_voltage_in_pu(current_voltage - alpha * delta_voltage);
            }
         } else {
            break;
         }
      }
   }

}


void PF_SOLVER::update_bus_angle(vector<double>& update)
{
   PF_DATA& psdb = toolkit->get_database();
   ostringstream osstream;
   NET_MATRIX& network_matrix = get_network_matrix();

   double limit = get_maximum_angle_change_in_rad();
   if (fabs(limit) < DOUBLE_EPSILON) {
      return;
   }

   double max_dv = 0.0;
   size_t max_delta_angle_bus = 0;
   size_t nv = update.size();
   for (size_t i = 0; i != nv; ++i) {
      if (max_dv < abs(update[i])) {
         max_dv = abs(update[i]);
         size_t internal_bus = internal_P_equation_buses[i];
         max_delta_angle_bus = network_matrix.get_physical_bus_number_of_internal_bus(internal_bus);
      }
   }
   osstream << "Maximum angle   change is: " << max_dv << " rad (" << rad2deg(max_dv) << " deg) at physical bus " << max_delta_angle_bus << " [" << psdb.bus_number2bus_name(max_delta_angle_bus) << "]";
   toolkit->show_information(osstream);

   if (max_dv > limit) {
      double scale = limit / max_dv;
#ifdef ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
      set_openmp_number_of_threads(toolkit->get_thread_number());
      #pragma omp parallel for schedule(static)
#endif // ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
      for (size_t i = 0; i < nv; ++i) {
         update[i] *= scale;
      }
   }

   double Perror0 = get_maximum_active_power_mismatch_in_MW();

   size_t nP = internal_P_equation_buses.size();

   double alpha = get_iteration_accelerator();

#ifdef ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
   set_openmp_number_of_threads(toolkit->get_thread_number());
   #pragma omp parallel for schedule(static)
#endif // ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
   for (size_t i = 0; i < nP; ++i) {
      size_t internal_bus = internal_P_equation_buses[i];
      //BUS* bus = psdb.get_bus(physical_bus);
      BUS* bus = internal_bus_pointers[internal_bus];

      double current_angle = bus->get_positive_sequence_angle_in_rad();

      double delta_angle = update[i];

      bus->set_positive_sequence_angle_in_rad(current_angle + alpha * delta_angle);
   }


   if (get_non_divergent_solution_logic() == true) {
      for (size_t i = 0; i < 10; ++i) {
         try_to_solve_2t_lcc_hvdc_steady_state();
         calculate_raw_bus_power_mismatch();
         double Perror = get_maximum_active_power_mismatch_in_MW();
         if (Perror > Perror0 and Perror > get_allowed_max_active_power_imbalance_in_MW()) {
            alpha *= 0.5;
#ifdef ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
            set_openmp_number_of_threads(toolkit->get_thread_number());
            #pragma omp parallel for schedule(static)
#endif // ENABLE_OPENMP_FOR_POWERFLOW_SOLVER
            for (size_t i = 0; i < nP; ++i) {
               size_t internal_bus = internal_P_equation_buses[i];
               //size_t physical_bus = network_matrix.get_physical_bus_number_of_internal_bus(internal_bus);
               //BUS* bus = psdb.get_bus(physical_bus);
               BUS* bus = internal_bus_pointers[internal_bus];

               double current_angle = bus->get_positive_sequence_angle_in_rad();

               double delta_angle = update[i];

               bus->set_positive_sequence_angle_in_rad(current_angle - alpha * delta_angle);
            }
         } else {
            break;
         }
      }
   }
}

void PF_SOLVER::show_powerflow_result()
{
   PF_DATA& psdb = toolkit->get_database();
   double sbase = psdb.get_system_base_power_in_MVA();

   ostringstream osstream;

   char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];

   if (is_converged())
      snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Powerflow converged within %zu iterations. Results are listed as follows.",
               get_iteration_count());
   else
      snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Powerflow failed to converge within %zu iterations. Results of the last iteration are listed as follows.",
               get_iteration_count());
   toolkit->show_information(buffer);

   snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Solved bus voltage and angle:");
   toolkit->show_information(buffer);

   snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "bus      voltage(pu)     angle(deg)");
   toolkit->show_information(buffer);

   NET_MATRIX& network_matrix = toolkit->get_network_matrix();

   //vector<BUS*> buses = psdb.get_all_buses();
   size_t nbus = buses.size();
   for (size_t i = 0; i != nbus; ++i) {
      size_t bus = buses[i]->get_bus_number();
      size_t inbus = network_matrix.get_internal_bus_number_of_physical_bus(bus);
      complex<double> smismatch = bus_power[inbus] * sbase;
      if (fabs(smismatch) >= 1000.0) {
         snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%8zu %12.6f %12.6f %s (mismatch: %-8.3f MW + %-8.3f MVar)",
                  buses[i]->get_bus_number(), buses[i]->get_positive_sequence_voltage_in_pu(), buses[i]->get_positive_sequence_angle_in_deg(), (buses[i]->get_bus_name()).c_str(), smismatch.real(), smismatch.imag());
         toolkit->show_information(buffer);
      }
   }
   for (size_t i = 0; i != nbus; ++i) {
      size_t bus = buses[i]->get_bus_number();
      size_t inbus = network_matrix.get_internal_bus_number_of_physical_bus(bus);
      complex<double> smismatch = bus_power[inbus] * sbase;
      if (fabs(smismatch)<1000.0 and fabs(smismatch) >= 500.0) {
         snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%8zu %12.6f %12.6f %s (mismatch: %-8.3f MW + %-8.3f MVar)",
                  buses[i]->get_bus_number(), buses[i]->get_positive_sequence_voltage_in_pu(), buses[i]->get_positive_sequence_angle_in_deg(), (buses[i]->get_bus_name()).c_str(), smismatch.real(), smismatch.imag());
         toolkit->show_information(buffer);
      }
   }
   for (size_t i = 0; i != nbus; ++i) {
      size_t bus = buses[i]->get_bus_number();
      size_t inbus = network_matrix.get_internal_bus_number_of_physical_bus(bus);
      complex<double> smismatch = bus_power[inbus] * sbase;
      if (fabs(smismatch)<500.0 and fabs(smismatch) >= 400.0) {
         snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%8zu %12.6f %12.6f %s (mismatch: %-8.3f MW + %-8.3f MVar)",
                  buses[i]->get_bus_number(), buses[i]->get_positive_sequence_voltage_in_pu(), buses[i]->get_positive_sequence_angle_in_deg(), (buses[i]->get_bus_name()).c_str(), smismatch.real(), smismatch.imag());
         toolkit->show_information(buffer);
      }
   }
   for (size_t i = 0; i != nbus; ++i) {
      size_t bus = buses[i]->get_bus_number();
      size_t inbus = network_matrix.get_internal_bus_number_of_physical_bus(bus);
      complex<double> smismatch = bus_power[inbus] * sbase;
      if (fabs(smismatch)<400.0 and fabs(smismatch) >= 300.0) {
         snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%8zu %12.6f %12.6f %s (mismatch: %-8.3f MW + %-8.3f MVar)",
                  buses[i]->get_bus_number(), buses[i]->get_positive_sequence_voltage_in_pu(), buses[i]->get_positive_sequence_angle_in_deg(), (buses[i]->get_bus_name()).c_str(), smismatch.real(), smismatch.imag());
         toolkit->show_information(buffer);
      }
   }
   for (size_t i = 0; i != nbus; ++i) {
      size_t bus = buses[i]->get_bus_number();
      size_t inbus = network_matrix.get_internal_bus_number_of_physical_bus(bus);
      complex<double> smismatch = bus_power[inbus] * sbase;
      if (fabs(smismatch)<300.0 and fabs(smismatch) >= 200.0) {
         snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%8zu %12.6f %12.6f %s (mismatch: %-8.3f MW + %-8.3f MVar)",
                  buses[i]->get_bus_number(), buses[i]->get_positive_sequence_voltage_in_pu(), buses[i]->get_positive_sequence_angle_in_deg(), (buses[i]->get_bus_name()).c_str(), smismatch.real(), smismatch.imag());
         toolkit->show_information(buffer);
      }
   }
   for (size_t i = 0; i != nbus; ++i) {
      size_t bus = buses[i]->get_bus_number();
      size_t inbus = network_matrix.get_internal_bus_number_of_physical_bus(bus);
      complex<double> smismatch = bus_power[inbus] * sbase;
      if (fabs(smismatch)<200.0 and fabs(smismatch) >= 100.0) {
         snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%8zu %12.6f %12.6f %s (mismatch: %-8.3f MW + %-8.3f MVar)",
                  buses[i]->get_bus_number(), buses[i]->get_positive_sequence_voltage_in_pu(), buses[i]->get_positive_sequence_angle_in_deg(), (buses[i]->get_bus_name()).c_str(), smismatch.real(), smismatch.imag());
         toolkit->show_information(buffer);
      }
   }
   for (size_t i = 0; i != nbus; ++i) {
      size_t bus = buses[i]->get_bus_number();
      size_t inbus = network_matrix.get_internal_bus_number_of_physical_bus(bus);
      complex<double> smismatch = bus_power[inbus] * sbase;
      if (fabs(smismatch)<100.0 and fabs(smismatch) >= 10.0) {
         snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%8zu %12.6f %12.6f %s (mismatch: %-8.3f MW + %-8.3f MVar)",
                  buses[i]->get_bus_number(), buses[i]->get_positive_sequence_voltage_in_pu(), buses[i]->get_positive_sequence_angle_in_deg(), (buses[i]->get_bus_name()).c_str(), smismatch.real(), smismatch.imag());
         toolkit->show_information(buffer);
      }
   }
   for (size_t i = 0; i != nbus; ++i) {
      size_t bus = buses[i]->get_bus_number();
      size_t inbus = network_matrix.get_internal_bus_number_of_physical_bus(bus);
      complex<double> smismatch = bus_power[inbus] * sbase;
      if (fabs(smismatch)<10.0 and fabs(smismatch) >= get_allowed_max_active_power_imbalance_in_MW()) {
         snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%8zu %12.6f %12.6f %s (mismatch: %-8.3f MW + %-8.3f MVar)",
                  buses[i]->get_bus_number(), buses[i]->get_positive_sequence_voltage_in_pu(), buses[i]->get_positive_sequence_angle_in_deg(), (buses[i]->get_bus_name()).c_str(), smismatch.real(), smismatch.imag());
         toolkit->show_information(buffer);
      }
   }
   for (size_t i = 0; i != nbus; ++i) {
      size_t bus = buses[i]->get_bus_number();
      size_t inbus = network_matrix.get_internal_bus_number_of_physical_bus(bus);
      complex<double> smismatch = bus_power[inbus] * sbase;
      if (fabs(smismatch) < get_allowed_max_active_power_imbalance_in_MW()) {
         snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%8zu %12.6f %12.6f %s",
                  buses[i]->get_bus_number(), buses[i]->get_positive_sequence_voltage_in_pu(), buses[i]->get_positive_sequence_angle_in_deg(), (buses[i]->get_bus_name()).c_str());
         toolkit->show_information(buffer);
      }
   }

   snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Solved machine power:");
   toolkit->show_information(buffer);

   snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "  bus      id       P(MW)        Q(MVar)");
   toolkit->show_information(buffer);

   //vector<SOURCE*> sources = psdb.get_all_sources();
   size_t nsource = sources.size();
   for (size_t i = 0; i != nsource; ++i) {
      BUS_TYPE btype = (sources[i]->get_bus_pointer())->get_bus_type();
      if (btype != SLACK_TYPE) {
         snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%8zu '%4s'  %12.6f  %12.6f  %-30s[%s]",
                  sources[i]->get_source_bus(), (sources[i]->get_identifier()).c_str(),
                  sources[i]->get_p_generation_in_MW(), sources[i]->get_q_generation_in_MVar(), (sources[i]->get_compound_device_name()).c_str(), psdb.bus_number2bus_name(sources[i]->get_source_bus()).c_str());
      } else {
         snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%8zu '%4s'  %12.6f  %12.6f  %-30s[%s] [slack bus]",
                  sources[i]->get_source_bus(), (sources[i]->get_identifier()).c_str(),
                  sources[i]->get_p_generation_in_MW(), sources[i]->get_q_generation_in_MVar(), (sources[i]->get_compound_device_name()).c_str(), psdb.bus_number2bus_name(sources[i]->get_source_bus()).c_str());
      }
      toolkit->show_information(buffer);
   }
}
void PF_SOLVER::save_powerflow_result(const string& filename) const
{
   PF_DATA& psdb = toolkit->get_database();

   ostringstream osstream;

   ofstream file(filename);
   if (file.is_open()) {
      time_t tt = time(NULL);
      tm* local_time = localtime(&tt);

      char time_stamp[40];
      char buffer[1000];
      snprintf(time_stamp, 40, "%d-%02d-%02d %02d:%02d:%02d", local_time->tm_year + 1900, local_time->tm_mon + 1,
               local_time->tm_mday, local_time->tm_hour, local_time->tm_min, local_time->tm_sec);

      file << "% Powerflow result exported at " << time_stamp << endl;
      snprintf(buffer, 1000, "%s", (psdb.get_case_information()).c_str());
      file << "% " << buffer << endl;
      snprintf(buffer, 1000, "%s", (psdb.get_case_additional_information()).c_str());
      file << "% " << buffer << endl;
      //vector<BUS*> buses = psdb.get_all_buses();
      size_t nbus = buses.size();
      if (nbus > 0) {
         file << "% Bus" << endl;
         file << "BUS,NAME,VBASE/KV,VOLTAGE/PU,ANGLE/DEG,VOLTAGE/KV,ANGLE/RAD" << endl;
         for (size_t i = 0; i != nbus; ++i) {
            snprintf(buffer, 1000, "%zu,\"%s\",%.6f,%.6f,%.6f,%.6f,%.6f",
                     buses[i]->get_bus_number(), (buses[i]->get_bus_name()).c_str(),
                     buses[i]->get_base_voltage_in_kV(),
                     buses[i]->get_positive_sequence_voltage_in_pu(), buses[i]->get_positive_sequence_angle_in_deg(),
                     buses[i]->get_positive_sequence_voltage_in_kV(), buses[i]->get_positive_sequence_angle_in_rad());
            file << buffer << endl;
         }
      }

      //vector<GENERATOR*> generators = psdb.get_all_generators();
      size_t ngen = generators.size();
      if (ngen > 0) {
         file << "% Generator" << endl;
         file << "BUS,ID,P/MW,Q/MVAR,VOLTAGE/PU" << endl;
         for (size_t i = 0; i != ngen; ++i) {
            snprintf(buffer, 1000, "%zu,\"%s\",%.6f,%.6f,%.6f",
                     generators[i]->get_generator_bus(), (generators[i]->get_identifier()).c_str(),
                     generators[i]->get_p_generation_in_MW(), generators[i]->get_q_generation_in_MVar(),
                     (generators[i]->get_bus_pointer())->get_positive_sequence_voltage_in_pu());
            file << buffer << endl;
         }
      }

      //vector<WT_GENERATOR*> wt_generators = psdb.get_all_wt_generators();
      size_t nsource = wt_generators.size();
      if (nsource > 0) {
         file << "% WT generator" << endl;
         file << "BUS,ID,P/MW,Q/MVAR,VOLTAGE/PU" << endl;
         for (size_t i = 0; i != nsource; ++i) {
            snprintf(buffer, 1000, "%zu,\"%s\",%.6f,%.6f,%.6f",
                     wt_generators[i]->get_generator_bus(), (wt_generators[i]->get_identifier()).c_str(),
                     wt_generators[i]->get_p_generation_in_MW(), wt_generators[i]->get_q_generation_in_MVar(),
                     (wt_generators[i]->get_bus_pointer())->get_positive_sequence_voltage_in_pu());
            file << buffer << endl;
         }
      }

      size_t npv = pv_units.size();
      if (npv > 0) {
         file << "% PV unit" << endl;
         file << "BUS,ID,P/MW,Q/MVAR,VOLTAGE/PU" << endl;
         for (size_t i = 0; i != npv; ++i) {
            snprintf(buffer, 1000, "%zu,\"%s\",%.6f,%.6f,%.6f",
                     pv_units[i]->get_unit_bus(), (pv_units[i]->get_identifier()).c_str(),
                     pv_units[i]->get_p_generation_in_MW(), pv_units[i]->get_q_generation_in_MVar(),
                     (pv_units[i]->get_bus_pointer())->get_positive_sequence_voltage_in_pu());
            file << buffer << endl;
         }
      }

      //vector<LOAD*> loads = psdb.get_all_loads();
      size_t nload = loads.size();
      if (nload > 0) {
         file << "% Load" << endl;
         file << "BUS,ID,P/MW,Q/MVAR,VOLTAGE/PU" << endl;
         for (size_t i = 0; i != nload; ++i) {
            complex<double> s = loads[i]->get_actual_total_load_in_MVA();
            snprintf(buffer, 1000, "%zu,\"%s\",%.6f,%.6f,%.6f",
                     loads[i]->get_load_bus(), (loads[i]->get_identifier()).c_str(),
                     s.real(), s.imag(),
                     (loads[i]->get_bus_pointer())->get_positive_sequence_voltage_in_pu());
            file << buffer << endl;
         }
      }

      //vector<AC_LINE*> lines = psdb.get_all_ac_lines();
      size_t nline = lines.size();
      if (nline > 0) {
         file << "% Line" << endl;
         file << "IBUS,JBUS,ID,PI/MW,QI/MVAR,PJ/MW,QJ/MVAR,II/KA,IJ/KA" << endl;
         for (size_t i = 0; i != nline; ++i) {
            complex<double> si = lines[i]->get_line_complex_power_at_sending_side_in_MVA();
            complex<double> sj = lines[i]->get_line_complex_power_at_receiving_side_in_MVA();
            snprintf(buffer, 1000, "%zu,%zu,\"%s\",%.6f,%.6f,%.6f,%.6f,%.6f,%.6f",
                     lines[i]->get_sending_side_bus(), lines[i]->get_receiving_side_bus(), (lines[i]->get_identifier()).c_str(),
                     si.real(), si.imag(), sj.real(), sj.imag(),
                     steps_fast_complex_abs(lines[i]->get_line_complex_current_at_sending_side_in_kA()),
                     steps_fast_complex_abs(lines[i]->get_line_complex_current_at_receiving_side_in_kA()));
            file << buffer << endl;
         }
      }

      //vector<TRANSFORMER*> transformers = psdb.get_all_transformers();
      size_t ntrans = transformers.size();
      if (ntrans > 0) {
         file << "% Transformer" << endl;
         file << "IBUS,JBUS,KBUS,ID,PI/MW,QI/MVAR,PJ/MW,QJ/MVAR,PK/MW,QK/MVAR,KI/PU,KJ/PU,KK/PU" << endl;
         for (size_t i = 0; i != ntrans; ++i) {
            complex<double> sp = transformers[i]->get_winding_complex_power_in_MVA(PRIMARY_SIDE);
            complex<double> ss = transformers[i]->get_winding_complex_power_in_MVA(SECONDARY_SIDE);
            complex<double> st = transformers[i]->get_winding_complex_power_in_MVA(TERTIARY_SIDE);
            snprintf(buffer, 1000, "%zu,%zu,%zu,\"%s\",%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f",
                     transformers[i]->get_winding_bus(PRIMARY_SIDE), transformers[i]->get_winding_bus(SECONDARY_SIDE),
                     transformers[i]->get_winding_bus(TERTIARY_SIDE), (transformers[i]->get_identifier()).c_str(),
                     sp.real(), sp.imag(), ss.real(), ss.imag(), st.real(), st.imag(),
                     transformers[i]->get_winding_off_nominal_turn_ratio_in_pu(PRIMARY_SIDE),
                     transformers[i]->get_winding_off_nominal_turn_ratio_in_pu(SECONDARY_SIDE),
                     transformers[i]->get_winding_off_nominal_turn_ratio_in_pu(TERTIARY_SIDE));
            file << buffer << endl;
         }
      }

      //vector<LCC_HVDC2T*> hvdcs = psdb.get_all_2t_lcc_hvdcs();
      size_t nhvdc = hvdcs.size();
      if (nhvdc > 0) {
         file << "% Hvdc" << endl;
         file << "IBUS,JBUS,ID,PR/MW,QR/MVAR,PI/MW,QI/MVAR,ALPHA/DEG,GAMMA/DEG,VDCR/KV,VDCI/KV,IDC/KA,VACR/PU,VACI/PU,KR/PU,KI/PU" << endl;
         for (size_t i = 0; i != nhvdc; ++i) {
            snprintf(buffer, 1000, "%zu,%zu,\"%s\",%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f",
                     hvdcs[i]->get_converter_bus(RECTIFIER), hvdcs[i]->get_converter_bus(INVERTER), (hvdcs[i]->get_identifier()).c_str(),
                     hvdcs[i]->get_converter_ac_active_power_in_MW(RECTIFIER),
                     hvdcs[i]->get_converter_ac_reactive_power_in_MVar(RECTIFIER),
                     hvdcs[i]->get_converter_ac_active_power_in_MW(INVERTER),
                     hvdcs[i]->get_converter_ac_reactive_power_in_MVar(INVERTER),
                     hvdcs[i]->get_converter_alpha_or_gamma_in_deg(RECTIFIER),
                     hvdcs[i]->get_converter_alpha_or_gamma_in_deg(INVERTER),
                     hvdcs[i]->get_converter_dc_voltage_in_kV(RECTIFIER),
                     hvdcs[i]->get_converter_dc_voltage_in_kV(INVERTER),
                     hvdcs[i]->get_converter_dc_current_in_kA(RECTIFIER),
                     (hvdcs[i]->get_bus_pointer(RECTIFIER))->get_positive_sequence_voltage_in_pu(),
                     (hvdcs[i]->get_bus_pointer(INVERTER))->get_positive_sequence_voltage_in_pu(),
                     hvdcs[i]->get_converter_transformer_tap_in_pu(RECTIFIER),
                     hvdcs[i]->get_converter_transformer_tap_in_pu(INVERTER));
            file << buffer << endl;
         }
      }
      size_t nvsc = vsc_hvdcs.size();
      //cout<<"nvsc: "<<nvsc<<endl;
      if (nvsc > 0) {
         file << "% VSC HVDC" << endl;
         for (size_t i = 0; i != nvsc; ++i) {
            file << "% VSC LCC_HVDC2T Project: " << vsc_hvdcs[i]->get_name() << endl;
            file << "DCBUS,DCBUSNAME,ACBUS,VDC/KV,VAC/PU,P2AC/MW,Q2AC/PW,P2DC/MW,PGEN/MW,PLOAD/MW" << endl;
            size_t n_dc_bus = vsc_hvdcs[i]->get_dc_bus_count();
            //cout<<"n_dc_bus :"<<n_dc_bus<<endl;
            for (size_t j = 0; j != n_dc_bus; ++j) {
               size_t ac_bus = vsc_hvdcs[i]->get_converter_ac_bus_number_with_dc_bus_index(j);
               double vac = 0.0;
               size_t converter_index = INDEX_NOT_EXIST;
               if (ac_bus != 0) {
                  vac = psdb.get_bus_positive_sequence_voltage_in_pu(ac_bus);
                  converter_index = vsc_hvdcs[i]->get_converter_index_with_ac_bus(ac_bus);
               }

               double p = 0.0;
               double q = 0.0;
               double pcmd = 0.0;
               if (converter_index != INDEX_NOT_EXIST) {
                  p = vsc_hvdcs[i]->get_converter_P_to_AC_bus_in_MW(converter_index);
                  q = vsc_hvdcs[i]->get_converter_Q_to_AC_bus_in_MVar(converter_index);
                  pcmd = vsc_hvdcs[i]->get_converter_dc_power_command(converter_index);
               }

               snprintf(buffer, 1000, "%zu,\"%s\",%zu,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f",
                        vsc_hvdcs[i]->get_dc_bus_number(j),
                        (vsc_hvdcs[i]->get_dc_bus_name(j)).c_str(),
                        ac_bus,
                        vsc_hvdcs[i]->get_dc_bus_Vdc_in_kV(j),
                        vac, p, q, pcmd,
                        vsc_hvdcs[i]->get_dc_bus_generation_power_in_MW(j),
                        vsc_hvdcs[i]->get_dc_bus_load_power_in_MW(j));
               file << buffer << endl;
            }

            file << "IDCBUS,JDCBUS,ID,IDC/KA,PDC_SENDING/MW,PDC_RECEIVING_MW,PLOSS_MW" << endl;
            size_t n_dc_line = vsc_hvdcs[i]->get_dc_line_count();
            //cout<<"n_dc_line :"<<n_dc_line<<endl;
            for (size_t j = 0; j != n_dc_line; ++j) {
               /*cout<<vsc_hvdcs[i]->get_dc_line_sending_side_bus(j)<<endl;
               cout<<vsc_hvdcs[i]->get_dc_line_receiving_side_bus(j)<<endl;
               cout<<vsc_hvdcs[i]->get_dc_line_current_in_kA(j,SENDING_SIDE)<<endl;
               cout<<vsc_hvdcs[i]->get_dc_line_power_in_MW(j,SENDING_SIDE)<<endl;
               cout<<vsc_hvdcs[i]->get_dc_line_power_in_MW(j,RECEIVING_SIDE)<<endl;
               cout<<vsc_hvdcs[i]->get_dc_line_loss_in_MW(j)<<endl;*/
               snprintf(buffer, 1000, "%zu,%zu,%s,%.6f,%.6f,%.6f,%.6f",
                        vsc_hvdcs[i]->get_dc_line_sending_side_bus(j),
                        vsc_hvdcs[i]->get_dc_line_receiving_side_bus(j),
                        vsc_hvdcs[i]->get_dc_line_identifier(j).c_str(),
                        vsc_hvdcs[i]->get_dc_line_current_in_kA(j, SENDING_SIDE),
                        vsc_hvdcs[i]->get_dc_line_power_in_MW(j, SENDING_SIDE),
                        vsc_hvdcs[i]->get_dc_line_power_in_MW(j, RECEIVING_SIDE),
                        vsc_hvdcs[i]->get_dc_line_loss_in_MW(j));
               file << buffer << endl;
            }
         }
      }
      file.close();
   } else {
      osstream << "File '" << filename << "' cannot be opened for saving powerflow result to file." << endl
               << "No powerflow result will be exported.";
      toolkit->show_information(osstream);
      return;
   }
}

void PF_SOLVER::save_extended_powerflow_result(const string& filename) const
{
   PF_DATA& psdb = toolkit->get_database();

   ostringstream osstream;

   ofstream file(filename);
   if (file.is_open()) {
      time_t tt = time(NULL);
      tm* local_time = localtime(&tt);

      char time_stamp[40];
      char buffer[1000];
      snprintf(time_stamp, 40, "%d-%02d-%02d %02d:%02d:%02d", local_time->tm_year + 1900, local_time->tm_mon + 1,
               local_time->tm_mday, local_time->tm_hour, local_time->tm_min, local_time->tm_sec);

      file << "% Powerflow result exported at " << time_stamp << endl;
      snprintf(buffer, 1000, "%s", (psdb.get_case_information()).c_str());
      file << "% " << buffer << endl;
      snprintf(buffer, 1000, "%s", (psdb.get_case_additional_information()).c_str());
      file << "% " << buffer << endl;
      //vector<BUS*> buses = psdb.get_all_buses();
      size_t nbus = buses.size();
      if (nbus > 0) {
         file << "% Bus" << endl;
         file << "BUS,NAME,VBASE/KV,VOLTAGE/PU,ANGLE/DEG,VOLTAGE/KV,ANGLE/RAD" << endl;
         for (size_t i = 0; i != nbus; ++i) {
            snprintf(buffer, 1000, "%zu,\"%s\",%.6f,%.6f,%.6f,%.6f,%.6f",
                     buses[i]->get_bus_number(), (buses[i]->get_bus_name()).c_str(),
                     buses[i]->get_base_voltage_in_kV(),
                     buses[i]->get_positive_sequence_voltage_in_pu(), buses[i]->get_positive_sequence_angle_in_deg(),
                     buses[i]->get_positive_sequence_voltage_in_kV(), buses[i]->get_positive_sequence_angle_in_rad());
            file << buffer << endl;
         }
      }

      //vector<GENERATOR*> generators = psdb.get_all_generators();
      size_t ngen = generators.size();
      if (ngen > 0) {
         file << "% Generator" << endl;
         file << "BUS,NAME,ID,P/MW,Q/MVAR,VOLTAGE/PU" << endl;
         for (size_t i = 0; i != ngen; ++i) {
            size_t bus = generators[i]->get_generator_bus();
            snprintf(buffer, 1000, "%zu,\"%s\",\"%s\",%.6f,%.6f,%.6f",
                     bus, psdb.bus_number2bus_name(bus).c_str(),
                     (generators[i]->get_identifier()).c_str(),
                     generators[i]->get_p_generation_in_MW(), generators[i]->get_q_generation_in_MVar(),
                     (generators[i]->get_bus_pointer())->get_positive_sequence_voltage_in_pu());
            file << buffer << endl;
         }
      }

      //vector<WT_GENERATOR*> wt_generators = psdb.get_all_wt_generators();
      size_t nsource = wt_generators.size();
      if (nsource > 0) {
         file << "% WT generator" << endl;
         file << "BUS,NAME,ID,P/MW,Q/MVAR,VOLTAGE/PU" << endl;
         for (size_t i = 0; i != nsource; ++i) {
            size_t bus = wt_generators[i]->get_source_bus();
            snprintf(buffer, 1000, "%zu,\"%s\",\"%s\",%.6f,%.6f,%.6f",
                     bus, psdb.bus_number2bus_name(bus).c_str(),
                     (wt_generators[i]->get_identifier()).c_str(),
                     wt_generators[i]->get_p_generation_in_MW(), wt_generators[i]->get_q_generation_in_MVar(),
                     (wt_generators[i]->get_bus_pointer())->get_positive_sequence_voltage_in_pu());
            file << buffer << endl;
         }
      }

      //vector<PV_UNIT*> pv_units = psdb.get_all_pv_units();
      size_t npv = pv_units.size();
      if (npv > 0) {
         file << "% PV unit" << endl;
         file << "BUS,NAME,ID,P/MW,Q/MVAR,VOLTAGE/PU" << endl;
         for (size_t i = 0; i != npv; ++i) {
            size_t bus = pv_units[i]->get_unit_bus();
            snprintf(buffer, 1000, "%zu,\"%s\",\"%s\",%.6f,%.6f,%.6f",
                     bus, psdb.bus_number2bus_name(bus).c_str(),
                     (pv_units[i]->get_identifier()).c_str(),
                     pv_units[i]->get_p_generation_in_MW(), pv_units[i]->get_q_generation_in_MVar(),
                     (pv_units[i]->get_bus_pointer())->get_positive_sequence_voltage_in_pu());
            file << buffer << endl;
         }
      }

      //vector<LOAD*> loads = psdb.get_all_loads();
      size_t nload = loads.size();
      if (nload > 0) {
         file << "% Load" << endl;
         file << "BUS,NAME,ID,P/MW,Q/MVAR,VOLTAGE/PU" << endl;
         for (size_t i = 0; i != nload; ++i) {
            size_t bus = loads[i]->get_load_bus();
            complex<double> s = loads[i]->get_actual_total_load_in_MVA();
            snprintf(buffer, 1000, "%zu,\"%s\",\"%s\",%.6f,%.6f,%.6f",
                     bus, psdb.bus_number2bus_name(bus).c_str(),
                     (loads[i]->get_identifier()).c_str(),
                     s.real(), s.imag(),
                     (loads[i]->get_bus_pointer())->get_positive_sequence_voltage_in_pu());
            file << buffer << endl;
         }
      }

      //vector<AC_LINE*> lines = psdb.get_all_ac_lines();
      size_t nline = lines.size();
      if (nline > 0) {
         file << "% Line" << endl;
         file << "IBUS,INAME,JBUS,JNAME,ID,PI/MW,QI/MVAR,PJ/MW,QJ/MVAR,II/KA,IJ/KA" << endl;
         for (size_t i = 0; i != nline; ++i) {
            size_t ibus = lines[i]->get_sending_side_bus();
            size_t jbus = lines[i]->get_receiving_side_bus();
            complex<double> si = lines[i]->get_line_complex_power_at_sending_side_in_MVA();
            complex<double> sj = lines[i]->get_line_complex_power_at_receiving_side_in_MVA();
            snprintf(buffer, 1000, "%zu,\"%s\",%zu,\"%s\",\"%s\",%.6f,%.6f,%.6f,%.6f,%.6f,%.6f",
                     ibus, psdb.bus_number2bus_name(ibus).c_str(),
                     jbus, psdb.bus_number2bus_name(jbus).c_str(),
                     (lines[i]->get_identifier()).c_str(),
                     si.real(), si.imag(), sj.real(), sj.imag(),
                     steps_fast_complex_abs(lines[i]->get_line_complex_current_at_sending_side_in_kA()),
                     steps_fast_complex_abs(lines[i]->get_line_complex_current_at_receiving_side_in_kA()));
            file << buffer << endl;
         }
      }

      //vector<TRANSFORMER*> transformers = psdb.get_all_transformers();
      size_t ntrans = transformers.size();
      if (ntrans > 0) {
         file << "% Transformer" << endl;
         file << "IBUS,INAME,JBUS,JNAME,KBUS,KNAME,ID,PI/MW,QI/MVAR,PJ/MW,QJ/MVAR,PK/MW,QK/MVAR,KI/PU,KJ/PU,KK/PU" << endl;
         for (size_t i = 0; i != ntrans; ++i) {
            size_t ibus = transformers[i]->get_winding_bus(PRIMARY_SIDE);
            size_t jbus = transformers[i]->get_winding_bus(SECONDARY_SIDE);
            size_t kbus = transformers[i]->get_winding_bus(TERTIARY_SIDE);

            complex<double> sp = transformers[i]->get_winding_complex_power_in_MVA(PRIMARY_SIDE);
            complex<double> ss = transformers[i]->get_winding_complex_power_in_MVA(SECONDARY_SIDE);
            complex<double> st = transformers[i]->get_winding_complex_power_in_MVA(TERTIARY_SIDE);
            snprintf(buffer, 1000, "%zu,\"%s\",%zu,\"%s\",%zu,\"%s\",\"%s\",%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f",
                     ibus, psdb.bus_number2bus_name(ibus).c_str(),
                     jbus, psdb.bus_number2bus_name(jbus).c_str(),
                     kbus, psdb.bus_number2bus_name(kbus).c_str(),
                     (transformers[i]->get_identifier()).c_str(),
                     sp.real(), sp.imag(), ss.real(), ss.imag(), st.real(), st.imag(),
                     transformers[i]->get_winding_off_nominal_turn_ratio_in_pu(PRIMARY_SIDE),
                     transformers[i]->get_winding_off_nominal_turn_ratio_in_pu(SECONDARY_SIDE),
                     transformers[i]->get_winding_off_nominal_turn_ratio_in_pu(TERTIARY_SIDE));
            file << buffer << endl;
         }
      }

      //vector<LCC_HVDC2T*> hvdcs = psdb.get_all_2t_lcc_hvdcs();
      size_t nhvdc = hvdcs.size();
      if (nhvdc > 0) {
         file << "% Hvdc" << endl;
         file << "IBUS,INAME,JBUS,JNAME,ID,PR/MW,QR/MVAR,PI/MW,QI/MVAR,ALPHA/DEG,GAMMA/DEG,VDCR/KV,VDCI/KV,IDC/KA,VACR/PU,VACI/PU,KR/PU,KI/PU" << endl;
         for (size_t i = 0; i != nhvdc; ++i) {
            size_t busr = hvdcs[i]->get_converter_bus(RECTIFIER);
            size_t busi = hvdcs[i]->get_converter_bus(INVERTER);
            snprintf(buffer, 1000, "%zu,\"%s\",%zu,\"%s\",\"%s\",%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f",
                     busr, psdb.bus_number2bus_name(busr).c_str(),
                     busi, psdb.bus_number2bus_name(busi).c_str(),
                     (hvdcs[i]->get_identifier()).c_str(),
                     hvdcs[i]->get_converter_ac_active_power_in_MW(RECTIFIER),
                     hvdcs[i]->get_converter_ac_reactive_power_in_MVar(RECTIFIER),
                     hvdcs[i]->get_converter_ac_active_power_in_MW(INVERTER),
                     hvdcs[i]->get_converter_ac_reactive_power_in_MVar(INVERTER),
                     hvdcs[i]->get_converter_alpha_or_gamma_in_deg(RECTIFIER),
                     hvdcs[i]->get_converter_alpha_or_gamma_in_deg(INVERTER),
                     hvdcs[i]->get_converter_dc_voltage_in_kV(RECTIFIER),
                     hvdcs[i]->get_converter_dc_voltage_in_kV(INVERTER),
                     hvdcs[i]->get_converter_dc_current_in_kA(RECTIFIER),
                     (hvdcs[i]->get_bus_pointer(RECTIFIER))->get_positive_sequence_voltage_in_pu(),
                     (hvdcs[i]->get_bus_pointer(INVERTER))->get_positive_sequence_voltage_in_pu(),
                     hvdcs[i]->get_converter_transformer_tap_in_pu(RECTIFIER),
                     hvdcs[i]->get_converter_transformer_tap_in_pu(INVERTER));
            file << buffer << endl;
         }
      }

      size_t nvsc = vsc_hvdcs.size();
      //cout<<"nvsc: "<<nvsc<<endl;
      if (nvsc > 0) {
         file << "% VSC HVDC" << endl;
         for (size_t i = 0; i != nvsc; ++i) {
            file << "% VSC LCC_HVDC2T Project: " << vsc_hvdcs[i]->get_name() << endl;
            file << "DCBUS,DCBUSNAME,ACBUS,VDC/KV,VAC/PU,P2AC/MW,Q2AC/PW,P2DC/MW,PGEN/MW,PLOAD/MW" << endl;
            size_t n_dc_bus = vsc_hvdcs[i]->get_dc_bus_count();
            //cout<<"n_dc_bus :"<<n_dc_bus<<endl;
            for (size_t j = 0; j != n_dc_bus; ++j) {
               size_t ac_bus = vsc_hvdcs[i]->get_converter_ac_bus_number_with_dc_bus_index(j);
               double vac = 0.0;
               size_t converter_index = INDEX_NOT_EXIST;
               if (ac_bus != 0) {
                  vac = psdb.get_bus_positive_sequence_voltage_in_pu(ac_bus);
                  converter_index = vsc_hvdcs[i]->get_converter_index_with_ac_bus(ac_bus);
               }

               double p = 0.0;
               double q = 0.0;
               double pcmd = 0.0;
               if (converter_index != INDEX_NOT_EXIST) {
                  p = vsc_hvdcs[i]->get_converter_P_to_AC_bus_in_MW(converter_index);
                  q = vsc_hvdcs[i]->get_converter_Q_to_AC_bus_in_MVar(converter_index);
                  pcmd = vsc_hvdcs[i]->get_converter_Pdc_command_to_dc_network_in_MW(converter_index);
               }

               snprintf(buffer, 1000, "%zu,\"%s\",%zu,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f",
                        vsc_hvdcs[i]->get_dc_bus_number(j),
                        (vsc_hvdcs[i]->get_dc_bus_name(j)).c_str(),
                        ac_bus,
                        vsc_hvdcs[i]->get_dc_bus_Vdc_in_kV(j),
                        vac, p, q, pcmd,
                        vsc_hvdcs[i]->get_dc_bus_generation_power_in_MW(j),
                        vsc_hvdcs[i]->get_dc_bus_load_power_in_MW(j));
               file << buffer << endl;
            }

            file << "IDCBUS,JDCBUS,ID,IDC/KA,PDC_SENDING/MW,PDC_RECEIVING_MW,PLOSS_MW" << endl;
            size_t n_dc_line = vsc_hvdcs[i]->get_dc_line_count();
            //cout<<"n_dc_line :"<<n_dc_line<<endl;
            for (size_t j = 0; j != n_dc_line; ++j) {
               /*cout<<vsc_hvdcs[i]->get_dc_line_sending_side_bus(j)<<endl;
               cout<<vsc_hvdcs[i]->get_dc_line_receiving_side_bus(j)<<endl;
               cout<<vsc_hvdcs[i]->get_dc_line_current_in_kA(j,SENDING_SIDE)<<endl;
               cout<<vsc_hvdcs[i]->get_dc_line_power_in_MW(j,SENDING_SIDE)<<endl;
               cout<<vsc_hvdcs[i]->get_dc_line_power_in_MW(j,RECEIVING_SIDE)<<endl;
               cout<<vsc_hvdcs[i]->get_dc_line_loss_in_MW(j)<<endl;*/
               snprintf(buffer, 1000, "%zu,%zu,%s,%.6f,%.6f,%.6f,%.6f",
                        vsc_hvdcs[i]->get_dc_line_sending_side_bus(j),
                        vsc_hvdcs[i]->get_dc_line_receiving_side_bus(j),
                        vsc_hvdcs[i]->get_dc_line_identifier(j).c_str(),
                        vsc_hvdcs[i]->get_dc_line_current_in_kA(j, SENDING_SIDE),
                        vsc_hvdcs[i]->get_dc_line_power_in_MW(j, SENDING_SIDE),
                        vsc_hvdcs[i]->get_dc_line_power_in_MW(j, RECEIVING_SIDE),
                        vsc_hvdcs[i]->get_dc_line_loss_in_MW(j));
               file << buffer << endl;
            }
         }
      }
      file.close();
   } else {
      osstream << "File '" << filename << "' cannot be opened for saving extended powerflow result to file." << endl
               << "No powerflow result will be exported.";
      toolkit->show_information(osstream);
      return;
   }
}

void PF_SOLVER::save_network_Y_matrix(const string& filename) const
{
   NET_MATRIX& network_matrix = toolkit->get_network_matrix();
   network_matrix.save_network_Y_matrix(filename);
}

void PF_SOLVER::save_jacobian_matrix(const string& filename)
{
   jacobian_builder->build_seprate_jacobians();

   jacobian_builder->save_jacobian_matrix(filename);
}

void PF_SOLVER::save_bus_powerflow_result_to_file(const string& filename) const
{
   ostringstream osstream;
   ofstream file(filename);
   if (file.is_open()) {
      file << "BUS,VOLTAGE,ANGLE" << endl;

      //vector<BUS*> buses = psdb.get_all_buses();
      size_t nbus = buses.size();
      for (size_t i = 0; i != nbus; ++i) {
         file << buses[i]->get_bus_number() << ","
              << setprecision(6) << fixed << buses[i]->get_positive_sequence_voltage_in_pu() << ","
              << setprecision(6) << fixed << buses[i]->get_positive_sequence_angle_in_deg() << endl;
      }
      file.close();
   } else {
      osstream << "File '" << filename << "' cannot be opened for saving bus powerflow result to file." << endl
               << "No bus powerflow result will be exported.";
      toolkit->show_information(osstream);
      return;
   }
}

size_t PF_SOLVER::get_iteration_count() const
{
   return iteration_count;
}

size_t PF_SOLVER::get_memory_usage()
{
   return jacobian.get_memory_usage() +
          BP.get_memory_usage() +
          BQ.get_memory_usage() +

          bus_active_power_mismatch_in_pu.capacity() * sizeof(double) +
          bus_reactive_power_mismatch_in_pu.capacity() * sizeof(double) +

          internal_P_equation_buses.capacity() * sizeof(size_t) +
          internal_Q_equation_buses.capacity() * sizeof(size_t) +

          S_mismatch.capacity() * sizeof(double) +
          P_mismatch.capacity() * sizeof(double) +
          Q_mismatch.capacity() * sizeof(double) +

          bus_current.capacity() * sizeof(complex<double>) +
          bus_power.capacity() * sizeof(complex<double>) +

          buses.capacity() * sizeof(BUS*) +
          sources.capacity() * sizeof(SOURCE*) +
          generators.capacity() * sizeof(GENERATOR*) +
          wt_generators.capacity() * sizeof(WT_GENERATOR*) +
          pv_units.capacity() * sizeof(PV_UNIT*) +
          e_storages.capacity() * sizeof(ENERGY_STORAGE*) +
          loads.capacity() * sizeof(LOAD*) +
          lines.capacity() * sizeof(AC_LINE*) +
          transformers.capacity() * sizeof(TRANSFORMER*) +
          hvdcs.capacity() * sizeof(LCC_HVDC2T*) +
          e_devices.capacity() * sizeof(EQUIVALENT_DEVICE*) +


          internal_bus_pointers.capacity() * sizeof(BUS*);
}
