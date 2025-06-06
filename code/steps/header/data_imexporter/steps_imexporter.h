#ifndef STEPS_IMEXPORTER_H
#define STEPS_IMEXPORTER_H

#include "header/data_imexporter/data_imexporter.h"
#include "header/device/source.h"
#include "header/device/transformer.h"
#include "header/device/lcc_hvdc2t.h"
#include <vector>
class STEPS_IMEXPORTER : public DATA_IMEXPORTER
{
    public:
        STEPS_IMEXPORTER(iSTEPS& toolkit);
        ~STEPS_IMEXPORTER();

        virtual bool load_powerflow_data(string file);
        void load_vsc_hvdc_raw_data(vector<vector<string> > DATA);
        virtual void load_powerflow_result(string file);
        virtual void load_dynamic_data(string dy_source);
        virtual void load_sequence_data(string sq_source);
        void convter_pf_data_from_steps_vector(const vector<vector<vector<string> > >& data);
        void load_dynamic_data_from_steps_vector(vector<vector<string> >& data);
        void load_sequence_data_from_steps_vector(vector<vector<vector<string> > >& data);
        void load_dynamic_data_from_psse_string(vector<string>& data);
        void load_one_model(vector<string>& data);
        void load_one_model(string& data);

        virtual void export_powerflow_data(string file, bool export_zero_impedance_line=true, POWERFLOW_DATA_SAVE_MODE save_mode = SAVE_TO_KEEP_ORIGINAL_BUS_ORDER);
        virtual void export_dynamic_data(string file);
        virtual void export_sequence_data(string file);

        size_t get_data_version() const;

    private:
        string trim_steps_comment(string str);
        void set_data_version(size_t version);

        size_t load_pf_data_into_ram(string file);
        void convert_all_devices_data();
        void convter_case_data();
        void convter_bus_data();
        void convter_load_data();
        void convter_fixed_shunt_data();
        void convter_source_data();
        void load_generator_data(vector<string>& data);
        void load_wt_generator_data(vector<string>& data);
        void load_pv_unit_data(vector<string>& data);
        void load_energy_storage_data(vector<string>& data);
        void load_source_common_data(vector<string>& data, SOURCE* source);
        void load_source_var_control_data(vector<string>& data, SOURCE* source);
        void convter_ac_line_data();
        void convter_transformer_data();
        void add_transformer_with_data(vector<vector<string> > trans_data);
        void add_transformer_basic_data(TRANSFORMER& trans, vector<string> data);
        void add_transformer_winding_data(TRANSFORMER&trans, TRANSFORMER_WINDING_SIDE winding, vector<string> data, TRANSFORMER_WINDING_TAP_CODE winding_code);
        void add_transformer_impedance_admittance_data(TRANSFORMER& trans, vector<string> data, TRANSFORMER_IMPEDANCE_CODE impedance_code, TRANSFORMER_ADMITTANCE_CODE magnetizing_code);
        void convter_area_data();
        void convter_2t_lcc_hvdc_data();
        void add_2t_lcc_hvdc_with_data(vector<vector<string> > hvdc_data);
        void add_2t_lcc_hvdc_basic_data(LCC_HVDC2T& hvdc, vector<string> data);
        void add_2t_lcc_hvdc_converter_data(LCC_HVDC2T& hvdc, CONVERTER_SIDE converter, vector<string> data);
        void convter_lcc_hvdc_data();
        void add_lcc_hvdc_with_data(vector<vector<string> > hvdc_data);
        void add_lcc_hvdc_basic_data(LCC_HVDC& hvdc, vector<string> data);
        void add_lcc_hvdc_converter_data(LCC_HVDC& hvdc, CONVERTER_SIDE side, size_t converter_index, vector<string> data);
        void convter_vsc_hvdc_data();
        void add_vsc_hvdc_with_data(vector<vector<string> > vsc_hvdc_data);
        void add_vsc_hvdc_basic_data(VSC_HVDC& vsc_hvdc, vector<string> data);
        void add_vsc_hvdc_converter_data(VSC_HVDC& vsc_hvdc, vector<vector<string> > vsc_hvdc_converter_data);
        void add_vsc_hvdc_dc_bus_data(VSC_HVDC& vsc_hvdc, vector<vector<string> > vsc_hvdc_dc_bus_data);
        void add_vsc_hvdc_dc_line_data(VSC_HVDC& vsc_hvdc, vector<vector<string> > vsc_hvdc_dc_line_data);

        void convter_transformer_impedance_correction_table_data();
        void convter_multi_terminal_hvdc_data();
        void convter_multi_section_line_data();
        void convter_zone_data();
        void convter_interarea_transfer_data();
        void convter_owner_data();
        void convter_facts_data();
        void convter_switched_shunt_data();

        string export_case_data() const;
        string export_bus_data() const;
        string export_load_data() const;
        string export_fixed_shunt_data() const;
        string export_source_data() const;
        string export_generator_data() const;
        string export_wt_generator_data() const;
        string export_pv_unit_data() const;
        string export_energy_storage_data() const;
        string export_source_common_data(SOURCE* source) const;
        string export_source_var_control_data(SOURCE* source) const;
        string export_ac_line_data() const;
        string export_transformer_data() const;
        string export_area_data() const;
        string export_2t_lcc_hvdc_data() const;
        string export_lcc_hvdc_data() const;
        string export_vsc_hvdc_data() const;
        string export_transformer_impedance_correction_table_data() const;
        string export_multi_terminal_hvdc_data() const;
        string export_multi_section_line_data() const;
        string export_zone_data() const;
        string export_interarea_transfer_data() const;
        string export_owner_data() const;
        string export_facts_data() const;
        string export_switched_shunt_data() const;


        size_t get_starting_index_of_device_powerflow_result(const vector<string>& data, const string& device_type);
        size_t get_data_column_index_in_powerflow_result(const vector<string>& data, size_t starting_index, const string& data_name);

        vector<string> load_powerflow_result_into_ram(string file);
        void load_bus_powerflow_result(const vector<string>& data);
        void load_generator_powerflow_result(const vector<string>& data);
        void load_wt_generator_powerflow_result(const vector<string>& data);
        void load_pv_unit_powerflow_result(const vector<string>& data);
        void load_energy_storage_powerflow_result(const vector<string>& data);
        void load_transformer_powerflow_result(const vector<string>& data);
        void load_2t_lcc_hvdc_powerflow_result(const vector<string>& data);

        void load_all_seq_data_to_devices();
        void load_change_code_data();
        void load_source_seq_data();
        void load_generator_seq_data(GENERATOR& generator, vector<string>& data);
        void load_wt_generator_seq_data(WT_GENERATOR& wt_generator, vector<string>& data);
        void load_constant_speed_wtg_seq_data(WT_GENERATOR& wt_gen, vector<string>& data);
        void load_doubly_fed_wtg_seq_data(WT_GENERATOR& wt_gen, vector<string>& data);
        void load_direct_driven_wtg_seq_data(WT_GENERATOR& wt_gen, vector<string>& data);
        void load_wt_generator_LVRT_data_for_iterative_method(WT_GENERATOR& wt_generator, vector<string>& data);
        void load_pv_unit_seq_data(PV_UNIT& pv_unit, vector<string>& data);
        void load_pv_unit_seq_data_for_non_iterative_method(PV_UNIT& pv_unit, vector<string>& data);
        void load_pv_unit_LVRT_data_for_iterative_method(PV_UNIT& pv_unit, vector<string>& data);
        void load_energy_storage_seq_data(ENERGY_STORAGE& estorage, vector<string>& data);
        void load_load_seq_data();
        void load_static_load_seq_data(LOAD& load, vector<string>& data);
        void load_motor_load_seq_data(LOAD& load, vector<string>& data);
        void load_zero_seq_non_transformer_branch_data();
        void load_zero_seq_mutual_impedance_data();
        void load_zero_seq_transformer_data();
        void load_two_winding_transformer_zero_seq_data(TRANSFORMER& trans, vector<string> data);
        void load_three_winding_transformer_zero_seq_data(TRANSFORMER& trans, vector<string> data);
        void load_zero_seq_swithed_shunt_data();
        void load_zero_seq_fixed_shunt_data();
        void load_induction_machine_seq_data();
        void load_vsc_hvdc_seq_data();

        void load_dynamic_data_into_ram(string file);

        void load_all_models();
        string get_dynamic_model_name(vector<string>& data);

        void add_GENCLS_model(vector<string>& data);
        void add_GENROU_model(vector<string>& data);
        void add_GENSAL_model(vector<string>& data);

        void add_COMP_model(vector<string>& data);
        void add_IEEEVC_model(vector<string>& data);
        void add_PSASPVC_model(vector<string>& data);

        void add_IEE2ST_model(vector<string>& data);
        void add_PSASPS1_model(vector<string>& data);
        void add_PSASPS2_model(vector<string>& data);
        void add_PSASPS3_model(vector<string>& data);
        void add_PSASPS4_model(vector<string>& data);
        void add_PSASPS5_model(vector<string>& data);
        void add_PSASPS6_model(vector<string>& data);
        void add_PSASPS8_model(vector<string>& data);

        void add_SEXS_model(vector<string>& data);
        void add_IEEET1_model(vector<string>& data);
        void add_PSASPE1_model(vector<string>& data);
        void add_PSASPE2_model(vector<string>& data);
        void add_CSEET1_model(vector<string>& data);
        void add_CSEET2_model(vector<string>& data);
        void add_PSASPE13_model(vector<string>& data);
        void add_PSASPE14_model(vector<string>& data);

        void add_TGOV1_model(vector<string>& data);
        void add_IEEEG1_model(vector<string>& data);
        void add_IEEEG2_model(vector<string>& data);
        void add_IEEEG3_model(vector<string>& data);
        void add_IEESGO_model(vector<string>& data);
        void add_IEEEG1SB_model(vector<string>& data);
        void add_GAST2A_model(vector<string>& data);
        void add_URCSCT_model(vector<string>& data);

        void add_LCFB1_model(vector<string>& data);


        vector<LOAD*> get_all_loads_of(vector<string>& data);
        vector<LOAD*> remove_loads_with_different_identifier(vector<LOAD*> loads, string identifier);

        void add_IEEL_model(vector<string>& data);
        void add_CIM6_model(vector<string>& data);

        void add_UVLS_model(vector<string>& data);

        void add_UFLS_model(vector<string>& data);
        void add_PUFLS_model(vector<string>& data);

        void add_CDC4T_model(vector<string>& data);
        void add_CDC6T_model(vector<string>& data);

        void add_VSCHVDCP0_model(vector<string>& data);
        void add_VSCHVDCC0_model(vector<string>& data);
        void add_VSCHVDCC1_model(vector<string>& data);
        void add_VSCHVDCC2_model(vector<string>& data);

        void add_WT3G1_model(vector<string>& data);
        void add_WT3G0_model(vector<string>& data);
        void add_WT3G2_model(vector<string>& data);
        void add_WT3GX_model(vector<string>& data);
        void add_AERD0_model(vector<string>& data);
        void add_WT3T0_model(vector<string>& data);
        void add_WT3E0_model(vector<string>& data);
        void add_WT3E1_model(vector<string>& data);
        void add_WT3P0_model(vector<string>& data);
        void add_FILEWIND_model(vector<string>& data);
        void add_WTRLY0_model(vector<string>& data);
        void add_WTVRTRLY1_model(vector<string>& data);
        void add_WTVRT3_model(vector<string>& data);

        void add_PVGU1_model(vector<string>& data);
        void add_PVCVX_model(vector<string>& data);
        void add_PVEX_model(vector<string>& data);
        void add_FILEIRRAD_model(vector<string>& data);
        void add_PVPNLX_model(vector<string>& data);

        DEVICE_ID get_generator_device_id_from_string_vector(vector<string>& data);
        DEVICE_ID get_wt_generator_device_id_from_string_vector(vector<string>& data);
        DEVICE_ID get_pv_unit_device_id_from_string_vector(vector<string>& data);
        DEVICE_ID get_load_device_id_from_string_vector(vector<string>& data);
        DEVICE_ID get_ac_line_device_id_from_string_vector(vector<string>& data);
        DEVICE_ID get_2t_lcc_hvdc_device_id_from_string_vector(vector<string>& data);
        string get_vsc_hvdc_name_from_string_vector(vector<string>& data);
        DEVICE_ID get_vsc_hvdc_device_id_from_string_vector(vector<string>& data);
        DEVICE_ID get_transformer_device_id_from_string_vector(vector<string>& data);


        void load_source_dynamic_data();
        void load_exciter_dynamic_data();
        void load_stabilizer_dynamic_data();
        void load_turbine_governor_dynamic_data();
        void load_load_dynamic_data();
        void load_ac_line_dynamic_data();

        size_t data_version;
        vector< vector< vector<string> > > splitted_sraw_data;
        vector< vector< vector<string> > > splitted_sseq_data_in_ram;

        // [
        //   [ [a,b,c,d],[info, info]],
        //   [ [a,b,c,d], [e,f,g,h],...], // for devices in one line, each [a,b,c,d] is a record
        //   [ [a,b,c,d], [e,f,g,h],...], // for devices in multi lines, [a,b,c,d],[e,f,g,h],... are a record
        //   [ [a,b,c,d], [e,f,g,h],...],
        // ]
        vector< vector<string> > splitted_sdyr_data_in_ram;
        // [
        //   [a,b,c,d], // each line is a record
        //   [e,f,g,h],
        // ]
};

#endif // POWERFLOW_ASSEMBLER_H
