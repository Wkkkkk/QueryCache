#include <iostream>

#include <absl/strings/str_split.h>
#include <boost/beast/version.hpp>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

#include "singleton.h"
#include "cache.h"
#include "request_handler.h"

using namespace QueryCache;
using json = nlohmann::json;

std::string RequestHandler::retrive_query_content_from_request_target(const http::request<http::string_body>& request)
{
    std::string content;

    std::string target = request.target().to_string();
    std::vector<std::string> parts = absl::StrSplit(target, "?");
    if (parts.size() == 2) {
        std::vector<std::string> all_parameters = absl::StrSplit(parts[1], "&");

        for (std::string one_parameter : all_parameters) {
            std::vector<std::string> parameter_pair = absl::StrSplit(one_parameter, "=");
            if (parameter_pair.size() == 2) {
                if (parameter_pair[0] == "query") {
                    content = parameter_pair[1];

                    break;
                }
            }
        }
    }

    return content;
}

QueryCache::RequestHandler::RequestHandler() {}

RequestHandler::result_pair
RequestHandler::handle(const http::request<http::string_body>& request)
{
    // Analyze the query content
    std::string query = retrive_query_content_from_request_target(request);
    if (query.empty()) {
        return { RequestHandler::STATUS::illegal_request, request.target().to_string() };
    }

    // Look for the query in the cache
    if (auto query_cache = singleton::get_instance<Cache>(RequestHandler::max_cache_size).get(query)) {
        return { RequestHandler::STATUS::ok, query_cache->content };
    }

    // Get a rpc client
    auto& client = singleton::get_instance<rpc::client>("127.0.0.1", 9000);

    // Search for matched documents
    auto ids = client.call("ReverseIndexQuery", query).as<std::set<int>>();
    if (ids.empty()) {
        return { RequestHandler::STATUS::not_found, query };
    }
    auto documents = client.call("DocumentQuery", ids).as<std::vector<Document>>();
    if (documents.empty()) {
        return { RequestHandler::STATUS::not_found, query };
    }

    // generate response json
    json j;
    for (const auto& document : documents) {
        json doc;
        doc["id"] = document.id;
        doc["title"] = document.title;
        doc["snippet"] = document.snippet;
        doc["content"] = document.content;

        j.push_back(doc);
    }
    std::string result = j.dump();

    // save it to cache
    singleton::get_instance<Cache>(RequestHandler::max_cache_size).set({ query, result });

    return { RequestHandler::STATUS::ok, result };
}
