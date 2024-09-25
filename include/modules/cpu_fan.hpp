#pragma once

#include "modules/meta/timer_module.hpp"
#include "modules/meta/types.hpp"
#include "settings.hpp"

POLYBAR_NS

namespace modules{
  enum class fan {SPEED};

  class cpu_fan_module : public timer_module<cpu_fan_module>{
    public:
      explicit cpu_fan_module(const bar_settings&, string, const config&);

      bool update();
      void teardown();
      bool build(builder* builder, const string& tag) const;

      static constexpr auto TYPE = CPU_FAN_TYPE;


    protected:
      void subthread();

    private:
      static constexpr const char* TAG_LABEL{"<label>"};
      static constexpr const char* TAG_ANIMATION_SPIN{"<animation-spin>"};


      label_t m_label;
      animation_t m_animation_spin;

      thread m_subthread;

  };
}

POLYBAR_NS_END

