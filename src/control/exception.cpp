//Mike Maxim
//Exception implementation

#include <exception.h>

Exception::Exception() : m_msg("An error has occurred") { }

Exception::Exception(const string& str) : m_msg(str) {

}

string Exception::getMessage() {
	return m_msg;
}
