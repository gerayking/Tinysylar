#include <yaml-cpp/yaml.h>

#include "../sylar/config.h"
#include "../sylar/log.h"
sylar::ConfigVar<int>::ptr g_int_value_config =
    sylar::Config::Lookup("system.port", (int)8080, "system port");
sylar::ConfigVar<int>::ptr g_float_value_config =
    sylar::Config::Lookup("system.value", 10, "system value");
sylar::ConfigVar<std::vector<int>>::ptr g_int_vec_value_config =
    sylar::Config::Lookup("system.int_vec", std::vector<int>(2, 2),
                          "system int value");
sylar::ConfigVar<std::list<int> >::ptr g_int_list_value_config =
    sylar::Config::Lookup("system.int_list", std::list<int>(2, 10),
                          "system int value");
sylar::ConfigVar<std::set<int> >::ptr g_int_set_value_config =
    sylar::Config::Lookup("system.int_set", std::set<int>{1, 2},
                          "system int value");
sylar::ConfigVar<std::unordered_set<int> >::ptr
    g_int_unordered_set_value_config = sylar::Config::Lookup(
        "system.int_unordered_set", std::unordered_set<int>{1, 2},
        "system int value");
sylar::ConfigVar<std::map<std::string, int> >::ptr g_str_int_map_value_config =
    sylar::Config::Lookup("system.str_int_map",
                          std::map<std::string, int>{{"k", 2}},
                          "system std_int map value");

void print_yaml(const YAML::Node& node, int level) {
  if (node.IsScalar()) {
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT)
        << std::string(level * 4, ' ') << node.Scalar() << " - " << node.Type()
        << " - " << level;
  } else if (node.IsNull()) {
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT) << std::string(level * 4, ' ') << "NULL - "
                                   << node.Type() << " - " << level;
  } else if (node.IsMap()) {
    for (auto it = node.begin(); it != node.end(); ++it) {
      SYLAR_LOG_INFO(SYLAR_LOG_ROOT)
          << std::string(level * 4, ' ') << it->first << " - "
          << it->second.Type() << " - " << level;
      print_yaml(it->second, level + 1);
    }
  } else if (node.IsSequence()) {
    for (size_t i = 0; i < node.size(); i++) {
      SYLAR_LOG_INFO(SYLAR_LOG_ROOT)
          << std::string(level * 4, ' ') << i << " - " << node[i].Type()
          << " - " << level;
      print_yaml(node[i], level + 1);
    }
  }
}
void test_yaml() {
  YAML::Node root =
      YAML::LoadFile("/home/geray/cplusplus/workspace/bin/log.yaml");
  print_yaml(root, 0);
  SYLAR_LOG_INFO(SYLAR_LOG_ROOT) << root;
}
void test_config() {
  SYLAR_LOG_INFO(SYLAR_LOG_ROOT) << "Before :" << g_int_value_config->getVal();
  SYLAR_LOG_INFO(SYLAR_LOG_ROOT)
      << "Before :" << g_float_value_config->toString();
#ifndef XX
#define XX(g_val, name, prefix)                                      \
  {                                                                  \
    auto& v = g_val->getVal();                                       \
    for (auto& i : v) {                                              \
      SYLAR_LOG_INFO(SYLAR_LOG_ROOT) << #prefix "" #name " : " << i; \
    }                                                                \
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT) << g_val->toString();             \
  }

#define XX_M(g_val, name, prefix)                                              \
  {                                                                            \
    auto& v = g_val->getVal();                                                 \
    for (auto& i : v) {                                                        \
      SYLAR_LOG_INFO(SYLAR_LOG_ROOT)                                           \
          << #prefix "" #name " : {" << i.first << " - " << i.second << "}"; \
    }                                                                          \
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT)<< #prefix "" #name " yaml: "<<g_val->toString();               \
  }
  XX(g_int_vec_value_config, int_vec, before)
  XX(g_int_list_value_config, int_list, before)
  XX(g_int_set_value_config, int_set, before)
  XX(g_int_unordered_set_value_config, int_unordered_set, before)
  XX_M(g_str_int_map_value_config,int_map,before)

  YAML::Node root =
      YAML::LoadFile("/home/geray/cplusplus/workspace/bin/log.yaml");
  sylar::Config::LoadFromYaml(root);
  SYLAR_LOG_INFO(SYLAR_LOG_ROOT) << "after :" << g_int_value_config->getVal();
  SYLAR_LOG_INFO(SYLAR_LOG_ROOT)
      << "after :" << g_float_value_config->toString();
  XX(g_int_vec_value_config, int_vec, after)
  XX(g_int_list_value_config, int_list, after);
  XX(g_int_set_value_config, int_set, after)
  XX(g_int_unordered_set_value_config, int_unordered_set, after)
  XX_M(g_str_int_map_value_config,int_map,after)

#endif
}
class Person{
 public:
  std::string m_name;
  int m_age = 0;
  bool m_sex = 0;
  std::string toString()const{
    std::stringstream ss;
    ss<<"[Person name="<<m_name
    << " age="<<m_age
    << " sex="<<m_sex
    << "]";
    return ss.str();
  }
};
sylar::ConfigVar<Person>::ptr g_person = sylar::Config::Lookup("class.person",Person(),"system person");
sylar::ConfigVar<std::map<std::string,Person>>::ptr g_person_map = sylar::Config::Lookup("class.map",std::map<std::string,Person>(),"system person");
sylar::ConfigVar<std::map<std::string,std::vector<Person>>>::ptr g_person_map_vector = sylar::Config::Lookup("class.vec_map",std::map<std::string,std::vector<Person>>(),"system person");

namespace sylar{
template <>
class LexicalCast<std::string, Person> {
 public:
  Person operator()(const std::string& v) {
    YAML::Node node = YAML::Load(v);
    Person p;
    p.m_name = node["name"].as<std::string>();
    p.m_age = node["age"].as<int>();
    p.m_sex = node["sex"].as<bool>();
    return p;
  }
};
template <>
class LexicalCast<Person, std::string> {
 public:
  std::string operator()(const Person& v) {
    YAML::Node node;
    std::stringstream ss;
    node["name"]=v.m_name;
    node["age"] = v.m_age;
    node["sex"] = v.m_sex;
    ss << node;
    return ss.str();
  }
};
}
void  test_class(){
  SYLAR_LOG_INFO(SYLAR_LOG_ROOT)<<"before : "<<g_person->getVal().toString()<<" - "<<g_person->toString();
#define XX_PM(g_val,prefix) \
  {                          \
  auto m = g_val->getVal();  \
    for(auto &i : m){        \
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT)<<prefix<<" : "<<i.first<<" - "<<i.second.toString(); \
  }                         \
  SYLAR_LOG_INFO(SYLAR_LOG_ROOT)<<prefix<<": size = "<<m.size();                          \
  }
  XX_PM(g_person_map,"before")
  YAML::Node root =

      YAML::LoadFile("/home/geray/cplusplus/workspace/bin/log.yaml");
  sylar::Config::LoadFromYaml(root);
  SYLAR_LOG_INFO(SYLAR_LOG_ROOT)<<"after : "<<g_person->getVal().toString()<<" - "<<g_person->toString();
  SYLAR_LOG_INFO(SYLAR_LOG_ROOT)<<"after : "<<g_person_map_vector->toString();
  XX_PM(g_person_map,"after")
}
int main(int argc, char** argv) {
//  test_config();
  test_class();
  return 0;
}
