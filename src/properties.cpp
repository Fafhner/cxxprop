#include "../include/cxxprop/properties.hpp"

#include <unordered_map>
#include <list>
#include <string>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <iostream>
#include <sstream>
#include <set>

namespace cp {


	Properties::Properties(char group_join, Overwrite lock) :
		group_join(group_join), lock(lock), parent(nullptr), group_name("") {
		};

	Properties::~Properties() {
		this->properties.clear();
		this->groups.clear();
		this->parent = nullptr;
	};

	const std::list<std::string> Properties::toGroups(std::string key) const {
		std::list<std::string>  init;
		size_t s_pos = 0;
		size_t f_pos = key.size();

		while (f_pos != std::string::npos) {
			f_pos = key.find(this->group_join, s_pos);

			if (f_pos == std::string::npos) {
				init.push_back(key.substr(s_pos, key.size() - s_pos));
			}
			else {
				init.push_back(key.substr(s_pos, f_pos - s_pos));
				s_pos = f_pos + 1;
			}

		}
		return init;
	}

	inline bool Properties::hasGroups(std::string key) const
	{
		if (key.find(this->group_join, 0) != std::string::npos) {
			return true;
		}
		return false;
	}

	void Properties::setParent(Properties * prop) {
		this->parent = prop;
	}


	inline std::unordered_map<std::string, std::string>::iterator Properties::begin() noexcept {
		return this->properties.begin();
	}

	inline std::unordered_map<std::string, std::string>::const_iterator Properties::cbegin() const {
		return this->properties.cbegin();
	}

	inline std::unordered_map<std::string, std::string>::iterator Properties::end() noexcept {
		return this->properties.end();
	}

	inline std::unordered_map<std::string, std::string>::const_iterator Properties::cend() const {
		return this->properties.cend();
	}

	std::string Properties::operator[](const std::string& key) {
		return this->get(key);
	};

	std::string Properties::operator[](const char* key) {
		return this->get(static_cast<std::string>(key));
	};

	void Properties::addBranch(const char* branch_names, std::shared_ptr<Properties> prop) {
		this->addBranch(this->toGroups(branch_names), prop);
	}

	void Properties::addBranch(const std::string& branch_names, std::shared_ptr<Properties> prop) {
		this->addBranch(this->toGroups(branch_names), prop);
	}

	

	std::string Properties::asFullGroupPath(const std::string& key) const {
		std::string gp{ this->getGroupPath() };
		if (gp.empty()) {
			return key;
		}
		else {
			return gp + this->group_join + key;
		}

	}

	Properties& Properties::cutOff(const std::string& path, bool clear_name) {
		Properties & prop = this->getByGroup(path);
		prop.parent->groups.extract(prop.group_name);
		prop.parent = nullptr;
		if(clear_name) { prop.group_name = ""; }
		return prop;
	}

	Properties& Properties::createGroups(const char* groups_name, bool chain) {
		return this->createGroups(this->toGroups(groups_name), chain);
	}

	
	std::unordered_map<std::string, std::string> Properties::getAll() {
		std::unordered_map<std::string, std::string> prop;
		for (auto&& p : this->properties) {
			prop.insert({ this->asFullGroupPath(p.first), p.second });
		}

		for (auto&& g : this->groups) {
			for (auto&& kv : g.second->getAll()) {
				prop.insert(kv);
			}
		}

		return prop;
	}

	Properties& Properties::getByGroup(const char* group_name) {
		return this->getByGroup(std::string{ group_name });
	}

	Properties& Properties::getByGroup(const std::string& group_name) {
		if (this->hasGroups(group_name) == true) {
			return this->getByGroup(this->toGroups(group_name));
		}
		return *this->groups.at(group_name);
	}

	std::set<std::string> Properties::getPropertyKeys() const {
		std::set<std::string> keys;
		for (auto&& kv : this->properties) {
			keys.insert(kv.first);
		}
		return keys;
	}

	bool  Properties::groupExist(const char* group_name) const {
		return this->groups.count(group_name) > 0;
	}

	bool  Properties::groupExist(const std::string& group_name) const {
		return this->groups.count(group_name) > 0;
	}

	

	std::string Properties::getGroupPath() const {
		std::string p = this->group_name;
		if (this->parent == nullptr || this->parent->getGroupPath() == "") {
			return this->group_name;
		}
		else {
			return this->parent->getGroupPath() + this->group_join + this->group_name;
		}
	}

	std::string Properties::get(const std::string& key) {
		std::list<std::string> tokens{ this->toGroups(key) };

		if (tokens.size() == 1) {
			return this->properties.at(tokens.back());
		}

		std::string key_value = tokens.back();
		tokens.pop_back();

		return this->getByGroup(tokens).get(key_value);
	}

	Properties& Properties::insert(const std::string& key, const char* value) {
		std::string v(value);
		return this->insert(key, v);
	}

	Properties& Properties::ret(size_t times) {
		Properties* prop = this;
		while (times > 0) {
			if (prop->parent == nullptr) {
				times = 1;
			}
			else {
				prop = prop->parent;
			}
			times--;
		}
		return *prop;
	}

	Properties& Properties::getRoot() {
		return this->ret(SIZE_MAX);
	}

	bool Properties::propertyExist(std::string key) {
		auto & keys = this->getPropertyKeys();
		if(keys.find(key) != keys.end()){
			return true;
		}
		else {
			return false;
		}
	}

	bool Properties::propertyExist(const char * key) {
		return this->propertyExist(std::string(key));
	}

}