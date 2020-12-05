#pragma once

#include <string>
#include <rpc/msgpack.hpp>

struct Document {
	int id;
	std::string title;
	std::string snippet;
	std::string content;

	MSGPACK_DEFINE_ARRAY(id, title, snippet, content)
};

inline bool operator==(const Document& lhs, const Document& rhs) {
	return lhs.id == rhs.id;
}