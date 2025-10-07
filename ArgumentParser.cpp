#include "ArgumentParser.h"
#include <boost/program_options.hpp>
#include <iostream>

ArgumentParser::PARSE_RES_CODE ArgumentParser::parse()
{
	namespace po = boost::program_options;

	po::variables_map vm;

	if (argc == 1) {
		std::cerr << "Error: no arguments. Run with --help to get help" << std::endl;
		return PARSE_RES_CODE::NO_ARGUMENTS;
	}

	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "produce help message")
		("directories", po::value<std::vector<std::string>>()->multitoken(), "directories to scan")
		("exclude", po::value<std::vector<std::string>>()->multitoken()->default_value(std::vector<std::string>{}, ""), "directories to exclude")
		("level", po::value<size_t>()->default_value(0), "scan level depth (0 [default]- only current directory)")
		("masks", po::value<std::vector<std::string>>()->multitoken()->default_value(std::vector<std::string>{}, ""), "file name masks (case insensitive) - all [default]")
		("min-size", po::value<size_t>()->default_value(1), "minimum file size, bytes - 1 [default]")
		("block-size", po::value<size_t>()->default_value(1024), "block size, bytes - 1024 [default]")
		("hash", po::value<std::string>()->default_value("crc32"), "hash algorithm (crc32 [default], md5)")
		("delete", po::value<bool>()->default_value(false), "delete duplicates except of one from folder")
		;

	try {
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);
	}
	catch (const po::error& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return PARSE_RES_CODE::PARSE_ERROR;
	}

	if (vm.count("help")) {
		std::cout << desc << std::endl;
		return PARSE_RES_CODE::OK;
	}

	if (vm.count("directories")) {
		data_.directories = vm["directories"].as<std::vector<std::string>>();
	}
	else {
		std::cerr << "Error: no directories specified. Run with --help to get help" << std::endl;
		return PARSE_RES_CODE::NO_DIRECTORIES;
	}

	if (vm.count("exclude"))
		data_.excludeDirectories = vm["exclude"].as<std::vector<std::string>>();

	if (vm.count("level"))
		data_.level = vm["level"].as<size_t>();

	if (vm.count("masks"))
		data_.masks = vm["masks"].as<std::vector<std::string>>();

	if (vm.count("min-size"))
		data_.minFileSize = vm["min-size"].as<size_t>();

	try {
		data_.hashAlgorithm = HashAlgorithmFactory::create(vm["hash"].as<std::string>());
	}
	catch (const std::invalid_argument& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return PARSE_RES_CODE::INVALID_HASH_ALGORITHM;
	}

	if (vm.count("block-size"))
		data_.blockSize = vm["block-size"].as<size_t>();

	if (vm.count("delete"))
		data_.deleteflag = vm["delete"].as<bool>();

	return PARSE_RES_CODE::OK;
}