#include <stdio.h>
#include <stdlib.h>

#include "utf8cpp/utf8.h"
#include "message_reader.h"


#define MAX_TEXT_LENGTH 10

void MessageReader::
make(char * msg, bool isRss, std::vector<std::string> * ret_text, std::string * ret_logo)
{

	
    std::string text;
    
    if(isRss)
    {
		std::string cmd("wget -O - \"");
		cmd += msg;
		cmd += "\"\n";
		FILE* pipe = popen(cmd.c_str(), "r");
		if (pipe)
		{
			char buffer[128];
			while(!feof(pipe))
				if(fgets(buffer, 128, pipe) != NULL)
					text += buffer;
			pclose(pipe);
		}
		
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


std::size_t MessageReader::
get_element(std::string * msg, std::string * ret, const char * item, std::size_t start_pos)
{
	std::string key_item = "<>";
	std::string key_itemend = "</>";
	key_item.insert(1,item);
	key_itemend.insert(2,item);
	std::size_t found_key_item = start_pos;
	std::size_t found_key_itemend = start_pos;
	found_key_item = msg->find(key_item,found_key_itemend);
	found_key_itemend = msg->find(key_itemend,found_key_item);
	
	if(found_key_item != std::string::npos)
	{
		ret->assign( msg->substr(found_key_item + key_item.length(), 
								found_key_itemend - (found_key_item + key_item.length()))
				);
		found_key_itemend += key_itemend.length();
	}	
	
	return found_key_itemend;
		

	
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
rss_to_text(std::string * msg, std::string * logo)
{
	std::string ret_text;
	std::size_t pos = 0;
	while(pos != std::string::npos)
	{
		std::string item, title, description;
		pos = get_element(msg, &item, "item", pos);
		get_element(&item, &title, "title", 0);
		get_element(&item, &description, "description", 0);
		ret_text += title + " " + description;
		ret_text += " ";
	}
	
	del_html_tags(&ret_text);
	pos = 0;
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
	
	std::string logo_file_name, url_logo;
	get_element(msg, &url_logo, "url", 0);
	
	if(!url_logo.empty())
	{
		 std::size_t i = 0;
		 for(pos = 0; i < url_logo.length(); i++)
			logo_file_name += url_logo[i] != '/' ? url_logo[i] : '_';
		 logo_file_name = "/tmp/" + logo_file_name;
		 std::string cmd("wget -O \"");
		 cmd += logo_file_name;
		 cmd += "\" \"" + url_logo + "\"";
		 
		 system(cmd.c_str());
	}
	
	msg->assign(ret_text);
	logo->assign(logo_file_name);
	
}

void MessageReader::
fix_utf8_string(std::string * str)
{
    std::string temp;
    utf8::replace_invalid(str->begin(), str->end(), back_inserter(temp));
    str->assign(temp);
}
