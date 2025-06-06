#ifndef PSSE_IMEXPORTER_H
#define PSSE_IMEXPORTER_H

#include "header/data_imexporter/data_imexporter.h"
#include "header/device/source.h"
#include "header/device/transformer.h"
#include "header/device/lcc_hvdc2t.h"
#include <vector>

//class iSTEPS;

class PSSE_IMEXPORTER : public DATA_IMEXPORTER
{
    public:
        PSSE_IMEXPORTER(iSTEPS& toolkit);
        ~PSSE_IMEXPORTER();

        virtual bool load_powerflow_data(string file);
        virtual void load_vsc_powerflow_data(string file);
        virtual void load_powerflow_result(string file);
        virtual void load_dynamic_data(string dy_source);
        virtual void load_sequence_data(string sq_source);

        void load_one_model(string data);

        virtual void export_powerflow_data(string file, bool export_zero_impedance_line=true, POWERFLOW_DATA_SAVE_MODE save_mode = SAVE_TO_KEEP_ORIGINAL_BUS_ORDER);
        virtual void export_vsc_powerflow_data(string file);
        virtual void export_dynamic_data(string file);
        virtual void export_sequence_data(string file);

        // iSTEPS* ptoolkit;
    private:

        string trim_psse_comment(string str);

        void load_pf_data_into_ram(const std::string& rawfile);
        void load_vsc_hvdc_data_into_ram(const std::string& rawfile);
        vector<vector<vector<string> > > convert_psse_raw_data2steps_vector() const;

        vector<vector<string> > convert_i_th_type_data2steps_vector(size_t i) const;
        vector<vector<string> > convert_case_data2steps_vector() const;
        vector<vector<string> > convert_bus_data2steps_vector() const;
        vector<vector<string> > convert_load_data2steps_vector() const;
        vector<vector<string> > convert_fixed_shunt_data2steps_vector() const;
        vector<vector<string> > convert_source_data2steps_vector() const;
        vector<vector<string> > convert_ac_line_data2steps_vector() const;
        vector<vector<string> > convert_transformer_data2steps_vector() const;
        vector<vector<string> > convert_2t_lcc_hvdc_data2steps_vector() const;
        vector<vector<string> > convert_area_data2steps_vector() const;
        vector<vector<string> > convert_vsc_hvdc_data2steps_vector() const;
        vector<vector<string> > convert_vsc_hvdc_raw_data2steps_vector() const;
        vector<vector<string> > convert_transformer_inpedance_correction_table_data2steps_vector() const;
        vector<vector<string> > convert_multi_terminal_hvdc_data2steps_vector() const;
        vector<vector<string> > convert_multi_section_line_data2steps_vector() const;
        vector<vector<string> > convert_zone_data2steps_vector() const;
        vector<vector<string> > convert_interarea_transfer_data2steps_vector() const;
        vector<vector<string> > convert_owner_data2steps_vector() const;
        vector<vector<string> > convert_facts_data2steps_vector() const;
        vector<vector<string> > convert_switched_shunt_data2steps_vector() const;

        string export_case_data() const;
        string export_all_bus_data() const;
        string export_bus_data(const BUS* bus, const vector<BUS*> buses) const;
        string export_all_load_data() const;
        string export_load_data(const LOAD* load) const;
        string export_all_fixed_shunt_data() const;
        string export_fixed_shunt_data(const FIXED_SHUNT* shunt) const;
        string export_all_source_data() const;
        string export_all_generator_data() const;
        string export_generator_data(const GENERATOR* generator) const;
        string export_all_wt_generator_data() const;
        string export_wt_generator_data(const WT_GENERATOR* generator) const;
        string export_all_pv_unit_data() const;
        string export_pv_unit_data(const PV_UNIT* pvu) const;
        string export_all_energy_storage_data() const;
        string export_energy_storage_data(const ENERGY_STORAGE* estorage) const;
        string export_source_common_data(const SOURCE* source) const;
        string export_source_var_control_data(const SOURCE* source) const;
        string export_all_ac_line_data() const;
        string export_ac_line_data(const AC_LINE* line) const;
        string export_all_transformer_data() const;
        string export_transformer_data(const TRANSFORMER* transformer) const;
        string export_all_area_data() const;
        string export_area_data(const AREA* area) const;
        string export_all_2t_lcc_hvdc_data() const;
        string export_2t_lcc_hvdc_data(const LCC_HVDC2T* hvdc) const;
        string export_all_vsc_hvdc_data() const;
        //string export_vsc_hvdc_data() const;
        string export_all_transformer_impedance_correction_table_data() const;
        //string export_transformer_impedance_correction_table_data() const;
        string export_all_multi_terminal_hvdc_data() const;
        //string export_multi_terminal_hvdc_data() const;
        string export_all_multi_section_line_data() const;
        //string export_multi_section_line_data() const;
        string export_all_zone_data() const;
        string export_zone_data(const ZONE* zone) const;
        string export_all_interarea_transfer_data() const;
        //string export_interarea_transfer_data() const;
        string export_all_owner_data() const;
        string export_owner_data(const OWNER* owner) const;
        string export_all_facts_data() const;
        //string export_facts_data() const;
        string export_all_switched_shunt_data() const;
        //string export_switched_shunt_data() const;


        void load_sequence_data_into_ram(string file);
        vector<vector<vector<string> > >  convert_psse_seq_data2steps_vector() const;

        vector<vector<string> > convert_i_th_type_seq_data2steps_vector(size_t i) const;
        vector<vector<string> >  convert_change_code_data2steps_vector() const;
        vector<vector<string> >  convert_source_seq_data2steps_vector() const;
        vector<vector<string> >  convert_load_seq_data2steps_vector() const;
        vector<vector<string> >  convert_zero_seq_non_transformer_branch_data2steps_vector() const;
        vector<vector<string> >  convert_zero_seq_mutual_impedance_data2steps_vector() const;
        vector<vector<string> >  convert_zero_seq_transformer_data2steps_vector() const;
        vector<vector<string> >  convert_zero_seq_switched_shunt_data2steps_vector() const;
        vector<vector<string> >  convert_zero_seq_fixed_shunt_data2steps_vector() const;
        vector<vector<string> >  convert_induction_machine_seq_data2steps_vector() const;

        string export_change_code_data() const;
        string export_all_source_sequence_data() const;
        string export_all_generator_sequence_data() const;
        string export_generator_sequence_data(const GENERATOR* generator) const;
        string export_all_wt_generator_sequence_data() const;
        string export_wt_generator_sequence_data(const WT_GENERATOR* wt_generator) const;
        string export_all_pv_unit_sequence_data() const;
        string export_pv_unit_sequence_data(const PV_UNIT* pv_unit) const;
        string export_all_load_sequence_data() const;
        string export_load_sequence_data(const LOAD* load) const;
        string export_all_ac_line_zero_sequence_data() const;
        string export_ac_line_zero_sequence_data(const AC_LINE* line) const;
        string export_all_zero_sequence_mutual_impedance_data() const;
        string export_zero_sequence_mutual_impedance_data(const MUTUAL_DATA* mutual) const;
        string export_all_transformer_zero_sequence_data() const;
        string export_two_winding_transformer_zero_sequence_data(const TRANSFORMER* transformer) const;
        string export_three_winding_transformer_zero_sequence_data(const TRANSFORMER* transformer) const;
        string export_all_switched_shunt_zero_sequence_data() const;
        string export_all_fixed_shunt_zero_sequence_data() const;
        string export_fixed_shunt_zero_sequence_data(const FIXED_SHUNT* fixed_shunt) const;
        string export_all_induction_machine_sequence_data() const;


        void load_dynamic_data_into_ram(string file);

        void load_all_models();

        size_t data_version;
        vector< vector<string> > raw_data_in_ram;
        vector<string> dyr_data_in_ram;
        vector< vector<string> > seq_data_in_ram;
};

#endif // POWERFLOW_ASSEMBLER_H
