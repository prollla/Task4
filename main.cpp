#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <fstream>
#include <algorithm>
#include <stdexcept>
#include <cstring>


struct UrlInfo {
    CString file_name;
    int line_number;
    int column_number;
};

class HtmlFilesParser {
public:
    HtmlFilesParser(const CString& files_list_str) : files_list(files_list_str) {
        // Проверка, что строка с файлами не пустая
        if (files_list.IsEmpty()) {
            throw std::invalid_argument("Files list is empty");
        }
    }

    std::vector<UrlInfo> FindUrls() {
        std::vector<UrlInfo> result;
        CString delimiter = _T(",");
        CString file_name = files_list.Tokenize(delimiter, 0);

        while (file_name != _T("")) {
            std::ifstream file(file_name);
            if (!file) {
                throw std::invalid_argument("File does not exist: " + std::string(CT2A(file_name)));
            }

            CString file_str;
            CString line_str;
            int line_number = 0;

            while (std::getline(file, std::string(CT2A(line_str)))) {
                line_number++;
                file_str += line_str + _T("\n");
            }

            std::regex url_regex("(http|ftp|gopher|news|telnet|file)://www\\.tratata\\.ppp\\.ru");
            std::smatch match;
            int pos = 0;

            while (std::regex_search(std::string(CT2A(file_str)), match, url_regex)) {
                UrlInfo url_info;
                url_info.file_name = file_name;
                url_info.line_number = line_number;
                url_info.column_number = match.position() - pos + 1;
                result.push_back(url_info);

                pos += match.position() + match.length();
                file_str = file_str.Right(file_str.GetLength() - pos);
            }

            file_name = files_list.Tokenize(delimiter, 0);
        }

        return result;
    }

private:
    CString files_list;
};

int main() {
    try {
        HtmlFilesParser parser(_T("file1.html,file2.html,file3.html"));
        auto urls = parser.FindUrls();

        for (const auto& url_info : urls) {
            std::cout << CT2A(url_info.file_name) << ": " << url_info.line_number << ", " << url_info.column_number << std::endl;
        }
    }
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
