#include "header/data_imexporter/psse_imexporter.h"
#include "header/data_imexporter/steps_imexporter.h"
#include "header/basic/utility.h"
#include "header/steps_namespace.h"
#include <cstdio>
#include <istream>
#include <iostream>
#include <algorithm>
using namespace std;

void PSSE_IMEXPORTER::load_sequence_data(string sq_source)
{
    ostringstream osstream;
    osstream<<"Loading sequence data from PSS/E file: "<<sq_source;
    iSTEPS& toolkit = get_toolkit();
    toolkit.show_information(osstream);

    load_sequence_data_into_ram(sq_source);

    if(seq_data_in_ram.size()==0)
    {
        osstream<<"No data in the given PSS/E file: "<<sq_source<<endl
                <<"Please check if the file exist or not.";
        toolkit.show_information(osstream);
        return;
    }
    STEPS_IMEXPORTER steps_importer(toolkit);

    vector<vector<vector<string> > > data = convert_psse_seq_data2steps_vector();

    steps_importer.load_sequence_data_from_steps_vector(data);
    osstream<<"Done loading sequence data from sequence file.";
    toolkit.show_information(osstream);

    if(get_supplement_sequence_model_from_dynamic_model_logic()==true)
    {
        osstream<<"Check and supplement sequence model from dynamic model if sequence model is missing.";
        toolkit.show_information(osstream);

        check_and_supplement_sequence_model_from_dynamic_model();

        osstream<<"Done check and supplement sequence model from dynamic model.";
        toolkit.show_information(osstream);
    }
}

void PSSE_IMEXPORTER::load_sequence_data_into_ram(string file)
{
    ostringstream osstream;
    iSTEPS& toolkit = get_toolkit();

    seq_data_in_ram.clear();

    FILE* fid = fopen(file.c_str(),"rt");
    if(fid == NULL)
    {
        osstream<<"PSS/E seq file '"<<file<<"' is not accessible. Loading PSS/E seq data is failed.";
        toolkit.show_information(osstream);
        return;
    }

    const size_t buffer_size = 4096;
    char buffer[buffer_size];
    string sbuffer;

    vector<string> data_of_one_type;
    data_of_one_type.clear();

    if(fgets(buffer, buffer_size, fid)==NULL)
    {
        fclose(fid);
        return;
    }
    sbuffer = trim_psse_comment(buffer);
    sbuffer = trim_string(sbuffer);
    data_of_one_type.push_back(sbuffer);

    seq_data_in_ram.push_back(data_of_one_type);

    data_of_one_type.clear();

    while(true)
    {
        if(fgets(buffer, buffer_size, fid)==NULL)
        {
            if(data_of_one_type.size()!=0)
            {
                seq_data_in_ram.push_back(data_of_one_type);
                data_of_one_type.clear();
            }
            break;
        }
        sbuffer = trim_psse_comment(buffer);
        sbuffer = trim_string(sbuffer);
        if(sbuffer.size()!=0)
        {
            if(sbuffer != "0" && sbuffer!="\'0\'" && sbuffer!="\"0\"")
            {
                data_of_one_type.push_back(sbuffer);
            }
            else
            {
                seq_data_in_ram.push_back(data_of_one_type);
                data_of_one_type.clear();
            }
        }
        else
            break;
    }
    fclose(fid);
}

vector<vector<vector<string> > > PSSE_IMEXPORTER::convert_psse_seq_data2steps_vector() const
{
    vector<vector<vector<string> > > data;
    data.push_back(convert_change_code_data2steps_vector());
    data.push_back(convert_source_seq_data2steps_vector());
    data.push_back(convert_load_seq_data2steps_vector());
    data.push_back(convert_zero_seq_non_transformer_branch_data2steps_vector());
    data.push_back(convert_zero_seq_mutual_impedance_data2steps_vector());
    data.push_back(convert_zero_seq_transformer_data2steps_vector());
    data.push_back(convert_zero_seq_switched_shunt_data2steps_vector());
    data.push_back(convert_zero_seq_fixed_shunt_data2steps_vector());
    data.push_back(convert_induction_machine_seq_data2steps_vector());

    return data;
}

vector<vector<string> > PSSE_IMEXPORTER::convert_i_th_type_seq_data2steps_vector(size_t i) const
{
    vector<vector<string> > data;
    size_t n = seq_data_in_ram.size();
    if(i<n)
    {
        vector<string> DATA = seq_data_in_ram[i];
        size_t m = DATA.size();
        for(size_t j=0; j<m; ++j)
            data.push_back(split_string(DATA[j],","));
    }
    return data;
}

vector<vector<string> >  PSSE_IMEXPORTER::convert_change_code_data2steps_vector() const
{
    vector<vector<string> > data;
    data.push_back(split_string(seq_data_in_ram[0][0],","));
    return data;
}

vector<vector<string> >  PSSE_IMEXPORTER::convert_source_seq_data2steps_vector() const
{
    return convert_i_th_type_seq_data2steps_vector(1);
}

vector<vector<string> >  PSSE_IMEXPORTER::convert_load_seq_data2steps_vector() const
{
    return convert_i_th_type_seq_data2steps_vector(2);
}

vector<vector<string> >  PSSE_IMEXPORTER::convert_zero_seq_non_transformer_branch_data2steps_vector() const
{
    return convert_i_th_type_seq_data2steps_vector(3);
}

vector<vector<string> >  PSSE_IMEXPORTER::convert_zero_seq_mutual_impedance_data2steps_vector() const
{
    return convert_i_th_type_seq_data2steps_vector(4);
}

vector<vector<string> >  PSSE_IMEXPORTER::convert_zero_seq_transformer_data2steps_vector() const
{
    return convert_i_th_type_seq_data2steps_vector(5);
}

vector<vector<string> >  PSSE_IMEXPORTER::convert_zero_seq_switched_shunt_data2steps_vector() const
{
    return convert_i_th_type_seq_data2steps_vector(6);
}

vector<vector<string> >  PSSE_IMEXPORTER::convert_zero_seq_fixed_shunt_data2steps_vector() const
{
    return convert_i_th_type_seq_data2steps_vector(7);
}

vector<vector<string> >  PSSE_IMEXPORTER::convert_induction_machine_seq_data2steps_vector() const
{
    return convert_i_th_type_seq_data2steps_vector(8);
}

void PSSE_IMEXPORTER::export_sequence_data(string file)
{
    ostringstream osstream;
    iSTEPS& toolkit = get_toolkit();

    ofstream ofs(file);
    if(!ofs)
    {
        osstream<<"Warning. PSS/E seq file "<<file<<" cannot be opened for exporting sequence data.";
        toolkit.show_information(osstream);
        return;
    }

    ofs<<export_change_code_data();
    ofs<<export_all_source_sequence_data();
    ofs<<"0 / END OF MACHINE IMPEDANCE DATA, BEGIN LOAD DATA"<<endl;
    ofs<<export_all_load_sequence_data();
    ofs<<"0 / END OF LOAD DATA, BEGIN ZERO SEQ. NON-TRANSFORMER BRANCH DATA"<<endl;
    ofs<<export_all_ac_line_zero_sequence_data();
    ofs<<"0 / END OF ZERO SEQ. NON-TRANSFORMER BRANCH DATA, BEGIN ZERO SEQ. MUTUAL DATA"<<endl;
    ofs<<export_all_zero_sequence_mutual_impedance_data();
    ofs<<"0 / END OF ZERO SEQ. MUTUAL DATA, BEGIN ZERO SEQ. TRANSFORMER DATA"<<endl;
    ofs<<export_all_transformer_zero_sequence_data();
    ofs<<"0 / END OF ZERO SEQ. TRANSFORMER DATA, BEGIN ZERO SEQ. SWITCHED SHUNT DATA"<<endl;
    ofs<<export_all_switched_shunt_zero_sequence_data();
    ofs<<"0 / END OF ZERO SEQ. SWITCHED SHUNT DATA, BEGIN ZERO SEQ. FIXED SHUNT DATA"<<endl;
    ofs<<export_all_fixed_shunt_zero_sequence_data();
    ofs<<"0 / END OF ZERO SEQ. FIXED SHUNT DATA, BEGIN INDUCTION MACHINE DATA"<<endl;
    ofs<<export_all_induction_machine_sequence_data();
    ofs<<"0 / END OF INDUCTION MACHINE DATA"<<endl;
    ofs<<"Q";
}

string PSSE_IMEXPORTER::export_change_code_data() const
{
    ostringstream osstream;
    char buffer[10];

    snprintf(buffer, 10, "0, 33");
    osstream<<buffer<<endl;
    return osstream.str();
}

string PSSE_IMEXPORTER::export_all_source_sequence_data() const
{
    ostringstream osstream;
    osstream<<export_all_generator_sequence_data();
    osstream<<export_all_wt_generator_sequence_data();
    osstream<<export_all_pv_unit_sequence_data();

    return osstream.str();
}

string PSSE_IMEXPORTER::export_all_generator_sequence_data() const
{
    ostringstream osstream;
    iSTEPS& toolkit = get_toolkit();
    PF_DATA& psdb = toolkit.get_database();

    vector<GENERATOR*> generators = psdb.get_all_generators();
    size_t n = generators.size();
    for(size_t i=0; i!=n; ++i)
        osstream<<export_generator_sequence_data(generators[i]);

    return osstream.str();
}

string PSSE_IMEXPORTER::export_generator_sequence_data(const GENERATOR* generator) const
{
    ostringstream osstream;

    size_t bus = generator->get_generator_bus();
    string ickt = generator->get_identifier();
    osstream<<right
            <<setw(8)<<bus<<", "
            <<setw(6)<<("\""+ickt+"\"")<<", "
            <<setw(12)<<setprecision(6)<<fixed<<generator->get_positive_sequence_resistance_in_pu()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<generator->get_positive_sequence_subtransient_reactance_in_pu()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<generator->get_positive_sequence_transient_reactance_in_pu()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<generator->get_positive_sequence_syncronous_reactance_in_pu()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<generator->get_negative_sequence_resistance_in_pu()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<generator->get_negative_sequence_reactance_in_pu()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<generator->get_zero_sequence_resistance_in_pu()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<generator->get_zero_sequence_reactance_in_pu()<<", "
            <<setw(4)<<"1"<<", "
            <<setw(12)<<setprecision(6)<<fixed<<generator->get_grounding_resistance_in_pu()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<generator->get_grounding_reactance_in_pu()<<", "
            <<setw(4)<<"0"
            <<endl;

    return osstream.str();
}

string PSSE_IMEXPORTER::export_all_wt_generator_sequence_data() const
{
    ostringstream osstream;
    iSTEPS& toolkit = get_toolkit();
    PF_DATA& psdb = toolkit.get_database();

    vector<WT_GENERATOR*> wt_generators = psdb.get_all_wt_generators();
    size_t n = wt_generators.size();
    for(size_t i=0; i!=n; ++i)
        osstream<<export_wt_generator_sequence_data(wt_generators[i]);

    return osstream.str();
}

string PSSE_IMEXPORTER::export_wt_generator_sequence_data(const WT_GENERATOR* wt_generator) const
{
    ostringstream osstream;
    size_t bus = wt_generator->get_source_bus();
    string ickt = wt_generator->get_identifier();
    osstream<<right
            <<setw(8)<<bus<<", "
            <<setw(6)<<("\""+ickt+"\"")<<", "
            <<setw(12)<<setprecision(6)<<fixed<<wt_generator->get_positive_sequence_resistance_in_pu()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<wt_generator->get_positive_sequence_subtransient_reactance_in_pu()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<wt_generator->get_positive_sequence_transient_reactance_in_pu()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<wt_generator->get_positive_sequence_syncronous_reactance_in_pu()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<wt_generator->get_negative_sequence_resistance_in_pu()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<wt_generator->get_negative_sequence_reactance_in_pu()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<wt_generator->get_zero_sequence_resistance_in_pu()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<wt_generator->get_zero_sequence_reactance_in_pu()<<", "
            <<setw(4)<<"1"<<", "
            <<setw(12)<<setprecision(6)<<fixed<<wt_generator->get_grounding_resistance_in_pu()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<wt_generator->get_grounding_reactance_in_pu()<<", "
            <<setw(4)<<"1"
            <<endl;

    return osstream.str();
}

string PSSE_IMEXPORTER::export_all_pv_unit_sequence_data() const
{
    ostringstream osstream;
    iSTEPS& toolkit = get_toolkit();
    PF_DATA& psdb = toolkit.get_database();

    vector<PV_UNIT*> pv_units = psdb.get_all_pv_units();
    size_t n = pv_units.size();
    for(size_t i=0; i!=n; ++i)
        osstream<<export_pv_unit_sequence_data(pv_units[i]);

    return osstream.str();
}

string PSSE_IMEXPORTER::export_pv_unit_sequence_data(const PV_UNIT* pv_unit) const
{
    ostringstream osstream;
    size_t bus = pv_unit->get_source_bus();
    string ickt = pv_unit->get_identifier();
    osstream<<right
            <<setw(8) <<bus<<", "
            <<setw(6) <<("\""+ickt+"\"")<<", "
            <<setw(12)<<setprecision(6)<<fixed<<pv_unit->get_positive_sequence_resistance_in_pu()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<0.0
            <<setw(12)<<setprecision(6)<<fixed<<0.0
            <<setw(12)<<setprecision(6)<<fixed<<pv_unit->get_positive_sequence_reactance_in_pu()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<pv_unit->get_negative_sequence_resistance_in_pu()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<pv_unit->get_negative_sequence_reactance_in_pu()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<pv_unit->get_zero_sequence_resistance_in_pu()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<pv_unit->get_zero_sequence_reactance_in_pu()<<", "
            <<setw(4) <<"1"<<", "
            <<setw(12)<<setprecision(6)<<fixed<<pv_unit->get_grounding_resistance_in_pu()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<pv_unit->get_grounding_reactance_in_pu()<<", "
            <<setw(4) <<"2"
            <<endl;

    return osstream.str();
}

string PSSE_IMEXPORTER::export_all_load_sequence_data() const
{
    ostringstream osstream;
    iSTEPS& toolkit = get_toolkit();
    PF_DATA& psdb = toolkit.get_database();

    vector<LOAD*> loads = psdb.get_all_loads();
    size_t n = loads.size();
    for(size_t i=0; i!=n; ++i)
        osstream<<export_load_sequence_data(loads[i]);

    return osstream.str();
}

string PSSE_IMEXPORTER::export_load_sequence_data(const LOAD* load) const
{
    ostringstream osstream;

    size_t bus = load->get_load_bus();
    string ickt = load->get_identifier();
    osstream<<right
            <<setw(8)<<bus<<", "
            <<setw(6)<<("\""+ickt+"\"")<<", "
            <<setw(12)<<setprecision(6)<<fixed<<load->get_negative_sequence_load_in_MVA().real()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<load->get_negative_sequence_load_in_MVA().imag()<<", "
            <<setw(4)<<"1"<<", "
            <<setw(12)<<setprecision(6)<<fixed<<load->get_zero_sequence_load_in_MVA().real()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<load->get_zero_sequence_load_in_MVA().imag()
            <<endl;

    return osstream.str();
}

string PSSE_IMEXPORTER::export_all_ac_line_zero_sequence_data() const
{
    ostringstream osstream;
    iSTEPS& toolkit = get_toolkit();
    PF_DATA& psdb = toolkit.get_database();

    vector<AC_LINE*> lines = psdb.get_all_ac_lines();
    size_t n = lines.size();
    for(size_t i=0; i!=n; ++i)
        osstream<<export_ac_line_zero_sequence_data(lines[i]);

    return osstream.str();
}

string PSSE_IMEXPORTER::export_ac_line_zero_sequence_data(const AC_LINE* line) const
{
    ostringstream osstream;
    osstream<<right
            <<setw(8)<<line->get_sending_side_bus()<<", "
            <<setw(8)<<line->get_receiving_side_bus()<<", "
            <<setw(6)<<("\""+line->get_identifier()+"\"")<<", "
            <<setw(12)<<setprecision(6)<<fixed<<line->get_line_zero_sequence_z_in_pu().real()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<line->get_line_zero_sequence_z_in_pu().imag()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<line->get_line_zero_sequence_y_in_pu().imag()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<line->get_shunt_zero_sequence_y_at_sending_side_in_pu().real()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<line->get_shunt_zero_sequence_y_at_sending_side_in_pu().imag()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<line->get_shunt_zero_sequence_y_at_receiving_side_in_pu().real()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<line->get_shunt_zero_sequence_y_at_receiving_side_in_pu().imag()
            <<endl;

    return osstream.str();
}

string PSSE_IMEXPORTER::export_all_zero_sequence_mutual_impedance_data() const
{
    return "";
}

string PSSE_IMEXPORTER::export_zero_sequence_mutual_impedance_data(const MUTUAL_DATA* mutual) const
{
;
}

string PSSE_IMEXPORTER::export_all_transformer_zero_sequence_data() const
{
    ostringstream osstream;
    iSTEPS& toolkit = get_toolkit();
    PF_DATA& psdb = toolkit.get_database();

    vector<TRANSFORMER*> transformers = psdb.get_all_transformers();
    size_t n = transformers.size();
    for(size_t i=0; i!=n; ++i)
    {
        if(transformers[i]->is_two_winding_transformer())
            osstream<<export_two_winding_transformer_zero_sequence_data(transformers[i]);
        else
            osstream<<export_three_winding_transformer_zero_sequence_data(transformers[i]);
    }

    return osstream.str();
}

string PSSE_IMEXPORTER::export_two_winding_transformer_zero_sequence_data(const TRANSFORMER* transformer) const
{
    ostringstream osstream;
    complex<double> Zg1 = transformer->get_winding_zero_sequence_impedance_based_on_winding_nominals_in_pu(PRIMARY_SIDE);
    complex<double> Zg2 = transformer->get_winding_zero_sequence_impedance_based_on_winding_nominals_in_pu(SECONDARY_SIDE);

    complex<double> Z01 = transformer->get_zero_sequence_impedance_between_windings_based_on_winding_nominals_in_pu(PRIMARY_SIDE, SECONDARY_SIDE);
    complex<double> Z02 = transformer->get_zero_sequence_impedance_between_windings_based_on_winding_nominals_in_pu(SECONDARY_SIDE, TERTIARY_SIDE);
    complex<double>Znutrl = transformer->get_common_zero_sequence_nutural_grounding_impedance_based_on_winding_nominals_in_pu();

    size_t primary_cc, secondary_cc;
    primary_cc = transformer->get_winding_connection_type(PRIMARY_SIDE);
    secondary_cc = transformer->get_winding_connection_type(SECONDARY_SIDE);

    osstream<<right
            <<setw(8) <<transformer->get_winding_bus(PRIMARY_SIDE)<<", "
            <<setw(8) <<transformer->get_winding_bus(SECONDARY_SIDE)<<", "
            <<setw(8) <<0<<", "
            <<setw(6) <<("\""+transformer->get_identifier()+"\"")<<", "
            <<setw(4) <<"2"<<", "
            <<setw(4) <<"2"<<", "
            <<setw(4) <<primary_cc<<secondary_cc<<","
            <<setw(12)<<setprecision(6)<<fixed<<Zg1.real()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<Zg1.imag()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<Z01.real()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<Z01.imag()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<Zg2.real()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<Zg2.imag()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<Z02.real()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<Z02.imag()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<Znutrl.real()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<Znutrl.imag()
            <<endl;

    return osstream.str();
}

string PSSE_IMEXPORTER::export_three_winding_transformer_zero_sequence_data(const TRANSFORMER* transformer) const
{
    ostringstream osstream;
    complex<double> Zg1 = transformer->get_winding_zero_sequence_impedance_based_on_winding_nominals_in_pu(PRIMARY_SIDE);
    complex<double> Zg2 = transformer->get_winding_zero_sequence_impedance_based_on_winding_nominals_in_pu(SECONDARY_SIDE);
    complex<double> Zg3 = transformer->get_winding_zero_sequence_impedance_based_on_winding_nominals_in_pu(TERTIARY_SIDE);

    complex<double> Z01 = transformer->get_zero_sequence_impedance_between_windings_based_on_winding_nominals_in_pu(PRIMARY_SIDE, SECONDARY_SIDE);
    complex<double> Z02 = transformer->get_zero_sequence_impedance_between_windings_based_on_winding_nominals_in_pu(SECONDARY_SIDE, TERTIARY_SIDE);
    complex<double> Z03 = transformer->get_zero_sequence_impedance_between_windings_based_on_winding_nominals_in_pu(TERTIARY_SIDE, PRIMARY_SIDE);
    complex<double>Znutrl = transformer->get_common_zero_sequence_nutural_grounding_impedance_based_on_winding_nominals_in_pu();

    size_t primary_cc, secondary_cc, tertiary_cc;
    primary_cc = transformer->get_winding_connection_type(PRIMARY_SIDE);
    secondary_cc = transformer->get_winding_connection_type(SECONDARY_SIDE);
    tertiary_cc = transformer->get_winding_connection_type(TERTIARY_SIDE);

    osstream<<right
            <<setw(8) <<transformer->get_winding_bus(PRIMARY_SIDE)<<", "
            <<setw(8) <<transformer->get_winding_bus(SECONDARY_SIDE)<<", "
            <<setw(8) <<transformer->get_winding_bus(TERTIARY_SIDE)<<", "
            <<setw(6) <<("\""+transformer->get_identifier()+"\"")<<", "
            <<setw(4) <<"2"<<", "
            <<setw(4) <<"2"<<", "
            <<setw(4) <<primary_cc<<secondary_cc<<tertiary_cc<<", "
            <<setw(12)<<setprecision(6)<<fixed<<Zg1.real()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<Zg1.imag()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<Z01.real()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<Z01.imag()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<Zg2.real()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<Zg2.imag()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<Z02.real()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<Z02.imag()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<Zg3.real()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<Zg3.imag()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<Z03.real()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<Z03.imag()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<Znutrl.real()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<Znutrl.imag()
            <<endl;

    return osstream.str();
}

string PSSE_IMEXPORTER::export_all_switched_shunt_zero_sequence_data() const
{
    return "";
}

string PSSE_IMEXPORTER::export_all_fixed_shunt_zero_sequence_data() const
{
    ostringstream osstream;
    iSTEPS& toolkit = get_toolkit();
    PF_DATA& psdb = toolkit.get_database();

    vector<FIXED_SHUNT*> fixed_shunts = psdb.get_all_fixed_shunts();
    size_t n = fixed_shunts.size();
    for(size_t i=0; i!=n; ++i)
        osstream<<export_fixed_shunt_zero_sequence_data(fixed_shunts[i]);

    return osstream.str();
}

string PSSE_IMEXPORTER::export_fixed_shunt_zero_sequence_data(const FIXED_SHUNT* fixed_shunt) const
{
    ostringstream osstream;
    iSTEPS& toolkit = get_toolkit();
    PF_DATA& psdb = toolkit.get_database();
    double sbase = psdb.get_system_base_power_in_MVA();
    complex<double> Y0 = fixed_shunt->get_nominal_zero_sequence_admittance_shunt_in_pu();

    osstream<<right
            <<setw(8)<<fixed_shunt->get_shunt_bus()<<", "
            <<setw(6)<<("\""+fixed_shunt->get_identifier()+"\"")<<", "
            <<setw(12)<<setprecision(6)<<fixed<<sbase*Y0.real()<<", "
            <<setw(12)<<setprecision(6)<<fixed<<sbase*Y0.imag()<<", "
            <<endl;

    return osstream.str();
}

string PSSE_IMEXPORTER::export_all_induction_machine_sequence_data() const
{
    return "";
}
