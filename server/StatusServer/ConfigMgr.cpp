#include "ConfigMgr.h"
#include "const.h"

ConfigMgr::ConfigMgr()
{
	//获取当前目录
	boost::filesystem::path current_path = boost::filesystem::current_path();

	//构造config.ini文件完整路径
	boost::filesystem::path config_path = current_path / "config.ini";
	std::cout << "config path: " << config_path << std::endl;

	//使用boost.PropertyTree来读取INI文件
	boost::property_tree::ptree pt;
	boost::property_tree::read_ini(config_path.string(), pt);

	// 遍历INI文件中的所有section  
	for (const auto& section_pair : pt) {
		const std::string& section_name = section_pair.first;
		const boost::property_tree::ptree& section_tree = section_pair.second;

		SectionInfo sectionInfo;
		for (const auto& key_value_pair : section_tree) {
			const std::string& key = key_value_pair.first;
			const std::string& value = key_value_pair.second.get_value<std::string>();
			
			sectionInfo._section_data[key] = value;
		}

		_config_map[section_name] = sectionInfo;
	}

	//输出所有的section和Key-value
	for (const auto& section_entry : _config_map) {
		const std::string& section_name = section_entry.first;
		SectionInfo sectioninfo = section_entry.second;
		std::cout << "[" << section_name << "]" << std::endl;

		for (const auto& key_value_pair : sectioninfo._section_data) {
			std::cout << key_value_pair.first << "=" << key_value_pair.second << std::endl;

		}
	}
}
