#include "httprequest.h"
#include "utils.h"

#include <curl/curl.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <chrono>

// ---------------------------------------------------------------------------
// cURL callback for writing response data
// ---------------------------------------------------------------------------

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t total_size = size * nmemb;
    output->append((char*)contents, total_size);
    return total_size;
}

// ---------------------------------------------------------------------------
// HttpRequest implementation
// ---------------------------------------------------------------------------

HttpRequest::HttpRequest(const std::string& apiToken, const std::string& endpoint)
    : apiToken_(apiToken), endpoint_(endpoint) {
    curl_global_init(CURL_GLOBAL_ALL);
}

HttpRequest::Response HttpRequest::SendPlayerEvent(
    const std::string& steamId, 
    const std::string& discordUserId, 
    bool isJoin
) {
    Response response;
    response.statusCode = 0;
    response.error = "";
    
    CURL* curl = curl_easy_init();
    if (!curl) {
        response.error = "Failed to initialize cURL";
        return response;
    }
    
    std::string jsonBody = Utils::BuildPlayerEventJson(steamId, discordUserId, isJoin);
    
    // Build headers
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    
    if (!apiToken_.empty()) {
        std::string authHeader = "Authorization: Bearer " + apiToken_;
        headers = curl_slist_append(headers, authHeader.c_str());
    }
    
    // Set cURL options
    curl_easy_setopt(curl, CURLOPT_URL, endpoint_.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonBody.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response.body);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L); // 10 second timeout
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    
    // Perform the request
    CURLcode res = curl_easy_perform(curl);
    
    if (res != CURLE_OK) {
        response.error = std::string("cURL error: ") + curl_easy_strerror(res);
    } else {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.statusCode);
    }
    
    // Cleanup
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    
    return response;
}

void HttpRequest::SetEndpoint(const std::string& url) {
    endpoint_ = url;
}

std::string HttpRequest::GetCurrentTimestamp() {
    return Utils::GetCurrentUTCTime();
}
