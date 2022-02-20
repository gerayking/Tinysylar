#include "../sylar/config.h"
#include "../sylar/log.h"
#include <yaml-cpp/yaml.h>
sylar::ConfigVar<int>::ptr g_int_value_config = sylar::Config::Lookup("system.port",(int)8080,"system port");
sylar::ConfigVar<int>::ptr g_float_value_config = sylar::Config::Lookup("system.value",10,"system value");
sylar::ConfigVar<std::vector<int> >::ptr g_int_vec_value_config = sylar::Config::Lookup("system.int_vec",std::vector<int>(2,2),"system int value");
void print_yaml(const YAML::Node& node,int level){
  if(node.IsScalar()){
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT)<<std::string(level*4,' ')<<node.Scalar()<<" - "<<node.Type()<<" - "<<level;
  }else if(node.IsNull()){
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT)<<std::string(level*4,' ')<<"NULL - "<<node.Type()<<" - "<<level;
  }else if(node.IsMap()){
    for(auto it = node.begin(); it != node.end() ; ++it){
      SYLAR_LOG_INFO(SYLAR_LOG_ROOT)<<std::string(level*4,' ')<<it->first<<" - "<<it->second.Type()<<" - "<<level;
      print_yaml(it->second,level+1);
    }
  }else if(node.IsSequence()){
    for(size_t i = 0; i < node.size(); i++){
      SYLAR_LOG_INFO(SYLAR_LOG_ROOT)<<std::string(level*4,' ')<<i<<" - "<<node[i].Type()<<" - "<<level;
      print_yaml(node[i] , level+1);
    }
  }
}
void test_yaml(){
  YAML::Node root = YAML::LoadFile("/home/geray/cplusplus/workspace/bin/log.yaml");
  print_yaml(root,0);
  SYLAR_LOG_INFO(SYLAR_LOG_ROOT)<<root;
}
void test_config(){
  SYLAR_LOG_INFO(SYLAR_LOG_ROOT)<<"Before :"<<g_int_value_config->getVal();
  SYLAR_LOG_INFO(SYLAR_LOG_ROOT)<<"Before :"<<g_float_value_config->toString();
  auto &v = g_int_vec_value_config->getVal();
  for(auto it : v){
    SYLAR_LOG_INFO(sylar::LoggerMgr::GetInstance()->getRoot())<<"Before : "<<it;
  }
  YAML::Node root = YAML::LoadFile("/home/geray/cplusplus/workspace/bin/log.yaml");
  sylar::Config::LoadFromYaml(root);
  SYLAR_LOG_INFO(SYLAR_LOG_ROOT)<<"after :"<<g_int_value_config->getVal();
  SYLAR_LOG_INFO(SYLAR_LOG_ROOT)<<"after :"<<g_float_value_config->toString();
  auto vi = g_int_vec_value_config->getVal();
  for(auto it : vi){
    SYLAR_LOG_INFO(sylar::LoggerMgr::GetInstance()->getRoot())<<"After : "<<it;
  }
}


int main(int argc,char** argv){
  test_config();

  return 0;
}
