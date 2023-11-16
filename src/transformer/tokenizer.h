
#pragma once

#include <stdint.h>

#include <memory>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <span>

namespace cpuft {

enum class TokenType {
    UNKNOWN      = 0,
    NORMAL       = 1,
    CONTROL      = 2,
    BYTE         = 3,
    USER_DEFINED = 4,
    UNUSED       = 5,
};

struct Token {
    char*       index_text;
    char*       show_text;
    TokenType   type;
    float       score;
};

enum class SpecialTokenType {
    NONE = 0,
    BOS  = 1,
    EOS  = 2,
    PAD  = 3,

    MAX  = 8,
};

class Tokenizer {
public:
    Tokenizer();
    ~Tokenizer() { release(); }
    Tokenizer(Tokenizer&& other);

    Tokenizer& operator=(Tokenizer&& other);

    bool load(std::string_view path, int vocab_size);
    bool set(int vocab_size,
             std::unique_ptr<Token[]>&& vocab,
             std::unique_ptr<char[]>&& text_data,
             std::string_view conn_tag,
             int special_tokens[int(SpecialTokenType::MAX)]) noexcept;

    int encode(std::string_view text, std::span<int> output_tokens, bool add_bos=false, bool add_eos=false) const noexcept;
    const char* decode(int token, int prev_token=-1) const noexcept;
    std::string_view decode(std::span<const int> tokens, std::span<char> buffer) const noexcept;

    inline int vocab_size() const noexcept {
        return _vocab_size;
    }
    inline int bos_token_id() const noexcept {
        return _bos_token_id;
    }
    inline int eos_token_id() const noexcept {
        return _eos_token_id;
    }
    inline int pad_token_id() const noexcept {
        return _pad_token_id;
    }
    inline const std::string& name() const noexcept {
        return _name;
    }

    inline void set_vocab_size(int vocab_size) {
        _vocab_size = vocab_size;
    }
    void set_token_types (std::span<const int>   types);
    void set_token_scores(std::span<const float> scores);

    void set_token_texts (const std::vector<std::string>& texts);
    void set_token_types (const std::vector<int>& types) {
        return set_token_types({types.data(), types.size()});
    }
    void set_token_scores(const std::vector<float>& scores) {
        return set_token_scores({scores.data(), scores.size()});
    }

    inline void set_bos_token_id(int bos_token_id) noexcept {
        _bos_token_id = bos_token_id;
    }
    inline void set_eos_token_id(int eos_token_id) noexcept {
        _eos_token_id = eos_token_id;
    }
    inline void set_pad_token_id(int pad_token_id) noexcept {
        _pad_token_id = pad_token_id;
    }
    inline void set_name(std::string_view name) noexcept {
        _name = name;
    }
    void release() noexcept {
        _vocab.reset();
        _text_data.reset();
    }

    const Token& operator[](int i) const noexcept {
        return _vocab[i];
    }

protected:
    void build_text2id_map();

    int  search_text(std::string_view text) const noexcept;

private:
    std::unique_ptr<Token[]>      _vocab;
    int                           _vocab_size   = 0;
    int                           _bos_token_id = 1;
    int                           _eos_token_id = 2;
    int                           _pad_token_id = 0;
    int                           _underline_id = -1;
    int                           _max_token_length = 0;
    std::string_view              _conn_tag;

    std::unordered_map<std::string_view, int> _text2id;
    char                          _byte_pieces[512];

    // 低频读取的字段放到后边
    std::string                   _name;
    std::unique_ptr<char[]>       _text_data;
};

}
