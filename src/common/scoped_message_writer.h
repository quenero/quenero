// Copyright (c) 2014-2019, The Monero Project
// 
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//    of conditions and the following disclaimer in the documentation and/or other
//    materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#pragma once

#include "epee/misc_log_ex.h"
#include "epee/readline_suspend.h"
#include <iostream>

#include "common/quenero_integration_test_hooks.h"

namespace tools
{

/************************************************************************/
/*                                                                      */
/************************************************************************/
class scoped_message_writer
{
private:
  bool m_flush;
  std::ostringstream m_oss;
  epee::console_colors m_color;
  bool m_bright;
  el::Level m_log_level;
public:
  scoped_message_writer(
      epee::console_colors color = epee::console_color_default
    , bool bright = false
    , std::string prefix = {}
    , el::Level log_level = el::Level::Info
    )
    : m_flush(true)
    , m_color(color)
    , m_bright(bright)
    , m_log_level(log_level)
  {
#if defined(QUENERO_ENABLE_INTEGRATION_TEST_HOOKS)
    m_color = epee::console_color_default; // NOTE(quenero): No ANSI color codes in the output. Makes parsing harder.
#endif
    m_oss << prefix;
  }

  scoped_message_writer(scoped_message_writer&& rhs)
    : m_flush(std::move(rhs.m_flush))
    , m_oss(std::move(rhs.m_oss))
    , m_color(std::move(rhs.m_color))
    , m_log_level(std::move(rhs.m_log_level))
  {
    rhs.m_flush = false;
  }

  scoped_message_writer(scoped_message_writer& rhs) = delete;
  scoped_message_writer& operator=(scoped_message_writer& rhs) = delete;
  scoped_message_writer& operator=(scoped_message_writer&& rhs) = delete;

  template<typename T>
  std::ostream& operator<<(const T& val)
  {
    m_oss << val;
    return m_oss;
  }

  ~scoped_message_writer();
};

inline scoped_message_writer success_msg_writer(bool color = true)
{
  return scoped_message_writer(color ? epee::console_color_green : epee::console_color_default, false, std::string(), el::Level::Info);
}

inline scoped_message_writer msg_writer(epee::console_colors color = epee::console_color_default)
{
  return scoped_message_writer(color, false, std::string(), el::Level::Info);
}

inline scoped_message_writer fail_msg_writer()
{
  return scoped_message_writer(epee::console_color_red, true, "Error: ", el::Level::Error);
}

} // namespace tools
