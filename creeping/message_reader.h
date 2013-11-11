#include <string>
#include <vector>


class MessageReader
{
	public:
		void make(char * msg, bool isRss, std::vector<std::string> * ret_text, std::string * ret_logo);
	private:
		void rss_to_text(std::string * msg, std::string * logo);
		std::size_t get_element(std::string * msg, std::string * ret, const char * item, std::size_t start_pos);
		void del_html_tags(std::string * msg);
		void fix_utf8_string(std::string * str);
		
};
