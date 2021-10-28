#pragma once
#include <string>
#include <fstream>
#include <vector>

#define NAJG_VERSION 1.0

class najg
{
    private:
        enum type
        {
            VALUE,
            ARRAY,
            SUBCLASS,
            OTHER
        };

        static void remove_whitespace(std::string& string)
        {
            std::string res;
            for (int i = 0; i < string.size(); i++)
            {
                if (string[i] != ' ')
                {
                    res += string[i];
                }
            }
            string = res;
        }
        static bool is_whitespace(std::string string)
        {
            int whitespace = 0;
            for (; string[whitespace] == ' '; whitespace++);

            if (whitespace == string.size()) { return true; }
            return false;
        }

        struct subclass_data
        {
            std::vector<std::string> titles;
            std::vector<std::string> array_titles; 
            std::vector<std::string> values;

            std::vector< std::vector<std::string> > arrays;
        };

        static type parse_line(std::string& title, std::string& value, std::vector<std::string>& array_elements, subclass_data& subclass_elements, std::ifstream& json, bool& is_subclass)
        {
            std::string line;

            std::getline(json, line);
            if (line != "{" && line != "}" && !is_whitespace(line))
            {
                title = "";
                value = "";

                int title_index = 1;
                remove_whitespace(line);

                if ((line == "}" || line == "},") && is_subclass)
                {
                    is_subclass = false;
                    return OTHER;
                }

                switch(line[line.size() - 1])
                {
                    case '{':
                        /* Element title */
                        for (; line[title_index] != '"'; title_index++)
                        {
                            title += line[title_index];
                        }

                        /* Subclass elements */
                        is_subclass = true;
                        return SUBCLASS;
                    case '[':
                        /* Element title */
                        for (; line[title_index] != '"'; title_index++)
                        {
                            title += line[title_index];
                        }

                        /* Array elements */
                        while (line != "]")
                        {
                            std::string res;
                            std::getline(json, line);
                            remove_whitespace(line);

                            for (int i = 1; i < line.size(); i++)
                            {
                                if (line[i] != '"' && line[i] != ',')
                                {
                                    res += line[i];
                                }
                            }
                            array_elements.push_back(res);
                        }
                        array_elements.pop_back();

                        if (is_subclass)
                        {
                            subclass_elements.arrays.push_back(array_elements);
                            array_elements.clear();

                            subclass_elements.array_titles.push_back(title);
                        }
                        return ARRAY;
                    default:
                        /* Element title */
                        int title_index = 1;
                        for (; line[title_index] != '"'; title_index++)
                        {
                            title += line[title_index];
                        }

                        /* Element value */
                        for (; title_index < line.size(); title_index++)
                        {   
                            if (line[title_index] != '"' && line[title_index] != ':' && line[title_index] != ',')
                            {
                                value += line[title_index];
                            }
                        }

                        if (is_subclass)
                        {
                            subclass_elements.titles.push_back(title);
                            title = "";

                            subclass_elements.values.push_back(value);
                            value = "";
                        }
                        return VALUE;
                }
            } else {
                value = line;
            }
            return OTHER;
        }
    public:
        struct output
        {
            std::vector<std::string> titles, values;

            std::vector<std::string> array_titles;
            std::vector< std::vector<std::string> > array_elements;

            std::vector<std::string> subclass_titles;
            std::vector<subclass_data> subclass_elements;
        };

        static void parse(output& res, const std::string file)
        {
            bool is_subclass;
            std::ifstream json(file);

            std::string title, value;
            std::vector<std::string> array;
            subclass_data subclass;

            while (!json.eof())
            {
                switch(parse_line(title, value, array, subclass, json, is_subclass))
                {
                    case VALUE:
                        res.titles.push_back(title);
                        res.values.push_back(value);
                    break;
                    case ARRAY:
                        res.array_titles.push_back(title);
                        res.array_elements.push_back(array);
                    break;
                    case SUBCLASS:
                        res.subclass_titles.push_back(title);
                    break;
                    default:

                    /* Do nothing */

                    break;
                }

                if (subclass.array_titles.size() > 0 || subclass.arrays.size() > 0 || subclass.titles.size() > 0 || subclass.values.size() > 0 && !is_subclass)
                {
                    res.subclass_elements.push_back(subclass);

                    /* Clear elements */
                    subclass.array_titles.clear();
                    subclass.arrays.clear();

                    subclass.titles.clear();
                    subclass.values.clear();
                }
            }
        }

        static output parse(const std::string file)
        { 
            output res;
            bool is_subclass;
            std::ifstream json(file);

            std::string title, value;
            std::vector<std::string> array;
            subclass_data subclass;

            while (!json.eof())
            {
                switch(parse_line(title, value, array, subclass, json, is_subclass))
                {
                    case VALUE:
                        res.titles.push_back(title);
                        res.values.push_back(value);
                    break;
                    case ARRAY:
                        res.array_titles.push_back(title);
                        res.array_elements.push_back(array);
                    break;
                    case SUBCLASS:
                        res.subclass_titles.push_back(title);
                    break;
                    default:

                    /* Do nothing here */

                    break;
                }

                if (subclass.array_titles.size() > 0 || subclass.arrays.size() > 0 || subclass.titles.size() > 0 || subclass.values.size() > 0 && !is_subclass)
                {
                    res.subclass_elements.push_back(subclass);

                    /* Clear elements */
                    subclass.array_titles.clear();
                    subclass.arrays.clear();

                    subclass.titles.clear();
                    subclass.values.clear();
                }
            }
            return res;
        }
};
