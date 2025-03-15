#include "ArgumentParser.h"
#include "FileCollector.h"
#include "FileComparator.h"

int main(int argc, char* argv[]) {
	ArgumentParser parser(argc, argv);
	if (auto res = parser.parse(); res != ArgumentParser::PARSE_RES_CODE::OK)
		return static_cast<int>(res);
	FileCollector fileCollector(parser.data());
	FileComparator comparator(fileCollector.fileGroups(), parser.data());
	comparator.compareGroups();
	return 0;
}