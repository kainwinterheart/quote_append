#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <unordered_map>
#include <string>
#include <utility>
#include <cstdlib>
#include <chrono>
#include <ctime>

static const size_t max_len = 4096;
static const std::unordered_map<char, char> quotes = {
    {'\n', 'n'},
    {'\0', '0'},
    {'\t', 't'},
    {'\r', 'r'}
};

static const char quotes2 [256] = {'0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 't', 'n', '\0', '\0', 'r', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'};

static std::string&& quote_append(const std::string& in) {
    size_t in_len = in.length();
    size_t out_len = in_len * 2;
    std::string out (out_len, '\0');
    size_t out_pos = 0;

    for(size_t i = 0; i < in_len; ++i) {
        auto it = quotes.find(in[i]);

        if(it == quotes.end()) {
            out[out_pos++] = in[i];
        } else {
            out[out_pos++] = '\\';
            out[out_pos++] = it->second;
        }
    }

    out.shrink_to_fit();

    return std::move(out);
}

static char* quote_append(const char* in, const size_t& in_len, size_t& out_len) {
    out_len = in_len * 2;
    char* out = (char*)malloc(out_len);
    size_t out_pos = 0;

    for(size_t i = 0; i < in_len; ++i) {
        auto it = quotes.find(in[i]);

        if(it == quotes.end()) {
            out[out_pos++] = in[i];
        } else {
            out[out_pos++] = '\\';
            out[out_pos++] = it->second;
        }
    }

    out_len -= out_len - out_pos;
    out = (char*)realloc(out, out_len);

    return out;
}

static std::string&& quote_append2(const std::string& in) {
    size_t in_len = in.length();
    size_t out_len = in_len * 2;
    std::string out (out_len, '\0');
    size_t out_pos = 0;

    for(size_t i = 0; i < in_len; ++i) {
        if(quotes2[in[i]]) {
            out[out_pos++] = '\\';
            out[out_pos++] = quotes2[in[i]];
        } else {
            out[out_pos++] = in[i];
        }
    }

    out.shrink_to_fit();

    return std::move(out);
}

static char* quote_append2(const char* in, const size_t& in_len, size_t& out_len) {
    out_len = in_len * 2;
    char* out = (char*)malloc(out_len);
    size_t out_pos = 0;

    for(size_t i = 0; i < in_len; ++i) {
        if(quotes2[in[i]]) {
            out[out_pos++] = '\\';
            out[out_pos++] = quotes2[in[i]];
        } else {
            out[out_pos++] = in[i];
        }
    }

    out_len -= out_len - out_pos;
    out = (char*)realloc(out, out_len);

    return out;
}

static std::string&& quote_append3(const std::string& in) {
    std::string out;

    for(const char& c : in) {
        if(quotes2[c]) {
            out += '\\';
            out += quotes2[c];
        } else {
            out += c;
        }
    }

    return std::move(out);
}

static std::string&& quote_append4(const std::string& in) {
    std::string out;

    for(const char& c : in) {
        switch(c) {
            case '\t':
                out += "\\t";
                break;
            case '\n':
                out += "\\n";
                break;
            case '\r':
                out += "\\r";
                break;
            case '\0':
                out += "\\0";
                break;
            default:
                out += c;
                break;
        }
    }

    return std::move(out);
}

static const int iterations = 5000000;

template<typename F>
void wtime(const char* str, F& code) {
    std::chrono::time_point<std::chrono::system_clock> start, end;

    start = std::chrono::system_clock::now();
    code();
    end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start;

    printf("%s: %f\n\n", str, elapsed_seconds.count());
}

int main(int argc, char** argv) {
    std::string in;
    char line [max_len];
    size_t len;
    size_t in_len = 0;

    while((len = read(0, line, max_len)) > 0) {
        in.append(line, len);
        in_len += len;
    }

    const char* in_cstr = in.c_str();

    {
        std::string out (quote_append(in));

        printf("%s\n", out.c_str());

        {
            auto code = [&in](){
                for(int i = 0; i < iterations; ++i) {
                    quote_append(in);
                }
            };
            wtime("quote_append(std::string)", code);
        }
    }

    {
        size_t out_len;
        char* out = quote_append(in_cstr, in_len, out_len);

        printf("%zu, %s\n", out_len, out);

        {
            auto code = [&in_cstr, &in_len, &out_len](){
                for(int i = 0; i < iterations; ++i) {
                    quote_append(in_cstr, in_len, out_len);
                }
            };
            wtime("quote_append(char*)", code);
        }
    }

    {
        std::string out (quote_append2(in));

        printf("%s\n", out.c_str());

        {
            auto code = [&in](){
                for(int i = 0; i < iterations; ++i) {
                    quote_append2(in);
                }
            };
            wtime("quote_append2(std::string)", code);
        }
    }

    {
        size_t out_len;
        char* out = quote_append2(in_cstr, in_len, out_len);

        printf("%zu, %s\n", out_len, out);

        {
            auto code = [&in_cstr, &in_len, &out_len](){
                for(int i = 0; i < iterations; ++i) {
                    quote_append2(in_cstr, in_len, out_len);
                }
            };
            wtime("quote_append2(char*)", code);
        }
    }

    {
        std::string out (quote_append3(in));

        printf("%s\n", out.c_str());

        {
            auto code = [&in](){
                for(int i = 0; i < iterations; ++i) {
                    quote_append3(in);
                }
            };
            wtime("quote_append3(std::string)", code);
        }
    }

    {
        std::string out (quote_append4(in));

        printf("%s\n", out.c_str());

        {
            auto code = [&in](){
                for(int i = 0; i < iterations; ++i) {
                    quote_append4(in);
                }
            };
            wtime("quote_append4(std::string)", code);
        }
    }

    return 0;
}
