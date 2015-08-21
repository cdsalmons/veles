/*! @file workflow_loader.cc
 *  @brief Implementation of WorkflowLoader class.
 *  @author Vadim Markovtsev <v.markovtsev@samsung.com>, Bulychev Egor <e.bulychev@samsung.com>
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

#include "veles/workflow_loader.h"
#include <istream>
#include <vector>
#include <libarchive/libarchive/archive.h>  // NOLINT(*)
#include <libarchive/libarchive/archive_entry.h>  // NOLINT(*)
#include "inc/veles/make_unique.h"
#include "inc/veles/unit_factory.h"
#include "src/iarchivestream.h"
#include "src/main_file_loader.h"
#include "src/workflow_archive.h"

namespace veles {

/// Name of the file which describes the workflow.
const char* WorkflowLoader::kMainFile = "contents.json";

WorkflowLoader::WorkflowLoader() {
}

Workflow WorkflowLoader::Load(const std::string& /*archive*/) {
  return Workflow();
}

std::shared_ptr<WorkflowArchive> WorkflowLoader::ExtractArchive(
    const std::string& file_name) {
  int error;
  auto arch = WorkflowArchive::Open(file_name, &error);
  if (!arch) {
    ERR("Failed to open %s: %d", file_name.c_str(), error);
    return nullptr;
  }
  DBG("Successfully opened %s, scanning for %s", file_name.c_str(), kMainFile);
  archive_entry* entry;
  while (archive_read_next_header(arch.get(), &entry) == ARCHIVE_OK) {
    std::string efn = archive_entry_pathname(entry);
    if (efn == kMainFile) {
      DBG("Found %s", kMainFile);
      auto instr = std::make_unique<iarchivestream>(arch.get());
      auto wdef = MainFileLoader().Load(instr.get());
      return std::make_shared<WorkflowArchive>(wdef);
    } else {
      DBG("Skipping %s...", efn.c_str());
      archive_read_data_skip(arch.get());
    }
  }
  ERR("%s was not found", kMainFile);
  return nullptr;
}

}  // namespace veles
