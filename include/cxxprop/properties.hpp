#ifndef PROPERTIES_HPP
#define PROPERTIES_HPP

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

	enum class Overwrite {
		LOCKED = 0,
		SOFTLOCKED = 1,
		UNLOCKED = 2
	};

	class Properties {
	protected:
		std::unordered_map<std::string, std::string> properties;
		std::unordered_map<std::string, std::shared_ptr<cp::Properties>> groups;
		Properties * parent;
		std::string group_name;
		cp::Overwrite lock = Overwrite::UNLOCKED;
		char group_join = '.';

		const std::list<std::string> toGroups(std::string key) const;
		bool hasGroups(std::string key) const;
		void setParent(Properties * prop);
	public:
		Properties(char group_join = '.', Overwrite lock = Overwrite::UNLOCKED);
		virtual ~Properties();

		std::string operator[](const std::string& key);
		std::string operator[](const char* key);
		inline std::unordered_map<std::string, std::string>::iterator begin() noexcept;
		inline std::unordered_map<std::string, std::string>::const_iterator cbegin() const;
		inline std::unordered_map<std::string, std::string>::iterator end() noexcept;
		inline std::unordered_map<std::string, std::string>::const_iterator cend() const;

		void addBranch(const char* branch_names, std::shared_ptr<Properties> prop);
		void addBranch(const std::string& branch_name, std::shared_ptr<Properties> prop);
		template <class T = std::initializer_list<std::string>>
		void addBranch(const T& branch_names, std::shared_ptr<Properties> prop);
		std::string asFullGroupPath(const std::string& key) const;
		Properties& cutOff(const std::string& path, bool clear_name=false);
		Properties& createGroups(const char* groups_name, bool chain = true);
		template <class T = std::initializer_list<std::string>>
		Properties& createGroups(const T& groups_name, bool chain = true);
		std::unordered_map<std::string, std::string> getAll();
		Properties& getByGroup(const char* group_name);
		Properties& getByGroup(const std::string& group_name);
		template <class T = std::initializer_list<std::string>>
		Properties& getByGroup(const T& groups_name);
		inline std::string getGroupName();
		std::set<std::string> getPropertyKeys() const;
		bool groupExist(const char* group_name) const;
		bool groupExist(const std::string& group_name) const;
		template <class T = std::initializer_list<std::string>>
		bool groupExist(const T& groups_name) const;
		std::string getGroupPath() const;
		std::string get(const std::string& key);
		Properties& insert(const std::string& key, const char* value);
		template<class T = std::string>
		Properties& insert(const std::string& key, const T& value);
		template <class T = std::initializer_list<std::initializer_list<std::string>>>
		Properties& insert(const T& kv_lists);
		Properties& ret(size_t times = 1);
		Properties& getRoot();
		bool propertyExist(std::string key);
		bool propertyExist(const char * key);
		inline void renameGroup(const std::string& new_name);
		inline void setGroupJoinChar(char k);
		inline void setLock(Overwrite lock);
		inline size_t size() const;
	};

	template <class T>
	void Properties::addBranch(const T& branch_names, std::shared_ptr<Properties> prop) {
		std::list<std::string> names{ branch_names };
		if (names.size() == 1) {
			std::string key(names.back());
			if (this->lock != Overwrite::UNLOCKED && this->properties.count(key) > 0) {
				if (this->lock == Overwrite::LOCKED) {
					std::stringstream err;
					err << "Unable to overwrite group with key: " << this->asFullGroupPath(key) << '\n';
					throw err.str();
				}
			}
			else {
				std::string gname = names.back();
				this->groups.insert({ gname, prop});
				this->groups.at(gname)->group_name = gname;
				this->groups.at(gname)->setParent(this);
			}

		}
		else if (names.size() > 1) {
			std::string gname = names.back();
			names.pop_back();
			this->getByGroup(names).addBranch(gname, prop);
		}
	}


	template <class T>
	Properties& Properties::createGroups(const T& groups_name, bool chain) {
		std::list<std::string> groups{ groups_name };
		Properties * prop = this;
		for (auto group_name : groups) {
			if(prop->groupExist(group_name) == false) {
				std::shared_ptr<Properties> new_prop(new Properties(this->group_join, this->lock));
				new_prop->group_name = group_name;
				prop->groups.insert({ group_name,  new_prop});
				new_prop->setParent(prop);
				
			}
			if (chain == true) {
				prop = &prop->getByGroup(group_name);
			}
		}
		groups.clear();
		return *prop;
	}


	template <class T>
	Properties& Properties::getByGroup(const T& groups_name) {
		Properties* prop = this;
		std::list<std::string> s{ groups_name };

		for (auto&& group_name : s) {
			prop = &prop->getByGroup(group_name);
		}
		return *prop;
	}

	inline std::string Properties::getGroupName() {
		return this->group_name;
	}


	template<class T>
	bool Properties::groupExist(const T& groups_name) const {
		Properties* prop = this;
		for (auto&& group_name : groups_name) {
			if (prop->groupExist(group_name)) {
				prop = &this->getByGroup(group_name);
			}
			else {
				prop = nullptr;
				return false;
			}

		}
		return true;
	}


	template<class T>
	Properties& Properties::insert(const std::string& key, const T& value) {
		std::list<std::string> tokens{ this->toGroups(key) };
		std::stringstream s;
		s << value;
		std::string v{ s.str() };

		if (tokens.size() == 1) {
			if (this->lock != Overwrite::UNLOCKED && this->properties.count(key) > 0) {
				if (this->lock == Overwrite::LOCKED) {
					std::stringstream err;
					err << "Unable to overwrite property with key: " << this->asFullGroupPath(key) << '\n';
					throw err.str();
				}
			}
			else {
				this->properties.insert({ key, v });
			}

		}
		else {
			std::string key_value = tokens.back();
			tokens.pop_back();
			this->createGroups(tokens).insert(key_value, v);
		}

		return *this;
	}


	template <class T>
	Properties& Properties::insert(const T& kv_lists) {
		for (auto&& kv_pair : kv_lists) {
			if (kv_pair.size() != 2) {
				std::stringstream err;
				err << "Got " << kv_pair.size() << "values, expected a pair of {key, value}";
				throw err.str();
			}

			this->insert(*kv_pair.begin(),      // key
				*(kv_pair.begin() + 1));  // value
		}
		return *this;
	}

	inline void Properties::renameGroup(const std::string& new_name) {
		this->group_name = new_name;
	}

	inline void Properties::setGroupJoinChar(char k) {
		this->group_join = k;
	}

	inline void Properties::setLock(Overwrite lock) {
		this->lock = lock;
	}

	inline size_t Properties::size() const {
		return this->properties.size();
	}
}
#endif