/*
 * cpp11_operator1.cpp
 *
 *  Created on: Sep 21, 2017
 *      Author: tingqian
 */
#include <iostream>
#include <cstring>

class words{
public:
	words(const char * separator = " "){
		m_content[0] = 0;
		m_sep = separator;
	}
	words& operator<<(const char * x)
	{
		if(strlen(m_content) > 0)
			strcat(m_content,m_sep);//spertaor

		strcat(m_content, x);
		return *this;
	}
	words& operator+=(const char * x)
	{
		return operator<<(x);
	}
	const char * m_sep;
	char m_content[1024];
};


std::ostream& operator<<(std::ostream& os, words& w)
{
	return os<< "words{.content=" << w.m_content << "}";
}

int main()
{
	words w(",");
	int c = 0;
	w <<"hello" << "world";
	(w += "operator") += "overload";
	(c+=10) +=9;

	std::cout << "c=" << c << std::endl;
	std::cout << w << std::endl;
}



