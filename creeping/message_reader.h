#include <string>
#include <vector>
#include <libxml/tree.h>
#include <libxml/parser.h>

class MessageReader
{
	public:
		void make(char * msg, bool isRss, std::vector<std::string> * ret_text, std::string * ret_logo);
	private:
		void parse_rss_items(xmlNode * item_node, std::string * ret, std::string * url);
		void rss_to_text(std::string * msg, std::string * logo);
		void del_html_tags(std::string * msg);
		void fix_utf8_string(std::string * str);
		
};
