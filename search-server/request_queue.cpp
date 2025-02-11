#include "request_queue.h"

RequestQueue::RequestQueue(const SearchServer& search_server)
    : search_server_(search_server)
    , no_result_requests_(0)
    , current_time_(0) {
}

std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentStatus status) {
    const auto result = search_server_.FindTopDocuments(raw_query, status);
    AddRequest(result.size());
    return result;
}

std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query) {
    const auto result = search_server_.FindTopDocuments(raw_query);
    AddRequest(result.size());
    return result;
}

int RequestQueue::GetNoResultRequests() const {
    return no_result_requests_;
}

void RequestQueue::AddRequest(int results_num) {
    ++current_time_;
    bool is_empty = (results_num == 0);

    requests_.push_back({current_time_, is_empty});
    if (is_empty) {
        ++no_result_requests_;
    }

    while (!requests_.empty() && requests_.front().timestamp <= current_time_ - min_in_day_) {
        if (requests_.front().is_empty) {
            --no_result_requests_;
        }
        requests_.pop_front();
    }
}
