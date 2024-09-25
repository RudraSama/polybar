#include "fstream"

#include "modules/cpu_fan.hpp"
#include "modules/meta/base.inl"
#include "drawtypes/label.hpp"
#include "drawtypes/animation.hpp"

POLYBAR_NS

namespace modules {
  template class module<cpu_fan_module>;

  cpu_fan_module::cpu_fan_module(const bar_settings& bar, string name_, const config& config) : timer_module<cpu_fan_module>(bar, move(name_), config){


    m_formatter->add(DEFAULT_FORMAT, TAG_LABEL, {TAG_LABEL, TAG_ANIMATION_SPIN});

    if(m_formatter->has(TAG_LABEL)){
      m_label = load_optional_label(m_conf, name(), TAG_LABEL, "%fan_rpm%%");
    }

    if(m_formatter->has(TAG_ANIMATION_SPIN)){
      m_animation_spin = load_animation(m_conf, name(), TAG_ANIMATION_SPIN);

    }
  }


  bool cpu_fan_module::update(){
    //to call subthread for single time
    if(!m_subthread.joinable()){
      m_subthread = thread(&cpu_fan_module::subthread, this);
    }

    //cpu fan speed can't go over 12000 RPM
    unsigned short fan_rpm{0U};


    try{
      std::ifstream cpufaninfo(PATH_CPU_FAN);

      std::string line;

      while(std::getline(cpufaninfo, line)){
        fan_rpm = std::stoi(line);
      }

    }catch (const std::exception& err){
      m_log.err("Failed to read CPU FAN values (what: %s)", err.what());
    }

    const auto replace_tokens = [&](label_t& label){
      label->reset_tokens();
      label->replace_token("%fan_rpm%", std::to_string(fan_rpm));
    };

    if(m_label){
      replace_tokens(m_label);
    }

    return true;
  }


  //clean up subthead when module is stopped
  void  cpu_fan_module::teardown(){
    if(m_subthread.joinable()){
      m_subthread.join();
    }
  }



  bool cpu_fan_module::build(builder* builder, const string& tag) const {
    if(tag == TAG_ANIMATION_SPIN){
      builder->node(m_animation_spin->get());
    }
    else if(tag == TAG_LABEL){
      builder->node(m_label);
    }
    else{
      return false;
    }

    return true;
  }




  void cpu_fan_module::subthread(){
    m_log.trace("%s : Start of subthread", name());

    while(running()){
      auto now = chrono::steady_clock::now();
      auto framerate = 1000U; //milliseconds

      if(m_animation_spin){
        m_animation_spin->increment();
        broadcast();
        framerate = m_animation_spin->framerate();
      }


      now += chrono::milliseconds(framerate);
      this_thread::sleep_until(now);

    }

    m_log.trace("%s : End of subthread", name());

  }

}



POLYBAR_NS_END
