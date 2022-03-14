
// Created by geray on 2022/2/19.
//

#ifndef SYLAR_CONFIG_H
#define SYLAR_CONFIG_H
#include <yaml-cpp/yaml.h>

#include <boost/lexical_cast.hpp>
#include <memory>
#include <sstream>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include "log.h"
namespace sylar {
class ConfigVarBase {
 public:
  typedef std::shared_ptr<ConfigVarBase> ptr;
  ConfigVarBase(const std::string& mName, const std::string& mDescription = "");
  const std::string& getName() const;
  const std::string& getDescription() const;
  virtual ~ConfigVarBase();
  virtual std::string toString() = 0;
  virtual bool fromString(const std::string& val) = 0;
  virtual std::string getTypeName() const = 0;

 private:
  std::string m_name;
  std::string m_description;
};

// F : From type
// T : To type
// function: change type F to type T
template <class F, class T>
class LexicalCast {
 public:
  // Cast type F to T, when T is 'int','float'....,basic type etc.
  T operator()(const F& v) { return boost::lexical_cast<T>(v); }
};
// Cast string to Vector<T>
template <class T>
class LexicalCast<std::string, std::vector<T>> {
 public:
  std::vector<T> operator()(const std::string& v) {
    YAML::Node node = YAML::Load(v);
    typename std::vector<T> vec;
    std::stringstream ss;
    for (size_t i = 0; i < node.size(); ++i) {
      ss.str("");


      ss << node[i];
      vec.push_back(typename sylar::LexicalCast<std::string, T>()(ss.str()));
    }
    return vec;
  }
};
// Cast string to Vector<T>
template <class T>
class LexicalCast<std::vector<T>, std::string> {
 public:
  std::string operator()(const std::vector<T>& v) {
    YAML::Node node;
    for (auto& i : v) {
      node.push_back(typename sylar::LexicalCast<T, std::string>()(i));
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
  }
};
template <class T>
class LexicalCast<std::string, std::list<T>> {
 public:
  std::list<T> operator()(const std::string& v) {
    YAML::Node node = YAML::Load(v);
    typename std::list<T> lis;
    std::stringstream ss;
    for (size_t i = 0; i < node.size(); ++i) {
      ss.str("");
      ss << node[i];
      lis.push_back(typename sylar::LexicalCast<std::string, T>()(ss.str()));
    }
    return lis;
  }
};
template <class T>
class LexicalCast<std::list<T>, std::string> {
 public:
  std::string operator()(const std::list<T>& v) {
    YAML::Node node;
    for (auto& i : v) {
      node.push_back(typename sylar::LexicalCast<T, std::string>()(i));
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
  }
};
template <class T>
class LexicalCast<std::string, std::set<T>> {
 public:
  std::set<T> operator()(const std::string& v) {
    YAML::Node node = YAML::Load(v);
    typename std::set<T> st;
    std::stringstream ss;
    for (size_t i = 0; i < node.size(); ++i) {
      ss.str("");
      ss << node[i];
      st.insert(typename sylar::LexicalCast<std::string, T>()(ss.str()));
    }
    return st;
  }
};
template <class T>
class LexicalCast<std::set<T>, std::string> {
 public:
  std::string operator()(const std::set<T>& v) {
    YAML::Node node;
    for (auto& i : v) {
      node.push_back(typename sylar::LexicalCast<T, std::string>()(i));
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
  }
};
template <class T>
class LexicalCast<std::string, std::unordered_set<T>> {
 public:
  std::unordered_set<T> operator()(const std::string& v) {
    YAML::Node node = YAML::Load(v);
    typename std::unordered_set<T> st;
    std::stringstream ss;
    for (size_t i = 0; i < node.size(); ++i) {
      ss.str("");
      ss << node[i];
      st.insert(typename sylar::LexicalCast<std::string, T>()(ss.str()));
    }
    return st;
  }
};
template <class T>
class LexicalCast<std::unordered_set<T>, std::string> {
 public:
  std::string operator()(const std::unordered_set<T>& v) {
    YAML::Node node;
    for (auto& i : v) {
      node.push_back(typename sylar::LexicalCast<T, std::string>()(i));
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
  }
};

template <class T>
class LexicalCast<std::string, std::map<std::string,T>> {
 public:
  std::map<std::string,T> operator()(const std::string& v) {
    YAML::Node node = YAML::Load(v);
    typename std::map<std::string,T> mp;
    std::stringstream ss;
    for(auto it = node.begin(); it != node.end(); ++it){
      ss.str("");
      ss<<it->second;
      mp.insert(std::make_pair(it->first.Scalar(),
                               LexicalCast<std::string,T>()(ss.str())));
    }
    return mp;
  }
};
template <class T>
class LexicalCast<std::map<std::string,T>, std::string> {
 public:
  std::string operator()(const std::map<std::string,T>& v) {
    YAML::Node node;
    for (auto& i : v) {
      node[i.first] = YAML::Load(LexicalCast<T,std::string>()(i.second));
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
  }
};

// FromStr T operator()(const std::string)
// ToStr std::string()(const T &)
template <class T, class FromStr = LexicalCast<std::string, T>,
          class ToStr = LexicalCast<T, std::string>>
class ConfigVar : public ConfigVarBase {
 public:
  typedef std::shared_ptr<ConfigVar> ptr;
  ConfigVar(const std::string& name, const T& default_value,
            const std::string& description = "")
      : ConfigVarBase(name, description), m_val(default_value){};
  std::string toString() override {
    try {
      // return boost::lexical_cast<std::string>(m_val);
      return ToStr()(m_val);
    } catch (std::exception& e) {
      SYLAR_LOG_ERROR(SYLAR_LOG_ROOT)
          << "ConfigVar::toString exception" << e.what()
          << "convert: " << typeid(m_val).name() << "to string";
    }
    return "";
  }
  bool fromString(const std::string& val) override {
    try {
      // m_val = boost::lexical_cast<T>(val);
      setVal(FromStr()(val));
    } catch (std::exception& e) {
      SYLAR_LOG_ERROR(SYLAR_LOG_ROOT)
          << "ConfigVar::toString exception" << e.what() << "convert: string to"
          << typeid(m_val).name();
    }
    return true;
  }
  const T getVal() const { return m_val; }
  void setVal(const T& v) { m_val = v; }
  std::string getTypeName()const override {return typeid(T).name();}
 private:
  T m_val;
};
class Config {
 public:
  typedef std::map<std::string, ConfigVarBase::ptr> ConfigVarMap;
  typedef std::shared_ptr<Config> ptr;
  template <class T>
  static typename ConfigVar<T>::ptr Lookup(const std::string& name,
                                           const T& default_value,
                                           const std::string description = "") {
    auto it = s_datas.find(name);
    if( it != s_datas.end()){
      auto tmp = std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
      if(tmp){
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT) << "Lookup name" << name << " exists";
        return tmp;
      }else {
        SYLAR_LOG_ERROR(SYLAR_LOG_ROOT) << "Lookup name" << name << " exists but type invalid :"<< it->second->getTypeName()
            << " "<<it->second->toString();
        return nullptr;
      }
    }
    if (name.find_first_not_of("abcdefghijklmnopqrstuvwxyz._1234567890") !=
        std::string::npos) {
      SYLAR_LOG_ERROR(SYLAR_LOG_ROOT) << "Lookup name invalid " << name;
      throw std::invalid_argument(name);
    }
    typename ConfigVar<T>::ptr v(
        new ConfigVar<T>(name, default_value, description));
    s_datas[name] = v;
    return v;
  }
  template <class T>
  static typename ConfigVar<T>::ptr Lookup(const std::string& name) {
    auto it = s_datas.find(name);
    if (it == s_datas.end()) return nullptr;
    return std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
  }
  static void LoadFromYaml(const YAML::Node& root);
  static ConfigVarBase::ptr LookupBase(const std::string& name);

 private:
  static ConfigVarMap s_datas;
};
}  // namespace sylar
#endif  // SYLAR_CONFIG_H
