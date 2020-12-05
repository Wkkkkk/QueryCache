#include <iostream>
#include <sstream>

#include <absl/strings/str_split.h>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

#include "service.h"

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;

using nlohmann::json;

Service::Service()
	: inst()
	, conn(mongo::uri("mongodb+srv://Wkkkkk:wkkkkk@cluster0.wqx0r.mongodb.net/CacheQuery?retryWrites=true&w=majority"))
	, reverse_index_collection(conn["CacheQuery"]["reverse_index"])
	, document_collection(conn["CacheQuery"]["document"])
{
	// insert some test content into mongo
	//{
	//	bsoncxx::builder::stream::document document{};
	//	document << "id" << 1
	//			 << "title" << "Greets"
	//			 << "snippet" << "Hi"
	//		     << "content" << " Hello world from china";

	//	document_collection.insert_one(document.view());
	//}
	//{
	//	bsoncxx::builder::stream::document document{};
	//	document << "id" << 2
	//		<< "title" << "Greets2"
	//		<< "snippet" << "Hi"
	//		<< "content" << " Hello world";

	//	document_collection.insert_one(document.view());
	//} 
	//{
	//	bsoncxx::builder::stream::document document{};
	//	document << "id" << 3
	//		<< "title" << "Greets3"
	//		<< "snippet" << "Hi"
	//		<< "content" << " Hello sweden";

	//	document_collection.insert_one(document.view());
	//}

	//{
		//bsoncxx::builder::stream::document document{};
		//document << "word" << "hello" << "index" << make_array(1, 2, 3);

		//reverse_index_collection.update_one(document.view());
	//}

	//{
	//	bsoncxx::builder::stream::document document{};
	//	document << "word" << "world" << "index" << make_array(1, 2);

	//	reverse_index_collection.insert_one(document.view());
	//}
	//
	//{
	//	bsoncxx::builder::stream::document document{};
	//	document << "word" << "china" << "index" << make_array(1);

	//	reverse_index_collection.insert_one(document.view());
	//}

	//{
	//	bsoncxx::builder::stream::document document{};
	//	document << "word" << "sweden" << "index" << make_array(3);

	//	reverse_index_collection.insert_one(document.view());
	//}


	// update data in case
	//document_collection.update_one(document{} << "id" << "1" << finalize,
	//	document{} << "$set" << open_document << "id" << 1 << close_document << finalize);
}

std::set<int> Service::query_reverse_index(std::string query)
{
	std::cout << "You're querying: " << query << std::endl;

	std::vector<std::string> words = absl::StrSplit(query, "+");
	std::vector<int> ids;

	for (auto word : words) {
		bsoncxx::builder::stream::document document{};
		document << "word" << word;
		auto result = reverse_index_collection.find_one(document.view());
		if (result) {
			std::string result_str = bsoncxx::to_json(*result);

			json body = json::parse(result_str);
			std::vector<int> index = body["index"];

			ids.insert(ids.end(), index.begin(), index.end());
		}
	}

	return std::set<int>(ids.begin(), ids.end());;
}


std::vector<Document> Service::query_documents_by_ids(std::set<int> ids)
{
	std::vector<Document> documents;

	for (auto id : ids) {
		bsoncxx::builder::stream::document document{};
		document << "id" << id;

		auto result = document_collection.find_one(document.view());
		if (result) {
			std::string result_str = bsoncxx::to_json(*result);
			json body = json::parse(result_str);

			Document doc;
			doc.id = body["id"];
			doc.title = body["title"];
			doc.snippet = body["snippet"];
			doc.content = body["content"];

			documents.push_back(doc);
		}
	}

	return documents;
}
