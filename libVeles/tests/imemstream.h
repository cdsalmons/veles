/*! @file imemstream.h
 *  @brief istream which reads from buffer in memory.
 *  @author Vadim Markovtsev <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright © 2013 Samsung R&D Institute Russia
 *
 *  @section License
 *  Licensed to the Apache Software Foundation (ASF) under one
 *  or more contributor license agreements.  See the NOTICE file
 *  distributed with this work for additional information
 *  regarding copyright ownership.  The ASF licenses this file
 *  to you under the Apache License, Version 2.0 (the
 *  "License"); you may not use this file except in compliance
 *  with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing,
 *  software distributed under the License is distributed on an
 *  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 *  KIND, either express or implied.  See the License for the
 *  specific language governing permissions and limitations
 *  under the License.
 */

#ifndef TESTS_IMEMSTREAM_H_
#define TESTS_IMEMSTREAM_H_

#include <istream>

namespace {

struct membuf: std::streambuf {
  membuf(const char* base, size_t size) {
    char* p(const_cast<char*>(base));
    this->setg(p, p, p + size);
  }

 protected:
  virtual pos_type seekoff(off_type offset, std::ios::seekdir seekdir,
      std::ios::openmode = std::ios::in | std::ios::out) override {
    switch (seekdir) {
      case std::ios::beg:
        assert(eback() + offset < egptr());
        _M_in_cur = eback() + offset;
        break;
      case std::ios::cur:
        assert(gptr() + offset < egptr());
        _M_in_cur += offset;
        break;
      case std::ios::end:
        assert(egptr() - offset >= eback());
        _M_in_cur = egptr() - offset;
        break;
      default:
        break;
    }
    return pos_type(off_type(gptr() - eback()));
  }
};

struct imemstream: virtual membuf, public std::istream {
  imemstream(const char* base, size_t size)
    : membuf(base, size)
    , std::istream(static_cast<std::streambuf*>(this)) {
  }
};

}

#endif  // TESTS_IMEMSTREAM_H_
