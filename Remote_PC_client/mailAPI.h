#pragma once

#include <iostream>
#include <nlohmann/json.hpp>
#include <fstream>
#include <curl/curl.h>
#include <string>
#include <thread>
#include <chrono>
#include <unordered_set>
#include <stdexcept>


using json = nlohmann::json;
// Hàm callback để nhận dữ liệu từ server
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
    size_t totalSize = size * nmemb;
    s->append((char*)contents, totalSize);
    return totalSize;
}

// Hàm để lấy access token từ refresh token
std::string getAccessTokenFromRefreshToken(
    const std::string& client_id,
    const std::string& client_secret,
    const std::string& token_uri,
    const std::string& refresh_token) {
    CURL* curl;
    CURLcode res;
    std::string access_token;
    std::string readBuffer;

    // Khởi tạo CURL
    curl = curl_easy_init();

    if (curl) {
        // Thiết lập URL cho token endpoint
        curl_easy_setopt(curl, CURLOPT_URL, token_uri.c_str());

        // Tạo dữ liệu POST để gửi
        std::string postFields = "client_id=" + client_id +
            "&client_secret=" + client_secret +
            "&refresh_token=" + refresh_token +
            "&grant_type=refresh_token";

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields.c_str());

        // Đặt callback để lưu phản hồi từ server
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Gửi request
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
        }
        else {

            // Parse JSON phản hồi từ server để lấy access_token
            auto response_json = nlohmann::json::parse(readBuffer);
            if (response_json.contains("access_token")) {
                access_token = response_json["access_token"];
            }
            else {
            }
        }

        // Cleanup
        curl_easy_cleanup(curl);
    }

    return access_token;  // Trả về access token
}

// đọc file json lấy client_id, client_secret, token_uri
std::string readJson(std::string fileJsonName) {
    //Đọc file JSON để lấy thông tin OAuth 2.0
    std::ifstream file(fileJsonName);
    nlohmann::json config;
    file >> config;

    std::string client_id = config["web"]["client_id"];
    std::string client_secret = config["web"]["client_secret"];
    std::string token_uri = config["web"]["token_uri"];
    std::string refresh_token = config["web"]["refresh_token"];  // Thay bằng refresh token của bạn

    std::string access_token = getAccessTokenFromRefreshToken(client_id, client_secret, token_uri, refresh_token);
    return access_token;
}

//lay teu de cua mail được gửi đến
std::string getEmailSubject(const std::string& accessToken, const std::string& messageId) {
    std::string readBuffer;
    CURL* curl;
    CURLcode res;

    curl = curl_easy_init();
    if (curl) {
        std::string url = "https://gmail.googleapis.com/gmail/v1/users/me/messages/" + messageId;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + accessToken).c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res == CURLE_OK) {
            // Parse JSON response
            nlohmann::json emailData = nlohmann::json::parse(readBuffer);
            // Tìm tiêu đề trong phần headers
            for (const auto& header : emailData["payload"]["headers"]) {
                if (header["name"] == "Subject") {
                    return header["value"];
                }
            }
        }
        else {
        }
    }
    return "";
}

std::string decodeBase64(const std::string& encoded) {
    static const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    auto is_base64 = [](unsigned char c) {
        return (isalnum(c) || (c == '+') || (c == '/'));
        };

    std::string decoded;
    int in_len = encoded.size();
    int i = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];

    while (in_len-- && (encoded[in_] != '=') && is_base64(encoded[in_])) {
        char_array_4[i++] = encoded[in_]; in_++;
        if (i == 4) {
            for (i = 0; i < 4; i++)
                char_array_4[i] = base64_chars.find(char_array_4[i]);

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; (i < 3); i++)
                decoded += char_array_3[i];
            i = 0;
        }
    }

    if (i) {
        for (int j = i; j < 4; j++)
            char_array_4[j] = 0;

        for (int j = 0; j < 4; j++)
            char_array_4[j] = base64_chars.find(char_array_4[j]);

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (int j = 0; (j < i - 1); j++) decoded += char_array_3[j];
    }

    return decoded;
}

//lay Body cua email duoc gui den
std::string getEmailBody(const std::string& accessToken, const std::string& messageId) {
    std::string readBuffer;
    CURL* curl;
    CURLcode res;

    curl = curl_easy_init();
    if (curl) {
        std::string url = "https://gmail.googleapis.com/gmail/v1/users/me/messages/" + messageId + "?format=full";
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Set HTTP headers
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + accessToken).c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set callback function to capture response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Perform CURL request
        res = curl_easy_perform(curl);

        // Clean up
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);

        if (res == CURLE_OK) {
            try {
                // Parse JSON response
                nlohmann::json emailData = nlohmann::json::parse(readBuffer);

                // Kiểm tra nếu payload có body
                if (emailData.contains("payload")) {
                    const auto& payload = emailData["payload"];

                    // Nếu body trực tiếp trong payload
                    if (payload.contains("body") && payload["body"].contains("data")) {
                        std::string rawData = payload["body"]["data"];
                        return decodeBase64(rawData);
                    }

                    // Nếu email là multipart, duyệt các phần để tìm body
                    if (payload.contains("parts")) {
                        for (const auto& part : payload["parts"]) {
                            if (part.contains("body") && part["body"].contains("data")) {
                                std::string rawData = part["body"]["data"];
                                return decodeBase64(rawData);
                            }
                        }
                    }
                }
                return "No body content found";
            }
            catch (const nlohmann::json::exception& e) {
                //std::cerr << "JSON parsing error: " << e.what() << std::endl;
                return "Error parsing JSON";
            }
        }
        else {
            //std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
            return "CURL error";
        }
    }

    return "CURL initialization failed";
}

// lấy id của các mail
nlohmann::json getEmailIds(std::string accessToken) {
    std::string readBuffer;

    CURL* curl;
    CURLcode res;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://gmail.googleapis.com/gmail/v1/users/me/messages");
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + accessToken).c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res == CURLE_OK) {
            // Parse JSON response
            return nlohmann::json::parse(readBuffer);
        }
        else {
            //std::cerr << "Error: " << curl_easy_strerror(res) << std::endl;
            return nullptr;
        }
    }
    return nullptr;
}

// Callback để lưu dữ liệu phản hồi của CURL
static size_t WriteCallback44(void* contents, size_t size, size_t nmemb, std::string* s) {
    size_t newLength = size * nmemb;
    try {
        s->append((char*)contents, newLength);
    }
    catch (std::bad_alloc& e) {
        return 0;
    }
    return newLength;
}

std::string GetSenderEmail(const std::string& accessToken, const std::string& messageId) {
    std::string url = "https://gmail.googleapis.com/gmail/v1/users/me/messages/" + messageId;
    std::string readBuffer;

    CURL* curl = curl_easy_init();
    if (curl) {
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + accessToken).c_str());

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback44);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        CURLcode res = curl_easy_perform(curl);
        /*if (res != CURLE_OK) {
            std::cerr << "Lỗi khi thực hiện yêu cầu: " << curl_easy_strerror(res) << std::endl;
        }*/

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    // Phân tích phản hồi JSON để lấy địa chỉ email người gửi
    nlohmann::json jsonResponse = nlohmann::json::parse(readBuffer, nullptr, false);
    if (jsonResponse.is_discarded()) {
        //std::cerr << "Lỗi khi phân tích JSON." << std::endl;
        return "";
    }

    if (jsonResponse.contains("payload") && jsonResponse["payload"].contains("headers")) {
        for (const auto& header : jsonResponse["payload"]["headers"]) {
            if (header["name"] == "From") {
                std::string fromHeader = header["value"];
                size_t start = fromHeader.find('<');
                size_t end = fromHeader.find('>');

                if (start != std::string::npos && end != std::string::npos && end > start) {
                    return fromHeader.substr(start + 1, end - start - 1);
                }
                else {
                    return fromHeader; // Trả về cả chuỗi nếu không tìm thấy <email@example.com>
                }
            }
        }
    }

    /*std::cerr << "Khong tim thay dia chi email cua nguoi gui!." << std::endl;*/
    return "";
}

// ma hoa để gửi mail
std::string encodeBase64(const std::string& data) {
    static constexpr char sEncodingTable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    size_t in_len = data.size();
    size_t out_len = 4 * ((in_len + 2) / 3);
    std::string ret(out_len, '\0');
    int val = 0, valb = -6;
    size_t j = 0;
    for (unsigned char c : data) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            ret[j++] = sEncodingTable[(val >> valb) & 0x3F];
            valb -= 6;
        }
    }
    if (valb > -6) ret[j++] = sEncodingTable[((val << 8) >> (valb + 8)) & 0x3F];
    while (j < out_len) ret[j++] = '=';
    return ret;
}

// Hàm chuyển cv::Mat sang chuỗi Base64
std::string matToBase64(const cv::Mat& image) {
    std::vector<uchar> buf;
    cv::imencode(".jpg", image, buf);
    return encodeBase64(std::string(buf.begin(), buf.end()));
}

// tạo Mime message để gửi mail
std::string createMimeMessage(const std::string& recipient, const std::string& subject, const std::string& bodyText, const std::string& imageBase64) {
    std::ostringstream oss;
    std::string boundary = "boundary12345";

    oss << "To: " << recipient << "\r\n";
    oss << "Subject: " << subject << "\r\n";
    oss << "MIME-Version: 1.0\r\n";
    oss << "Content-Type: multipart/mixed; boundary=" << boundary << "\r\n\r\n";

    // Phần thân
    oss << "--" << boundary << "\r\n";
    oss << "Content-Type: text/plain; charset=\"UTF-8\"\r\n\r\n";
    oss << bodyText << "\r\n\r\n";

    // Phần đính kèm ảnh
    oss << "--" << boundary << "\r\n";
    oss << "Content-Type: image/jpeg\r\n";
    oss << "Content-Transfer-Encoding: base64\r\n";
    oss << "Content-Disposition: attachment; filename=\"image.jpg\"\r\n\r\n";
    oss << imageBase64 << "\r\n";
    oss << "--" << boundary << "--";

    return oss.str();
}

// Hàm tạo chuỗi MIME cho email có file đính kèm
std::string createMimeMessage_2(const std::string& recipient, const std::string& subject, const std::string& bodyText, const std::string& fileBase64, const std::string& fileName) {
    std::ostringstream oss;
    oss << "To: " << recipient << "\r\n"
        << "Subject: " << subject << "\r\n"
        << "MIME-Version: 1.0\r\n"
        << "Content-Type: multipart/mixed; boundary=\"boundary\"\r\n\r\n"
        << "--boundary\r\n"
        << "Content-Type: text/plain; charset=\"UTF-8\"\r\n\r\n"
        << bodyText << "\r\n\r\n"
        << "--boundary\r\n"
        << "Content-Type: application/octet-stream; name=\"" << fileName << "\"\r\n"
        << "Content-Transfer-Encoding: base64\r\n"
        << "Content-Disposition: attachment; filename=\"" << fileName << "\"\r\n\r\n"
        << fileBase64 << "\r\n"
        << "--boundary--";

    return oss.str();
}

// Hàm gửi email với ảnh cv::Mat đã có sẵn
bool sendImagelWithGmailAPI(const std::string& recipient, const std::string& subject, const std::string& bodyText, const cv::Mat& image, const std::string& accessToken) {
    // Mã hóa ảnh thành Base64
    std::string imageBase64 = matToBase64(image);
    // Tạo chuỗi MIME cho email
    std::string mimeMessage = createMimeMessage(recipient, subject, bodyText, imageBase64);
    std::string rawMessage = encodeBase64(mimeMessage);

    // Tạo JSON chứa nội dung email đã mã hóa
    nlohmann::json emailData;
    emailData["raw"] = rawMessage;

    CURL* curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://gmail.googleapis.com/gmail/v1/users/me/messages/send");

        // Thêm access token vào header Authorization
        struct curl_slist* headers = NULL;
        std::string authHeader = "Authorization: Bearer " + accessToken;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, authHeader.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Chuyển emailData sang chuỗi JSON
        std::string jsonData = emailData.dump();
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());

        // Gửi yêu cầu và nhận phản hồi
        CURLcode res = curl_easy_perform(curl);
        bool success = (res == CURLE_OK);

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return success;
    }
    return false;
}

std::string fileToBase64(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    std::ostringstream oss;
    oss << file.rdbuf();
    std::string fileData = oss.str();
    return encodeBase64(fileData);
}

// Hàm gửi email với file đính kèm sử dụng Gmail API
bool sendFileWithGmailAPI(const std::string& accessToken, const std::string& recipientEmail,
    const std::string& subject, const std::string& bodyText, const std::string& filePath) {
    std::string fileBase64 = fileToBase64(filePath);
    std::string fileName = filePath.substr(filePath.find_last_of("/\\") + 1);
    std::string mimeMessage = createMimeMessage_2(recipientEmail, subject, bodyText, fileBase64, fileName);
    std::string rawMessage = encodeBase64(mimeMessage);

    // Tạo JSON chứa nội dung email đã mã hóa
    nlohmann::json emailData;
    emailData["raw"] = rawMessage;

    CURL* curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://gmail.googleapis.com/gmail/v1/users/me/messages/send");

        // Thêm access token vào header Authorization
        struct curl_slist* headers = NULL;
        std::string authHeader = "Authorization: Bearer " + accessToken;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, authHeader.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Chuyển emailData sang chuỗi JSON
        std::string jsonData = emailData.dump();
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());

        // Gửi yêu cầu và nhận phản hồi
        CURLcode res = curl_easy_perform(curl);
        bool success = (res == CURLE_OK);
        if (!success) {
            return false;
        }
        else {
            return true;
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return success;
    }
    return false;
}

std::string base64_encode(const std::string& in) {
    std::string out;
    int val = 0, valb = -6;
    for (unsigned char c : in) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[((val << 8) >> (valb + 8)) & 0x3F]);
    while (out.size() % 4) out.push_back('=');
    return out;
}

// tao mail de gui file TXT
std::string createEmail(const std::string& to, const std::string& subject, const std::string& body, const std::string& filename) {
    std::ostringstream emailStream;

    // Header
    emailStream << "To: " << to << "\r\n";
    emailStream << "Subject: " << subject << "\r\n";
    emailStream << "MIME-Version: 1.0\r\n";
    emailStream << "Content-Type: multipart/mixed; boundary=\"boundary\"\r\n\r\n";

    // Body
    emailStream << "--boundary\r\n";
    emailStream << "Content-Type: text/plain; charset=\"UTF-8\"\r\n\r\n";
    emailStream << body << "\r\n";

    // Attachment
    emailStream << "--boundary\r\n";
    emailStream << "Content-Type: text/plain; name=\"" << filename << "\"\r\n";
    emailStream << "Content-Disposition: attachment; filename=\"" << filename << "\"\r\n";
    emailStream << "Content-Transfer-Encoding: base64\r\n\r\n";

    // Đọc file và mã hóa base64
    std::ifstream file(filename, std::ios::binary);
    if (file) {
        std::string fileContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        emailStream << base64_encode(fileContent);
    }

    emailStream << "\r\n--boundary--\r\n";
    return emailStream.str();
}

// gui file TXT
void sendTXT_Email(const std::string& accessToken, const std::string& to, const std::string& subject, const std::string& body, const std::string& filename) {
    CURL* curl;
    CURLcode res;

    std::string emailContent = createEmail(to, subject, body, filename);
    std::string encodedEmail = base64_encode(emailContent);

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + accessToken).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");

        // Tạo JSON cho API
        std::string jsonData = "{\"raw\": \"" + encodedEmail + "\"}";

        curl_easy_setopt(curl, CURLOPT_URL, "https://gmail.googleapis.com/gmail/v1/users/me/messages/send");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());

        // Gửi yêu cầu
        res = curl_easy_perform(curl);

        // Kiểm tra lỗi
        if (res != CURLE_OK) {
            //std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        // Dọn dẹp
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
}

bool send_Text_email(const std::string& accessToken, std::string recipients, std::string subject, std::string body) {
    CURL* curl;
    CURLcode res;
    // Tạo nội dung email dưới dạng MIME
    std::stringstream emailData;
    emailData << "To: " << recipients; // Người nhận 

    emailData << "\r\n";
    emailData << "Subject: " << subject << "\r\n";
    emailData << "\r\n"; // Dòng trống phân cách tiêu đề và nội dung
    emailData << body;


    // Base64 encode cho  APIGmail
    std::string rawMessage = base64_encode(emailData.str());

    // Cấu trúc JSON cho yêu cầu gửi email
    nlohmann::json jsonMessage = {
        {"raw", rawMessage}
    };

    std::string jsonStr = jsonMessage.dump();

    curl = curl_easy_init();
    if (curl) {
        std::string url = "https://gmail.googleapis.com/gmail/v1/users/me/messages/send";
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, ("Authorization: Bearer " + accessToken).c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonStr.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, jsonStr.length());

        // Thực hiện gửi email
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        if (res != CURLE_OK) {
            return false;
        }
        else {
            return true;
        }
    }
}






