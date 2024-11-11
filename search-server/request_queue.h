#pragma once

#include "search_server.h"
#include <deque>

class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server);

    template <typename DocumentPredicate>
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate);

    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentStatus status);

    std::vector<Document> AddFindRequest(const std::string& raw_query);

    int GetNoResultRequests() const;

private:
    struct QueryResult {
        int timestamp;
        bool is_empty;
    };

    const SearchServer& search_server_;
    std::deque<QueryResult> requests_;
    int no_result_requests_;
    int current_time_;
    const static int min_in_day_ = 1440;

    void AddRequest(int results_num);
};

template <typename DocumentPredicate>
    std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate) {
        const auto result = search_server_.FindTopDocuments(raw_query, document_predicate);
        AddRequest(result.size());
        return result;
    }