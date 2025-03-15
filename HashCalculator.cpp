#include "HashCalculator.h"
#include <boost/algorithm/hex.hpp>
#include <boost/crc.hpp>
#include <boost/uuid/detail/md5.hpp>
#include <bit>

std::string CRC32Hash::calculateHash(const std::vector<char>& block) const
{
	boost::crc_32_type result;
	result.process_bytes(block.data(), block.size());
	return std::to_string(result.checksum());
}

std::string MD5Hash::calculateHash(const std::vector<char>& block) const
{
	boost::uuids::detail::md5 hash;
	hash.process_bytes(block.data(), block.size());
	boost::uuids::detail::md5::digest_type digest;
	hash.get_digest(digest);
	const auto charDigest = std::bit_cast<const char*>(&digest);
	std::string result;
	boost::algorithm::hex(charDigest, charDigest + sizeof(boost::uuids::detail::md5::digest_type), std::back_inserter(result));
	return result;
}

HashCalculator::HashCalculator(IHashAlgorithm* algorithm, size_t blockSize)
	: algorithm_(algorithm), blockSize_(blockSize)
{
}

std::string HashCalculator::calculateHash(const std::vector<char>& block) const
{
	return algorithm_->calculateHash(block);
}

std::unique_ptr<IHashAlgorithm> HashAlgorithmFactory::create(const std::string_view& algorithm) {
	if (algorithm == "crc32") {
		return std::make_unique<CRC32Hash>();
	}
	else if (algorithm == "md5") {
		return std::make_unique<MD5Hash>();
	}
	throw std::invalid_argument("Invalid hash algorithm");
}