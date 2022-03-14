//
// Created by geray on 2022/2/19.
//
#include "config.h"
namespace sylar {
Config::ConfigVarMap Config::s_datas;
ConfigVarBase::ConfigVarBase(const std::string& mName,
                             const std::string& mDescription)
    : m_name(mName), m_description(mDescription) {
  std::transform(m_name.begin(),m_name.end(),m_name.begin(),::tolower);
}
ConfigVarBase::~ConfigVarBase() {}
const std::string& ConfigVarBase::getName() const { return m_name; }
const std::string& ConfigVarBase::getDescription() const {
  return m_description;
}
static void ListAllMember(const std::string& prefix,
                          const YAML::Node& node,
                          std::list<std::pair<std::string,const YAML::Node>> &output){
  if(prefix.find_first_not_of("abcdefghijklmnopqrstuvwxyz._1234567890") != std::string::npos){
    SYLAR_LOG_ERROR(SYLAR_LOG_ROOT)<<"Config invalid name :" <<prefix << " : ";
    return ;
  }
  output.push_back(std::make_pair(prefix,node));
  if(node.IsMap()){
    for(auto it = node.begin();it != node.end() ; it++){
      ListAllMember(prefix.empty()?it->first.Scalar() : prefix+"."+it->first.Scalar(),it->second,output);
    }
   }
}
//"A.B", 10
//A:
//  B: 10
//  C: str
void Config::LoadFromYaml(const YAML::Node& root) {
  std::list<std::pair<std::string,const YAML::Node> >all_nodes;
  ListAllMember("",root,all_nodes);
  for(auto &i : all_nodes){
    std::string key = i.first;
    if(key.empty()){
      continue;
    }
    std::transform(key.begin(),key.end(),key.begin(),::tolower);
    ConfigVarBase::ptr var = LookupBase(key);
    if(var){
      if(i.second.IsScalar()){
        var->fromString(i.second.Scalar());
      }else{
        std::stringstream  ss;
        ss<< i.second;
        var->fromString(ss.str());
      }
    }
  }
}
ConfigVarBase::ptr Config::LookupBase(const std::string& name) {
  auto it = s_datas.find(name);
  return it==s_datas.end()? nullptr : it->second;
}
}
