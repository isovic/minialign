/*
 * main.cc
 *
 *  Created on: Jan 24, 2016
 *      Author: isovic
 */

#include <sequences/sequence_test.h>
#include <stdio.h>
#include "log_system/log_system.h"
#include <sstream>
#include "sequences/sequence_file.h"
#include "argparser.h"
#include "parameters.h"
#include "mhap.h"

void RunTests() {
  TEST_CLASS_SEQUENCE_ALIGNMENT();
  exit(0);
}

void HashQnames(const SequenceFile &seqs, std::map<std::string, int64_t> &qname_to_ids) {
  for (int32_t i=0; i<seqs.get_sequences().size(); i++) {
    std::string header = std::string(seqs.get_sequences()[i]->get_header());
    qname_to_ids[header] = i;
    qname_to_ids[TrimToFirstSpace(header)] = i;
    std::size_t found = header.find(":");
    qname_to_ids[header.substr(0, found)] = i;
//    printf ("'%s'\t'%s'\n", TrimToFirstSpace(header).c_str(), header.substr(0, found).c_str());
  }
}

int main(int argc, char* argv[]) {
  //  RunTests();

  bool help = false;
  ProgramParameters parameters;
  ArgumentParser argparser;
  argparser.AddArgument(&(parameters.ref_path), VALUE_TYPE_STRING, "", "raw", "", "Path to the raw contig/read sequences (output from the layout step). GFA, FASTA/FASTQ or SAM formats allowed.", -3, "Input/Output options");
  argparser.AddArgument(&(parameters.reads_path), VALUE_TYPE_STRING, "", "reads", "", "Reads in FASTQ format.", -2, "Input/Output options");
  argparser.AddArgument(&(parameters.overlap_path), VALUE_TYPE_STRING, "", "alnpath", "", "Path to a PAF/MHAP file with read-to-reference overlaps.", -1, "Input/Output options");
//  argparser.AddArgument(&(parameters.is_paf), VALUE_TYPE_BOOL, "", "paf", "0", "Overlaps are in PAF format instead of MHAP.", 0, "Input/Output options");
  argparser.AddArgument(&(parameters.is_mhap), VALUE_TYPE_BOOL, "", "mhap", "0", "Overlaps are in PAF format instead of MHAP.", 0, "Input/Output options");
  argparser.AddArgument(&(parameters.num_threads), VALUE_TYPE_INT32, "t", "threads", "4", "Number of threads to use.", 0, "Control");
  argparser.AddArgument(&(parameters.verbose_level), VALUE_TYPE_INT32, "v", "verbose", "5", "Verbose level. 0 off, 1 low, 2 medium, 3 high, 4 and 5 all levels, 6-9 debug.", 0, "Other");
  argparser.AddArgument(&help, VALUE_TYPE_BOOL, "h", "help", "0", "View this help.", 0, "Other options");

  if (argc == 1) {
    fprintf (stderr, "  %s [options] <ref.fasta> <reads.fastq> <overlaps.mhap>\n\n", argv[0]);
    fprintf (stderr, "%s\n", argparser.VerboseUsage().c_str());
    exit(1);
  }

  // Process the command line arguments.
  argparser.ProcessArguments(argc, argv);

  // Store the command line arguments for later use.
  for (int32_t i=0; i<argc; i++) { parameters.cmd_arguments.push_back(argv[i]); }
  parameters.program_folder = parameters.cmd_arguments[0].substr(0, parameters.cmd_arguments[0].find_last_of("\\/"));
  parameters.program_bin = parameters.cmd_arguments[0].substr(parameters.cmd_arguments[0].find_last_of("\\/") + 1);
//    // Verbose the current state of the parameters after.
//    fprintf (stderr, "%s\n\n", argparser.VerboseArguments().c_str());

  if (parameters.is_mhap == false) {
    parameters.is_paf = true;
  }

  if (parameters.verbose_level == 1) {
    LogSystem::GetInstance().LOG_VERBOSE_TYPE = LOG_VERBOSE_STD;
  } else if (parameters.verbose_level > 1) {
    LogSystem::GetInstance().LOG_VERBOSE_TYPE = LOG_VERBOSE_FULL | LOG_VERBOSE_STD;
  }

  // Set the verbose level for the execution of this program.
  LogSystem::GetInstance().SetProgramVerboseLevelFromInt(parameters.verbose_level);

  /// Check if help was triggered.
  if (argparser.GetArgumentByLongName("help")->is_set == true) {
    fprintf (stderr, "  %s [options] <ref.fasta> <reads.fastq> <overlaps.mhap>\n\n", argv[0]);
    fprintf (stderr, "%s\n", argparser.VerboseUsage().c_str());
    exit(1);
  }

  SequenceFile seqs_gfa(SEQ_FORMAT_AUTO, parameters.ref_path);

  std::string overlaps_file = parameters.overlap_path;
  if (parameters.is_paf == true) { LOG_ALL("Using PAF for input alignments. (%s)\n", overlaps_file.c_str());}
  else { LOG_ALL("Using MHAP for input alignments. (%s)\n", overlaps_file.c_str()); }

  std::vector<OverlapLine> overlaps, overlaps_filtered, overlaps_final;

  LOG_ALL("Loading reads.\n");
  SequenceFile seqs_reads(SEQ_FORMAT_AUTO, parameters.reads_path);
  // Sanity check to see if the reads have quality values.
  if (seqs_reads.HasQV() == false) {
    fprintf (stderr, "ERROR: Reads are not specified in a format which contains quality information. Exiting.\n");
    exit(1);
  }

  // Hash the read sequences by their name.
  LOG_ALL("Hashing qnames.\n");
  std::map<std::string, int64_t> qname_to_ids;
  HashQnames(seqs_reads, qname_to_ids);
  HashQnames(seqs_gfa, qname_to_ids);

  LOG_ALL("Parsing the overlaps file.\n");
  if (overlaps_file == "-") { LOG_ALL("Stdin will be used to load the overlap lines.\n"); }
  if (parameters.is_paf == true) {
    ParsePAF(overlaps_file, qname_to_ids, overlaps);
  } else {
    ParseMHAP(overlaps_file, overlaps);
  }

  printf ("%s", seqs_gfa.GenerateSAMHeader().c_str());
  fflush(stdout);

  SequenceFile aligned;
  AlignOverlaps(seqs_gfa, seqs_reads, overlaps, parameters.num_threads, aligned, true, true);

//  for (int64_t i=0; i<aligned.get_sequences().size(); i++) {
//    std::string sam_line = aligned.get_sequences()[i]->MakeSAMLine();
//    fprintf (stdout, "%s\n", sam_line.c_str());
//    fflush(stdout);
//  }

  return 0;
}
