#include "UpFile.hpp"

namespace webserv
{
	UpFile::UpFile() :	_delimiter(""),
						_fileName(""),
						_fileContent("")
	{}

	UpFile::UpFile(UpFile const &other) :	_delimiter(other._delimiter),
											_fileName(other._fileName),
											_fileContent(other._fileContent)
	{}

	UpFile::~UpFile()
	{}

	UpFile& UpFile::operator=(UpFile const &other)
	{
		_delimiter = other._delimiter;
		_fileName = other._fileName;
		_fileContent = other._fileContent;

		return (*this);
	}

	std::string const	&UpFile::get_delimiter(void) const
	{
		return (_delimiter);
	}

	std::string const	&UpFile::get_fileName(void) const
	{
		return (_fileName);
	}

	std::string const	&UpFile::get_fileContent(void) const
	{
		return (_fileContent);
	}

	void				UpFile::set_delimiter(std::string delimiter)
	{
		_delimiter = delimiter;
	}

	void				UpFile::set_fileName(std::string fileName)
	{
		_fileName = fileName;
	}

	void				UpFile::set_fileContent(std::string fileContent)
	{
		_fileContent = fileContent;
	}

	void				UpFile::parse_fileStream(std::string const &buffer)
	{
		std::string	del_key("\r\n\r\n");
		if (_delimiter.empty())
		{
			int pos = buffer.find("\r\n");
			set_delimiter(buffer.substr(0, pos));
			std::string disposition_key("Content-Disposition:");
			std::string sub_buf(buffer.substr(buffer.find(disposition_key) + disposition_key.size(), buffer.find("\r\n", buffer.find(disposition_key) + disposition_key.size())));
			std::string file_key("filename=\"");
			set_fileName(sub_buf.substr(sub_buf.find(file_key) + file_key.size(), sub_buf.find("\r\n") - sub_buf.find(file_key) + file_key.size())); // unsafe - testing purposes
		}
		if (_delimiter == buffer.substr(0, buffer.find("\r\n")))
			_fileContent.append(buffer.substr(buffer.find(del_key) + del_key.size(), buffer.rfind(del_key, _delimiter.size() + 1)));
		else
			_fileContent.append(buffer.substr(0, buffer.rfind(del_key, _delimiter.size() + 1)));
	}
} // namespace webserv
