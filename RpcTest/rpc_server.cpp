// RpcTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <rpc/server.h>

#include "service.h"

void foo() {
    std::cout << "foo was called!" << std::endl;
}

int main(int argc, char* argv[]) {
    // Creating a server that listens on port 8080
    rpc::server srv(9000);

    // Our service
    Service service;

    // Binding the name "foo" to free function foo.
    // note: the signature is automatically captured
    srv.bind("foo", &foo);

    // Binding the name "XXX" to lambda function.
    srv.bind("ReverseIndexQuery", [&service](std::string query) {
        return service.query_reverse_index(query);
        });

    srv.bind("DocumentQuery", [&service](std::set<int> ids) {
        return service.query_documents_by_ids(ids);
        });

    // Run the server loop.
    srv.run();

    return 0;
}