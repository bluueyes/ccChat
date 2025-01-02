#pragma once
#include "const.h"

struct SectionInfo {
	SectionInfo() {}
	~SectionInfo() {
		_section_data.clear();
	}

	SectionInfo& operator=(const SectionInfo& src) {
		if (&src == this) {
			return *this;
		}

		this->_section_data = src._section_data;
		return *this;
	}

	std::string operator[](const std::string& str) {

		if (_section_data.find(str) == _section_data.end()) {
			return "";
		}
		return _section_data[str];
	}


	std::map<std::string, std::string> _section_data;
};

class ConfigMgr
{
public:
	~ConfigMgr() {
		_config_map.clear();
	}

	ConfigMgr();

	ConfigMgr(const ConfigMgr& src) {
		_config_map = src._config_map;
	}

	ConfigMgr& operator=(const ConfigMgr& src) {
		if (this == &src) {
			return *this;
		}
		_config_map = src._config_map;
		return *this;
	}

	SectionInfo operator[](const std::string& src) {
		if (_config_map.find(src) == _config_map.end()) {
			return SectionInfo();
		}

		return _config_map[src];
	}

	static ConfigMgr& Inst() {

		static ConfigMgr cfg_mgr;
		return cfg_mgr;
	}

private:
	std::map<std::string, SectionInfo> _config_map;
};

