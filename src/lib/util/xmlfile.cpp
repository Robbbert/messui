// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/***************************************************************************

    xmlfile.c

    XML file parsing code.

***************************************************************************/

#include <assert.h>

#include "xmlfile.h"
#include <expat.h>

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstring>


/***************************************************************************
    CONSTANTS
***************************************************************************/

#define TEMP_BUFFER_SIZE        4096



/***************************************************************************
    TYPE DEFINITIONS
***************************************************************************/

struct xml_parse_info
{
	XML_Parser          parser;
	xml_data_node *     rootnode;
	xml_data_node *     curnode;
	uint32_t            flags;
};



/***************************************************************************
    PROTOTYPES
***************************************************************************/

/* expat interfaces */
static bool expat_setup_parser(xml_parse_info &info, xml_parse_options const *opts);
static void expat_element_start(void *data, const XML_Char *name, const XML_Char **attributes);
static void expat_data(void *data, const XML_Char *s, int len);
static void expat_element_end(void *data, const XML_Char *name);



/***************************************************************************
    CORE IMPLEMENTATION
***************************************************************************/

/*-------------------------------------------------
    copystring - make an allocated copy of a
    string
-------------------------------------------------*/

static char *copystring(const char *input)
{
	char *newstr;

	/* nullptr just passes through */
	if (input == nullptr)
		return nullptr;

	/* make a lower-case copy if the allocation worked */
	newstr = (char *)malloc(strlen(input) + 1);
	if (newstr != nullptr)
		strcpy(newstr, input);

	return newstr;
}


/*-------------------------------------------------
    copystring_lower - make an allocated copy of
    a string and convert it to lowercase along
    the way
-------------------------------------------------*/

static char *copystring_lower(const char *input)
{
	char *newstr;
	int i;

	/* nullptr just passes through */
	if (input == nullptr)
		return nullptr;

	/* make a lower-case copy if the allocation worked */
	newstr = (char *)malloc(strlen(input) + 1);
	if (newstr != nullptr)
	{
		for (i = 0; input[i] != 0; i++)
			newstr[i] = tolower((uint8_t)input[i]);
		newstr[i] = 0;
	}

	return newstr;
}



/***************************************************************************
    XML FILE OBJECTS
***************************************************************************/

/*-------------------------------------------------
    xml_file_create - create a new xml file
    object
-------------------------------------------------*/

xml_data_node *xml_data_node::file_create()
{
	try { return new xml_data_node(); }
	catch (...) { return nullptr; }
}


/*-------------------------------------------------
    xml_file_read - parse an XML file into its
    nodes
-------------------------------------------------*/

xml_data_node *xml_data_node::file_read(util::core_file &file, xml_parse_options const *opts)
{
	xml_parse_info info;
	int done;

	/* set up the parser */
	if (!expat_setup_parser(info, opts))
		return nullptr;

	/* loop through the file and parse it */
	do
	{
		char tempbuf[TEMP_BUFFER_SIZE];

		/* read as much as we can */
		int bytes = file.read(tempbuf, sizeof(tempbuf));
		done = file.eof();

		/* parse the data */
		if (XML_Parse(info.parser, tempbuf, bytes, done) == XML_STATUS_ERROR)
		{
			if (opts != nullptr && opts->error != nullptr)
			{
				opts->error->error_message = XML_ErrorString(XML_GetErrorCode(info.parser));
				opts->error->error_line = XML_GetCurrentLineNumber(info.parser);
				opts->error->error_column = XML_GetCurrentColumnNumber(info.parser);
			}

			info.rootnode->file_free();
			XML_ParserFree(info.parser);
			return nullptr;
		}

	} while (!done);

	/* free the parser */
	XML_ParserFree(info.parser);

	/* return the root node */
	return info.rootnode;
}


/*-------------------------------------------------
    xml_string_read - parse an XML string into its
    nodes
-------------------------------------------------*/

xml_data_node *xml_data_node::string_read(const char *string, xml_parse_options const *opts)
{
	xml_parse_info info;
	int length = (int)strlen(string);

	/* set up the parser */
	if (!expat_setup_parser(info, opts))
		return nullptr;

	/* parse the data */
	if (XML_Parse(info.parser, string, length, 1) == XML_STATUS_ERROR)
	{
		if (opts != nullptr && opts->error != nullptr)
		{
			opts->error->error_message = XML_ErrorString(XML_GetErrorCode(info.parser));
			opts->error->error_line = XML_GetCurrentLineNumber(info.parser);
			opts->error->error_column = XML_GetCurrentColumnNumber(info.parser);
		}

		info.rootnode->file_free();
		XML_ParserFree(info.parser);
		return nullptr;
	}

	/* free the parser */
	XML_ParserFree(info.parser);

	/* return the root node */
	return info.rootnode;
}


/*-------------------------------------------------
    xml_file_write - write an XML tree to a file
-------------------------------------------------*/

void xml_data_node::file_write(util::core_file &file) const
{
	/* ensure this is a root node */
	if (get_name())
		return;

	/* output a simple header */
	file.printf("<?xml version=\"1.0\"?>\n");
	file.printf("<!-- This file is autogenerated; comments and unknown tags will be stripped -->\n");

	/* loop over children of the root and output */
	for (xml_data_node const *node = get_first_child(); node; node = node->get_next_sibling())
		node->write_recursive(0, file);
}


/*-------------------------------------------------
    xml_file_free - free an XML file object
-------------------------------------------------*/

void xml_data_node::file_free()
{
	/* ensure this is a root node */
	if (get_name())
		return;

	delete this;
}



/***************************************************************************
    XML NODE MANAGEMENT
***************************************************************************/

xml_data_node::xml_data_node()
	: line(0)
	, m_next(nullptr)
	, m_first_child(nullptr)
	, m_name(nullptr)
	, m_value(nullptr)
	, m_parent(nullptr)
	, m_attributes()
{
}

xml_data_node::xml_data_node(xml_data_node *parent, const char *name, const char *value)
	: line(0)
	, m_next(nullptr)
	, m_first_child(nullptr)
	, m_name(::copystring_lower(name))
	, m_value(::copystring(value))
	, m_parent(parent)
	, m_attributes()
{
}


/*-------------------------------------------------
    free_node_recursive - recursively free
    the data allocated to an XML node
-------------------------------------------------*/

xml_data_node::~xml_data_node()
{
	/* free name/value */
	if (m_name)
		std::free((void *)m_name);
	if (m_value)
		std::free((void *)m_value);

	/* free the children */
	for (xml_data_node *nchild = nullptr; m_first_child; m_first_child = nchild)
	{
		/* note the next node and free this node */
		nchild = m_first_child->get_next_sibling();
		delete m_first_child;
	}
}


void xml_data_node::set_value(char const *value)
{
	if (m_value)
		std::free((void *)m_value);

	m_value = ::copystring(value);
}


void xml_data_node::append_value(char const *value, int length)
{
	if (length)
	{
		/* determine how much data we currently have */
		int const oldlen = m_value ? int(std::strlen(m_value)) : 0;

		/* realloc */
		char *const newdata = reinterpret_cast<char *>(std::malloc(oldlen + length + 1));
		if (newdata)
		{
			if (m_value)
			{
				std::memcpy(newdata, m_value, oldlen);
				std::free((void *)m_value);
			}
			m_value = newdata;

			/* copy in the new data and NUL-terminate */
			std::memcpy(&newdata[oldlen], value, length);
			newdata[oldlen + length] = '\0';
		}
	}
}


void xml_data_node::trim_whitespace()
{
	if (m_value)
	{
		char *start = m_value;
		char *end = start + strlen(m_value);

		/* first strip leading spaces */
		while (*start && std::isspace(uint8_t(*start)))
			start++;

		/* then strip trailing spaces */
		while ((end > start) && std::isspace(uint8_t(end[-1])))
			end--;

		if (start == end)
		{
			/* if nothing left, just free it */
			std::free(m_value);
			m_value = nullptr;
		}
		else
		{
			/* otherwise, memmove the data */
			std::memmove(m_value, start, end - start);
			m_value[end - start] = '\0';
		}
	}
}


/*-------------------------------------------------
    xml_data_node::count_children - count the
    number of child nodes
-------------------------------------------------*/

int xml_data_node::count_children() const
{
	int count = 0;

	/* loop over children and count */
	for (xml_data_node const *node = get_first_child(); node; node = node->get_next_sibling())
		count++;
	return count;
}


/*-------------------------------------------------
    xml_data_node::get_child - find the first
    child of the specified node with the specified
    tag
-------------------------------------------------*/

xml_data_node *xml_data_node::get_child(const char *name)
{
	return m_first_child ? m_first_child->get_sibling(name) : nullptr;
}

xml_data_node const *xml_data_node::get_child(const char *name) const
{
	return m_first_child ? m_first_child->get_sibling(name) : nullptr;
}


/*-------------------------------------------------
    xml_find_first_matching_child - find the first
    child of the specified node with the
    specified tag or attribute/value pair
-------------------------------------------------*/

xml_data_node *xml_data_node::find_first_matching_child(const char *name, const char *attribute, const char *matchval)
{
	return m_first_child ? m_first_child->find_matching_sibling(name, attribute, matchval) : nullptr;
}

xml_data_node const *xml_data_node::find_first_matching_child(const char *name, const char *attribute, const char *matchval) const
{
	return m_first_child ? m_first_child->find_matching_sibling(name, attribute, matchval) : nullptr;
}


/*-------------------------------------------------
    xml_data_node::get_next_sibling - find the
    next sibling of the specified node with the
    specified tag
-------------------------------------------------*/

xml_data_node *xml_data_node::get_next_sibling(const char *name)
{
	return m_next ? m_next->get_sibling(name) : nullptr;
}

xml_data_node const *xml_data_node::get_next_sibling(const char *name) const
{
	return m_next ? m_next->get_sibling(name) : nullptr;
}


/*-------------------------------------------------
    xml_find_next_matching_sibling - find the next
    sibling of the specified node with the
    specified tag or attribute/value pair
-------------------------------------------------*/

xml_data_node *xml_data_node::find_next_matching_sibling(const char *name, const char *attribute, const char *matchval)
{
	return m_next ? m_next->find_matching_sibling(name, attribute, matchval) : nullptr;
}

xml_data_node const *xml_data_node::find_next_matching_sibling(const char *name, const char *attribute, const char *matchval) const
{
	return m_next ? m_next->find_matching_sibling(name, attribute, matchval) : nullptr;
}


/*-------------------------------------------------
    xml_add_child - add a new child node to the
    given node
-------------------------------------------------*/

xml_data_node *xml_data_node::add_child(const char *name, const char *value)
{
	/* new element: create a new node */
	xml_data_node *node;
	try { node = new xml_data_node(this, name, value); }
	catch (...) { return nullptr; }

	if (!node->get_name() || (!node->get_value() && value))
	{
		delete node;
		return nullptr;
	}

	/* add us to the end of the list of siblings */
	xml_data_node **pnode;
	for (pnode = &m_first_child; *pnode; pnode = &(*pnode)->m_next) { }
	*pnode = node;

	return node;
}


/*-------------------------------------------------
    xml_get_or_add_child - find a child node of
    the specified type; if not found, add one
-------------------------------------------------*/

xml_data_node *xml_data_node::get_or_add_child(const char *name, const char *value)
{
	/* find the child first */
	xml_data_node *const child = m_first_child->get_sibling(name);
	if (child)
		return child;

	/* if not found, do a standard add child */
	return add_child(name, value);
}


/*-------------------------------------------------
    xml_delete_node - delete a node and its
    children
-------------------------------------------------*/

void xml_data_node::delete_node()
{
	/* first unhook us from the list of children of our parent */
	if (m_parent)
	{
		for (xml_data_node **pnode = &m_parent->m_first_child; *pnode; pnode = &(*pnode)->m_next)
		{
			if (*pnode == this)
			{
				*pnode = this->m_next;
				break;
			}
		}
	}

	/* now free ourselves and our children */
	delete this;
}


/*-------------------------------------------------
    xml_get_next_sibling - find the next sibling of
    the specified node with the specified tag
-------------------------------------------------*/

xml_data_node *xml_data_node::get_sibling(const char *name)
{
	/* loop over siblings and find a matching name */
	for (xml_data_node *node = this; node; node = node->get_next_sibling())
		if (strcmp(node->get_name(), name) == 0)
			return node;
	return nullptr;
}

xml_data_node const *xml_data_node::get_sibling(const char *name) const
{
	/* loop over siblings and find a matching name */
	for (xml_data_node const *node = this; node; node = node->get_next_sibling())
		if (strcmp(node->get_name(), name) == 0)
			return node;
	return nullptr;
}


/*-------------------------------------------------
    xml_find_matching_sibling - find the next
    sibling of the specified node with the
    specified tag or attribute/value pair
-------------------------------------------------*/

xml_data_node *xml_data_node::find_matching_sibling(const char *name, const char *attribute, const char *matchval)
{
	/* loop over siblings and find a matching attribute */
	for (xml_data_node *node = this; node; node = node->get_next_sibling())
	{
		/* can pass nullptr as a wildcard for the node name */
		if (!name || !strcmp(name, node->get_name()))
		{
			/* find a matching attribute */
			attribute_node const *const attr = node->get_attribute(attribute);
			if (attr && !strcmp(attr->value.c_str(), matchval))
				return node;
		}
	}
	return nullptr;
}

xml_data_node const *xml_data_node::find_matching_sibling(const char *name, const char *attribute, const char *matchval) const
{
	/* loop over siblings and find a matching attribute */
	for (xml_data_node const *node = this; node; node = node->get_next_sibling())
	{
		/* can pass nullptr as a wildcard for the node name */
		if (!name || !strcmp(name, node->get_name()))
		{
			/* find a matching attribute */
			attribute_node const *const attr = node->get_attribute(attribute);
			if (attr && !strcmp(attr->value.c_str(), matchval))
				return node;
		}
	}
	return nullptr;
}



/***************************************************************************
    XML ATTRIBUTE MANAGEMENT
***************************************************************************/

bool xml_data_node::has_attribute(const char *attribute) const
{
	return get_attribute(attribute) != nullptr;
}


/*-------------------------------------------------
    xml_get_attribute - get the value of the
    specified attribute, or nullptr if not found
-------------------------------------------------*/

xml_data_node::attribute_node *xml_data_node::get_attribute(const char *attribute)
{
	/* loop over attributes and find a match */
	for (attribute_node &anode : m_attributes)
		if (strcmp(anode.name.c_str(), attribute) == 0)
			return &anode;
	return nullptr;
}

xml_data_node::attribute_node const *xml_data_node::get_attribute(const char *attribute) const
{
	/* loop over attributes and find a match */
	for (attribute_node const &anode : m_attributes)
		if (strcmp(anode.name.c_str(), attribute) == 0)
			return &anode;
	return nullptr;
}


/*-------------------------------------------------
    xml_get_attribute_string - get the string
    value of the specified attribute; if not
    found, return = the provided default
-------------------------------------------------*/

const char *xml_data_node::get_attribute_string(const char *attribute, const char *defvalue) const
{
	attribute_node const *attr = get_attribute(attribute);
	return attr ? attr->value.c_str() : defvalue;
}


/*-------------------------------------------------
    xml_get_attribute_int - get the integer
    value of the specified attribute; if not
    found, return = the provided default
-------------------------------------------------*/

int xml_data_node::get_attribute_int(const char *attribute, int defvalue) const
{
	char const *const string = get_attribute_string(attribute, nullptr);
	int value;
	unsigned int uvalue;

	if (string == nullptr)
		return defvalue;
	if (string[0] == '$')
		return (sscanf(&string[1], "%X", &uvalue) == 1) ? uvalue : defvalue;
	if (string[0] == '0' && string[1] == 'x')
		return (sscanf(&string[2], "%X", &uvalue) == 1) ? uvalue : defvalue;
	if (string[0] == '#')
		return (sscanf(&string[1], "%d", &value) == 1) ? value : defvalue;
	return (sscanf(&string[0], "%d", &value) == 1) ? value : defvalue;
}


/*-------------------------------------------------
    xml_get_attribute_int_format - return the
    format of the given integer attribute
-------------------------------------------------*/

int xml_data_node::get_attribute_int_format(const char *attribute) const
{
	char const *const string = get_attribute_string(attribute, nullptr);

	if (string == nullptr)
		return XML_INT_FORMAT_DECIMAL;
	if (string[0] == '$')
		return XML_INT_FORMAT_HEX_DOLLAR;
	if (string[0] == '0' && string[1] == 'x')
		return XML_INT_FORMAT_HEX_C;
	if (string[0] == '#')
		return XML_INT_FORMAT_DECIMAL_POUND;
	return XML_INT_FORMAT_DECIMAL;
}


/*-------------------------------------------------
    xml_get_attribute_float - get the float
    value of the specified attribute; if not
    found, return = the provided default
-------------------------------------------------*/

float xml_data_node::get_attribute_float(const char *attribute, float defvalue) const
{
	char const *const string = get_attribute_string(attribute, nullptr);
	float value;

	if (string == nullptr || sscanf(string, "%f", &value) != 1)
		return defvalue;
	return value;
}


/*-------------------------------------------------
    xml_set_attribute - set a new attribute and
    string value on the node
-------------------------------------------------*/

void xml_data_node::set_attribute(const char *name, const char *value)
{
	attribute_node *anode;

	/* first find an existing one to replace */
	anode = get_attribute(name);

	if (anode != nullptr)
	{
		/* if we found it, free the old value and replace it */
		anode->value = value;
	}
	else
	{
		/* otherwise, create a new node */
		add_attribute(name, value);
	}
}


/*-------------------------------------------------
    xml_set_attribute_int - set a new attribute and
    integer value on the node
-------------------------------------------------*/

void xml_data_node::set_attribute_int(const char *name, int value)
{
	char buffer[100];
	sprintf(buffer, "%d", value);
	set_attribute(name, buffer);
}


/*-------------------------------------------------
    xml_set_attribute_int - set a new attribute and
    float value on the node
-------------------------------------------------*/

void xml_data_node::set_attribute_float(const char *name, float value)
{
	char buffer[100];
	sprintf(buffer, "%f", (double) value);
	set_attribute(name, buffer);
}



/***************************************************************************
    MISCELLANEOUS INTERFACES
***************************************************************************/

/*-------------------------------------------------
    xml_normalize_string - normalize a string
    to ensure it doesn't contain embedded tags
-------------------------------------------------*/

const char *xml_normalize_string(const char *string)
{
	static char buffer[1024];
	char *d = &buffer[0];

	if (string != nullptr)
	{
		while (*string)
		{
			switch (*string)
			{
				case '\"' : d += sprintf(d, "&quot;"); break;
				case '&'  : d += sprintf(d, "&amp;"); break;
				case '<'  : d += sprintf(d, "&lt;"); break;
				case '>'  : d += sprintf(d, "&gt;"); break;
				default:
					*d++ = *string;
			}
			++string;
		}
	}
	*d++ = 0;
	return buffer;
}



/***************************************************************************
    EXPAT INTERFACES
***************************************************************************/

/*-------------------------------------------------
    expat_malloc/expat_realloc/expat_free -
    wrappers for memory allocation functions so
    that they pass through out memory tracking
    systems
-------------------------------------------------*/

static void *expat_malloc(size_t size)
{
	uint32_t *result = (uint32_t *)malloc(size + 4 * sizeof(uint32_t));
	*result = size;
	return &result[4];
}

static void expat_free(void *ptr)
{
	if (ptr != nullptr)
		free(&((uint32_t *)ptr)[-4]);
}

static void *expat_realloc(void *ptr, size_t size)
{
	void *newptr = expat_malloc(size);
	if (newptr == nullptr)
		return nullptr;
	if (ptr != nullptr)
	{
		uint32_t oldsize = ((uint32_t *)ptr)[-4];
		memcpy(newptr, ptr, oldsize);
		expat_free(ptr);
	}
	return newptr;
}


/*-------------------------------------------------
    expat_setup_parser - set up expat for parsing
-------------------------------------------------*/

static bool expat_setup_parser(xml_parse_info &info, xml_parse_options const *opts)
{
	XML_Memory_Handling_Suite memcallbacks;

	/* setup info structure */
	memset(&info, 0, sizeof(info));
	if (opts != nullptr)
	{
		info.flags = opts->flags;
		if (opts->error != nullptr)
		{
			opts->error->error_message = nullptr;
			opts->error->error_line = 0;
			opts->error->error_column = 0;
		}
	}

	/* create a root node */
	info.rootnode = xml_data_node::file_create();
	if (info.rootnode == nullptr)
		return false;
	info.curnode = info.rootnode;

	/* create the XML parser */
	memcallbacks.malloc_fcn = expat_malloc;
	memcallbacks.realloc_fcn = expat_realloc;
	memcallbacks.free_fcn = expat_free;
	info.parser = XML_ParserCreate_MM(nullptr, &memcallbacks, nullptr);
	if (info.parser == nullptr)
	{
		info.rootnode->file_free();
		return false;
	}

	/* configure the parser */
	XML_SetElementHandler(info.parser, expat_element_start, expat_element_end);
	XML_SetCharacterDataHandler(info.parser, expat_data);
	XML_SetUserData(info.parser, &info);

	/* optional parser initialization step */
	if (opts != nullptr && opts->init_parser != nullptr)
		(*opts->init_parser)(info.parser);
	return true;
}


/*-------------------------------------------------
    expat_element_start - expat callback for a new
    element
-------------------------------------------------*/

static void expat_element_start(void *data, const XML_Char *name, const XML_Char **attributes)
{
	xml_parse_info *parse_info = (xml_parse_info *) data;
	xml_data_node **curnode = &parse_info->curnode;
	xml_data_node *newnode;
	int attr;

	/* add a new child node to the current node */
	newnode = (*curnode)->add_child(name, nullptr);
	if (newnode == nullptr)
		return;

	/* remember the line number */
	newnode->line = XML_GetCurrentLineNumber(parse_info->parser);

	/* add all the attributes as well */
	for (attr = 0; attributes[attr]; attr += 2)
		newnode->add_attribute(attributes[attr+0], attributes[attr+1]);

	/* set us up as the current node */
	*curnode = newnode;
}


/*-------------------------------------------------
    expat_data - expat callback for an additional
    element data
-------------------------------------------------*/

static void expat_data(void *data, const XML_Char *s, int len)
{
	xml_parse_info *parse_info = (xml_parse_info *) data;
	xml_data_node **curnode = &parse_info->curnode;
	(*curnode)->append_value(s, len);
}


/*-------------------------------------------------
    expat_element_end - expat callback for the end
    of an element
-------------------------------------------------*/

static void expat_element_end(void *data, const XML_Char *name)
{
	xml_parse_info *parse_info = (xml_parse_info *) data;
	xml_data_node **curnode = &parse_info->curnode;

	/* strip leading/trailing spaces from the value data */
	if (!(parse_info->flags & XML_PARSE_FLAG_WHITESPACE_SIGNIFICANT))
		(*curnode)->trim_whitespace();

	/* back us up a node */
	*curnode = (*curnode)->get_parent();
}



/***************************************************************************
    NODE/ATTRIBUTE ADDITIONS
***************************************************************************/

/*-------------------------------------------------
    add_attribute - add a new attribute to the
    given node
-------------------------------------------------*/

void xml_data_node::add_attribute(const char *name, const char *value)
{
	try
	{
		attribute_node &anode = *m_attributes.emplace(m_attributes.end(), name, value);
		std::transform(anode.name.begin(), anode.name.end(), anode.name.begin(), [] (char ch) { return std::tolower(uint8_t(ch)); });
	}
	catch (...)
	{
	}
}



/***************************************************************************
    RECURSIVE TREE OPERATIONS
***************************************************************************/

/*-------------------------------------------------
    write_node_recursive - recursively write
    an XML node and its children to a file
-------------------------------------------------*/

void xml_data_node::write_recursive(int indent, util::core_file &file) const
{
	/* output this tag */
	file.printf("%*s<%s", indent, "", get_name());

	/* output any attributes */
	for (attribute_node const &anode : m_attributes)
		file.printf(" %s=\"%s\"", anode.name, anode.value);

	if (!get_first_child() && !get_value())
	{
		/* if there are no children and no value, end the tag here */
		file.printf(" />\n");
	}
	else
	{
		/* otherwise, close this tag and output more stuff */
		file.printf(">\n");

		/* if there is a value, output that here */
		if (get_value())
			file.printf("%*s%s\n", indent + 4, "", get_value());

		/* loop over children and output them as well */
		if (get_first_child())
		{
			for (xml_data_node const *child = this->get_first_child(); child; child = child->get_next_sibling())
				child->write_recursive(indent + 4, file);
		}

		/* write a closing tag */
		file.printf("%*s</%s>\n", indent, "", get_name());
	}
}
