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

	std::string const	&UpFile::get_buffer(void) const
	{
		return (_buffer);
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
		set_delimiter(_buffer.substr(0, _buffer.find("\r\n")));
		while (_buffer.compare("--\r\n"))
		{
			std::string disposition_key("Content-Disposition:");
			std::string file_key("filename=\"");
			std::string file_name;
			std::string file_content;
			std::string sub_buf(_buffer.substr(_buffer.find(disposition_key) + disposition_key.size(), _buffer.find("\r\n", _buffer.find(disposition_key) + disposition_key.size()) - (_buffer.find(disposition_key) + disposition_key.size())));
			size_t begin(_buffer.find("\r\n\r\n") + 4);
			size_t end(_buffer.find(_delimiter, begin) - 2);
			file_name = sub_buf.substr(sub_buf.find(file_key) + file_key.size(), sub_buf.size() - (sub_buf.find(file_key) + file_key.size() + 1));
			file_content = _buffer.substr(begin, end - begin);
			_files[file_name] = file_content;
			_buffer.erase(0, end + 2 + _delimiter.size());
		}
	}

	void				UpFile::write_to_file(std::string const &relative_path)
	{
		std::ofstream dest_file;
		parse_fileStream();
		std::map<std::string, std::string>::iterator it = _files.begin();
		while (it != _files.end())
		{
			dest_file.open(std::string(relative_path + it->first).c_str(), std::ios::binary);
			if (!dest_file.good())
				throw std::exception();
			dest_file << it->second;
			dest_file.close();
			it++;
		}
	}

	std::map<std::string, std::string> const	&UpFile::get_files() const {
		return _files;
	}

	void				UpFile::append_buf(char *buf, size_t n)
	{
		_buffer.append(buf, n);
	}

	bool				UpFile::is_file(void) const
	{
		return (_buffer.find("\r\nContent-Type: ") != std::string::npos);
	}
} // namespace webserv
