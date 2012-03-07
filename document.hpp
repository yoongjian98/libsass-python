#include <map>
#include "node.hpp"

namespace Sass {
  using std::vector;
  using std::map;
  using namespace Prelexer;

  struct Document {
    enum CSS_Style { nested, expanded, compact, compressed };
    
    char* path;
    char* source;
    char* position;
    size_t line_number;
    
    // TO DO: move the environment up into the context class when it's ready
    map<Token, Node> environment;
    
    vector<Node> statements;
    Token top;
    bool last_munch_succeeded;
    
    Document(char* _path, char* _source = 0);
    ~Document();
    
    template <prelexer mx>
    char* peek() { return mx(position); }

    template <prelexer mx>
    bool try_munching() {
      char* after_whitespace;
      if (mx == block_comment) {
        after_whitespace =
          zero_plus< alternatives<spaces, line_comment> >(position);
      }
      else if (mx == spaces || mx == ancestor_of) {
        after_whitespace = spaces(position);
        if (after_whitespace) {
          top = Token(position, after_whitespace);
          line_number += count_interval<'\n'>(position, after_whitespace);
          position = after_whitespace;
          return last_munch_succeeded = true;
        }
        else {
          return last_munch_succeeded = false;
        }
      }
      else if (mx == optional_spaces) {
        after_whitespace = optional_spaces(position);
      }
      else {
        after_whitespace = spaces_and_comments(position);
      }
      line_number += count_interval<'\n'>(position, after_whitespace);
      char* after_token = mx(after_whitespace);
      if (after_token) {
        top = Token(after_whitespace, after_token);
        position = after_token;
        return last_munch_succeeded = true;
      }
      else {
        return last_munch_succeeded = false;
      }
    }
    
    void parse_scss();
    Node parse_statement();
    Node parse_var_def();
    Node parse_ruleset();
    Node parse_selector();
    Node parse_block();
    Node parse_values();
    
    string emit_css(CSS_Style style);
  };
}