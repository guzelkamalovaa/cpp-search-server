#include "search_server.h"
#include "request_queue.h"
#include "paginator.h"
#include "document.h"

#include <iostream>
#include <string>
#include <vector>
#include <set>

using namespace std::string_literals;

// Optionally, you can use the entire std namespace
using namespace std;

int main() {
    SearchServer search_server("and in at"s);
    RequestQueue request_queue(search_server);

    search_server.AddDocument(1, "curly cat curly tail"s, DocumentStatus::ACTUAL, {7, 2, 7});
    search_server.AddDocument(2, "curly dog and fancy collar"s, DocumentStatus::ACTUAL, {1, 2, 3});
    search_server.AddDocument(3, "big cat fancy collar "s, DocumentStatus::ACTUAL, {1, 2, 8});
    search_server.AddDocument(4, "big dog sparrow Eugene"s, DocumentStatus::ACTUAL, {1, 3, 2});
    search_server.AddDocument(5, "big dog sparrow Vasiliy"s, DocumentStatus::ACTUAL, {1, 1, 1});

    // 1439 requests with empty results
    for (int i = 0; i < 1439; ++i) {
        request_queue.AddFindRequest("empty request"s);
    }
    // still 1439 requests with empty results
    request_queue.AddFindRequest("curly dog"s);
    // new day, first request removed, 1438 requests with empty results
    request_queue.AddFindRequest("big collar"s);
    // first request removed, 1437 requests with empty results
    request_queue.AddFindRequest("sparrow"s);

    std::cout << "Total empty requests: "s << request_queue.GetNoResultRequests() << std::endl;
    return 0;
}
