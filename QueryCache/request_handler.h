#pragma once

#include <string>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

#include <rpc/client.h>

namespace beast = boost::beast;     // from <boost/beast.hpp>
namespace http = beast::http;       // from <boost/beast/http.hpp>

// Business logic
namespace QueryCache {

	class RequestHandler
	{
	public:
		enum class STATUS {
			ok,
			not_found,
			illegal_request
		};

		using result_pair = std::pair<STATUS, std::string /* response_body */>;

		RequestHandler();

		result_pair handle(const http::request<http::string_body>& request);

	private:
		const static size_t max_cache_size = 3;

		std::string retrive_query_content_from_request_target(const http::request<http::string_body>& request);
	};

}