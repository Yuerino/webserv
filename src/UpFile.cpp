#include "UpFile.hpp"
#include "utils.hpp"

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

	void				UpFile::parse_fileStream(void)
	{
		std::string	del_key("\r\n\r\n");
		int pos = _buffer.find("\r\n");
		set_delimiter(_buffer.substr(0, pos));
		std::string disposition_key("Content-Disposition:");
		std::string sub_buf(_buffer.substr(_buffer.find(disposition_key) + disposition_key.size(), _buffer.find("\r\n", _buffer.find(disposition_key) + disposition_key.size()) - (_buffer.find(disposition_key) + disposition_key.size())));
		std::string file_key("filename=\"");
		set_fileName(sub_buf.substr(sub_buf.find(file_key) + file_key.size(), sub_buf.size() - (sub_buf.find(file_key) + file_key.size() + 1))); // unsafe - testing purposes
		_fileContent = _buffer.substr(_buffer.find("\r\n\r\n") + 4, (_buffer.rfind(_delimiter) - 2) - (_buffer.find("\r\n\r\n") + 4));
	}

	void				UpFile::write_to_file(std::string const &path)
	{
		std::ofstream dest_file;
		parse_fileStream();
		try
		{
			dest_file.open(std::string(path + _fileName).c_str(), std::ios::binary);
		}
		catch(const std::exception& e)
		{
			return ;
		}
		dest_file << _fileContent;
		dest_file.close();
	}


	void				UpFile::append_buf(char *buf, size_t n)
	{
		_buffer.append(buf, n);
	}
} // namespace webserv
