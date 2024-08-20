#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;

string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result = 0;
    cin >> result;
    ReadLine();
    return result;
}

vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        } else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}

struct Document {
    int id;
    double relevance;
};

class SearchServer {
public:
    void SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }

    void AddDocument(int document_id, const string& document) {
        const vector<string> words = SplitIntoWordsNoStop(document);
        double reciprocal_size = 1.0/words.size();
        for (const string& word : words){
            auto itr = word_to_document_freqs_.find(word);
            word_to_document_freqs_[word][document_id] += reciprocal_size;
        }
        document_count_++;
    }

    vector<Document> FindTopDocuments(const string& raw_query) const {
        const set<string> query_words = ParseQuery(raw_query);
        const set<string> minus_words = FindMinusWords(query_words);

        auto matched_documents = FindAllDocuments(query_words, minus_words);
        sort(matched_documents.begin(), matched_documents.end(),
             [](const Document& lhs, const Document& rhs) {
                 return lhs.relevance > rhs.relevance;
             });
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }
        return matched_documents;
    }

private:
    map<string, map<int, double>> word_to_document_freqs_;
    set<string> stop_words_;

    int document_count_ = 0;

    bool IsStopWord(const string& word) const {
        return stop_words_.count(word) > 0;
    }

    vector<string> SplitIntoWordsNoStop(const string& text) const {
        vector<string> words;
        for (const string& word : SplitIntoWords(text)) {
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
        }
        return words;
    }

    const double CalculateIDF(map<int, double>& id_relevance) const{
        return log(1.0*document_count_/static_cast<int>(id_relevance.size()));
    }

    set<string> ParseQuery(const string& text) const {
        set<string> query_words;
        for (const string& word : SplitIntoWordsNoStop(text)) {
            query_words.insert(word);
        }
        return query_words;
    }

    set<string> FindMinusWords(const set<string>& query_words) const{
        set<string> minus_words;
        for (auto i : query_words){
            if (i[0] == '-') {
                i.erase(0, 1);
                minus_words.insert(i);
            } 
        }
        
        return minus_words;
    }

    vector<Document> FindAllDocuments(const set<string>& query_words, const set<string>& minus_words) const {
        vector<Document> matched_documents;
        map<int, double> documents;
        for (const auto& i : query_words){
            if (word_to_document_freqs_.find(i) != word_to_document_freqs_.end()) {
                map<int, double> id_relevance = word_to_document_freqs_.find(i)->second;
                const double idf = CalculateIDF(id_relevance);
                for (auto j : id_relevance) {
                    double tf_idf = j.second * idf;
                    if (documents.find(j.first) != documents.end()){
                        documents[j.first] += tf_idf;
                    }
                    else {
                        documents.insert({j.first, tf_idf});
                    }
                }
            }
        }
        for (const auto& i : minus_words){
            if (word_to_document_freqs_.find(i) != word_to_document_freqs_.end()){
                for (auto j : word_to_document_freqs_.find(i)->second) {
                    documents.erase(j.first);
                }
            }         
        }
        for (const auto& i : documents){
            matched_documents.push_back({i.first, i.second});
        }
        return matched_documents;
    }
};

SearchServer CreateSearchServer() {
    SearchServer search_server;
    search_server.SetStopWords(ReadLine());

    const int document_count = ReadLineWithNumber();
    for (int document_id = 0; document_id < document_count; ++document_id) {
        search_server.AddDocument(document_id, ReadLine());
    }

    return search_server;
}

int main() {
    const SearchServer search_server = CreateSearchServer();

    const string query = ReadLine();
    for (const auto& [document_id, relevance] : search_server.FindTopDocuments(query)) {
        cout << "{ document_id = "s << document_id << ", "
             << "relevance = "s << relevance << " }"s << endl;
    }
}