/*
 * parameters.h
 *
 *  Created on: Feb 23, 2016
 *      Author: isovic
 */

#ifndef SRC_RACON_PARAMETERR_H_
#define SRC_RACON_PARAMETERR_H_

#include <string>


struct ProgramParameters {
  std::string ref_path = "";
  std::string reads_path = "";
  std::string overlap_path = "";
  int32_t num_threads = 1;

  std::string program_bin;
  std::string program_folder;
  std::vector<std::string> cmd_arguments;

  bool is_paf = false;
  bool is_mhap = false;

  int32_t verbose_level = 1;

  bool use_hard_clipping = false;
  bool use_basic_cigar = false;
};

#endif /* SRC_PARAMETERS_H_ */
