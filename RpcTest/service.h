#pragma once
#include <set>
#include <vector>
#include <string>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

#include "Document.h"

namespace mongo = mongocxx;

class Service
{
public:
	Service();

	std::set<int> query_reverse_index(std::string query);
	std::vector<Document> query_documents_by_ids(std::set<int> ids);

private:
	mongo::instance inst;
	mongo::client conn;
	mongo::collection reverse_index_collection;
	mongo::collection document_collection;
};

