#include "header/data_imexporter/psse_imexporter.h"
#include "header/data_imexporter/steps_imexporter.h"
#include "header/basic/utility.h"
#include "header/device/bus.h"
#include "header/device/load.h"
#include "header/device/fixed_shunt.h"
#include "header/device/generator.h"
#include "header/device/wt_generator.h"
#include "header/device/ac_line.h"
#include "header/device/transformer.h"
#include "header/steps_namespace.h"
#include <cstdio>
#include <istream>
#include <iostream>
#include <algorithm>
#include <filesystem>

using namespace std;

PSSE_IMEXPORTER::PSSE_IMEXPORTER(iSTEPS& toolkit) : DATA_IMEXPORTER(toolkit)
{
	raw_data_in_ram.clear();
	dyr_data_in_ram.clear();
	seq_data_in_ram.clear();

	ptoolkit = &toolkit;
}

PSSE_IMEXPORTER::~PSSE_IMEXPORTER()
{
	ptoolkit = nullptr;
}

bool PSSE_IMEXPORTER::load_powerflow_data(string rawfile)
{
	ostringstream stream;

	iSTEPS& toolkit = get_toolkit();

	stream<<"Begin Loading data from PSS/E raw file: "<<rawfile<<endl;
	toolkit.show_information(stream);

	if (!filesystem::exists(rawfile))
	{
		stream.str("");
		stream << "Error: Cannot open PSS/E file: " << rawfile << endl
		       << "File does not exist.\n";
		toolkit.show_information(stream);
		return false;
	}

	load_pf_data_into_ram(rawfile);
	//size_t p=raw_data_in_ram.size();
	//size_t q=raw_data_in_ram[0].size();
	if(raw_data_in_ram.size()==0)
	{
		stream.str("");
		stream<<"No data in the given PSS/E file: "<< rawfile <<endl
		      <<"Please check if the file exist or not.\n";
		toolkit.show_information(stream);
		return false;
	}

	vector<vector<vector<string> > > data = convert_psse_raw_data2steps_vector();

	STEPS_IMEXPORTER steps_importer(toolkit);
	steps_importer.convter_pf_data_from_steps_vector(data);  //from data(psse_imexp) to splitted_sraw_data(steps_imexp)

	stream<<"Done Loading data from PSS/E raw file: "<<rawfile<<endl<<endl;
	toolkit.show_information(stream);

	PF_DATA& psdb = toolkit.get_database();
	vector<size_t> buses = psdb.get_all_buses_number();
	size_t n = buses.size();
	size_t max_bus_number = 0;
	for(size_t i=0; i<n; ++i)
		if(buses[i]>max_bus_number) max_bus_number = buses[i];

	stream<<"Max Bus Number is "<<max_bus_number<<" of "<<psdb.bus_number2bus_name(max_bus_number);
	toolkit.show_information(stream);

	stream<<"There are totally:\n"
	      <<psdb.get_bus_count()<<" buses\n"
	      <<psdb.get_generator_count()<<" generators\n"
	      <<psdb.get_wt_generator_count()<<" WT generators\n"
	      <<psdb.get_pv_unit_count()<<" PV units\n"
	      <<psdb.get_energy_storage_count()<<" energy storages\n"
	      <<psdb.get_ac_line_count()<<" lines\n"
	      <<psdb.get_transformer_count()<<" transformers\n"
	      <<psdb.get_fixed_shunt_count()<<" fixed shunts\n"
	      <<psdb.get_2t_lcc_hvdc_count()<<" 2T LCC HVDCs\n"
	      <<psdb.get_vsc_hvdc_count()<<" VSC HVDCs\n"
	      <<psdb.get_load_count()<<" loads\n"
	      <<psdb.get_area_count()<<" areas\n"
	      <<psdb.get_zone_count()<<" zones\n"
	      <<psdb.get_owner_count()<<" owners";

	toolkit.show_information(stream);

	return true;
}
//
//void PSSE_IMEXPORTER::load_powerflow_data(string file)
//{
//    ostringstream osstream;
//    osstream<<"Loading powerflow data from PSS/E file: "<<file;
//    iSTEPS& toolkit = get_toolkit();
//    toolkit.show_information(osstream);
//
//    load_pf_data_into_ram(file);
//    //size_t p=raw_data_in_ram.size();
//    //size_t q=raw_data_in_ram[0].size();
//    if(raw_data_in_ram.size()==0)
//    {
//        osstream<<"No data in the given PSS/E file: "<<file<<endl
//          <<"Please check if the file exist or not.";
//        toolkit.show_information(osstream);
//        return;
//    }
//    STEPS_IMEXPORTER steps_importer(toolkit);
//
//    vector<vector<vector<string> > > data = convert_psse_raw_data2steps_vector();
//
//    steps_importer.convter_pf_data_from_steps_vector(data);
//
//    osstream<<"Done loading powerflow data.";
//    toolkit.show_information(osstream);
//
//    PF_DATA& psdb = toolkit.get_database();
//    vector<size_t> buses = psdb.get_all_buses_number();
//    size_t n = buses.size();
//    size_t max_bus_number = 0;
//    for(size_t i=0; i<n; ++i)
//        if(buses[i]>max_bus_number) max_bus_number = buses[i];
//
//    osstream<<"System maximum bus number is "<<max_bus_number<<" of "<<psdb.bus_number2bus_name(max_bus_number);
//    toolkit.show_information(osstream);
//
//    osstream<<"There are totally:\n"
//            <<psdb.get_bus_count()<<" buses\n"
//            <<psdb.get_generator_count()<<" generators\n"
//            <<psdb.get_wt_generator_count()<<" WT generators\n"
//            <<psdb.get_pv_unit_count()<<" PV units\n"
//            <<psdb.get_energy_storage_count()<<" energy storages\n"
//            <<psdb.get_ac_line_count()<<" lines\n"
//            <<psdb.get_transformer_count()<<" transformers\n"
//            <<psdb.get_fixed_shunt_count()<<" fixed shunts\n"
//            <<psdb.get_2t_lcc_hvdc_count()<<" 2T LCC HVDCs\n"
//            <<psdb.get_vsc_hvdc_count()<<" VSC HVDCs\n"
//            <<psdb.get_load_count()<<" loads\n"
//            <<psdb.get_area_count()<<" areas\n"
//            <<psdb.get_zone_count()<<" zones\n"
//            <<psdb.get_owner_count()<<" owners";
//    toolkit.show_information(osstream);
//}

void PSSE_IMEXPORTER::load_vsc_powerflow_data(string file)
{
	ostringstream osstream;
	osstream<<"Loading vsc powerflow data from vscraw file: "<<file;
	iSTEPS& toolkit = get_toolkit();
	toolkit.show_information(osstream);

	load_vsc_hvdc_data_into_ram(file);

	if(raw_data_in_ram.size()==0)
	{
		osstream<<"No data in the given vscraw file: "<<file<<endl
		        <<"Please check if the file exist or not.";
		toolkit.show_information(osstream);
		return;
	}

	STEPS_IMEXPORTER steps_importer(toolkit);

	vector<vector<string> > data = convert_vsc_hvdc_raw_data2steps_vector();

	steps_importer.load_vsc_hvdc_raw_data(data);

	osstream<<"Done loading powerflow vscraw data.";
	toolkit.show_information(osstream);

	PF_DATA& psdb = toolkit.get_database();
	vector<size_t> buses = psdb.get_all_buses_number();
	size_t n = buses.size();
	size_t max_bus_number = 0;
	for(size_t i=0; i<n; ++i)
		if(buses[i]>max_bus_number) max_bus_number = buses[i];

	osstream<<"System maximum bus number is "<<max_bus_number<<" of "<<psdb.bus_number2bus_name(max_bus_number);
	toolkit.show_information(osstream);

	osstream<<"There are totally:\n"
	        <<psdb.get_bus_count()<<" buses\n"
	        <<psdb.get_generator_count()<<" generators\n"
	        <<psdb.get_wt_generator_count()<<" WT generators\n"
	        <<psdb.get_pv_unit_count()<<" PV units\n"
	        <<psdb.get_energy_storage_count()<<" energy storages\n"
	        <<psdb.get_ac_line_count()<<" lines\n"
	        <<psdb.get_transformer_count()<<" transformers\n"
	        <<psdb.get_fixed_shunt_count()<<" fixed shunts\n"
	        <<psdb.get_2t_lcc_hvdc_count()<<" 2T LCC HVDCs\n"
	        <<psdb.get_vsc_hvdc_count()<<" VSC HVDCs\n"
	        <<psdb.get_load_count()<<" loads\n"
	        <<psdb.get_area_count()<<" areas\n"
	        <<psdb.get_zone_count()<<" zones\n"
	        <<psdb.get_owner_count()<<" owners";
	toolkit.show_information(osstream);
}


void PSSE_IMEXPORTER::load_powerflow_result(string file)
{
	return;
}

void PSSE_IMEXPORTER::load_pf_data_into_ram(const string& rawfile)
{
	ostringstream osstream;
	iSTEPS& toolkit = get_toolkit();

	vector<string> data_of_one_type;
	raw_data_in_ram.clear();
	data_of_one_type.clear();

	FILE* fid = fopen(rawfile.c_str(),"rt");
	if(fid == NULL)
	{
		osstream<<"PSS/E raw file '"<<rawfile<<"' is not accessible. \n Loading PSS/E raw data is failed.";
		toolkit.show_information(osstream);
		return;
	}

	const size_t buffer_size = 4096;
	char buffer[buffer_size];
	string sbuffer;

	if(fgets(buffer, buffer_size, fid)==NULL)
	{
		fclose(fid);
		return;
	}

	sbuffer = trim_psse_comment(buffer);
	sbuffer = trim_string(sbuffer);
	data_of_one_type.push_back(sbuffer);

	raw_data_in_ram.push_back(data_of_one_type);

	data_of_one_type.clear();

	for(size_t i=0; i!=2; ++i)
	{
		if(fgets(buffer, buffer_size, fid)==NULL)
		{
			fclose(fid);
			return;
		}
		sbuffer = trim_psse_comment(buffer);
		sbuffer = trim_string(sbuffer);
		data_of_one_type.push_back(sbuffer);
	}
	raw_data_in_ram.push_back(data_of_one_type);

	data_of_one_type.clear();

	while(true)
	{
		if(fgets(buffer, buffer_size, fid)==NULL)
		{
			if(data_of_one_type.size()!=0)
			{
				raw_data_in_ram.push_back(data_of_one_type);
				data_of_one_type.clear();
			}
			break;
		}
		sbuffer = trim_psse_comment(buffer);
		sbuffer = trim_string(sbuffer);
		if(sbuffer.size()!=0)
		{
			if(sbuffer != "0" and sbuffer!="\'0\'" and sbuffer!="\"0\"")
			{
				data_of_one_type.push_back(sbuffer);
			}
			else
			{
				raw_data_in_ram.push_back(data_of_one_type);
				data_of_one_type.clear();
			}
		}
		else
			break;
	}
	fclose(fid);
}

//void PSSE_IMEXPORTER::load_pf_data_into_ram(const string& rawfile)
//{
//    iSTEPS& toolkit = get_toolkit();
//
//    // 预先清空并预留容量，避免多次重新分配内存
//    raw_data_in_ram.clear();
//    raw_data_in_ram.reserve(20);
//
//    ifstream file(rawfile);
//    if (!file.is_open())
//    {
//        ostringstream osstream;
//        osstream << "PSS/E raw file '" << rawfile << "' is not accessible.\nLoading PSS/E raw data is failed.";
//        toolkit.show_information(osstream);
//        return;
//    }
//
//    string line;
//
//    // 处理第一行数据（标题行）; 文件为空，直接返回
//    if (!getline(file, line))
//        return;
//    line = trim_string(trim_psse_comment(line));
//
//    vector<string> data_of_one_type;
//    data_of_one_type.reserve(10); // 预估每个数据块的行数
//    data_of_one_type.push_back(move(line));
//    raw_data_in_ram.push_back(move(data_of_one_type));
//
//    // 处理第二和第三行（通常是版本信息和基础参数）
//    data_of_one_type.clear();
//    data_of_one_type.reserve(2);
//
//    for (size_t i = 0; i < 2; ++i)
//    {
//        if (!getline(file, line))
//        {
//            raw_data_in_ram.push_back(move(data_of_one_type));
//            return; // 文件提前结束
//        }
//
//        line = trim_string(trim_psse_comment(line));
//        data_of_one_type.push_back(move(line));
//    }
//
//    raw_data_in_ram.push_back(move(data_of_one_type));
//
//    // 处理剩余数据块
//    data_of_one_type.clear();
//    data_of_one_type.reserve(10); // 预估每个数据类型的条目数
//
//    while (getline(file, line))
//    {
//        line = trim_string(trim_psse_comment(line));
//
//        if (line.empty())
//            break; // 遇到空行结束
//
//        // 检查是否为分隔符（"0"、'0'或"0"）
//        if (line != "0" && line != "\'0\'" && line != "\"0\"")
//        {
//            data_of_one_type.push_back(move(line));
//        }
//        else
//        {
//            // 遇到分隔符，保存当前数据块并开始新的数据块
//            if (!data_of_one_type.empty())
//            {
//                raw_data_in_ram.push_back(move(data_of_one_type));
//                data_of_one_type.clear();
//                data_of_one_type.reserve(10); // 为新数据块预留空间
//            }
//        }
//    }
//
//    // 确保最后一个数据块被保存
//    if (!data_of_one_type.empty())
//    {
//        raw_data_in_ram.push_back(move(data_of_one_type));
//    }
//
//    // 文件在 ifstream 析构时自动关闭，无需手动调用 close()
//}

void PSSE_IMEXPORTER::load_vsc_hvdc_data_into_ram(const string& file)
{
	ostringstream osstream;
	iSTEPS& toolkit = get_toolkit();

	raw_data_in_ram.clear();

	FILE* fid = fopen(file.c_str(),"rt");
	if(fid == NULL)
	{
		osstream<<"PSS/E VSC-HVDC raw file '"<<file<<"' is not accessible. Loading PSS/E raw data is failed.";
		toolkit.show_information(osstream);
		return;
	}

	const size_t buffer_size = 4096;
	char buffer[buffer_size];
	string sbuffer;

	vector<string> data_of_one_type;
	data_of_one_type.clear();

	while(true)
	{
		if(fgets(buffer, buffer_size, fid)==NULL)
		{
			if(data_of_one_type.size()!=0)
			{
				raw_data_in_ram.push_back(data_of_one_type);
				data_of_one_type.clear();
			}
			break;
		}
		sbuffer = trim_psse_comment(buffer);
		sbuffer = trim_string(sbuffer);
		if(sbuffer.size()!=0)
			data_of_one_type.push_back(sbuffer);
		else
			break;
	}
	fclose(fid);
}

string PSSE_IMEXPORTER::trim_psse_comment(string str)
{
	if(str.size()==0)
		return str;

	size_t found = str.find_first_of('/');
	if(found!=string::npos) str.erase(found, str.size());
	return str;
}

vector<vector<vector<string> > >  PSSE_IMEXPORTER::convert_psse_raw_data2steps_vector() const
{
	vector<vector<vector<string> > > data;
	data.reserve(18); // 预先分配空间，避免多次重新分配

	data.push_back(move(convert_case_data2steps_vector()));
	data.push_back(move(convert_bus_data2steps_vector()));
	data.push_back(move(convert_load_data2steps_vector()));
	data.push_back(move(convert_fixed_shunt_data2steps_vector()));
	data.push_back(move(convert_source_data2steps_vector()));
	data.push_back(move(convert_ac_line_data2steps_vector()));
	data.push_back(move(convert_transformer_data2steps_vector()));
	data.push_back(move(convert_2t_lcc_hvdc_data2steps_vector()));
	data.push_back(move(convert_area_data2steps_vector()));
	data.push_back(move(convert_vsc_hvdc_data2steps_vector()));
	data.push_back(move(convert_transformer_inpedance_correction_table_data2steps_vector()));
	data.push_back(move(convert_multi_terminal_hvdc_data2steps_vector()));
	data.push_back(move(convert_multi_section_line_data2steps_vector()));
	data.push_back(move(convert_zone_data2steps_vector()));
	data.push_back(move(convert_interarea_transfer_data2steps_vector()));
	data.push_back(move(convert_owner_data2steps_vector()));
	data.push_back(move(convert_facts_data2steps_vector()));
	data.push_back(move(convert_switched_shunt_data2steps_vector()));

	return data;
}

vector<vector<string> > PSSE_IMEXPORTER::convert_i_th_type_data2steps_vector(size_t i) const
{
	vector<vector<string> > data;
	size_t n = raw_data_in_ram.size();
	if(i<n)
	{
		vector<string> DATA = raw_data_in_ram[i];
		size_t m = DATA.size();
		for(size_t j=0; j<m; ++j)
			data.push_back(split_string(DATA[j],","));
	}
	return data;
}

vector<vector<string> > PSSE_IMEXPORTER::convert_case_data2steps_vector() const
{
	vector<vector<string> > data;
	data.push_back(split_string(raw_data_in_ram[0][0],","));//case line
	data.push_back(raw_data_in_ram[1]);// two lines
	return data;
}
vector<vector<string> > PSSE_IMEXPORTER::convert_bus_data2steps_vector() const
{
	return convert_i_th_type_data2steps_vector(2);
}
vector<vector<string> > PSSE_IMEXPORTER::convert_load_data2steps_vector() const
{
	return convert_i_th_type_data2steps_vector(3);
}
vector<vector<string> > PSSE_IMEXPORTER::convert_fixed_shunt_data2steps_vector() const
{
	return convert_i_th_type_data2steps_vector(4);
}

vector<vector<string> > PSSE_IMEXPORTER::convert_source_data2steps_vector() const
{
	vector<vector<string> > source_lines = convert_i_th_type_data2steps_vector(5);
	vector<string> source_line;
	size_t n = source_lines.size();
	for(size_t i=0; i!=n; ++i)
	{
		source_line =  source_lines[i];
		size_t nz = source_line.size();
		if(nz>=29)
			continue;
		else
		{
			if(nz>=28)
			{
				int source_type = get_integer_data(source_line[26],"0");
				if(source_type==0)
					source_line.push_back("0"); // synchronous
				else
				{
					source_line.push_back("1"); // wt generator
				}
				source_lines[i] = source_line;
			}
		}
	}
	return source_lines;
}

vector<vector<string> > PSSE_IMEXPORTER::convert_ac_line_data2steps_vector() const
{
	return convert_i_th_type_data2steps_vector(6);
}
vector<vector<string> > PSSE_IMEXPORTER::convert_transformer_data2steps_vector() const
{
	return convert_i_th_type_data2steps_vector(7);
}
vector<vector<string> > PSSE_IMEXPORTER::convert_2t_lcc_hvdc_data2steps_vector() const
{
	return convert_i_th_type_data2steps_vector(8);
}
vector<vector<string> > PSSE_IMEXPORTER::convert_area_data2steps_vector() const
{
	return convert_i_th_type_data2steps_vector(9);
}
vector<vector<string> > PSSE_IMEXPORTER::convert_vsc_hvdc_data2steps_vector() const
{
	return convert_i_th_type_data2steps_vector(10);
}

vector<vector<string> > PSSE_IMEXPORTER::convert_vsc_hvdc_raw_data2steps_vector() const
{
	vector<vector<string> > data;
	vector<string> raw_data = raw_data_in_ram[0];
	size_t n = raw_data.size();
	for(size_t i=0; i<n; ++i)
	{
		string DATA = raw_data[i];
		data.push_back(split_string(DATA,","));
	}
	return data;
}
vector<vector<string> > PSSE_IMEXPORTER::convert_transformer_inpedance_correction_table_data2steps_vector() const
{
	return convert_i_th_type_data2steps_vector(11);
}
vector<vector<string> > PSSE_IMEXPORTER::convert_multi_terminal_hvdc_data2steps_vector() const
{
	return convert_i_th_type_data2steps_vector(12);
}
vector<vector<string> > PSSE_IMEXPORTER::convert_multi_section_line_data2steps_vector() const
{
	return convert_i_th_type_data2steps_vector(13);
}
vector<vector<string> > PSSE_IMEXPORTER::convert_zone_data2steps_vector() const
{
	return convert_i_th_type_data2steps_vector(14);
}
vector<vector<string> > PSSE_IMEXPORTER::convert_interarea_transfer_data2steps_vector() const
{
	return convert_i_th_type_data2steps_vector(15);
}
vector<vector<string> > PSSE_IMEXPORTER::convert_owner_data2steps_vector() const
{
	return convert_i_th_type_data2steps_vector(16);
}
vector<vector<string> > PSSE_IMEXPORTER::convert_facts_data2steps_vector() const
{
	return convert_i_th_type_data2steps_vector(17);
}
vector<vector<string> > PSSE_IMEXPORTER::convert_switched_shunt_data2steps_vector() const
{
	return convert_i_th_type_data2steps_vector(18);
}

void PSSE_IMEXPORTER::export_powerflow_data(string file, bool export_zero_impedance_line, POWERFLOW_DATA_SAVE_MODE save_mode)
{
	ostringstream osstream;
	iSTEPS& toolkit = get_toolkit();
	PF_DATA& psdb = toolkit.get_database();

	ofstream ofs(file);
	if(!ofs)
	{
		osstream<<"Warning. PSS/E raw file "<<file<<" cannot be opened for exporting powerflow data.";
		toolkit.show_information(osstream);
		return;
	}

	set_export_zero_impedance_line_logic(export_zero_impedance_line);
	set_powerflow_data_save_mode(save_mode);

	setup_ordered_buses_with_powerflow_data_save_mode(save_mode);

	if(export_zero_impedance_line==false)
		psdb.update_overshadowed_bus_count();
	else
		psdb.set_all_buses_un_overshadowed();

	ofs<<export_case_data();
	ofs<<export_all_bus_data();
	ofs<<"0 / END OF BUS DATA, BEGIN LOAD DATA"<<endl;
	ofs<<export_all_load_data();
	ofs<<"0 / END OF LOAD DATA, BEGIN FIXED SHUNT DATA"<<endl;
	ofs<<export_all_fixed_shunt_data();
	ofs<<"0 / END OF FIXED SHUNT DATA, BEGIN GENERATOR DATA"<<endl;
	ofs<<export_all_source_data();
	ofs<<"0 / END OF GENERATOR DATA, BEGIN TRANSMISSION LINE DATA"<<endl;
	ofs<<export_all_ac_line_data();
	ofs<<"0 / END OF TRANSMISSION LINE DATA, BEGIN TRANSFORMER DATA"<<endl;
	ofs<<export_all_transformer_data();
	ofs<<"0 / END OF TRANSFORMER DATA, BEGIN AREA DATA"<<endl;
	ofs<<export_all_area_data();
	ofs<<"0 / END OF AREA DATA, BEGIN TWO-TERMINAL LCC_HVDC2T DATA"<<endl;
	ofs<<export_all_2t_lcc_hvdc_data();
	ofs<<"'0' / END OF TWO-TERMINAL LCC_HVDC2T DATA, BEGIN VSC LCC_HVDC2T LINE DATA"<<endl;
	ofs<<export_all_vsc_hvdc_data();
	ofs<<"'0' / END OF VSC LCC_HVDC2T LINE DATA, BEGIN IMPEDANCE CORRECTION DATA"<<endl;
	ofs<<export_all_transformer_impedance_correction_table_data();
	ofs<<"0 / END OF IMPEDANCE CORRECTION DATA, BEGIN MULTI-TERMINAL LCC_HVDC2T DATA"<<endl;
	ofs<<export_all_multi_terminal_hvdc_data();
	ofs<<"0 / END OF MULTI-TERMINAL LCC_HVDC2T DATA, BEGIN MULTI-SECTION LINE DATA"<<endl;
	ofs<<export_all_multi_section_line_data();
	ofs<<"0 / END OF MULTI-SECTION LINE DATA, BEGIN ZONE DATA"<<endl;
	ofs<<export_all_zone_data();
	ofs<<"0 / END OF ZONE DATA, BEGIN INTER-AREA TRANSFER DATA"<<endl;
	ofs<<export_all_interarea_transfer_data();
	ofs<<"0 / END OF INTER-AREA TRANSFER DATA, BEGIN OWNER DATA"<<endl;
	ofs<<export_all_owner_data();
	ofs<<"0 / END OF OWNER DATA, BEGIN FACTS DEVICE DATA"<<endl;
	ofs<<export_all_facts_data();
	ofs<<"0 / END OF FACTS DEVICE DATA, BEGIN SWITCHED SHUNT DATA"<<endl;
	ofs<<export_all_switched_shunt_data();
	ofs<<"0 / END OF SWITCHED SHUNT DATA, BEGIN GNE DATA"<<endl;
	ofs<<"0 / END OF GNE DATA, BEGIN INDUCTION MACHINE DATA"<<endl;
	ofs<<"0 / END OF INDUCTION MACHINE DATA"<<endl;
	ofs<<"Q";

	ofs.close();

	if(export_zero_impedance_line==false)
	{
		export_shadowed_bus_pair("equiv_bus_pair.txt");
		psdb.set_all_buses_un_overshadowed();//recover
	}
}

void PSSE_IMEXPORTER::export_vsc_powerflow_data(string file)
{
	ostringstream osstream;
	iSTEPS& toolkit = get_toolkit();
	//PF_DATA& psdb = toolkit.get_database();

	ofstream ofs(file);
	if(!ofs)
	{
		osstream<<"Warning. PSS/E raw file "<<file<<" cannot be opened for exporting powerflow data.";
		toolkit.show_information(osstream);
		return;
	}

	ofs<<export_all_vsc_hvdc_data();

	ofs.close();
}

string PSSE_IMEXPORTER::export_case_data() const
{
	ostringstream osstream;
	iSTEPS& toolkit = get_toolkit();
	PF_DATA& psdb = toolkit.get_database();

	char buffer[1000];
	vector<size_t> buses = psdb.get_all_buses_number();
	double fbase = 50.0;
	if(buses.size()!=0)
		fbase = psdb.get_bus_base_frequency_in_Hz(buses[0]);
	snprintf(buffer, 1000, "0, %.2f, 33, 0, 0, %f", psdb.get_system_base_power_in_MVA(), fbase);
	osstream<<buffer<<endl;
	snprintf(buffer, 1000, "%s", (psdb.get_case_information()).c_str());
	osstream<<buffer<<endl;
	snprintf(buffer, 1000, "%s", (psdb.get_case_additional_information()).c_str());
	osstream<<buffer<<endl;
	return osstream.str();
}

string PSSE_IMEXPORTER::export_all_bus_data() const
{
	ostringstream osstream;
	iSTEPS& toolkit = get_toolkit();
	PF_DATA& psdb = toolkit.get_database();

	const vector<size_t>& ordered_buses = get_ordered_buses();
	size_t n = ordered_buses.size();
	vector<BUS*> buses;
	buses.reserve(n);

	for(size_t i=0; i!=n; ++i)
	{
		BUS* bus = psdb.get_bus(ordered_buses[i]);
		buses.push_back(bus);
	}

	for(size_t i=0; i!=n; ++i)
		osstream<<export_bus_data(buses[i], buses);

	return osstream.str();
}

string PSSE_IMEXPORTER::export_bus_data(const BUS* bus, const vector<BUS*> buses) const
{
	ostringstream osstream;
	iSTEPS& toolkit = get_toolkit();
	PF_DATA& psdb = toolkit.get_database();

	if(get_export_zero_impedance_line_logic()==false and psdb.get_equivalent_bus_of_bus(bus->get_bus_number())!=0)
		return "";

	BUS_TYPE bus_type = bus->get_bus_type();
	if(get_export_out_of_service_bus_logic()==false and bus_type==OUT_OF_SERVICE)
		return "";

	size_t n = buses.size();

	int type = 4;
	if(bus_type == PQ_TYPE) type = 1;
	if(bus_type == PV_TYPE) type = 2;
	if(bus_type == PV_TO_PQ_TYPE_1 or bus_type == PV_TO_PQ_TYPE_2 or
	      bus_type == PV_TO_PQ_TYPE_3 or bus_type == PV_TO_PQ_TYPE_4) type = -2;
	if(bus_type == SLACK_TYPE) type = 3;
	if(bus_type == OUT_OF_SERVICE) type = 4;

	if(get_export_zero_impedance_line_logic()==false)
	{
		size_t bus_number = bus->get_bus_number();
		for(size_t j=0; j!=n; ++j)
		{
			BUS* tbus = buses[j];
			if(tbus->get_equivalent_bus_number()==bus_number)
			{
				BUS_TYPE btype = tbus->get_bus_type();
				if(btype == SLACK_TYPE)
				{
					type = 3;
					break;
				}
				else
				{
					if((btype == PV_TYPE  or
					      btype == PV_TO_PQ_TYPE_1 or btype == PV_TO_PQ_TYPE_2 or
					      btype == PV_TO_PQ_TYPE_3 or btype == PV_TO_PQ_TYPE_4) and type==1)
						type = 2;
				}
			}
		}
	}

	NET_MATRIX& network = toolkit.get_network_matrix();
	size_t bus_number = bus->get_bus_number();
	if(get_export_internal_bus_number_logic()==true)
		bus_number = network.get_internal_bus_number_of_physical_bus(bus_number)+1;

	osstream<<right
	        <<setw(8)<<bus_number<<", \""
	        <<left
	        <<setw(20)<<bus->get_bus_name()<<"\", "
	        <<right
	        <<setw(8)<<setprecision(2)<<fixed<<bus->get_base_voltage_in_kV()<<", "
	        <<setw(2)<<type<<", "
	        <<setw(4)<<bus->get_area_number()<<", "
	        <<setw(4)<<bus->get_zone_number()<<", "
	        <<setw(4)<<bus->get_owner_number()<<", "
	        <<setw(10)<<setprecision(6)<<fixed<<bus->get_positive_sequence_voltage_in_pu()<<", "
	        <<setw(10)<<setprecision(6)<<fixed<<bus->get_positive_sequence_angle_in_deg()<<", "
	        <<setw(6)<<setprecision(4)<<fixed<<bus->get_normal_voltage_upper_limit_in_pu()<<", "
	        <<setw(6)<<setprecision(4)<<fixed<<bus->get_normal_voltage_lower_limit_in_pu()<<", "
	        <<setw(6)<<setprecision(4)<<fixed<<bus->get_emergency_voltage_upper_limit_in_pu()<<", "
	        <<setw(6)<<setprecision(4)<<fixed<<bus->get_emergency_voltage_lower_limit_in_pu()<<", "
	        <<setw(4)<<setprecision(1)<<fixed<<bus->get_base_frequency_in_Hz()
	        <<endl;

	return osstream.str();
}

string PSSE_IMEXPORTER::export_all_load_data() const
{
	ostringstream osstream;
	iSTEPS& toolkit = get_toolkit();
	PF_DATA& psdb = toolkit.get_database();

	const vector<size_t>& ordered_buses = get_ordered_buses();
	size_t n = ordered_buses.size();

	vector<LOAD*> loads;
	for(size_t i=0; i<n; ++i)
	{
		size_t bus = ordered_buses[i];
		vector<LOAD*> loads_at_bus = psdb.get_loads_connecting_to_bus(bus);
		size_t m = loads_at_bus.size();
		for(size_t j=0; j<m; ++j)
			loads.push_back(loads_at_bus[j]);
	}

	n = loads.size();
	for(size_t i=0; i!=n; ++i)
		osstream<<export_load_data(loads[i]);
	return osstream.str();
}

string PSSE_IMEXPORTER::export_load_data(const LOAD* load) const
{
	ostringstream osstream;
	iSTEPS& toolkit = get_toolkit();
	PF_DATA& psdb = toolkit.get_database();

	size_t bus = load->get_load_bus();
	string ickt = load->get_identifier();
	if(get_export_zero_impedance_line_logic()==false and psdb.get_equivalent_bus_of_bus(bus)!=0)
	{
		bus = psdb.get_equivalent_bus_of_bus(bus);
		ickt = ickt + toolkit.get_next_alphabeta();
	}
	if(get_export_out_of_service_bus_logic()==false and psdb.get_bus_type(bus)==OUT_OF_SERVICE)
		return "";

	NET_MATRIX& network = toolkit.get_network_matrix();
	if(get_export_internal_bus_number_logic()==true)
		bus = network.get_internal_bus_number_of_physical_bus(bus)+1;

	osstream<<right
	        <<setw(8)<<bus<<", "
	        <<setw(6)<<("\""+ickt+"\"")<<", "
	        <<load->get_status()<<", "
	        <<setw(8)<<load->get_area_number()<<", "
	        <<setw(8)<<load->get_zone_number()<<", "
	        <<setw(8)<<setprecision(6)<<load->get_nominal_constant_power_load_in_MVA().real()<<", "
	        <<setw(8)<<setprecision(6)<<load->get_nominal_constant_power_load_in_MVA().imag()<<", "
	        <<setw(8)<<setprecision(6)<<load->get_nominal_constant_current_load_in_MVA().real()<<", "
	        <<setw(8)<<setprecision(6)<<load->get_nominal_constant_current_load_in_MVA().imag()<<", "
	        <<setw(8)<<setprecision(6)<<load->get_nominal_constant_impedance_load_in_MVA().real()<<", "
	        <<setw(8)<<setprecision(6)<<load->get_nominal_constant_impedance_load_in_MVA().imag()<<", "
	        <<setw(8)<<load->get_owner_number()<<", "
	        <<setw(8)<<load->get_flag_interruptable()
	        <<endl;
	return osstream.str();
}

string PSSE_IMEXPORTER::export_all_fixed_shunt_data() const
{
	ostringstream osstream;
	iSTEPS& toolkit = get_toolkit();
	PF_DATA& psdb = toolkit.get_database();

	const vector<size_t>& ordered_buses = get_ordered_buses();
	size_t n = ordered_buses.size();

	vector<FIXED_SHUNT*> fshunts;
	for(size_t i=0; i<n; ++i)
	{
		size_t bus = ordered_buses[i];
		vector<FIXED_SHUNT*> fshunts_at_bus = psdb.get_fixed_shunts_connecting_to_bus(bus);
		size_t m = fshunts_at_bus.size();
		for(size_t j=0; j<m; ++j)
			fshunts.push_back(fshunts_at_bus[j]);
	}

	n = fshunts.size();
	for(size_t i=0; i!=n; ++i)
		osstream<<export_fixed_shunt_data(fshunts[i]);
	return osstream.str();
}

string PSSE_IMEXPORTER::export_fixed_shunt_data(const FIXED_SHUNT* shunt) const
{
	ostringstream osstream;
	iSTEPS& toolkit = get_toolkit();
	PF_DATA& psdb = toolkit.get_database();

	size_t bus = shunt->get_shunt_bus();
	string ickt = shunt->get_identifier();
	if(get_export_zero_impedance_line_logic()==false and psdb.get_equivalent_bus_of_bus(bus)!=0)
	{
		bus = psdb.get_equivalent_bus_of_bus(bus);
		ickt = ickt + toolkit.get_next_alphabeta();
	}
	if(get_export_out_of_service_bus_logic()==false and psdb.get_bus_type(bus)==OUT_OF_SERVICE)
		return "";

	NET_MATRIX& network = toolkit.get_network_matrix();
	if(get_export_internal_bus_number_logic()==true)
		bus = network.get_internal_bus_number_of_physical_bus(bus)+1;

	osstream<<right
	        <<setw(8)<<bus<<", "
	        <<setw(6)<<("\""+ickt+"\"")<<", "
	        <<shunt->get_status()<<", "
	        <<setw(12)<<setprecision(6)<<fixed<<shunt->get_nominal_positive_sequence_impedance_shunt_in_MVA().real()<<", "
	        <<setw(12)<<setprecision(6)<<fixed<<-shunt->get_nominal_positive_sequence_impedance_shunt_in_MVA().imag()<<endl;
	return osstream.str();
}

string PSSE_IMEXPORTER::export_all_source_data() const
{
	ostringstream osstream;
	osstream<<export_all_generator_data();
	osstream<<export_all_wt_generator_data();
	osstream<<export_all_pv_unit_data();
	osstream<<export_all_energy_storage_data();

	return osstream.str();
}

string PSSE_IMEXPORTER::export_all_generator_data() const
{
	ostringstream osstream;
	iSTEPS& toolkit = get_toolkit();
	PF_DATA& psdb = toolkit.get_database();

	const vector<size_t>& ordered_buses = get_ordered_buses();
	size_t n = ordered_buses.size();

	vector<GENERATOR*> generators;
	for(size_t i=0; i<n; ++i)
	{
		size_t bus = ordered_buses[i];
		vector<GENERATOR*> generators_at_bus = psdb.get_generators_connecting_to_bus(bus);
		size_t m = generators_at_bus.size();
		for(size_t j=0; j<m; ++j)
			generators.push_back(generators_at_bus[j]);
	}

	n = generators.size();
	for(size_t i=0; i!=n; ++i)
		osstream<<export_generator_data(generators[i]);
	return osstream.str();
}

string PSSE_IMEXPORTER::export_generator_data(const GENERATOR* generator) const
{
	ostringstream osstream;
	iSTEPS& toolkit = get_toolkit();
	PF_DATA& psdb = toolkit.get_database();

	if(get_export_out_of_service_bus_logic()==false and psdb.get_bus_type(generator->get_source_bus())==OUT_OF_SERVICE)
		return "";

	osstream<<export_source_common_data(generator);
	osstream<<"0, 0.00, 0"<<endl;
	return osstream.str();
}

string PSSE_IMEXPORTER::export_all_wt_generator_data() const
{
	ostringstream osstream;
	iSTEPS& toolkit = get_toolkit();
	PF_DATA& psdb = toolkit.get_database();

	const vector<size_t>& ordered_buses = get_ordered_buses();
	size_t n = ordered_buses.size();

	vector<WT_GENERATOR*> wt_generators;
	for(size_t i=0; i<n; ++i)
	{
		size_t bus = ordered_buses[i];
		vector<WT_GENERATOR*> wt_generators_at_bus = psdb.get_wt_generators_connecting_to_bus(bus);
		size_t m = wt_generators_at_bus.size();
		for(size_t j=0; j<m; ++j)
			wt_generators.push_back(wt_generators_at_bus[j]);
	}

	n = wt_generators.size();
	for(size_t i=0; i!=n; ++i)
		osstream<<export_wt_generator_data(wt_generators[i]);
	return osstream.str();
}


string PSSE_IMEXPORTER::export_wt_generator_data(const WT_GENERATOR* wt_generator) const
{
	ostringstream osstream;
	iSTEPS& toolkit = get_toolkit();
	PF_DATA& psdb = toolkit.get_database();

	if(get_export_out_of_service_bus_logic()==false and psdb.get_bus_type(wt_generator->get_source_bus())==OUT_OF_SERVICE)
		return "";

	osstream<<export_source_common_data(wt_generator);
	osstream<<export_source_var_control_data(wt_generator);
	osstream<<", 1"<<endl;

	return osstream.str();
}


string PSSE_IMEXPORTER::export_all_pv_unit_data() const
{
	ostringstream osstream;
	iSTEPS& toolkit = get_toolkit();
	PF_DATA& psdb = toolkit.get_database();

	const vector<size_t>& ordered_buses = get_ordered_buses();
	size_t n = ordered_buses.size();

	vector<PV_UNIT*> pv_units;
	for(size_t i=0; i<n; ++i)
	{
		size_t bus = ordered_buses[i];
		vector<PV_UNIT*> pv_units_at_bus = psdb.get_pv_units_connecting_to_bus(bus);
		size_t m = pv_units_at_bus.size();
		for(size_t j=0; j<m; ++j)
			pv_units.push_back(pv_units_at_bus[j]);
	}

	n = pv_units.size();
	for(size_t i=0; i!=n; ++i)
		osstream<<export_pv_unit_data(pv_units[i]);
	return osstream.str();
}


string PSSE_IMEXPORTER::export_pv_unit_data(const PV_UNIT* pv_unit) const
{
	ostringstream osstream;
	iSTEPS& toolkit = get_toolkit();
	PF_DATA& psdb = toolkit.get_database();

	if(get_export_out_of_service_bus_logic()==false and psdb.get_bus_type(pv_unit->get_source_bus())==OUT_OF_SERVICE)
		return "";

	osstream<<export_source_common_data(pv_unit);
	osstream<<export_source_var_control_data(pv_unit);

	osstream<<", 2"<<endl;
	return osstream.str();
}


string PSSE_IMEXPORTER::export_all_energy_storage_data() const
{
	ostringstream osstream;
	iSTEPS& toolkit = get_toolkit();
	PF_DATA& psdb = toolkit.get_database();

	const vector<size_t>& ordered_buses = get_ordered_buses();
	size_t n = ordered_buses.size();

	vector<ENERGY_STORAGE*> estorages;
	for(size_t i=0; i<n; ++i)
	{
		size_t bus = ordered_buses[i];
		vector<ENERGY_STORAGE*> estorages_at_bus = psdb.get_energy_storages_connecting_to_bus(bus);
		size_t m = estorages_at_bus.size();
		for(size_t j=0; j<m; ++j)
			estorages.push_back(estorages_at_bus[j]);
	}

	n = estorages.size();
	for(size_t i=0; i!=n; ++i)
		osstream<<export_energy_storage_data(estorages[i]);
	return osstream.str();
}

string PSSE_IMEXPORTER::export_energy_storage_data(const ENERGY_STORAGE* estorage) const
{
	ostringstream osstream;
	iSTEPS& toolkit = get_toolkit();
	PF_DATA& psdb = toolkit.get_database();

	if(get_export_out_of_service_bus_logic()==false and psdb.get_bus_type(estorage->get_source_bus())==OUT_OF_SERVICE)
		return "";

	osstream<<export_source_common_data(estorage);
	osstream<<export_source_var_control_data(estorage);

	osstream<<", 3"<<endl;

	return osstream.str();
}

string PSSE_IMEXPORTER::export_source_common_data(const SOURCE* source) const
{
	ostringstream osstream;
	iSTEPS& toolkit = get_toolkit();
	PF_DATA& psdb = toolkit.get_database();

	size_t bus = source->get_source_bus();
	size_t bus_to_regulate = source->get_bus_to_regulate();
	string ickt = source->get_identifier();
	if(get_export_zero_impedance_line_logic()==false and psdb.get_equivalent_bus_of_bus(bus)!=0)
	{
		bus = psdb.get_equivalent_bus_of_bus(bus);
		ickt = ickt + toolkit.get_next_alphabeta();
		if(bus_to_regulate!=0)
			bus_to_regulate = psdb.get_equivalent_bus_of_bus(bus_to_regulate);
	}

	NET_MATRIX& network = toolkit.get_network_matrix();
	if(get_export_internal_bus_number_logic()==true)
	{
		bus = network.get_internal_bus_number_of_physical_bus(bus)+1;
		bus_to_regulate = network.get_internal_bus_number_of_physical_bus(bus_to_regulate)+1;
	}

	osstream<<right
	        <<setw(8)<<bus<<", "
	        <<setw(6)<<("\""+ickt+"\"")<<", "
	        <<setw(12)<<setprecision(6)<<fixed<<source->get_p_generation_in_MW()<<", "
	        <<setw(12)<<setprecision(6)<<fixed<<source->get_q_generation_in_MVar()<<", "
	        <<setw(12)<<setprecision(6)<<fixed<<source->get_q_max_in_MVar()<<", "
	        <<setw(12)<<setprecision(6)<<fixed<<source->get_q_min_in_MVar()<<", "
	        <<setw(8)<<setprecision(6)<<fixed<<source->get_voltage_to_regulate_in_pu()<<", ";
	if(source->get_bus_to_regulate()==source->get_source_bus())
		osstream<<setw(8)<<0<<", ";
	else
		osstream<<setw(8)<<bus_to_regulate<<", ";
	osstream<<setw(10)<<setprecision(4)<<fixed<<source->get_mbase_in_MVA()<<", "
	        <<setw(9)<<setprecision(6)<<fixed<<source->get_source_impedance_in_pu().real()<<", "
	        <<setw(9)<<setprecision(6)<<fixed<<source->get_source_impedance_in_pu().imag()<<", "
	        <<setprecision(2)<<0.0<<", "<<0.0<<", "<<1.0<<", "
	        <<source->get_status()<<","
	        <<setprecision(2)<<100.0<<", "
	        <<setw(12)<<setprecision(6)<<fixed<<source->get_p_max_in_MW()<<", "
	        <<setw(12)<<setprecision(6)<<fixed<<source->get_p_min_in_MW()<<", ";
	if(source->get_owner_count()==0)
		osstream<<"1, 1.0, 0, 0.0, 0, 0.0, 0, 0.0, ";
	else
	{
		osstream<< setprecision(0) << source->get_owner_of_index(0) << ", " << setprecision(6) << source->get_fraction_of_owner_of_index(0) << ", "
		        << setprecision(0) << source->get_owner_of_index(1) << ", " << setprecision(6) << source->get_fraction_of_owner_of_index(1) << ", "
		        << setprecision(0) << source->get_owner_of_index(2) << ", " << setprecision(6) << source->get_fraction_of_owner_of_index(2) << ", "
		        << setprecision(0) << source->get_owner_of_index(3) << ", " << setprecision(6) << source->get_fraction_of_owner_of_index(3) << ", ";
	}

	return osstream.str();
}

string PSSE_IMEXPORTER::export_source_var_control_data(const SOURCE* source) const
{
	ostringstream osstream;
	double p = source->get_p_generation_in_MW();

	double qmax = source->get_q_max_in_MVar();
	double qmin = source->get_q_min_in_MVar();
	if(fabs(qmax+qmin)>DOUBLE_EPSILON)
	{
		if(fabs(qmax-qmin)>DOUBLE_EPSILON)
			osstream<<"0, 0.00";
		else
			osstream<<"3, 0.00";
	}
	else
	{
		double pf = p/steps_sqrt(p*p+qmax*qmax);
		osstream<<"2, "<<setw(4)<<setiosflags(ios::fixed)<<setprecision(2)<<pf;
	}
	return osstream.str();
}

string PSSE_IMEXPORTER::export_all_ac_line_data() const
{
	ostringstream osstream;
	iSTEPS& toolkit = get_toolkit();
	PF_DATA& psdb = toolkit.get_database();

	//vector<AC_LINE*> lines = psdb.get_all_ac_lines();
	//size_t n = lines.size();

	vector<DEVICE_ID> dids = psdb.get_all_ac_lines_device_id();
	sort(dids.begin(), dids.end());
	vector<AC_LINE*> lines;
	size_t n = dids.size();
	for(size_t i=0; i!=n; ++i)
		lines.push_back(psdb.get_ac_line(dids[i]));

	for(size_t i=0; i!=n; ++i)
		osstream<<export_ac_line_data(lines[i]);
	return osstream.str();
}

string PSSE_IMEXPORTER::export_ac_line_data(const AC_LINE* line) const
{
	ostringstream osstream;
	iSTEPS& toolkit = get_toolkit();
	PF_DATA& psdb = toolkit.get_database();

	size_t ibus = line->get_sending_side_bus();
	size_t jbus = line->get_receiving_side_bus();
	string ickt = line->get_identifier();
	if(get_export_zero_impedance_line_logic()==false and (psdb.get_equivalent_bus_of_bus(ibus)!=0 or psdb.get_equivalent_bus_of_bus(jbus)!=0))
	{
		if(psdb.get_equivalent_bus_of_bus(ibus)!=0)
			ibus = psdb.get_equivalent_bus_of_bus(ibus);
		if(psdb.get_equivalent_bus_of_bus(jbus)!=0)
			jbus = psdb.get_equivalent_bus_of_bus(jbus);
		ickt = ickt + toolkit.get_next_alphabeta();
	}
	if(get_export_out_of_service_bus_logic()==false and (psdb.get_bus_type(ibus)==OUT_OF_SERVICE or psdb.get_bus_type(jbus)==OUT_OF_SERVICE))
		return "";

	if(ibus==jbus)
		return "";

	size_t meterend = 1;
	if(line->get_meter_end_bus()==line->get_receiving_side_bus())
		meterend = 2;

	NET_MATRIX& network = toolkit.get_network_matrix();
	if(get_export_internal_bus_number_logic()==true)
	{
		ibus = network.get_internal_bus_number_of_physical_bus(ibus)+1;
		jbus = network.get_internal_bus_number_of_physical_bus(jbus)+1;
	}

	osstream<<right
	        <<setw(8)<<ibus<<", "
	        <<setw(8)<<jbus<<", "
	        <<setw(6)<<("\""+ickt+"\"")<<", "
	        <<setw(12)<<setprecision(6)<<fixed<<line->get_line_positive_sequence_z_in_pu().real()<<","
	        <<setw(12)<<setprecision(6)<<fixed<<line->get_line_positive_sequence_z_in_pu().imag()<<", "
	        <<setw(12)<<setprecision(6)<<fixed<<line->get_line_positive_sequence_y_in_pu().imag()<<", "
	        <<setw(10)<<setprecision(3)<<fixed<<line->get_rating().get_rating_A_MVA()<<", "
	        <<setw(10)<<setprecision(3)<<fixed<<line->get_rating().get_rating_B_MVA()<<", "
	        <<setw(10)<<setprecision(3)<<fixed<<line->get_rating().get_rating_C_MVA()<<", "
	        <<setw(12)<<setprecision(6)<<fixed<<line->get_shunt_positive_sequence_y_at_sending_side_in_pu().real()<<", "
	        <<setw(12)<<setprecision(6)<<fixed<<line->get_shunt_positive_sequence_y_at_sending_side_in_pu().imag()<<", "
	        <<setw(12)<<setprecision(6)<<fixed<<line->get_shunt_positive_sequence_y_at_receiving_side_in_pu().real()<<", "
	        <<setw(12)<<setprecision(6)<<fixed<<line->get_shunt_positive_sequence_y_at_receiving_side_in_pu().imag()<<", "
	        <<(line->get_sending_side_breaker_status() and line->get_receiving_side_breaker_status())<<", "
	        <<meterend<<", "
	        <<setw(6)<<setprecision(2)<<fixed<<line->get_length()<<", "
	        <<setw(4)<<line->get_owner_of_index(0)<<", "<<setw(6)<<setprecision(3)<<line->get_fraction_of_owner_of_index(0)<<", "
	        <<setw(4)<<line->get_owner_of_index(1)<<", "<<setw(6)<<setprecision(3)<<line->get_fraction_of_owner_of_index(1)<<", "
	        <<setw(4)<<line->get_owner_of_index(2)<<", "<<setw(6)<<setprecision(3)<<line->get_fraction_of_owner_of_index(2)<<", "
	        <<setw(4)<<line->get_owner_of_index(3)<<", "<<setw(6)<<setprecision(3)<<line->get_fraction_of_owner_of_index(3)<<endl;

	return osstream.str();
}

string PSSE_IMEXPORTER::export_all_transformer_data() const
{
	ostringstream osstream;
	iSTEPS& toolkit = get_toolkit();
	PF_DATA& psdb = toolkit.get_database();

	//vector<TRANSFORMER*> transformers = psdb.get_all_transformers();
	//size_t n = transformers.size();

	vector<DEVICE_ID> dids = psdb.get_all_transformers_device_id();
	sort(dids.begin(), dids.end());
	vector<TRANSFORMER*> transformers;
	size_t n = dids.size();
	for(size_t i=0; i!=n; ++i)
		transformers.push_back(psdb.get_transformer(dids[i]));

	for(size_t i=0; i!=n; ++i)
		osstream<<export_transformer_data(transformers[i]);
	return osstream.str();
}

string PSSE_IMEXPORTER::export_transformer_data(const TRANSFORMER* trans) const
{
	ostringstream osstream;
	iSTEPS& toolkit = get_toolkit();
	PF_DATA& psdb = toolkit.get_database();

	size_t ibus = trans->get_winding_bus(PRIMARY_SIDE);
	size_t jbus = trans->get_winding_bus(SECONDARY_SIDE);
	size_t kbus = trans->get_winding_bus(TERTIARY_SIDE);
	string ickt = trans->get_identifier();
	if(get_export_zero_impedance_line_logic()==false and (psdb.get_equivalent_bus_of_bus(ibus)!=0 or psdb.get_equivalent_bus_of_bus(jbus)!=0 or psdb.get_equivalent_bus_of_bus(kbus)!=0))
	{
		if(psdb.get_equivalent_bus_of_bus(ibus)!=0)
			ibus = psdb.get_equivalent_bus_of_bus(ibus);
		if(psdb.get_equivalent_bus_of_bus(jbus)!=0)
			jbus = psdb.get_equivalent_bus_of_bus(jbus);
		if(psdb.get_equivalent_bus_of_bus(kbus)!=0)
			kbus = psdb.get_equivalent_bus_of_bus(kbus);
		ickt = ickt + toolkit.get_next_alphabeta();
	}
	if(trans->is_two_winding_transformer())
	{
		if(get_export_out_of_service_bus_logic()==false and (psdb.get_bus_type(ibus)==OUT_OF_SERVICE or psdb.get_bus_type(jbus)==OUT_OF_SERVICE))
			return "";
	}
	else
	{
		if(get_export_out_of_service_bus_logic()==false and (psdb.get_bus_type(ibus)==OUT_OF_SERVICE or psdb.get_bus_type(jbus)==OUT_OF_SERVICE or psdb.get_bus_type(kbus)==OUT_OF_SERVICE))
			return "";
	}

	size_t nonmeterend = 2;
	if(trans->is_two_winding_transformer())
	{
		if(trans->get_non_metered_end_bus() == trans->get_winding_bus(PRIMARY_SIDE))
			nonmeterend = 1;
		else
			nonmeterend = 2;
	}
	else
	{
		if(trans->get_non_metered_end_bus() == trans->get_winding_bus(PRIMARY_SIDE))
			nonmeterend = 1;
		else
		{
			if(trans->get_non_metered_end_bus() == trans->get_winding_bus(SECONDARY_SIDE))
				nonmeterend = 2;
			else
			{
				if(trans->get_non_metered_end_bus() == trans->get_winding_bus(TERTIARY_SIDE))
					nonmeterend = 3;
				else
					nonmeterend = 2;
			}
		}
	}

	size_t status = 1;
	if(trans->is_two_winding_transformer())
	{
		if(trans->get_winding_breaker_status(PRIMARY_SIDE)==false or trans->get_winding_breaker_status(SECONDARY_SIDE)==false)
			status = 0;
	}
	else
	{
		if(trans->get_winding_breaker_status(PRIMARY_SIDE)==false and trans->get_winding_breaker_status(SECONDARY_SIDE)==false
		      and trans->get_winding_breaker_status(TERTIARY_SIDE)==false)
			status = 0;
		else
		{
			if(trans->get_winding_breaker_status(SECONDARY_SIDE)==false)
				status = 2;
			if(trans->get_winding_breaker_status(TERTIARY_SIDE)==false)
				status = 2;
			if(trans->get_winding_breaker_status(PRIMARY_SIDE)==false)
				status = 2;
		}
	}

	NET_MATRIX& network = toolkit.get_network_matrix();
	if(get_export_internal_bus_number_logic()==true)
	{
		ibus = network.get_internal_bus_number_of_physical_bus(ibus)+1;
		jbus = network.get_internal_bus_number_of_physical_bus(jbus)+1;
		if(kbus!=0)
			kbus = network.get_internal_bus_number_of_physical_bus(kbus)+1;
	}

	osstream<<right
	        <<setw(8)<<ibus<<", "
	        <<setw(8)<<jbus<<", "
	        <<setw(8)<<kbus<<", "
	        <<setw(6)<<("\""+ickt+"\"")<<", "
	        <<TAP_OFF_NOMINAL_TURN_RATIO_BASED_ON_WINDING_NOMNAL_VOLTAGE<<", "
	        <<IMPEDANCE_IN_PU_ON_WINDINGS_POWER_AND_WINDING_NOMINAL_VOLTAGE<<", "
	        <<ADMITTANCE_IN_PU_ON_SYSTEM_BASE_POWER_AND_PRIMARY_WINDING_BUS_BASE_VOLTAGE<<", "
	        <<setprecision(6)<<fixed<<trans->get_magnetizing_admittance_based_on_winding_norminal_voltage_and_system_base_power_in_pu().real()<<", "
	        <<setprecision(6)<<fixed<<trans->get_magnetizing_admittance_based_on_winding_norminal_voltage_and_system_base_power_in_pu().imag()<<", "
	        <<nonmeterend<<", "
	        <<"\""<<left
	        <<setw(16)<<trans->get_name()<<"\", "
	        <<right
	        <<status<<", "
	        <<trans->get_owner_of_index(0)<<", "<<setprecision(6)<<fixed<<trans->get_fraction_of_owner_of_index(0)<<", "
	        <<trans->get_owner_of_index(1)<<", "<<setprecision(6)<<fixed<<trans->get_fraction_of_owner_of_index(1)<<", "
	        <<trans->get_owner_of_index(2)<<", "<<setprecision(6)<<fixed<<trans->get_fraction_of_owner_of_index(2)<<", "
	        <<trans->get_owner_of_index(3)<<", "<<setprecision(6)<<fixed<<trans->get_fraction_of_owner_of_index(3)<<", "
	        <<"\"\""<<endl;
	if(trans->is_two_winding_transformer())
	{
		osstream<<"          "
		        <<trans->get_leakage_impedance_between_windings_based_on_winding_nominals_in_pu(PRIMARY_SIDE, SECONDARY_SIDE).real()<<", "
		        <<trans->get_leakage_impedance_between_windings_based_on_winding_nominals_in_pu(PRIMARY_SIDE, SECONDARY_SIDE).imag()<<", "
		        <<trans->get_winding_nominal_capacity_in_MVA(PRIMARY_SIDE, SECONDARY_SIDE)<<endl;

		TRANSFORMER_WINDING_SIDE winding = PRIMARY_SIDE;

		size_t control_mode;
		switch(trans->get_winding_control_mode(winding))
		{
			case TRANSFORMER_TAP_NO_CONTROL:
				control_mode = 0;
				break;
			case TRANSFORMER_TAP_VOLTAGE_CONTROL:
				control_mode = 1;
				break;
			case TRANSFORMER_TAP_REACTIVE_POWER_CONTROL:
				control_mode = 2;
				break;
			case TRANSFORMER_TAP_ACTIVE_POWER_CONTROL:
				control_mode = 3;
				break;
			case TRANSFORMER_TAP_HVDC_LINE_CONTROL:
				control_mode = 4;
				break;
			case TRANSFORMER_TAP_ASYMMETRIC_ACTIVE_POWER_CONTROL:
				control_mode = 5;
				break;
			default:
				control_mode = 0;
				break;
		}

		osstream<<"          "
		        <<setw(8)<<setprecision(6)<<fixed<<trans->get_winding_turn_ratio_based_on_winding_nominal_voltage_in_pu(winding)<<", "
		        <<setw(7)<<setprecision(2)<<fixed<<trans->get_winding_nominal_voltage_in_kV(winding)<<", "
		        <<setw(6)<<setprecision(2)<<fixed<<trans->get_winding_angle_shift_in_deg(winding)<<", "
		        <<setw(8)<<setprecision(2)<<fixed<<trans->get_winding_rating_in_MVA(winding).get_rating_A_MVA()<<", "
		        <<setw(8)<<setprecision(2)<<fixed<<trans->get_winding_rating_in_MVA(winding).get_rating_B_MVA()<<", "
		        <<setw(8)<<setprecision(2)<<fixed<<trans->get_winding_rating_in_MVA(winding).get_rating_C_MVA()<<", "
		        <<control_mode<<", ";
		switch(trans->get_winding_control_mode(winding))
		{
			case TRANSFORMER_TAP_VOLTAGE_CONTROL:
				osstream<<trans->get_winding_controlled_bus(winding)<<", "
				        <<setw(8)<<setprecision(6)<<fixed<<trans->get_winding_max_turn_ratio_based_on_winding_nominal_voltage_in_pu(winding)<<", "
				        <<setw(8)<<setprecision(6)<<fixed<<trans->get_winding_min_turn_ratio_based_on_winding_nominal_voltage_in_pu(winding)<<", "
				        <<setw(8)<<setprecision(2)<<fixed<<trans->get_winding_controlled_max_voltage_in_pu(winding)<<", "
				        <<setw(8)<<setprecision(2)<<fixed<<trans->get_winding_controlled_min_voltage_in_pu(winding)<<", ";
				break;
			case TRANSFORMER_TAP_REACTIVE_POWER_CONTROL:
				osstream<<trans->get_winding_controlled_bus(winding)<<", "
				        <<setw(8)<<setprecision(6)<<fixed<<trans->get_winding_max_turn_ratio_based_on_winding_nominal_voltage_in_pu(winding)<<", "
				        <<setw(8)<<setprecision(6)<<fixed<<trans->get_winding_min_turn_ratio_based_on_winding_nominal_voltage_in_pu(winding)<<", "
				        <<setw(8)<<setprecision(2)<<fixed<<trans->get_controlled_max_reactive_power_into_winding_in_MVar(winding)<<", "
				        <<setw(8)<<setprecision(2)<<fixed<<trans->get_controlled_min_reactive_power_into_winding_in_MVar(winding)<<", ";
				break;
			case TRANSFORMER_TAP_ACTIVE_POWER_CONTROL:
			case TRANSFORMER_TAP_ASYMMETRIC_ACTIVE_POWER_CONTROL:
				osstream<<trans->get_winding_controlled_bus(winding)<<", "
				        <<setw(8)<<setprecision(6)<<fixed<<trans->get_winding_max_angle_shift_in_deg(winding)<<", "
				        <<setw(8)<<setprecision(6)<<fixed<<trans->get_winding_min_angle_shift_in_deg(winding)<<", "
				        <<setw(8)<<setprecision(2)<<fixed<<trans->get_controlled_max_active_power_into_winding_in_MW(winding)<<", "
				        <<setw(8)<<setprecision(2)<<fixed<<trans->get_controlled_min_active_power_into_winding_in_MW(winding)<<", ";
				break;
			case TRANSFORMER_TAP_NO_CONTROL:
			case TRANSFORMER_TAP_HVDC_LINE_CONTROL:
			default:
				osstream<<0<<", "
				        <<setw(8)<<setprecision(6)<<fixed<<trans->get_winding_max_turn_ratio_based_on_winding_nominal_voltage_in_pu(winding)<<", "
				        <<setw(8)<<setprecision(6)<<fixed<<trans->get_winding_min_turn_ratio_based_on_winding_nominal_voltage_in_pu(winding)<<", "
				        <<setw(8)<<setprecision(2)<<fixed<<trans->get_winding_controlled_max_voltage_in_pu(winding)<<", "
				        <<setw(8)<<setprecision(2)<<fixed<<trans->get_winding_controlled_min_voltage_in_pu(winding)<<", ";
				break;
		}

		osstream<<setw(3)<<setprecision(0)<<trans->get_winding_number_of_taps(winding)<<", "
		        <<setw(3)<<setprecision(0)<<0<<", "
		        <<setw(3)<<setprecision(1)<<0.0<<", "
		        <<setw(3)<<setprecision(1)<<0.0<<", "
		        <<setw(3)<<setprecision(1)<<0.0<<endl;

		winding = SECONDARY_SIDE;
		osstream<<"          "
		        <<setw(8)<<setprecision(6)<<fixed<<trans->get_winding_turn_ratio_based_on_winding_nominal_voltage_in_pu(winding)<<", "
		        <<setw(7)<<setprecision(2)<<fixed<<trans->get_winding_nominal_voltage_in_kV(winding)<<endl;
	}
	else
	{
		osstream<<"          "
		        <<setw(8)<<setprecision(6)<<fixed<<trans->get_leakage_impedance_between_windings_based_on_winding_nominals_in_pu(PRIMARY_SIDE, SECONDARY_SIDE).real()<<", "
		        <<setw(8)<<setprecision(6)<<fixed<<trans->get_leakage_impedance_between_windings_based_on_winding_nominals_in_pu(PRIMARY_SIDE, SECONDARY_SIDE).imag()<<", "
		        <<setw(8)<<setprecision(2)<<fixed<<trans->get_winding_nominal_capacity_in_MVA(PRIMARY_SIDE, SECONDARY_SIDE)<<", "
		        <<setw(8)<<setprecision(6)<<fixed<<trans->get_leakage_impedance_between_windings_based_on_winding_nominals_in_pu(SECONDARY_SIDE, TERTIARY_SIDE).real()<<", "
		        <<setw(8)<<setprecision(6)<<fixed<<trans->get_leakage_impedance_between_windings_based_on_winding_nominals_in_pu(SECONDARY_SIDE, TERTIARY_SIDE).imag()<<", "
		        <<setw(8)<<setprecision(2)<<fixed<<trans->get_winding_nominal_capacity_in_MVA(SECONDARY_SIDE, TERTIARY_SIDE)<<", "
		        <<setw(8)<<setprecision(6)<<fixed<<trans->get_leakage_impedance_between_windings_based_on_winding_nominals_in_pu(PRIMARY_SIDE, TERTIARY_SIDE).real()<<", "
		        <<setw(8)<<setprecision(6)<<fixed<<trans->get_leakage_impedance_between_windings_based_on_winding_nominals_in_pu(PRIMARY_SIDE, TERTIARY_SIDE).imag()<<", "
		        <<setw(8)<<setprecision(2)<<fixed<<trans->get_winding_nominal_capacity_in_MVA(PRIMARY_SIDE, TERTIARY_SIDE)<<", "
		        <<"1.0, 0.0"<<endl;

		for(size_t j=1; j!=4; ++j)
		{
			TRANSFORMER_WINDING_SIDE winding=PRIMARY_SIDE;
			if(j==1) winding = PRIMARY_SIDE;
			if(j==2) winding = SECONDARY_SIDE;
			if(j==3) winding = TERTIARY_SIDE;

			size_t control_mode;
			switch(trans->get_winding_control_mode(winding))
			{
				case TRANSFORMER_TAP_NO_CONTROL:
					control_mode = 0;
					break;
				case TRANSFORMER_TAP_VOLTAGE_CONTROL:
					control_mode = 1;
					break;
				case TRANSFORMER_TAP_REACTIVE_POWER_CONTROL:
					control_mode = 2;
					break;
				case TRANSFORMER_TAP_ACTIVE_POWER_CONTROL:
					control_mode = 3;
					break;
				case TRANSFORMER_TAP_HVDC_LINE_CONTROL:
					control_mode = 4;
					break;
				case TRANSFORMER_TAP_ASYMMETRIC_ACTIVE_POWER_CONTROL:
					control_mode = 5;
					break;
				default:
					control_mode = 0;
					break;
			}

			osstream<<"          "
			        <<setw(8)<<setprecision(6)<<fixed<<trans->get_winding_turn_ratio_based_on_winding_nominal_voltage_in_pu(winding)<<", "
			        <<setw(7)<<setprecision(2)<<fixed<<trans->get_winding_nominal_voltage_in_kV(winding)<<", "
			        <<setw(6)<<setprecision(2)<<fixed<<trans->get_winding_angle_shift_in_deg(winding)<<", "
			        <<setw(8)<<setprecision(2)<<fixed<<trans->get_winding_rating_in_MVA(winding).get_rating_A_MVA()<<", "
			        <<setw(8)<<setprecision(2)<<fixed<<trans->get_winding_rating_in_MVA(winding).get_rating_B_MVA()<<", "
			        <<setw(8)<<setprecision(2)<<fixed<<trans->get_winding_rating_in_MVA(winding).get_rating_C_MVA()<<", "
			        <<control_mode<<", ";
			switch(trans->get_winding_control_mode(winding))
			{
				case TRANSFORMER_TAP_VOLTAGE_CONTROL:
					osstream<<trans->get_winding_controlled_bus(winding)<<", "
					        <<setw(8)<<setprecision(6)<<fixed<<trans->get_winding_max_turn_ratio_based_on_winding_nominal_voltage_in_pu(winding)<<", "
					        <<setw(8)<<setprecision(6)<<fixed<<trans->get_winding_min_turn_ratio_based_on_winding_nominal_voltage_in_pu(winding)<<", "
					        <<setw(8)<<setprecision(2)<<fixed<<trans->get_winding_controlled_max_voltage_in_pu(winding)<<", "
					        <<setw(8)<<setprecision(2)<<fixed<<trans->get_winding_controlled_min_voltage_in_pu(winding)<<", ";
					break;
				case TRANSFORMER_TAP_REACTIVE_POWER_CONTROL:
					osstream<<trans->get_winding_controlled_bus(winding)<<", "
					        <<setw(8)<<setprecision(6)<<fixed<<trans->get_winding_max_turn_ratio_based_on_winding_nominal_voltage_in_pu(winding)<<", "
					        <<setw(8)<<setprecision(6)<<fixed<<trans->get_winding_min_turn_ratio_based_on_winding_nominal_voltage_in_pu(winding)<<", "
					        <<setw(8)<<setprecision(2)<<fixed<<trans->get_controlled_max_reactive_power_into_winding_in_MVar(winding)<<", "
					        <<setw(8)<<setprecision(2)<<fixed<<trans->get_controlled_min_reactive_power_into_winding_in_MVar(winding)<<", ";
					break;
				case TRANSFORMER_TAP_ACTIVE_POWER_CONTROL:
				case TRANSFORMER_TAP_ASYMMETRIC_ACTIVE_POWER_CONTROL:
					osstream<<trans->get_winding_controlled_bus(winding)<<", "
					        <<setw(8)<<setprecision(6)<<fixed<<trans->get_winding_max_angle_shift_in_deg(winding)<<", "
					        <<setw(8)<<setprecision(6)<<fixed<<trans->get_winding_min_angle_shift_in_deg(winding)<<", "
					        <<setw(8)<<setprecision(2)<<fixed<<trans->get_controlled_max_active_power_into_winding_in_MW(winding)<<", "
					        <<setw(8)<<setprecision(2)<<fixed<<trans->get_controlled_min_active_power_into_winding_in_MW(winding)<<", ";
					break;
				case TRANSFORMER_TAP_NO_CONTROL:
				case TRANSFORMER_TAP_HVDC_LINE_CONTROL:
				default:
					osstream<<0<<", "
					        <<setw(8)<<setprecision(6)<<fixed<<trans->get_winding_max_turn_ratio_based_on_winding_nominal_voltage_in_pu(winding)<<", "
					        <<setw(8)<<setprecision(6)<<fixed<<trans->get_winding_min_turn_ratio_based_on_winding_nominal_voltage_in_pu(winding)<<", "
					        <<setw(8)<<setprecision(2)<<fixed<<trans->get_winding_controlled_max_voltage_in_pu(winding)<<", "
					        <<setw(8)<<setprecision(2)<<fixed<<trans->get_winding_controlled_min_voltage_in_pu(winding)<<", ";
					break;
			}

			osstream<<setw(3)<<setprecision(0)<<trans->get_winding_number_of_taps(winding)<<", "
			        <<setw(3)<<setprecision(0)<<0<<", "
			        <<setw(3)<<setprecision(1)<<0.0<<", "
			        <<setw(3)<<setprecision(1)<<0.0<<", "
			        <<setw(3)<<setprecision(1)<<0.0<<endl;
		}
	}
	return osstream.str();
}

string PSSE_IMEXPORTER::export_all_area_data() const
{
	ostringstream osstream;
	iSTEPS& toolkit = get_toolkit();
	PF_DATA& psdb = toolkit.get_database();

	//vector<AREA*> areas = psdb.get_all_areas();
	//size_t n = areas.size();

	vector<size_t> areas_number = psdb.get_all_areas_number();
	sort(areas_number.begin(), areas_number.end());
	vector<AREA*> areas;
	size_t n = areas_number.size();
	for(size_t i=0; i!=n; ++i)
		areas.push_back(psdb.get_area(areas_number[i]));

	for(size_t i=0; i!=n; ++i)
		osstream<<export_area_data(areas[i]);

	return osstream.str();
}

string PSSE_IMEXPORTER::export_area_data(const AREA* area) const
{
	ostringstream osstream;
	iSTEPS& toolkit = get_toolkit();
	PF_DATA& psdb = toolkit.get_database();

	size_t bus = area->get_area_swing_bus();
	if(get_export_zero_impedance_line_logic()==false and psdb.get_equivalent_bus_of_bus(bus)!=0)
	{
		bus = psdb.get_equivalent_bus_of_bus(bus);
	}

	NET_MATRIX& network = toolkit.get_network_matrix();
	if(get_export_internal_bus_number_logic()==true)
	{
		if(bus!=0)
			bus = network.get_internal_bus_number_of_physical_bus(bus)+1;
	}

	osstream<<setw(8)<<area->get_area_number()<<", "
	        <<bus<<", "
	        <<setprecision(3)<<area->get_expected_power_leaving_area_in_MW()<<", "
	        <<setprecision(3)<<area->get_area_power_mismatch_tolerance_in_MW()<<", "
	        <<"\""<<area->get_area_name()<<"\""<<endl;

	return osstream.str();
}

string PSSE_IMEXPORTER::export_all_2t_lcc_hvdc_data() const
{
	ostringstream osstream;
	iSTEPS& toolkit = get_toolkit();
	PF_DATA& psdb = toolkit.get_database();

	//vector<LCC_HVDC2T*> hvdcs = psdb.get_all_2t_lcc_hvdcs();
	//size_t n = hvdcs.size();

	vector<DEVICE_ID> dids = psdb.get_all_2t_lcc_hvdcs_device_id();
	sort(dids.begin(), dids.end());
	vector<LCC_HVDC2T*> hvdcs;
	size_t n = dids.size();
	for(size_t i=0; i!=n; ++i)
		hvdcs.push_back(psdb.get_2t_lcc_hvdc(dids[i]));

	for(size_t i=0; i!=n; ++i)
		osstream<<export_2t_lcc_hvdc_data(hvdcs[i]);

	return osstream.str();
}

string PSSE_IMEXPORTER::export_2t_lcc_hvdc_data(const LCC_HVDC2T* hvdc) const
{
	ostringstream osstream;
	iSTEPS& toolkit = get_toolkit();
	PF_DATA& psdb = toolkit.get_database();

	size_t rbus = hvdc->get_converter_bus(RECTIFIER);
	size_t ibus = hvdc->get_converter_bus(INVERTER);
	string ickt = hvdc->get_identifier();
	if(get_export_zero_impedance_line_logic()==false and (psdb.get_equivalent_bus_of_bus(rbus)!=0 or psdb.get_equivalent_bus_of_bus(ibus)!=0))
	{
		if(psdb.get_equivalent_bus_of_bus(rbus)!=0)
			rbus = psdb.get_equivalent_bus_of_bus(rbus);
		if(psdb.get_equivalent_bus_of_bus(ibus)!=0)
			ibus = psdb.get_equivalent_bus_of_bus(ibus);
		ickt = ickt + toolkit.get_next_alphabeta();
	}
	if(get_export_out_of_service_bus_logic()==false and (psdb.get_bus_type(rbus)==OUT_OF_SERVICE or psdb.get_bus_type(ibus)==OUT_OF_SERVICE))
		return "";

	osstream<<setw(16)<<("\""+hvdc->get_name()+"\"")<<", ";

	bool status = hvdc->get_status();
	HVDC_OPERATION_MODE mode = hvdc->get_converter_operation_mode(RECTIFIER);
	if(status == false)
		osstream<<0<<", ";
	else
	{
		if(mode==RECTIFIER_CONSTANT_CURRENT)
			osstream<<2<<", ";
		else
			osstream<<1<<", ";
	}
	osstream<<setw(6)<<setprecision(4)<<fixed<<hvdc->get_line_resistance_in_ohm()<<", ";
	if(mode==RECTIFIER_CONSTANT_CURRENT)
		osstream<<setw(6)<<setprecision(2)<<fixed<<hvdc->get_nominal_dc_current_in_kA()*100.0<<", ";
	else
	{
		CONVERTER_SIDE side = hvdc->get_side_to_hold_dc_power();
		if(side==RECTIFIER)
			osstream<<setw(6)<<setprecision(2)<<fixed<<hvdc->get_nominal_dc_power_in_MW()<<", ";
		else
			osstream<<setw(6)<<setprecision(2)<<fixed<<-hvdc->get_nominal_dc_power_in_MW()<<", ";
	}

	osstream<<setw(6)<<setprecision(2)<<fixed<<hvdc->get_nominal_dc_voltage_in_kV()<<", ";
	osstream<<setw(6)<<setprecision(2)<<fixed<<hvdc->get_threshold_dc_voltage_for_constant_power_and_constant_current_mode_in_kV()<<", ";
	osstream<<setw(6)<<setprecision(4)<<fixed<<hvdc->get_compensating_resistance_to_hold_dc_voltage_in_ohm()<<", ";
	osstream<<setw(6)<<setprecision(4)<<fixed<<hvdc->get_current_power_margin()<<", ";

	osstream<<"\""<<(hvdc->get_meter_end()==RECTIFIER?"R":"I")<<"\", ";
	osstream<<"0.0, 20, 1.0"<<endl;

	NET_MATRIX& network = toolkit.get_network_matrix();
	for(size_t j=0; j!=2; ++j)
	{
		CONVERTER_SIDE converter=RECTIFIER;
		size_t bus = rbus;
		if(j==0) converter = RECTIFIER;
		if(j==1)
		{
			converter = INVERTER;
			bus = ibus;
		}

		if(get_export_internal_bus_number_logic()==true)
			bus = network.get_internal_bus_number_of_physical_bus(bus)+1;

		osstream<<"        "
		        <<setw(8)<<bus<<", "
		        <<hvdc->get_converter_number_of_bridge(converter)<<", "
		        <<setw(6)<<setprecision(2)<<fixed<<hvdc->get_converter_max_alpha_or_gamma_in_deg(converter)<<", "
		        <<setw(6)<<setprecision(2)<<fixed<<hvdc->get_converter_min_alpha_or_gamma_in_deg(converter)<<", "
		        <<setw(6)<<setprecision(4)<<fixed<<hvdc->get_converter_transformer_impedance_in_ohm(converter).real()<<", "
		        <<setw(6)<<setprecision(4)<<fixed<<hvdc->get_converter_transformer_impedance_in_ohm(converter).imag()<<", "
		        <<setw(6)<<setprecision(2)<<fixed<<hvdc->get_converter_transformer_grid_side_base_voltage_in_kV(converter)<<", ";
		double turn_ratio = hvdc->get_converter_transformer_converter_side_base_voltage_in_kV(converter)/
		                    hvdc->get_converter_transformer_grid_side_base_voltage_in_kV(converter);
		osstream<<setw(6)<<setprecision(4)<<fixed<<turn_ratio<<", "
		        <<setw(6)<<setprecision(4)<<fixed<<hvdc->get_converter_transformer_tap_in_pu(converter)<<", "
		        <<setw(6)<<setprecision(4)<<fixed<<hvdc->get_converter_transformer_max_tap_in_pu(converter)<<", "
		        <<setw(6)<<setprecision(4)<<fixed<<hvdc->get_converter_transformer_min_tap_in_pu(converter)<<", ";
		double tap_step = (hvdc->get_converter_transformer_max_tap_in_pu(converter)-
		                   hvdc->get_converter_transformer_min_tap_in_pu(converter))/
		                  (hvdc->get_converter_transformer_number_of_taps(converter)-1);
		osstream<<setw(6)<<setprecision(5)<<fixed<<tap_step<<", ";
		osstream<<"0, 0, 0, \"1\", 0.0"<<endl;
	}

	return osstream.str();
}

string PSSE_IMEXPORTER::export_all_vsc_hvdc_data() const
{
	ostringstream osstream;
	iSTEPS& toolkit=get_toolkit();
	PF_DATA& psdb=toolkit.get_database();

	vector<VSC_HVDC*> vsc_hvdcs=psdb.get_all_vsc_hvdcs();
	size_t n=vsc_hvdcs.size();
	for(size_t i=0; i!=n; ++i)
	{
		VSC_HVDC* vsc_hvdc=vsc_hvdcs[i];
		osstream<<"\""<<left
		        <<setw(8)<<vsc_hvdc->get_name()<<"\""<<", "
		        <<right
		        <<setw(8)<<vsc_hvdc->get_converter_count()<<", "
		        <<setw(8)<<vsc_hvdc->get_dc_bus_count()<<", "
		        <<setw(8)<<vsc_hvdc->get_dc_line_count()<<", ";
		bool status=vsc_hvdc->get_status();
		if (status==false)
			osstream<<0<<", ";
		else
			osstream<<1<<", ";
		osstream<<setw(8)<<vsc_hvdc->get_dc_network_base_voltage_in_kV()<<endl;
		size_t ncon=vsc_hvdc->get_converter_count();
		size_t nbus=vsc_hvdc->get_dc_bus_count();
		size_t ndc_line=vsc_hvdc->get_dc_line_count();
		for(size_t i=0; i!=ncon; ++i)
		{
			osstream<<setw(4)<<vsc_hvdc->get_converter_ac_bus(i)<<", "
			        <<setw(4)<<vsc_hvdc->get_converter_active_power_operation_mode(i)<<", "
			        <<setw(4)<<vsc_hvdc->get_converter_dc_voltage_control_priority(i)<<", "
			        <<setw(4)<<vsc_hvdc->get_converter_reactive_power_operation_mode(i)<<", ";
			switch(vsc_hvdc->get_converter_active_power_operation_mode(i))
			{
				case VSC_DC_VOLTAGE_CONTORL:
					osstream<<setw(4)<<vsc_hvdc->get_converter_nominal_dc_voltage_command_in_kV(i)<<", ";
					break;
				case VSC_AC_ACTIVE_POWER_CONTORL:
					osstream<<setw(4)<<vsc_hvdc->get_converter_nominal_ac_active_power_command_in_MW(i)<<", ";
					break;
				case VSC_DC_ACTIVE_POWER_VOLTAGE_DROOP_CONTROL:
					osstream<<setw(4)<<vsc_hvdc->get_converter_initial_dc_active_power_reference_in_MW(i)<<", ";
					osstream<<setw(4)<<vsc_hvdc->get_converter_initial_dc_voltage_reference_in_kV(i)<<", ";
					osstream<<setw(4)<<vsc_hvdc->get_converter_initial_droop_coefficient_for_droop_control(i)<<", ";
					break;
				case VSC_DC_CURRENT_VOLTAGE_DROOP_CONTROL:
					osstream<<setw(4)<<vsc_hvdc->get_converter_initial_dc_current_reference_in_kA(i)<<", ";
					osstream<<setw(4)<<vsc_hvdc->get_converter_initial_dc_voltage_reference_in_kV(i)<<", ";
					osstream<<setw(4)<<vsc_hvdc->get_converter_initial_droop_coefficient_for_droop_control(i)<<", ";
					break;
				case VSC_AC_VOLTAGE_ANGLE_CONTROL:
				default:
					break;
			}

			switch(vsc_hvdc->get_converter_reactive_power_operation_mode(i))
			{
				case VSC_AC_REACTIVE_POWER_CONTROL:
					osstream<<setw(4)<<vsc_hvdc->get_converter_nominal_ac_reactive_power_command_in_Mvar(i)<<", ";
					break;
				case VSC_AC_VOLTAGE_CONTROL:
					osstream<<setw(4)<<vsc_hvdc->get_converter_nominal_ac_voltage_command_in_pu(i)<<", ";
					break;
				default:
					break;
			}
			osstream<<setw(8)<<setprecision(4)<<fixed<<vsc_hvdc->get_converter_loss_factor_A_in_kW(i)<<", "
			        <<setw(8)<<setprecision(4)<<fixed<<vsc_hvdc->get_converter_loss_factor_B_in_kW_per_amp(i)<<", "
			        <<setw(8)<<setprecision(4)<<fixed<<vsc_hvdc->get_converter_minimum_loss_in_kW(i)<<", "
			        <<setw(8)<<setprecision(4)<<fixed<<vsc_hvdc->get_converter_rated_capacity_in_MVA(i)<<", "
			        <<setw(8)<<setprecision(4)<<fixed<<vsc_hvdc->get_converter_rated_current_in_A(i)<<", "

			        <<setw(8)<<setprecision(4)<<fixed<<vsc_hvdc->get_converter_commutating_impedance_in_ohm(i)<<", "
			        <<setw(8)<<setprecision(4)<<fixed<<vsc_hvdc->get_converter_Pmax_in_MW(i)<<", "
			        <<setw(8)<<setprecision(4)<<fixed<<vsc_hvdc->get_converter_Pmin_in_MW(i)<<", "

			        <<setw(8)<<setprecision(4)<<fixed<<vsc_hvdc->get_converter_Qmax_in_MVar(i)<<", "
			        <<setw(8)<<setprecision(4)<<fixed<<vsc_hvdc->get_converter_Qmin_in_MVar(i)<<", "
			        <<setw(8)<<setprecision(4)<<fixed<<vsc_hvdc->get_converter_Udmax_in_kV(i)<<", "
			        <<setw(8)<<setprecision(4)<<fixed<<vsc_hvdc->get_converter_Udmin_in_kV(i)<<", "
			        <<setw(8)<<vsc_hvdc->get_converter_remote_bus_to_regulate(i)<<", "
			        <<setw(8)<<setprecision(2)<<fixed<<vsc_hvdc->get_converter_remote_regulation_percent(i)<<endl;
		}
		for(size_t i=0; i!=nbus; ++i)
		{
			osstream<<setw(8)<<vsc_hvdc->get_dc_bus_number(i)<<", "
			        <<setw(8)<<vsc_hvdc->get_converter_ac_bus_number_with_dc_bus_index(i)<<", "
			        <<setw(8)<<vsc_hvdc->get_dc_bus_Vdc_in_kV(i)<<", "
			        <<setw(8)<<vsc_hvdc->get_dc_bus_area(i)<<", "
			        <<setw(8)<<vsc_hvdc->get_dc_bus_zone(i)<<", "
			        <<"\""<<left
			        <<setw(8)<<vsc_hvdc->get_dc_bus_name(i)<<"\""<<", "
			        <<right
			        <<setw(8)<<setprecision(4)<<vsc_hvdc->get_dc_bus_ground_resistance_in_ohm(i)<<", "
			        <<setw(8)<<vsc_hvdc->get_dc_bus_owner_number(i)<<", "
			        <<setw(8)<<vsc_hvdc->get_dc_bus_generation_power_in_MW(i)<<", "
			        <<setw(8)<<vsc_hvdc->get_dc_bus_load_power_in_MW(i)<<endl;
		}
		for(size_t i=0; i!=ndc_line; ++i)
		{
			osstream<<setw(8)<<vsc_hvdc->get_dc_line_sending_side_bus(i)<<", "
			        <<setw(8)<<vsc_hvdc->get_dc_line_receiving_side_bus(i)<<", "
			        <<"\""
			        <<left
			        <<setw(8)<<vsc_hvdc->get_dc_line_identifier(i)
			        <<"\""<<", "
			        <<right
			        <<setw(8)<<vsc_hvdc->get_dc_line_meter_end_bus(i)<<", "
			        <<setw(8)<<vsc_hvdc->get_dc_line_meter_end_bus(i)<<", "
			        <<setw(8)<<setprecision(4)<<fixed<<vsc_hvdc->get_dc_line_resistance_in_ohm(i)<<", "
			        <<setw(8)<<setprecision(4)<<fixed<<vsc_hvdc->get_dc_line_inductance_in_mH(i)<<endl;
		}

	}
	return osstream.str();
}

string PSSE_IMEXPORTER::export_all_transformer_impedance_correction_table_data() const
{
	return "";
}

string PSSE_IMEXPORTER::export_all_multi_terminal_hvdc_data() const
{
	return "";
}

string PSSE_IMEXPORTER::export_all_multi_section_line_data() const
{
	return "";
}

string PSSE_IMEXPORTER::export_all_zone_data() const
{
	ostringstream osstream;
	iSTEPS& toolkit = get_toolkit();
	PF_DATA& psdb = toolkit.get_database();

	//vector<ZONE*> zones = psdb.get_all_zones();
	//size_t n = zones.size();

	vector<size_t> zones_number = psdb.get_all_zones_number();
	sort(zones_number.begin(), zones_number.end());
	vector<ZONE*> zones;
	size_t n = zones_number.size();
	for(size_t i=0; i!=n; ++i)
		zones.push_back(psdb.get_zone(zones_number[i]));

	for(size_t i=0; i!=n; ++i)
		osstream<<export_zone_data(zones[i]);

	return osstream.str();
}

string PSSE_IMEXPORTER::export_zone_data(const ZONE* zone) const
{
	ostringstream osstream;

	osstream<<setw(8)<<zone->get_zone_number()<<", "
	        <<"\""<<zone->get_zone_name()<<"\""<<endl;

	return osstream.str();
}

string PSSE_IMEXPORTER::export_all_interarea_transfer_data() const
{
	return "";
}

string PSSE_IMEXPORTER::export_all_owner_data() const
{
	ostringstream osstream;
	iSTEPS& toolkit = get_toolkit();
	PF_DATA& psdb = toolkit.get_database();

	//vector<OWNER*> owners = psdb.get_all_owners();
	//size_t n = owners.size();

	vector<size_t> owners_number = psdb.get_all_owners_number();
	sort(owners_number.begin(), owners_number.end());
	vector<OWNER*> owners;
	size_t n = owners_number.size();
	for(size_t i=0; i!=n; ++i)
		owners.push_back(psdb.get_owner(owners_number[i]));

	for(size_t i=0; i!=n; ++i)
		osstream<<export_owner_data(owners[i]);

	return osstream.str();
}

string PSSE_IMEXPORTER::export_owner_data(const OWNER* owner) const
{
	ostringstream osstream;

	osstream<<setw(8)<<owner->get_owner_number()<<", "
	        <<"\""<<owner->get_owner_name()<<"\""<<endl;

	return osstream.str();
}

string PSSE_IMEXPORTER::export_all_facts_data() const
{
	return "";
}

string PSSE_IMEXPORTER::export_all_switched_shunt_data() const
{
	return "";
}


