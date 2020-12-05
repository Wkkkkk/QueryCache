#pragma once

#include <list>
#include <map>
#include <string>
#include <optional>
#include <iostream>

#include "Document.h"

struct Node {
	std::string query;
	std::string content;

	Node() {}

	Node(std::string query, std::string content) {
		this->query = query;
		this->content = content;
	}
};

inline bool operator==(const Node& lhs, const Node& rhs) {
	return lhs.query == rhs.query and lhs.content == rhs.content;
}

class Cache
{
	size_t max_size_;
	size_t size_;
	std::list<Node> nodes_;
	std::map<std::string, Node> map_;

public:
	Cache(size_t max_size)
		: max_size_(max_size)
		, size_(0) {
		std::cout << "Cache with max size: " << max_size << " is created." << std::endl;
	}

	void set(Node node) {
		auto iter = map_.find(node.query);

		if (iter != map_.end()) {
			// Key exists in cache, update the content
			Node& n = iter->second;
			n.content = node.content;

			// move the node to the front of the list
			move_to_front(node);
		}
		else {
			// Key does not exist in cache
			if (size_ == max_size_) {
				// Remove the oldest entry from the linked list and map
				auto oldest_node_it = nodes_.back();
				std::string oldest_query = oldest_node_it.query;
				// std::cout << "remove cache: " << oldest_node_it.query << " " << oldest_node_it.content << std::endl;

				map_.erase(oldest_query);	// erasing by key
				nodes_.pop_back();
			}
			else {
				size_++;
			}

			// Add the new key and value
			nodes_.insert(nodes_.begin(), node);
			map_[node.query] = node;
		}
	}

	std::optional<Node> get(std::string query) {
		auto iter = map_.find(query);
		if (iter == map_.end()) return {};

		Node node = iter->second;
		move_to_front(node);

		return node;
	}

private:
	// move the node to the front of the list
	void move_to_front(const Node& node) {
		auto it = std::find(nodes_.begin(), nodes_.end(), node);
		nodes_.splice(nodes_.begin(), nodes_, it); // O(1)
	}

};

