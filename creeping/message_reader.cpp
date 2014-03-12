#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>

#include "utf8cpp/utf8.h"
#include "message_reader.h"
#include "utils.h"

#define MAX_TEXT_LENGTH 10



void MessageReader::
make(char * msg, bool isRss, std::vector<std::string> * ret_text, std::string * ret_logo)
{

	
	std::string text;
	if(isRss)
	{
		utils_net_dowmload_data(msg,&text);
		if(text.empty())
			text = "can't download rss";
		else
			rss_to_text(&text, ret_logo);
	}else{
		text.assign(msg);
	}
	
	fix_utf8_string(&text);

	int start_pos = 0,
		end_pos = text.length();

	while(start_pos != end_pos)
	{
		int offset = end_pos - start_pos >= MAX_TEXT_LENGTH ? MAX_TEXT_LENGTH : (end_pos - start_pos);		
		ret_text->push_back(text.substr(start_pos,offset));
		start_pos += offset;
	}
	
}



void MessageReader::
del_html_tags(std::string * msg)
{
	std::string key_item = "&lt;";
	std::string key_itemend = "&gt;";

	std::size_t found_key_item = 0;
	std::size_t found_key_itemend = 0;
	while(!msg->empty())
	{
		found_key_item = msg->find(key_item,found_key_item);
		found_key_itemend = msg->find(key_itemend,found_key_item);
		
		if(found_key_item == std::string::npos ||
		   found_key_itemend == std::string::npos)
			break;
		
		msg->erase( found_key_item , 
					found_key_itemend + key_itemend.length()- found_key_item
				   );
	}

	
}

void MessageReader::
parse_rss_items(xmlNode * item_node, std::string * text, std::string * url)
{
	xmlNode *cur_node = NULL;
	for (cur_node = item_node; cur_node; cur_node = cur_node->next)
	{
		
		if (cur_node->type != XML_ELEMENT_NODE)
			continue;
		if (strcmp((char *)cur_node->name, "url") == 0)
		{
			*url = (char *)xmlNodeGetContent(cur_node);
			continue;
		}
		if (strcmp((char *)cur_node->name, "item") != 0)
		{
			parse_rss_items(cur_node->children, text, url);
			continue;
		}
		xmlNode *cur_child_node = NULL;
		std::string title,description;
		for (cur_child_node = cur_node->children; cur_child_node; cur_child_node = cur_child_node->next) 
		{
			if (cur_child_node->type != XML_ELEMENT_NODE)
				continue;
			if (!strcmp((char *)cur_child_node->name, "title")) 
				title = (char *)xmlNodeGetContent(cur_child_node);
			if (!strcmp((char *)cur_child_node->name, "description"))
				description = (char *)xmlNodeGetContent(cur_child_node);
			
		}
		*text += title + " ";
		*text += description + " ";
    }
}

void MessageReader::
rss_to_text(std::string * msg, std::string * logo)
{
	std::string ret_text, path_to_logo, url_logo;;
	del_html_tags(msg);

	xmlDoc *doc = NULL;
	xmlNode *root_element = NULL;
	LIBXML_TEST_VERSION
	doc = xmlParseMemory(msg->c_str(), msg->capacity());
	root_element = xmlDocGetRootElement(doc);
	parse_rss_items(root_element, &ret_text, &url_logo);
	xmlFreeDoc(doc);
	xmlCleanupParser();

	
	std::size_t pos = 0;
	while (pos != std::string::npos)
	{
		pos=ret_text.find_first_of("\t\r\n\0",pos+1);
		ret_text[pos]=' ';
	}
	
	std::string key_space = "  ";
	std::size_t found_key_space = 0;
	while(!ret_text.empty())
	{
		found_key_space = ret_text.find(key_space,found_key_space);
		
		if(found_key_space == std::string::npos)
			break;
		
		ret_text.replace(found_key_space , 
					key_space.length(),
					" "
				   );
	}
	
	
	if(!url_logo.empty())
	{
		std::size_t i = 0;
		std::string logo_file_name;
		for(pos = 0; i < url_logo.length(); i++)
			logo_file_name += url_logo[i] != '/' ? url_logo[i] : '_';
		utils_make_tmp_file_path(&path_to_logo,logo_file_name.c_str());
		utils_net_dowmload_file(url_logo.c_str(), path_to_logo.c_str());
	}
	
	msg->assign(ret_text);
	logo->assign(path_to_logo);
	
}

void MessageReader::
fix_utf8_string(std::string * str)
{
    std::string temp;
    utf8::replace_invalid(str->begin(), str->end(), back_inserter(temp));
    str->assign(temp);
}
